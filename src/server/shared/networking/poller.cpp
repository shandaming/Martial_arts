/*
 * Copyright (C) 2018
 */

#include <sys/epoll.h>
#include <unistd.h>

#include <cassert>

#include "poller.h"
#include "net/event_loop.h"
#include "log/logging.h"

namespace
{
const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;
}

Poller::Poller(Event_loop* loop) : 
	owner_loop_(loop), 
	epollfd_(epoll_create1(EPOLL_CLOEXEC)),
	events_(kInit_event_list_size)
{
	if(epollfd_ < 0)
	{
		std::error_code ec(errno, std::system_category());
		networking_exception ex(ec);
		throw ex;
	}
}

Poller::~Poller()
{
	close(epollfd_);
}

void Poller::poll(int timeoutMs, Channel_list* active_channels)
{
	//LOG_TRACE << "fd total count " << channels_.size();
	LOG_TRACE("networking", "Total number of events %u", channels_size());

	int num_events = epoll_wait(epollfd_, &*events_.begin(),
			static_cast<int>(events_.size()), timeoutMs);
	int saved_errno = errno;

	if(num_events > 0)
	{
		LOG_TRACE("networking", "%d events happened", num_events);

		fill_active_channels(num_events, active_channels);
		if(static_cast<size_t>(num_events) == events_.size())
			events_.resize(events_.size() * 2);
	}
	else if(num_events == 0)
		LOG_TRACE("networking", "nothing happened");
	else
	{
		// error happens, log uncommon ones
		if(saved_errno != EINTR)
		{
			errno = saved_errno;
			LOG_SYSERR << "Poller::poll()";
		}
	}
}

void Poller::fill_active_channels(int num_events,
		Channel_list* active_channels) const
{
	assert(static_cast<size_t>(num_events) <= events_.size());

	for (int i = 0; i < num_events; ++i)
	{
		Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
#ifndef NDEBUG
		const int fd = channel->get_descriptor();
		Channel_map::const_iterator it = channels_.find(fd);
		assert(it != channels_.end());
		assert(it->second == channel);
#endif
		channel->set_revents(events_[i].events);
		active_channels->push_back(channel);
	}
}

void Poller::update_channel(Channel* channel)
{
	assert_in_loop_thread();

	const int index = channel->index();
	const int fd = channel->get_descriptor();

	LOG_TRACE << "fd = " << channel->fd() << 
		" events = " << channel->events() << " index = " << index;

	if(index == kNew || index == kDeleted)
	{
		// a new one, add with EPOLL_CTL_ADD
		if(index == kNew)
		{
			assert(channels_.find(fd) == channels_.end());
			channels_[fd] = channel;
		}
		else // index == kDeleted
		{
			assert(channels_.find(fd) != channels_.end());
			assert(channels_[fd] == channel);
		}

		channel->set_index(kAdded);
		update(EPOLL_CTL_ADD, channel);
	}
	else
	{
		// update existing one with EPOLL_CTL_MOD/DEL
		assert(channels_.find(fd) != channels_.end());
		assert(channels_[fd] == channel);
		assert(index == kAdded);

		if(channel->is_none_event())
		{
			update(EPOLL_CTL_DEL, channel);
			channel->set_index(kDeleted);
		}
		else
		{
			update(EPOLL_CTL_MOD, channel);
		}
	}
}

void Poller::remove_channel(Channel* channel)
{
	assert_in_loop_thread();
	const int fd = channel->get_descriptor();

	LOG_TRACE << "fd = " << fd;

	assert(channels_.find(fd) != channels_.end());
	assert(channels_[fd] == channel);
	assert(channel->is_none_event());

	int index = channel->index();
	assert(index == kAdded || index == kDeleted);
	size_t n = channels_.erase(fd);
	assert(n == 1);

	if(index == kAdded)
	{
		update(EPOLL_CTL_DEL, channel);
	}
	channel->set_index(kNew);
}

void Poller::update(int operation, Channel* channel)
{
	struct epoll_event event;
	bzero(&event, sizeof event);
	event.events = channel->events();
	event.data.ptr = channel;
	const int fd = channel->get_descriptor();

	LOG_TRACE << "epoll_ctl op = " << operation_to_string(operation) << 
		" fd = " << fd << " event = { " << channel->events_to_string() << 
		" }";

	if(epoll_ctl(epollfd_, operation, fd, &event) < 0)
	{
		if(operation == EPOLL_CTL_DEL)
		{
			LOG_SYSERR << "epoll_ctl op =" << 
				operation_to_string(operation) << " fd =" << fd;
		}
		else
		{
			LOG_SYSFATAL << "epoll_ctl op =" << 
				operation_to_string(operation) << " fd =" << fd;
		}
	}
}

const char* Poller::operation_to_string(int op)
{
	switch (op)
	{
		case EPOLL_CTL_ADD:
			return "ADD";
		case EPOLL_CTL_DEL:
			return "DEL";
		case EPOLL_CTL_MOD:
			return "MOD";
		default:
			assert(false && "ERROR op");
			return "Unknown Operation";
	}
}

bool Poller::has_channel(Channel* channel) const
{
	assert_in_loop_thread();

	Channel_map::const_iterator it = channels_.find(channel->fd());
	return it != channels_.end() && it->second == channel;
}

void Poller::assert_in_loop_thread() const
{
	owner_loop_->assert_in_loop_thread();
}
