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

channel::channel(event_loop* loop, const std::shared_ptr<socket>& sockfd) : 
	loop_(loop), 
	socket_(sockfd), 
	events_(0), 
	revents_(0), 
	index_(-1),
	log_hup_(true), 
	tied_(false), 
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

void channel::tie(const std::shared_ptr<void>& obj)
{
	tie_ = obj;
	tied_ = true;
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

void channel::handle_event(Timestamp&& receive_time)
{
	std::shared_ptr<void> guard;
	if (tied_)
	{
		guard = tie_.lock();
		if (guard)
		{
			handle_event_with_guard(std::move(receive_time));
		}
	}
	else
	{
		handle_event_with_guard(std::move(receive_time));
	}
}

void channel::handle_event_with_guard(Timestamp&& receive_time)
{
	event_handling_ = true;
	LOG_TRACE << revents_to_string();
int sockfd = -1;
std::shared_ptr<socket> socket = socket_.lock();
//if(socket)
sockfd = *socket;
	if ((revents_ & POLLHUP) && !(revents_ & POLLIN))
	{
		if (log_hup_)
		{
			LOG_WARN << "fd = " << sockfd << " channel::handle_event() POLLHUP";
		}
		if (close_callback_) 
		{
			close_callback_();
		}
	}

	if (revents_ & POLLNVAL)
	{
		LOG_WARN << "fd = " << sockfd << " channel::handle_event() POLLNVAL";
	}

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
int sockfd = -1;
std::shared_ptr<socket> socket = socket_.lock();
//if(socket)
sockfd = *socket;

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
