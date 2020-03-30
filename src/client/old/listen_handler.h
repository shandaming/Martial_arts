/*
 * Copyright (C) 2018
 */

#ifndef LISTEN_HANDLER_H
#define LISTEN_HANDLER_H

#include "event.h"

class Listen_handler : public Event_handler
{
	public:
		Listen_handler(Socket fd) : fd_(fd) {}

		bool listen()
		{
			if(listen(fd_, SOMAXCONN) == -1)
				return false;
			return true;
		}

		int get_fd() const { return fd_; }

		bool read_handler(Event_handler* handler);

		bool accept();
	private:
		Socket& fd_;
};

#endif
