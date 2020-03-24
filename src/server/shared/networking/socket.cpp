/*
 * Copyright (C) 2018
 */

#include <netinet/tcp.h>

#include "socket.h"
#include "socket_type.h"
#include "socket_operations.h"
#include "logging/log.h"

socket::~socket()
{
	close();
}

void socket::open(int sockfd)
{
	if(is_open())
		close();

	fd_ = sockfd;
}

void socket::shutdown_write()
{
	if(!is_open())
		return;

	std::error_code ec;
	shutdown(fd_, SHUT_WR, ec);
	if(ec)
		LOG_ERROR("Networking", "shutdown_write() failed. code[%d], message:%s", ec.code(), ec.message().c_str());
}

void socket::shutdown(shutdown_type type, std::error_code& ec)
{
	shutdown(fd_, type, ec);	
}

void socket::close()
{
	if(!is_open())
		return;

	std::error_code ec;
	close(fd_, ec);
	if(ec)
		LOG_ERROR("Networking", "close() failed. code[%d], message:%s", ec.code(), ec.message().c_str());

	fd_ = -1;
}

bool socket::set_reuse_addr()
{
	std::error_code ec;
	int optval;
	setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval), ec);
	if(ec)
	{
		LOG_ERROR("Networking", "set_reuse_addr() failed. code[%d], message:%s", ec.code(), ec.message().c_str());
		return false;
	}
	return true;
}

bool socket::set_reuse_port()
{
	std::error_code ec;
#ifdef SO_REUSEPORT
	int optval;
	setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval), ec);
	if(ec)
	{
		LOG_ERROR("Networking", "set_reuse_port() failed. code[%d], message:%s", ec.code(), ec.message().c_str());
		return false;
	}
#endif
	return true;
}

bool socket::set_tcp_no_delay()
{
	std::error_code ec;
	int optval;
	setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval), ec);
	if(ec)
	{
		LOG_ERROR("Networking","set_tcp_no_delay() failed. code[%d], message:%s", ec.code(), ec.message(.c_str()));
		return false;
	}
	return true;
}

bool socket::set_keep_alive()
{
	std::error_code ec
	int optval;
	setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval), ec);
	if(ec)
	{
		LOG_ERROR("networking", "set_keep_alive() failed. code[%d], message:%s", ec.code(), ec.message().c_str());
		return false;
	}
	return true;
}

bool socket::set_send_buffer_size(int size)
{
	std::error_code ec;
	setsockopt(fd_, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size), ec);
	if(ec)
	{
		LOG_ERROR("network", "set_send_buffer_size() failed. error %d: %d", ec.value(), ec.message().c_str());
		return false;
	}
	return true;
}

bool socket::set_option(const socket_option& option)
{
	std::error_code ec;
	int level = option.level;
	int optname = option.optname;
	int optval = option.optval;
	const std::string description = option.description;

	setsockopt(fd_, level, optname, (const char*)&optval, sizeof(optval), ec);
	if(ec)
	{
		LOG_ERROR("network", "set %s failed. error %d: %d", description.c_str(), ec.value(), ec.message().c_str());
		return false;
	}
	return true;
}


endpoint socket::local_endpoint(std::error_code& ec)
{
	endpoint e;
	socklen_t addrlen = e.caparity();
	if(getsockname(fd_, e.data(), &addrlen, ec))
		return endpoint();
	e.resize(addrlen);
	return e;
}

endpoint socket::remote_endpoint(std::error_code& ec)
{
	endpoint e;
	socklen_t addrlen = e.capacity();
	if(getpeername(fd_, e.data(), &addrlen, ec))
		return endpoint();
	e.resize(addrlen);
	return e;
}

bool socket::is_open() const { return fd_ != invalid_socket; }
