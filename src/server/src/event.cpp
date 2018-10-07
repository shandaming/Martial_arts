/*
 * Copyright (C) 2018
 */

#include "event.h"

namespace event
{
	void Context::add_handler(Event_handler* ptr)
	{
		handlers.push_back(ptr);
	}

	bool Context::remove_handler(Event_handler* ptr)
	{
		if(!handlers.empty())
		{
			auto it = std::find(handlers.begin(), handlers.end(), it);
			if(it != handlers.end())
			{
				handlers.erase(it);
				return true;
			}
		}
		return false;
	}


	Epoll_handler::Epoll_handler(int size, const Socket& fd) : epollfd_(-1),
		events_(new epoll_event(size))
	{
		if((epollfd_ = epoll_create(size)) == -1)
			return;

		epoll_event e;
		e.events = EPOLLIN;
		e.data.fd = fd;

		if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &e) == -1)
			return;
	}

	Epoll_handler::~Epoll_handler()
	{
		if(events_)
			delete[] events_;
	}

	bool register_event(void* obj, int event_mask)
	{
		if(!obj)
			return false;

		struct epoll_event e;
		e.events = event_mask | EPOLLHUP | EPOLLERR;
		e.data.ptr = obj;
		if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, obj->get(), &e) == -1)
			return false;

		obj->set_event(this);
		return true;
	}

	bool run_event_loop()
	{
	}
}
