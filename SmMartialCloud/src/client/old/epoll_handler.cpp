/*
 * Copyright (C) 2018
 */

#include "epoll_handler.h"
#include "net/socket.h"
#include "net/netaddr.h"

namespace event
{

	static int listenfd = -1;

	Epoll_handler::Epoll_handler(int max) : max_(max), epollfd_(-1), events_(new epoll_event[max])
	{
		Socket socket(STREAM, 0, Netaddr("127.0.0.1", 10000));
		socket.set_reuseaddr();
		socket.set_nodelay();
		socket.set_keepalive();
		socket.set_quick_close();
		socket.set_delay_accept();
		socket.set_recv_buffer_size(4096);
		socket.set_send_buffer_size(4096);

		bind_socket(socket);
		listen_socket(socket);

		listenfd = socket;

		epollfd_ = epoll_create(max);
		if(epollfd_ = -1)
			return;

		epoll_event e;
		e.events = EPOLLIN;
		e.data.fd = listenfd;

		if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, listenfd, &e) == -1)
			return;
	}

	Epoll_handler::~Epoll_handler()
	{
		if(events_)
			delete[] events_;
	}


	bool Epoll_handler::run_event_loop(int mes)
	{
		int fds = epoll_wait(epollfd_, events_, max_, mes);
		if(fds != 0)
		{
			for(size_t i = 0; i < fds; ++i)
			{
				if(events_[i].data.fd == listenfd)
					handle_accept(events_[i]);
				else
					handle_client(events_[i]);
			}
		}
	}

	void handle_accept(epoll_event ev)
	{
		sockaddr_in client_sin;
		socklen_t sin_size = sizeof(client_sin);

		int client_fd = accept(ev.data.fd, reinterpret_cast<sockaddr *>(&client_sin), &sin_size);
		if(client_fd == -1)
			return false;

		if(!SetNonblocking(client_fd))
			return false;

		Player* player = new Player(client_fd, Netaddr(client_sin), timeout_secs_);
		if(!player)
			return false;

		epoll_event ev;
		ev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
		ev.data.ptr = player;

		if(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &ev) == 1)
		{
			delete client;
			return false;
		}

		clients_[client_fd] = client;
		clients.add_player(client_fd, player);

		return true;
	}

	void handle_client(epoll_event ev)
	{
		Player *player = reinterpret_cast<Player*>(ev.data.ptr);

		if(ev.events & EPOLLIN)
		{
			if(!player->ReadReady())
			{
				RemoveClient(client);
				client->ServerClose();
				delete client;
				return false;
			}
		}

		if(ev.events & EPOLLRDHUP)
		{
			RemoveClient(client);
			client->ClientClose();
			delete client;
			return false;
		}

		if(ev.events & EPOLLOUT)
		{
			if(!client->WriteReady())
			{
				RemoveClient(client);
				client->ServerClose();
				delete client;
				return false;
			}
		}

		return true;
	}

	void Epoll_handler::connect(Dispatcher* dispatcher)
	{
		assert(std::find(dispatchers_.begin(), dispatchers_.end(), dispatcher));

		dispatchers_.push_back(dispatcher);
	}

	void Epoll_handler::disconnect(Dispatcher* dispatcher)
	{
		auto it = std::find(dispatchers_.begin(), dispatchers_.end(), dispatcher)；
		assert(it != dispatchers_.end());

		dispatchers_.erase(it);
	}
}
