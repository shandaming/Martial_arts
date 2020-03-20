/*
 * Copyright (C) 2018
 */

#include <sys/epoll.h>
#include <unistd.h>

#include "epoll.h"
#include "channel.h"
#include "event_loop.h"
#include "log/logging.h"

epoll::epoll(event_loop* loop) : 
	owner_loop_(loop), 
	epollfd_(epoll_create1(EPOLL_CLOEXEC)),
	events_(event_list_size)
{
	ASSERT(epollfd_ != -1, "create epoll fd failed. Error %d: %s", errno, std::strerror(errno));
}

epoll::~epoll()
{
	close(epollfd_);
}

void epoll::poll(int timeout_ms, channel_list* active_channels)
{
	LOG_TRACE("networking", "Total number of events %u", channels_size());

	int num_events = epoll_wait(epollfd_, &*events_.begin(),
			static_cast<int>(events_.size()), timeout_ms);
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
			LOG_SYSERR << "epoll::poll()";
		}
	}
}

void epoll::fill_active_channels(int num_events,
		channel_list* active_channels) const
{
	ASSERT(static_cast<size_t>(num_events) <= events_.size());

	for (int i = 0; i < num_events; ++i)
	{
		channel* channel = static_cast<channel*>(events_[i].data.ptr);
#ifndef NDEBUG
		const int fd = channel->get_descriptor();
		
		ASSERT(channels_.count(fd));
		ASSERT(channels_[fd] == channel);
#endif
		channel->set_revents(events_[i].events);
		active_channels->push_back(channel);
	}
}

void epoll::update_channel(channel* channel)
{
	assert_in_loop_thread();

	channel::state current_state = channel->get_state();
	const int fd = channel->get_descriptor();

	LOG_TRACE("networking", "channel: %d events: %d index: %d", fd, channel->events(), index);

	if(current_state == channel::NEW || current_state == channel::DELETED)
	{
		// a new one, add with EPOLL_CTL_ADD
		if(current_state == channel::NEW)
		{
			ASSERT(channels_.count(fd));
			
			channels_[fd] = channel;
		}
		else // index == channel::DELETED
		{
			ASSERT(channels_.count(fd));
			ASSERT(channels_[fd] == channel);
		}

		channel->set_state(channel::ADDED);
		update(EPOLL_CTL_ADD, channel);
	}
	else
	{
		// update existing one with EPOLL_CTL_MOD/DEL
		ASSERT(channels_.count(fd));
		ASSERT(channels_[fd] == channel);
		ASSERT(current_state == channel::ADDED);

		if(channel->is_none_event())
		{
			update(EPOLL_CTL_DEL, channel);
			channel->set_state(channel::DELETED);
		}
		else
		{
			update(EPOLL_CTL_MOD, channel);
		}
	}
}

void epoll::remove_channel(channel* channel)
{
	assert_in_loop_thread();
	const int fd = channel->get_descriptor();

	LOG_TRACE("networking", "remove channel:%d", fd);

	ASSERT(channels_.count(fd));
	ASSERT(channels_[fd] == channel);
	ASSERT(channel->is_none_event());

	channel::state current_state = channel->get_state();
	assert(current_state == channel::ADDED || current_state == channel::DELETED);
	size_t n = channels_.erase(fd);
	ASSERT(n == 1);

	if(current_state == channel::ADDED)
		update(EPOLL_CTL_DEL, channel);

	channel->set_state(channel::NEW);
}

void epoll::update(int operation, channel* channel)
{
	struct epoll_event event;
	bzero(&event, sizeof event);
	event.events = channel->events();
	event.data.ptr = channel;
	const int fd = channel->get_descriptor();

	LOG_TRACE("networking", "epoll_ctl op %s channel:%d event{%s}", 
		  operation_to_string(operation), fd, channel_->events_to_string());

	if(epoll_ctl(epollfd_, operation, fd, &event) < 0)
	{
		if(operation == EPOLL_CTL_DEL)
			LOG_ERROR("epoll_ctl op %s channel:%d", operation_to_string(), fd);
		else
			LOG_FATAL("epoll_ctl op %s channel:%d", operation_to_string(), fd);
	}
}

const char* epoll::operation_to_string(int op)
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

bool epoll::has_channel(channel* channel) const
{
	assert_in_loop_thread();
	
	const int fd = channel->get_descriptor();
	return (channels_.count(fd) && channels_[fd] == channel);
}

void epoll::assert_in_loop_thread() const
{
	owner_loop_->assert_in_loop_thread();
}
