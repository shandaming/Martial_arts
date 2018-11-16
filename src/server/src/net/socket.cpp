/*
 * Copyright (C) 2018
 */

#include "socket.h"
#include "net_utils.h"

namespace net
{
Socket::~Socket()
{
	close_sockfd(fd_);
}

bool Socket::bind(const Inet_address& addr)
{
	return bind_socket(fd_, addr.get_addr());
}

bool Socket::listen()
{
	return listen_socket(fd_);
}

int Socket::accept(Inet_address& addr)
{
	return accept_net_connection(fd_, addr);
}

void Socket::shutdown_write()
{
	shutdown_write(fd_);
}

void Socket::set_reuse_addr()
{
	int optval;
	setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::set_reuse_port()
{
#ifdef SO_REUSEPORT
	int optval;
	if(setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval) ==
				 -1))
	{
		LOG_SYSERR << "SO_REUSEPORT failed.";
	}
#else
    LOG_ERROR << "SO_REUSEPORT is not supported.";
#endif
}

void Socket::set_tcp_no_delay()
{
	int optval;
	setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void Socket::set_keep_alive()
{
	int optval;
	setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}
} // end namespace
