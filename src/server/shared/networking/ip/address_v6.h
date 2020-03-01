/*
 * Copyright (C) 2020
 */

#ifndef IP_ADDRESS_V6_H
#define IP_ADDRESS_V6_H

#include <array>
#include <system_error>

#include "networking/socket_types.h"

class address_v6
{
public:
	typedef std::array<uint8_t, 16> bytes_type;

	address_v6();
	explicit address_v6(const bytes_type& bytes, uint64_t scope_id = 0);
	address_v6(const address_v6& other) ;
	address_v6(address_v6&& other) ;

	address_v6& operator=(const address_v6& other) ;
	address_v6& operator=(address_v6&& other) ;

	uint64_t scope_id() const { return scope_id_; }
	void scope_id(uint64_t id) { scope_id_ = id; }

	bytes_type to_bytes() const ;
	std::string to_string() const;


	bool is_loopback() const ;
	bool is_unspecified() const ;
	bool is_link_local() const ;
	bool is_site_local() const ;
	bool is_v4_mapped() const ;
	bool is_multicast() const ;
	bool is_multicast_global() const ;
	bool is_multicast_link_local() const ;
	bool is_multicast_node_local() const ;
	bool is_multicast_org_local() const ;
	bool is_multicast_site_local() const ;

	friend bool operator==(const address_v6& a1, const address_v6& a2) ;
	
	friend bool operator!=(const address_v6& a1, const address_v6& a2) 
	{
		return !(a1 == a2);
	}

	friend bool operator<(const address_v6& a1, const address_v6& a2) ;

	friend bool operator>(const address_v6& a1, const address_v6& a2) 
	{
		return a2 < a1;
	}


	friend bool operator<=(const address_v6& a1, const address_v6& a2) 
	{
		return !(a2 < a1);
	}

	friend bool operator>=(const address_v6& a1, const address_v6& a2) 
	{
		return !(a1 < a2);
	}

	static address_v6 any() { return address_v6(); }
	static address_v6 loopback() ;
private:
	friend class basic_address_iterator<address_v6>;

	in6_addr_type addr_;
	uint64_t scope_id_;
};

inline address_v6 make_address_v6(const address_v6::bytes_type& bytes,
    uint64_t scope_id = 0)
{
	return address_v6(bytes, scope_id);
}

address_v6 make_address_v6(const char* str);
address_v6 make_address_v6(const char* str, std::error_code& ec) ;

address_v6 make_address_v6(const std::string& str);
address_v6 make_address_v6(const std::string& str, std::error_code& ec) ;


enum v4_mapped_t { v4_mapped };

address_v4 make_address_v4(v4_mapped_t, const address_v6& v6_addr);
address_v6 make_address_v6(v4_mapped_t, const address_v4& v4_addr);

#endif
