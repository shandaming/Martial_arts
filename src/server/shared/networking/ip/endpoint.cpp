/*
 * Copyright (C) 2020
 */

#include <sstream>
#include <cstring>

#include "endpoint.h"
#include "socket_operations.h"
#include "network_exception.h"

endpoint::endpoint() : data_()
{
	data_.v4.sin_family = AF_INET;
	data_.v4.sin_port = 0;
	data_.v4.sin_addr.s_addr = host_to_network_long(INADDR_ANY);
}

endpoint::endpoint(int family, uint16_t port_num) : data_()
{
	if(family == (AF_INET))
	{
		data_.v4.sin_family = AF_INET;
		data_.v4.sin_port = host_to_network_short(port_num);
		data_.v4.sin_addr.s_addr = host_to_network_long(INADDR_ANY);
	}
	else
	{
		data_.v6.sin6_family = AF_INET6;
		data_.v6.sin6_port = host_to_network_short(port_num);
		data_.v6.sin6_flowinfo = 0;
		data_.v6.sin6_addr.s6_addr[0] = 0; data_.v6.sin6_addr.s6_addr[1] = 0;
		data_.v6.sin6_addr.s6_addr[2] = 0; data_.v6.sin6_addr.s6_addr[3] = 0;
		data_.v6.sin6_addr.s6_addr[4] = 0; data_.v6.sin6_addr.s6_addr[5] = 0;
		data_.v6.sin6_addr.s6_addr[6] = 0; data_.v6.sin6_addr.s6_addr[7] = 0;
		data_.v6.sin6_addr.s6_addr[8] = 0; data_.v6.sin6_addr.s6_addr[9] = 0;
		data_.v6.sin6_addr.s6_addr[10] = 0; data_.v6.sin6_addr.s6_addr[11] = 0;
		data_.v6.sin6_addr.s6_addr[12] = 0; data_.v6.sin6_addr.s6_addr[13] = 0;
		data_.v6.sin6_addr.s6_addr[14] = 0; data_.v6.sin6_addr.s6_addr[15] = 0;
		data_.v6.sin6_scope_id = 0;
  }
}

endpoint::endpoint(const address& addr, uint16_t port_num) : data_()
{
	if(addr.is_v4())
	{
		data_.v4.sin_family = AF_INET;
		data_.v4.sin_port = host_to_network_short(port_num);
		data_.v4.sin_addr.s_addr = host_to_network_long(addr.to_v4().to_uint());
	}
	else
	{
		data_.v6.sin6_family = AF_INET6;
		data_.v6.sin6_port = host_to_network_short(port_num);
		data_.v6.sin6_flowinfo = 0;

		address_v6 v6_addr = addr.to_v6();
		address_v6::bytes_type bytes = v6_addr.to_bytes();
		memcpy(data_.v6.sin6_addr.s6_addr, bytes.data(), 16);
		data_.v6.sin6_scope_id =
      static_cast<uint32_t>(v6_addr.scope_id());
  }
}

void endpoint::resize(std::size_t new_size)
{
	if(new_size > sizeof(sockaddr_storage_type))
	{
		std::error_code ec(std::errc::invalid_argument);
		networking_exception ex(ec);
		throw ex;
	}
}

uint16_t endpoint::port() const 
{
	if(is_v4())
		return network_to_host_short(data_.v4.sin_port);
	else
		return network_to_host_short(data_.v6.sin6_port);
}

void endpoint::port(uint16_t port_num) 
{
	if(is_v4())
		data_.v4.sin_port = host_to_network_short(port_num);
	else
		data_.v6.sin6_port = host_to_network_short(port_num);
}

address endpoint::address() const 
{
	if(is_v4())
		return address_v4(network_to_host_long(data_.v4.sin_addr.s_addr));
	else
	{
		address_v6::bytes_type bytes;
		memcpy(bytes.data(), data_.v6.sin6_addr.s6_addr, 16);

		return address_v6(bytes, data_.v6.sin6_scope_id);
	}
}

void endpoint::address(const address& addr) 
{
	endpoint tmp_endpoint(addr, port());
	data_ = tmp_endpoint.data_;
}

bool operator==(const endpoint& e1, const endpoint& e2) 
{
	return e1.address() == e2.address() && e1.port() == e2.port();
}

bool operator<(const endpoint& e1, const endpoint& e2) 
{
	if(e1.address() < e2.address())
		return true;
	if(e1.address() != e2.address())
		return false;
	return e1.port() < e2.port();
}

std::string endpoint::to_string() const
{
	std::ostringstream tmp_os;
	tmp_os.imbue(std::locale::classic());
	if(is_v4())
	  tmp_os << address();
	else
		tmp_os << '[' << address() << ']';
	tmp_os << ':' << port();

	return tmp_os.str();
}
