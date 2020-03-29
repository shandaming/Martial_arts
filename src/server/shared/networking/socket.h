/*
 * Copyright (C) 2018
 */

#ifndef NET_SOCKET_H
#define NET_SOCKET_H

#include "ip/endpoint.h"

struct socket_option
{
	option(int level_, int optname_, int optval_, const std::string& desc) :
		level(level_), optname(optname_), optval(optval_), description(desc) {}

	int level;
	int optname;
	int optval;
	std::string ;description;
};

struct option
{
	static socket_option tcp_no_delay() 
	{
		return socket_option(IPPROTO_TCP, TCP_NODELAY, 1, "tcp no delay");
	}

	static socket_option reuse_addr() 
	{
		return socket_option(SOL_SOCKET, SO_REUSEADDR, 1, "socket reuse address");
	}

	static socket_option reuse_port() 
	{
		return socket_option(SOL_SOCKET, SO_REUSEPORT, 1, "socket reuse port");
	}

	static socket_option keep_alive() 
	{
		return socket_option(SOL_SOCKET, SO_KEEPALIVE, 1, "socket keep alive");
	}

	static socket_option send_buffer_size(int size) 
	{
		return socket_option(SOL_SOCKET, SO_SNDBUF, size, "socket send buffer");
	}
};

class socket
{
public:
	socket() : fd_(-1) {}
	explicit socket(int sockfd) : fd_(-1) { open(sockfd); }

	socket(const socket& right) : fd_(right.fd_) {}
	socket& operator=(const socket& right) 
	{
		fd_ = right.fd_;
	}

	socket(socket&& right) : fd_(right.fd_)
	{
		right.fd_ = -1;
	}

	socket& operator=(socket&& right)
	{
		fd_ = right.fd_;
		right.fd_ = -1;
	}

	~socket();
	
	enum shutdown_type
	{
		shutdown_receive = SHUT_RD,
		shutdown_send = SHUT_WR,
		shutdown_both = SHUT_RDWR
	};

	void open(int sockfd);
	void shutdown_write();
	void shutdown(shutdown_type type, std::error_code& ec);
	void close();

	bool set_reuse_addr();
	bool set_reuse_port();

	bool set_tcp_no_delay();
	bool set_keep_alive();
	bool set_send_buffer_size();

	bool set_option(const socket_option& option);

	endpoint local_endpoint(std::error_code& ec);
	endpoint remote_endpoint(std::error_code& ec);

	//int get_socketfd() const { return fd_; }

	operator int() { return fd_; }

	bool is_open() const;
private:
	const int fd_;
};

#endif
