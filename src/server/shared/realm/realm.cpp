/*
 * Copyright (C) 2020
 */

#include <algorithm>
#include <cctype>

#include "realm.h"
#include "address.h"
#include "string_format.h"

void realm::set_name(std::string& name)
{
	name = name;
	normalized_name = name;
	normalized_name.erase(std::remove_if(normalized_name.begin(), normailized_name.end(), ::isspace), normalized_name.end());
}

address realm::get_address_for_client(const address& client_addr) const
{
	address realm_ip;

	if(client_addr.is_loopback())
	{
		if(local_address->is_loopback() || external_address->is_loopback())
			realm_ip = client_addr;
		else
			realm_ip = *local_address;
	}
	else
	{
		if(client_addr.is_v4() && is_in_network(local_address->to_v4(), local_subnet_mask->to_v4(), client_addr.to_v4()))
			realm_ip = *local_address;
		else
			realm_ip = *external_address;
	}

	return realm_ip;
}

uint32_t realm::get_config_id() const { return config_id_by_type[type]; }

const uint32_t realm::config_id_by_type[MAX_CLIENT_REALM_TYPE] = 
{
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14
};

std::string battlenet::realm_handle::get_address_string() const
{
	return string_format("%u-%u-%u", region, site, realm);
}

std::string battlenet::realm_handle::get_sub_region_address() const
{
	return string_format("%u-%u-0", region, site);
}
