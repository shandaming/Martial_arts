/*
 * Copyright (C) 2020
 */

#include <cstring>

#include "address_v4.h"
#include "socket_operations.h"
#include "network_exception.h"

address_v4::address_v4(const address_v4::bytes_type& bytes)
{
	memcpy(&addr_.s_addr, bytes.data(), 4);
}

address_v4::address_v4(address_v4::uint_type addr)
{
	static_assert(std::numeric_limits<uint_type>::max > 0xFFFFFFFF,
			"address_v4 from unsigned integer");
	/*
	if((std::numeric_limits<uint_type>::max)() > 0xFFFFFFFF)
	{
		std::out_of_range ex("address_v4 from unsigned integer");
		throw ex;
	}
	*/

	addr_.s_addr = host_to_network_long(addr);
}

address_v4::bytes_type address_v4::to_bytes() const 
{
	bytes_type bytes;
	memcpy(bytes.data(), &addr_.s_addr, 4);
	return bytes;
}

address_v4::uint_type address_v4::to_uint() const 
{
	return network_to_host_long(addr_.s_addr);
}


std::string address_v4::to_string() const
{
	std::error_code ec;
	char addr_str[max_addr_v4_str_len];
	const char* addr = inet_ntop(AF_INET, &addr_, addr_str, max_addr_v4_str_len, 0, ec);
	if(addr == 0)
	{
		networking_exception ex(ec);
		throw ex;
	}
	return addr;
}


bool address_v4::is_loopback() const 
{
	return (to_uint() & 0xFF000000) == 0x7F000000;
}

bool address_v4::is_unspecified() const 
{
	return to_uint() == 0;
}


bool address_v4::is_multicast() const 
{
	return (to_uint() & 0xF0000000) == 0xE0000000;
}


address_v4 make_address_v4(const char* str)
{
	std::error_code ec;
	address_v4 addr = make_address_v4(str, ec);
	if(ec)
	{
		networking_exception ex(ec);
		throw ex;
	}
	return addr;
}

address_v4 make_address_v4(const char* str, std::error_code& ec) 
{
	address_v4::bytes_type bytes;
	if(inet_pton(AF_INET, str, &bytes, 0, ec) <= 0)
		return address_v4();
	return address_v4(bytes);
}

address_v4 make_address_v4(const std::string& str)
{
	return make_address_v4(str.c_str());
}

address_v4 make_address_v4(const std::string& str, std::error_code& ec) 
{
	return make_address_v4(str.c_str(), ec);
}
