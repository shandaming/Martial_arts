/*
 * Copyright (C) 2020
 */

#ifndef IP_ADDRESS_H
#define IP_ADDRESS_H

#include "address_v4.h"
#include "address_v6.h"

class address
{
public:
	address() ;
	address(const address_v4& ipv4_address) ;
	address(const address_v6& ipv6_address) ;
	address(const address& other) ;
	address(address&& other) ;

	address& operator=(const address& other) ;
	address& operator=(address&& other) ;
	address& operator=(const address_v4& ipv4_address) ;
	address& operator=(const address_v6& ipv6_address) ;

	bool is_v4() const { return type_ == ipv4; }
	bool is_v6() const { return type_ == ipv6; }

	address_v4 to_v4() const;
	address_v6 to_v6() const;

	std::string to_string() const;

	bool is_loopback() const ;
	bool is_unspecified() const ;
	bool is_multicast() const ;

	friend bool operator==(const address& a1, const address& a2) ;
	friend bool operator!=(const address& a1, const address& a2) 
	{
		return !(a1 == a2);
	}

	friend bool operator<(const address& a1, const address& a2) ;
	friend bool operator>(const address& a1, const address& a2) 
	{ 
		return a2 < a1;
	}

	friend bool operator<=(const address& a1, const address& a2) 
	{
		return !(a2 < a1);
	}

	friend bool operator>=(const address& a1, const address& a2) 
	{
		return !(a1 < a2);
	}
private:
	enum { ipv4, ipv6 } type_;

	address_v4 ipv4_address_;
	address_v6 ipv6_address_;
};

address make_address(const char* str);
address make_address(const char* str, std::error_code& ec) ;

address make_address(const std::string& str);
address make_address(const std::string& str, std::error_code& ec) ;

#endif
