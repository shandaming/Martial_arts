/*
 * Copyright (C) 2018
 */

#include <sys/timerfd.h>
#include <unistd.h>

#include "net/timer_queue.h"
#include "net/event_loop.h"
#include "log/logging.h"

namespace detail
{
int create_timerfd()
{
	int timerfd = timerfd_create(CLOCK_MONOTONIC, 
			TFD_NONBLOCK | TFD_CLOEXEC);
	if (timerfd < 0)
	{
		LOG_SYSFATAL << "Failed in timerfd_create";
	}
	return timerfd;
}

timespec how_much_time_from_now(Timestamp when)
{
	int64_t microseconds = when.micro_seconds_since_epoch() -
		Timestamp::now().micro_seconds_since_epoch();
	if (microseconds < 100)
	{
		microseconds = 100;
	}

	struct timespec ts;
	ts.tv_sec = static_cast<time_t>(microseconds / 
			Timestamp::kMicro_seconds_per_second);
	ts.tv_nsec = static_cast<long>((microseconds % 
				Timestamp::kMicro_seconds_per_second) * 1000);
	return ts;
}

void read_timerfd(int timerfd, Timestamp now)
{
	uint64_t howmany;
	ssize_t n = read(timerfd, &howmany, sizeof howmany);

	LOG_TRACE << "timer_queue::handleRead() " << howmany << " at " << 
		now.to_string();

	if (n != sizeof howmany)
	{
		LOG_ERROR << "timer_queue::handleRead() reads " << n << 
			" bytes instead of 8";
	}
}

void reset_timerfd(int timerfd, Timestamp expiration)
{
	// wake up loop by timerfd_settime()
	itimerspec new_value = {0};
	itimerspec old_value = {0};

	new_value.it_value = how_much_time_from_now(expiration);
	int ret = timerfd_settime(timerfd, 0, &new_value, &old_value);
	if (ret)
	{
		LOG_SYSERR << "timerfd_settime()";
	}
}
}

namespace net
{
timer_queue::timer_queue(event_loop* loop) :
	loop_(loop),
    timerfd_(detail::create_timerfd()),
    timerfd_channel_(loop, timerfd_),
    timers_(),
    calling_expired_timers_(false)
{
	timerfd_channel_.set_read_callback(std::bind(&timer_queue::handleRead, 
			  this));
	// we are always reading the timerfd, we disarm it with timerfd_settime.
	timerfd_channel_.enable_read();
}

timer_queue::~timer_queue()
{
	timerfd_channel_.disable_all();
	timerfd_channel_.remove();
	close(timerfd_);
	// do not remove channel, since we're in event_loop::dtor();
	for(auto& it : timers_)
	{
		delete it.second;
	}
}

Timer_id timer_queue::add_timer(Timer_callback&& cb, Timestamp when,
		double interval)
{
	timer* timer = new timer(std::move(cb), when, interval);
	if(timer == nullptr)
	{
		return Timer_id();
	}

	loop_->run_in_loop(std::bind(&timer_queue::add_timer_in_loop, this, 
				timer));
	return Timer_id(timer, timer->sequence());
}

void timer_queue::cancel(const Timer_id& timerId)
{
	loop_->run_in_loop(std::bind(&timer_queue::cancel_in_loop, this, 
				std::ref(timerId)));
}

void timer_queue::add_timer_in_loop(timer* timer)
{
	loop_->assert_in_loop_thread();
	bool earliest_changed = insert(timer);

	if (earliest_changed)
	{
		detail::reset_timerfd(timerfd_, timer->expiration());
	}
}

void timer_queue::cancel_in_loop(const Timer_id& timerId)
{
	loop_->assert_in_loop_thread();

	assert(timers_.size() == active_timers_.size());

	Active_timer timer(timerId.timer_, timerId.sequence_);
	Active_timer_set::iterator it = active_timers_.find(timer);
	if (it != active_timers_.end())
	{
		size_t n = timers_.erase(Entry(it->first->expiration(), it->first));

		assert(n == 1); (void)n;

		delete it->first; // FIXME: no delete please
		active_timers_.erase(it);
	}
	else if (calling_expired_timers_)
	{
		canceling_timers_.insert(timer);
	}

	assert(timers_.size() == active_timers_.size());
}

void timer_queue::handleRead()
{
	loop_->assert_in_loop_thread();
	Timestamp now(Timestamp::now());
	detail::read_timerfd(timerfd_, now);

	std::vector<Entry> expired = get_expired(now);

	calling_expired_timers_ = true;
	canceling_timers_.clear();
	// safe to callback outside critical section
	for(auto& it : expired)
	{
		it.second->run();
	}
	calling_expired_timers_ = false;

	reset(expired, now);
}

std::vector<timer_queue::Entry> timer_queue::get_expired(Timestamp now)
{
	assert(timers_.size() == active_timers_.size());

	std::vector<Entry> expired;
	Entry sentry(now, reinterpret_cast<timer*>(UINTPTR_MAX));
	Timer_list::iterator end = timers_.lower_bound(sentry);

	assert(end == timers_.end() || now < end->first);

	std::copy(timers_.begin(), end, back_inserter(expired));
	timers_.erase(timers_.begin(), end);

	for(auto& it : expired)
	{
		Active_timer timer(it.second, it.second->sequence());
		size_t n = active_timers_.erase(timer);
		assert(n == 1); (void)n;
	}

	assert(timers_.size() == active_timers_.size());

	return expired;
}

void timer_queue::reset(const std::vector<Entry>& expired, Timestamp now)
{
	Timestamp next_expire;

	for(auto& it : expired)
	{
		Active_timer timer(it.second, it.second->sequence());
		if (it.second->repeat() && 
				canceling_timers_.find(timer) == canceling_timers_.end())
		{
			it.second->restart(now);
			insert(it.second);
		}
		else
		{
			// FIXME move to a free list
			delete it.second; // FIXME: no delete please
		}
	}

	if (!timers_.empty())
	{
		next_expire = timers_.begin()->second->expiration();
	}

	if (next_expire.valid())
	{
		detail::reset_timerfd(timerfd_, next_expire);
	}
}

bool timer_queue::insert(timer* timer)
{
	loop_->assert_in_loop_thread();

	assert(timers_.size() == active_timers_.size());

	bool earliest_changed = false;
	Timestamp when = timer->expiration();
	Timer_list::iterator it = timers_.begin();
	if (it == timers_.end() || when < it->first)
	{
		earliest_changed = true;
	}

	{
		std::pair<Timer_list::iterator, bool> result = 
			timers_.insert(Entry(when, timer));
		assert(result.second); (void)result;
	}

	{
		std::pair<Active_timer_set::iterator, bool> result = 
			active_timers_.insert(Active_timer(timer, timer->sequence()));
		assert(result.second); (void)result;
	}

	assert(timers_.size() == active_timers_.size());
	return earliest_changed;
}
}
