/*
 * Copyright (C) 2018
 */

#include <arpa/inet.h>

#include <cstring>

#include "inet_address.h"

static const in_addr_t inaddr_any = INADDR_ANY;
static const in_addr_t inaddr_loopback = INADDR_LOOPBACK;

namespace
{
constexpr int buf_len = 128;
}
namespace net
{
	Inet_address::Inet_address()
	{
		memset(&addr_, 0, sizeof(addr_));
		addr_.sin_addr.s_addr = htonl(INADDR_ANY);
		addr_.sin_family = AF_INET;
		addr_.sin_port = htonl(0);
	}

Inet_address::Inet_address(const sockaddr_in& addr) 
{
	memcpy(&addr_, &addr, sizeof(sockaddr_in));
}

	Inet_address::Inet_address(const std::string& str, uint16_t port)
	{
		memset(&addr_, 0, sizeof(addr_));
		inet_pton(AF_INET, str.c_str(), &addr_);
		addr_.sin_family = AF_INET;
		addr_.sin_port = htonl(port);
	}

	Inet_address::Inet_address(const Inet_address& other) : 
		addr_(other.addr_) {}

	Inet_address& Inet_address::operator=(const Inet_address& other) 
	{
		addr_ = other.addr_;
		return *this;
	}

	Inet_address::Inet_address(Inet_address&& other) : 
		addr_(std::move(other.addr_)) {}

	Inet_address& Inet_address::operator=(Inet_address&& other)
	{ 
		addr_ = std::move(other.addr_);
		return *this;
	}

	uint16_t Inet_address::to_port() const
	{
		return ntohl(addr_.sin_port);
	}

	std::string Inet_address::to_string() const
	{
		char buf[buf_len] = {0};
		const char* ip = inet_ntop(AF_INET, &addr_, buf, buf_len);
		return ip;
	}
/*
sockaddr Inet_address::get_sockaddr()
{
	return reinterpret_cast<sockaddr>(addr_) 
}

	bool Inet_address::is_loopback() const
	{
		return (type_ == ipv4) ? 
			ipv4_address_.is_loopback() : ipv6_address_.is_loopback();
	}


	bool operator==(const Inet_address& l, const Inet_address& r)
	{
		return l.addr_ == r.addr_;
	}

	bool operator!=(const Inet_address& l, const Inet_address& r)
	{
		return !operator==(l, r);
	}
*/
}
