/*
 * Copyright (C) 2018
 */

#ifndef NET_ACCEPTOR_H
#define NET_ACCEPTOR_H

#include "socket.h"
#include "event_loop.h"

class acceptor
{
public:
	typedef std::function<void (int sockfd, const Inet_address&)> new_connection_callback;

	acceptor(event_loop* loop, const std::string& bind_ip, uint16_t port);
	~acceptor();

	void set_new_connection_callback(const new_connection_callback& cb) 
	{
		new_connection_callback_ = cb; 
	}

	bool listenning() const { return listenning_; }
	void listen();
private:
	void handle_read();

	event_loop* loop_;
	socket accept_socket_;
	channel accept_channel_;
	new_connection_callback new_connection_callback_;
	bool listenning_;
	int idle_fd_;
};

#endif
