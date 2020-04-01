/*
 * Copyright (C) 2018
 */

#ifndef NET_ACCEPTOR_H
#define NET_ACCEPTOR_H

#include "socket.h"
#include "channel.h"

class event_loop;

class acceptor
{
public:
	typedef std::function<void(socket&&)> new_connection_callback;

	acceptor(event_loop* loop, const std::string& bind_ip, uint16_t port);
	~acceptor();

	void set_new_connection_callback(const new_connection_callback& cb);

	bool bind();
	bool is_open() const { return is_open_; }
	
private:
	acceptor(const acceptor&) = delete;
	acceptor& operator=(const acceptor&) = delete;

	int open(std::error_code& ec);
	void close();
	void bind(std::error_code& ec);
	void listen(std::error_code& ec);
	void handle_read();
	std::error_code accept(socket& peer, endpoint& peer_endpoint, std::error_code& ec);

	event_loop* loop_;
	socket accept_socket_;
	channel accept_channel_;
	new_connection_callback new_connection_callback_;

	int idle_fd_;

	bool is_open_;
	tcp::endpoint endpoint_;
};

#endif
