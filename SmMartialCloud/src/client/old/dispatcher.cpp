/*
 * Copyright (C) 2018
 */

#include "dispatcher.h"

namespace event
{
	Dispatcher::Dispatcher(int size, Event_handler* handler) : 
		listen_handler_(handler), events_(new epoll_event[size])
	{
		struct epoll_event e;
		e.events = EPOLLIN;
		e.data.ptr = handler_;

		// 注册监听事件
		if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, listen_handler_->get_fd(), &e) == -1)
		{}
		handler->set_dispatcher(this);
	}

	Dispatcher::~Dispatcher()
	{
		delete[] events_;
	}

	bool Dispatcher::register_handler(Event_handler* handler, int events)
	{
		if(!handler)
			return nullptr;

		struct epoll_event e;
		e.events = events;
		e.data.ptr = handler;

		if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, handler, &e) == -1)
			return false;
		handler->set_dispatcher(this);
		return true;
	}

	int Dispatcher::run_event(Event_handler* client_handler, int msec)
	{
		int listenfd = listen_handler_->get_fd();

		int fds = epoll_wait(epollfd_, events_, size, msec);
		if(fds != -1)
		{
			for(size_t t = 0; i < fds; ++i)
			{
				if(events_[i].data.fd == listenfd)
					client_handler->handle_accept(listenfd);
				else
					handler_client(events_[i])
			}
		}
	}
}
