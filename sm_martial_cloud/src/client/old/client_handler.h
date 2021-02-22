/*
 * Copyright (C) 2018
 */

#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

class Client_handler : public Event_handler
{
	public:
		Client_handler();

		~Client_handler();

		void add_client(int fd, Socket client);

		bool handle_accept(int listenfd)
		{
			sockaddr_in client_sin;
			socklen_t sin_size = sizeof(client_sin);

			int client_fd = accept(listenfd, reinterpret_cast<sockaddr*>(&client_sin), &sin_size);
			if(client_fd == -1)
				return false;

			Socket clinet_socket(client_fd, Netaddr(client_sin));
			client_socket.set_nonblocking();

			clients_.emplace(client_fd, client_socket);
			return true;
		}
	private:
		std::map<int, Socket> clients_;
};

#endif
