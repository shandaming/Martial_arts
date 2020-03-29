/*
 * Copyright (C) 2020
 */

#ifndef IP_ENDPOINT_H
#define IP_ENDPOINT_H

#include "addres.h"

class endpoint
{
public:
	endpoint() ;
	endpoint(int family, uint16_t port_num) ;
	endpoint(const address& addr, uint16_t port_num) ;
	endpoint(const endpoint& other) : data_(other.data_) {}

	endpoint& operator=(const endpoint& other) 
	{
		data_ = other.data_;
		return *this;
	}

	socket_addr_type* data()  { return &data_.base; }

	const socket_addr_type* data() const { return &data_.base; }

	std::size_t size() const 
	{
		if (is_v4())
			return sizeof(sockaddr_in4_type);
		else
			return sizeof(sockaddr_in6_type);
	}

	void resize(std::size_t new_size);

	std::size_t capacity() const { return sizeof(data_); }

	uint16_t port() const ;

	void port(uint16_t port_num) ;

	address address() const ;

	void address(const address& addr) ;

	friend bool operator==(const endpoint& e1, const endpoint& e2) ;
	friend bool operator<(const endpoint& e1, const endpoint& e2) ;

	bool is_v4() const 
	{
		return data_.base.sa_family == (AF_INET);
	}

	std::string to_string() const;
private:
	union data_union
	{
		socket_addr_type base;
		sockaddr_in4_type v4;
		sockaddr_in6_type v6;
	} data_;
};

#endif
