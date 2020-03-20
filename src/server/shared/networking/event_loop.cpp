/*
 * Copyright (C) 2018
 */

#include <sys/eventfd.h>
#include <unistd.h>
#include <cassert>

#include "event_loop.h"
#include "log/logging.h"
#include "common/thread.h"

namespace
{
thread_local net::event_loop* loop_in_this_thread = nullptr;

constexpr int poll_time_ms = 10000;

int create_event_fd()
{
	int event_fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	ASSERT(event_fd != -1, "create event fd failed. Error %d: %s", errno, std::strerror(errno));
	return event_fd;
}
}

event_loop* event_loop::get_event_loop_of_current_thread()
{
	return loop_in_this_thread;
}

event_loop::event_loop() : looping_(false), quit_(false), 
	event_handling_(false),
	calling_pending_functors_(false),
    thread_id_(get_current_thread_id()),
    poller_(new Poller(this)),
    timer_queue_(new Timer_queue(this)),
    wakeup_fd_(create_event_fd()),
    wakeup_channel_(new channel(this, wakeup_fd_)),
    current_active_channel_(NULL)
{
	LOG_DEBUG("networking", "create event_loop in thread id %d", get_current_thread_id());

	FATAL(loop_in_this_thread == nullptr, "Another event_loop %p exists in this thread %d", 
	      loop_in_this_thread, get_current_thread_id());

	loop_in_this_thread = this;

	wakeup_channel_->set_read_callback(
		std::bind(&event_loop::handle_read, this));
	// we are always reading the wakeupfd
	wakeup_channel_->enable_read();
}

event_loop::~event_loop()
{
	LOG_DEBUG << "event_loop " << this << " of thread " << thread_id_
				<< " destructs in thread " << get_current_thread_id();
	wakeup_channel_->disable_all();
	wakeup_channel_->remove();
	close(wakeup_fd_);
	loop_in_this_thread = NULL;
}

void event_loop::loop()
{
	ASSERT(!looping_);
	assert_in_loop_thread();
	looping_ = true;
	quit_ = false;  // FIXME: what if someone calls quit() before loop() ?
	
	LOG_TRACE("networking", "event_loop:%p stats loop in thread id %d", this, get_current_thread_id());

	while (!quit_)
	{
		active_channels_.clear();
		poller_->poll(poll_time_ms, &active_channels_);
		#ifdef DEBUG
		{
		print_active_channels();
		}
		#endif
		// TODO sort channel by priority
		event_handling_ = true;

		for(auto& it : active_channels_)
		{
			current_active_channel_ = it;
			current_active_channel_->handle_event();
		}
		current_active_channel_ = NULL;
		event_handling_ = false;
		do_pending_functors();
	}

	LOG_TRACE("networking", "event_loop:%p stop loop in thread id %d", this, get_current_thread_id());
	
	looping_ = false;
}

void event_loop::quit()
{
	quit_ = true;
	// loop（）有可能在（！quit_）执行时退出，
	//然后Event_loop析构，然后我们访问一个无效的对象。
	//可以在两个地方使用mutex_进行修复。
	if (!is_in_loop_thread())
	{
		wakeup();
	}
}

size_t event_loop::queue_size()
{
	std::lock_guard<std::mutex> lock(mutex_);
	return pending_functors_.size();
}

// FIXME: remove duplication
void event_loop::run_in_loop(functor&& cb)
{
	if (is_in_loop_thread())
		cb();
	else
		queue_in_loop(std::move(cb));
}

void event_loop::queue_in_loop(functor&& cb)
{
	std::lock_guard<std::mutex> lock(mutex_);
	pending_functors_.push_back(std::move(cb));  // emplace_back

	if (!is_in_loop_thread() || calling_pending_functors_)
	{
		wakeup();
	}
}

Timer_id event_loop::run_at(const Timestamp& time, Timer_callback&& cb)
{
	return timer_queue_->add_timer(std::move(cb), time, 0.0);
}

Timer_id event_loop::run_after(double delay, Timer_callback&& cb)
{
	Timestamp time(add_time(Timestamp::now(), delay));
	return run_at(time, std::move(cb));
}

Timer_id event_loop::run_every(double interval, Timer_callback&& cb)
{
	Timestamp time(add_time(Timestamp::now(), interval));
	return timer_queue_->add_timer(std::move(cb), time, interval);
}

void event_loop::cancel(Timer_id timerId)
{
	return timer_queue_->cancel(timerId);
}

void event_loop::update_channel(channel* channel)
{
	ASSERT(channel->owner_loop() == this);
	assert_in_loop_thread();
	poller_->update_channel(channel);
}

void event_loop::remove_channel(channel* channel)
{
	ASSERT(channel->owner_loop() == this);
	assert_in_loop_thread();
	if (event_handling_)
	{
		ASSERT(current_active_channel_ == channel ||
			std::find(active_channels_.begin(), active_channels_.end(), channel) == active_channels_.end());
	}
	poller_->remove_channel(channel);
}

bool event_loop::has_channel(channel* channel)
{
	ASSERT(channel->owner_loop() == this);
	assert_in_loop_thread();
	return poller_->has_channel(channel);
}

void event_loop::wakeup()
{
	uint64_t one = 1;
	ssize_t n = write(wakeup_fd_, &one, sizeof one);
	if (n != sizeof one)
		LOG_ERROR << "event_loop::wakeup() writes " << n << " bytes instead of 8";
}

void event_loop::handle_read()
{
	uint64_t one = 1;
	ssize_t n = read(wakeup_fd_, &one, sizeof one);
	if (n != sizeof one)
		LOG_ERROR << "event_loop::handleRead() reads " << n << " bytes instead of 8";
}

void event_loop::do_pending_functors()
{
	std::vector<functor> functors;
	calling_pending_functors_ = true;

	{
		std::lock_guard<std::mutex> lock(mutex_);
		functors.swap(pending_functors_);
	}

	for (size_t i = 0; i < functors.size(); ++i)
	{
		functors[i]();
	}
	calling_pending_functors_ = false;
}

void event_loop::print_active_channels() const
{
	for(auto& it : active_channels_)
	{
		const channel* ch = it;
		LOG_TRACE << "{" << ch->revents_to_string() << "} ";
	}
}

void event_loop::assert_in_loop_thread()
{
	FATAL(is_in_loop_thread(), "event_loop:%p was created in thread id:%d"
			", current thread id:%d", this, thread_id_, get_current_thread_id());
}
