/*
 * Copyright (C) 2018
 */

#ifndef SOCKET_H
#define SOCKET_H

#include "inet_address.h"

namespace net
{
class Socket
{
public:
	explicit Socket(int sockfd) : fd_(sockfd) {}

	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;

	~Socket();

	bool bind(const Inet_address& addr);
	bool listen();
	int accept(const Inet_address& addr);

	void shutdown_write();

	void set_reuse_addr();
	void set_reuse_port();

	void set_tcp_no_delay();
	void set_keep_alive();

	int get_socketfd() const { return fd_; }

			//Inet_address get_netaddr() const { return addr_; }	
			//std::string get_ip() const { return addr_.get_ip(); }
			//int get_port() const { return addr_.get_port(); }
	operator int() { return fd_; }
private:
	const int fd_;
};
}

#endif
