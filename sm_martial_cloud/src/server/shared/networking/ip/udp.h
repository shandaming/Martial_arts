/*
 * Copyright (C) 2020
 */

#ifndef NET_UDP_H
#define NET_UDP_H

#include "basic_endpoint.h"

class udp
{
public:
	typedef basic_endpoint<udp> endpoint;

	static udp v4() { return udp(AF_INET); }

	static udp v6() { return udp(AF_INET6);}

	int type() const { return SOCK_DGRAM; }

	int protocol() const { return IPPROTO_UDP; }

	int family() const { return family_; }

	//typedef basic_stream_socket<udp> socket;

	//typedef basic_socket_acceptor<udp> acceptor;


	friend bool operator==(const udp& p1, const udp& p2)
	{
		return p1.family_ == p2.family_;
	}

	friend bool operator!=(const udp& p1, const udp& p2)
	{
		return p1.family_ != p2.family_;
	}
private:
	explicit udp(int protocol_family) : family_(protocol_family) {}

	int family_;
};

#endif
