/*
 * Copyright (C) 2020
 */

#include "address.h"
#include "network_exception.h"

address::address() : type_(ipv4), ipv4_address_(), ipv6_address_() {}

address::address(const address_v4& ipv4_address) : 
	type_(ipv4),
	ipv4_address_(ipv4_address),
	ipv6_address_() {}

address::address(const address_v6& ipv6_address) :
	type_(ipv6),
	ipv4_address_(),
	ipv6_address_(ipv6_address) {}

address::address(const address& other) :
	type_(other.type_),
	ipv4_address_(other.ipv4_address_),
	ipv6_address_(other.ipv6_address_) {}

address::address(address&& other) :
	type_(other.type_),
	ipv4_address_(other.ipv4_address_),
	ipv6_address_(other.ipv6_address_) {}

address& address::operator=(const address& other) 
{
	type_ = other.type_;
	ipv4_address_ = other.ipv4_address_;
	ipv6_address_ = other.ipv6_address_;
	return *this;
}

address& address::operator=(address&& other) 
{
	type_ = other.type_;
	ipv4_address_ = other.ipv4_address_;
	ipv6_address_ = other.ipv6_address_;
	return *this;
}

address& address::operator=(const address_v4& ipv4_address) 
{
	type_ = ipv4;
	ipv4_address_ = ipv4_address;
	ipv6_address_ = address_v6();
	return *this;
}

address& address::operator=(const address_v6& ipv6_address) 
{
	type_ = ipv6;
	ipv4_address_ = address_v4();
	ipv6_address_ = ipv6_address;
	return *this;
}

address make_address(const char* str)
{
	std::error_code ec;
	address addr = make_address(str, ec);
	if(ec)
	{
		networking_exception ex(ec);
		throw ec;
	}
	return addr;
}

address make_address(const char* str, std::error_code& ec) 
{
	address_v6 ipv6_address = make_address_v6(str, ec);
	if(!ec)
		return address(ipv6_address);

	address_v4 ipv4_address = make_address_v4(str, ec);
	if(!ec)
		return address(ipv4_address);

	return address();
}

address make_address(const std::string& str)
{
	return make_address(str.c_str());
}

address make_address(const std::string& str, std::error_code& ec) 
{
	return make_address(str.c_str(), ec);
}

address_v4 address::to_v4() const
{
	if(type_ != ipv4)
	{
		networking_exception ex("Bad IPV4 address cast.");
		throw ex;
	}
	return ipv4_address_;
}

address_v6 address::to_v6() const
{
	if(type_ != ipv6)
	{
		networking_exception ex("Bad IPV6 address cast.");
		throw ex;
	}
	return ipv6_address_;
}

std::string address::to_string() const
{
	if(type_ == ipv6)
		return ipv6_address_.to_string();
	return ipv4_address_.to_string();
}

bool address::is_loopback() const 
{
	return (type_ == ipv4) ? 
		ipv4_address_.is_loopback() :
		ipv6_address_.is_loopback();
}

bool address::is_unspecified() const 
{
	return (type_ == ipv4) ?
		ipv4_address_.is_unspecified() :
		ipv6_address_.is_unspecified();
}

bool address::is_multicast() const 
{
	return (type_ == ipv4) ?
		ipv4_address_.is_multicast() :
		ipv6_address_.is_multicast();
}

bool operator==(const address& a1, const address& a2) 
{
	if(a1.type_ != a2.type_)
		return false;
	if(a1.type_ == address::ipv6)
		return a1.ipv6_address_ == a2.ipv6_address_;
	return a1.ipv4_address_ == a2.ipv4_address_;
}

bool operator<(const address& a1, const address& a2) 
{
	if(a1.type_ < a2.type_)
		return true;
	if(a1.type_ > a2.type_)
		return false;
	if(a1.type_ == address::ipv6)
		return a1.ipv6_address_ < a2.ipv6_address_;
	return a1.ipv4_address_ < a2.ipv4_address_;
}
