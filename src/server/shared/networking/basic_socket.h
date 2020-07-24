/*
 * Copyright (C) 2018
 */

#ifndef NET_BASIC_SOCKET_H
#define NET_BASIC_SOCKET_H

#include "endpoint.h"

struct socket_option
{
	socket_option(int level_, int optname_, int optval_, const std::string& desc) :
		level(level_), optname(optname_), optval(optval_), description(desc) {}

	int level;
	int optname;
	int optval;
	std::string description;
};

struct option
{
	static socket_option tcp_no_delay();
	static socket_option reuse_address();
	static socket_option reuse_port();
	static socket_option keep_alive();
	static socket_option send_buffer_size(int size);
};

template<typename Protocol>
class basic_socket
{
public:
	basic_socket() : fd_(-1) {}
	explicit basic_socket(int sockfd) : fd_(-1) { open(sockfd); }

	basic_socket(const basic_socket& right) : fd_(right.fd_) {}
	basic_socket& operator=(const basic_socket& right) 
	{
		fd_ = right.fd_;
		return *this;
	}

	basic_socket(basic_socket&& right) : fd_(right.fd_)
	{
		right.fd_ = -1;
	}

	basic_socket& operator=(basic_socket&& right)
	{
		fd_ = right.fd_;
		right.fd_ = -1;
		return *this;
	}

	~basic_socket();
	
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

	bool is_open() const { return fd_ != invalid_socket; }
private:
	void close_socket();

	int fd_;
};

#endif
