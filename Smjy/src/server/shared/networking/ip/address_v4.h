/*
 * Copyright (C) 2020
 */

#ifndef IP_ADDRESS_V4_H
#define IP_ADDRESS_V4_H

#include <array>
#include <system_error>

#include "socket_types.h"

class address_v4
{
public:
	typedef uint32_t uint_type;
	typedef std::array<unsigned char, 4> bytes_type;

	address_v4() { addr_.s_addr = 0; }
	explicit address_v4(const bytes_type& bytes);
	explicit address_v4(uint_type addr);

	address_v4(const address_v4& other) : addr_(other.addr_) {}
	address_v4(address_v4&& other) : addr_(other.addr_) {}

	address_v4& operator=(const address_v4& other) 
	{
		addr_ = other.addr_;
		return *this;
	}

	address_v4& operator=(address_v4&& other) 
	{
		addr_ = other.addr_;
		return *this;
	}

	bytes_type to_bytes() const ;
	uint_type to_uint() const ;

	std::string to_string() const;

	bool is_loopback() const ;
	bool is_unspecified() const ;
	bool is_multicast() const ;

	friend bool operator==(const address_v4& a1, const address_v4& a2) 
	{
		return a1.addr_.s_addr == a2.addr_.s_addr;
	}

	friend bool operator!=(const address_v4& a1, const address_v4& a2) 
	{
		return a1.addr_.s_addr != a2.addr_.s_addr;
	}

	friend bool operator<(const address_v4& a1, const address_v4& a2) 
	{
		return a1.to_uint() < a2.to_uint();
	}

	friend bool operator>(const address_v4& a1, const address_v4& a2) 
	{
		return a1.to_uint() > a2.to_uint();
	}

	friend bool operator<=(const address_v4& a1, const address_v4& a2) 
	{
		return a1.to_uint() <= a2.to_uint();
	}

	friend bool operator>=(const address_v4& a1, const address_v4& a2) 
	{
		return a1.to_uint() >= a2.to_uint();
	}

	static address_v4 any() { return address_v4(); }
	static address_v4 loopback() { return address_v4(0x7F000001); }
	static address_v4 broadcast() { return address_v4(0xFFFFFFFF); }
private:
	in4_addr_type addr_;
};

inline address_v4 make_address_v4(const address_v4::bytes_type& bytes)
{
	return address_v4(bytes);
}

inline address_v4 make_address_v4(address_v4::uint_type addr)
{
	return address_v4(addr);
}

address_v4 make_address_v4(const char* str);
address_v4 make_address_v4(const char* str, std::error_code& ec) ;

address_v4 make_address_v4(const std::string& str);
address_v4 make_address_v4(const std::string& str, std::error_code& ec) ;

template <typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>& operator<<(
    std::basic_ostream<Elem, Traits>& os, const address_v4& addr);

#endif
