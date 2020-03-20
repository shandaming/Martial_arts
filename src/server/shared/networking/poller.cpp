/*
 * Copyright (C) 2018
 */

#include <sys/epoll.h>
#include <unistd.h>

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
	ASSERT(epollfd_ != -1, "create epoll fd failed. Error %d: %s", errno, std::strerror(errno));
}

Poller::~Poller()
{
	close(epollfd_);
}

void Poller::poll(int timeoutMs, Channel_list* active_channels)
{
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
	ASSERT(static_cast<size_t>(num_events) <= events_.size());

	for (int i = 0; i < num_events; ++i)
	{
		Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
#ifndef NDEBUG
		const int fd = channel->get_descriptor();
		
		ASSERT(channels_.count(fd));
		ASSERT(channels_[fd] == channel);
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

	LOG_TRACE("networking", "channel: %d events: %d index: %d", fd, channel->events(), index);

	if(index == kNew || index == kDeleted)
	{
		// a new one, add with EPOLL_CTL_ADD
		if(index == kNew)
		{
			ASSERT(channels_.count(fd));
			
			channels_[fd] = channel;
		}
		else // index == kDeleted
		{
			ASSERT(channels_.count(fd));
			ASSERT(channels_[fd] == channel);
		}

		channel->set_index(kAdded);
		update(EPOLL_CTL_ADD, channel);
	}
	else
	{
		// update existing one with EPOLL_CTL_MOD/DEL
		ASSERT(channels_.count(fd));
		ASSERT(channels_[fd] == channel);
		ASSERT(index == kAdded);

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

	LOG_TRACE("networking", "remove channel:%d", fd);

	ASSERT(channels_.count(fd));
	ASSERT(channels_[fd] == channel);
	ASSERT(channel->is_none_event());

	const int index = channel->index();
	assert(index == kAdded || index == kDeleted);
	size_t n = channels_.erase(fd);
	ASSERT(n == 1);

	if(index == kAdded)
		update(EPOLL_CTL_DEL, channel);

	channel->set_index(kNew);
}

void Poller::update(int operation, Channel* channel)
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
	
	const int fd = channel->get_descriptor();
	return (channels_.count(fd) && channels_[fd] == channel);
}

void Poller::assert_in_loop_thread() const
{
	owner_loop_->assert_in_loop_thread();
}
