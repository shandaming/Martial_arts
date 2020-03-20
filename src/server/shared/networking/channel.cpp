/*
 * Copyright (C) 2018
 */

#include <sys/epoll.h>
#include <poll.h> //???

#include <cassert>

#include "channel.h"
#include "event_loop.h"
#include "logging/log.h"

const int channel::none_event_ = 0;
const int channel::read_event_ = POLLIN | POLLPRI; // ???EPOLLIN...
const int channel::write_event_ = POLLOUT;

channel::channel(event_loop* loop, int sockfd) : 
	loop_(loop), 
	sockfd_(sockfd), 
	events_(0), 
	revents_(0), 
	index_(-1),
	log_hup_(true), 
	event_handling_(false), 
	added_to_loop_(false) {}

channel::~channel()
{
	assert(!event_handling_);
	assert(!added_to_loop_);
	if (loop_->is_in_loop_thread())
	{
		assert(!loop_->has_channel(this));
	}
}

void channel::update()
{
	added_to_loop_ = true;
	loop_->update_channel(this);
}

void channel::remove()
{
	assert(is_none_event());

	added_to_loop_ = false;
	loop_->remove_channel(this);
}

void channel::handle_event()
{
	handle_event_with_guard();
}

void channel::handle_event_with_guard()
{
	event_handling_ = true;
	LOG_TRACE << revents_to_string();
	
	if ((revents_ & POLLHUP) && !(revents_ & POLLIN))
	{
		if (log_hup_)
			LOG_WARN("networking", "channel:%d handle event: POLLHUP", sockfd_);
		if (close_callback_) 
			close_callback_();
	}

	if (revents_ & POLLNVAL)
		LOG_WARN("networking", "channel:%d handle event: POLLNVAL", sockfd_);

	if (revents_ & (POLLERR | POLLNVAL))
	{
		if (error_callback_) 
			error_callback_();
	}
	if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
	{
		if (read_callback_) 
			read_callback_(std::forward<Timestamp>(receive_time));
	}
	if (revents_ & POLLOUT)
	{
		if (write_callback_) 
			write_callback_();
	}
	event_handling_ = false;
}

std::string channel::revents_to_string() const
{
	return events_to_string(socket_, revents_);
}

std::string channel::events_to_string() const
{
	return events_to_string(socket_, events_);
}

std::string channel::events_to_string(int fd, int ev)
{
	std::ostringstream oss;
	oss << fd << ": ";
	if (ev & POLLIN)
	{
		oss << "IN ";
	}
	if (ev & POLLPRI)
	{
		oss << "PRI ";
	}
	if (ev & POLLOUT)
	{
		oss << "OUT ";
	}
	if (ev & POLLHUP)
	{
		oss << "HUP ";
	}
	if (ev & POLLRDHUP)
	{
		oss << "RDHUP ";
	}
	if (ev & POLLERR)
	{
		oss << "ERR ";
	}
	if (ev & POLLNVAL)
	{
		oss << "NVAL ";
	}

	return oss.str().c_str();
}
