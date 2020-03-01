/*
 * Copyright (C) 2020/
 */

#ifndef NET_TCP
#define NET_TCP

#include "basic_endpoint.h"

class tcp
{
public:
	typedef basic_endpoint<tcp> endpoint;

	static tcp v4() { return tcp(AF_INET); }
	static tcp v6() { return tcp(AF_INET6);}

	int type() const { return SOCK_STREAM; }

	int protocol() const { return IPPROTO_TCP; }
	int family() const { return family_; }

	//typedef basic_stream_socket<tcp> socket;
	//typedef basic_socket_acceptor<tcp> acceptor;

	friend bool operator==(const tcp& p1, const tcp& p2)
	{
		return p1.family_ == p2.family_;
	}

	friend bool operator!=(const tcp& p1, const tcp& p2)
	{
		return p1.family_ != p2.family_;
	}
private:
	explicit tcp(int protocol_family) : family_(protocol_family) {}

	int family_;
};

#endif
