/*
 * Copyright (C) 2018
 */

#include "listen_handler.h"

namespace event
{
	bool Listen_handler::read_handler(Event_hander* handler)
	{
		Socket client;
		Netaddr addr;

		do
		{
			struct sockaddr_in client_sin;
			int client_len = sizeof(struct sockaddr_in);

			int client_fd = accept(socket_, reinterpret_cast<sockaddr*>(&client_sin), &client_len);
			if(client_fd == -1)
			{
				int err = errno;
				if(err == EAGAIN)
					break;
				if(err == ECONNABORTED || err == EPROTO || err == EINTR)
					continue;
				break;
			}

			Socket client(client_fd);
		}
	}

	bool accept(Socket& fd, Netaddr& addr)
	{
		struct sockaddr_in sin;
		int len = sizeof(struct sockaddr_in);

		int client_fd = accept(fd, reinterpret_cast<sockaddr*>(&sin), &len);
		if(client_fd == -1)
		{}

		fd = client_fd;
		addr = sin;
	}
e
