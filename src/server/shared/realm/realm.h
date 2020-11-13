/*
 * Copyright (C) 2020
 */

#ifndef _REALM_H
#define _REALM_H

#include <cstdint>
#include <memory>

#include "address.h"
#include "common.h"

enum realm_flags
{
	REALM_FLAG_NONE				 = 0x00,
	REALM_FLAG_VERSION_MISMATCH	 = 0x01,
	REALM_FLAG_OFFLINE			 = 0x02,
	REALM_FLAG_SPECIFYBUILD		 = 0x04,
	REALM_FLAG_UNK1				 = 0x08,
	REALM_FLAG_UNK2				 = 0x10,
	REALM_FLAG_RECOMMENDED		 = 0x20,
	REALM_FLAG_NEW				 = 0x40,
	REALM_FLAG_FULL				 = 0x80
};

namespace battlenet
{
struct realm_handle
{
	realm_handle(): region(0), site(0), realm(0) {}
	realm_handle(uint8_t region, uint8_t battlegroup, uint32_t index) :
		region(region), site(battlegroup), realm(index) {}
	realm_handle(uint32_t realm_address) : region((realm_address >> 24) & 0xFF), site((realm_address >> 16) & 0xFF), realm(realm_address & 0xFFFF) {}

	bool operator<(const realm_handle& r) const { return realm < r.realm; }

	uint32_t get_address() const { return (region << 24) | (site << 16) | uint16_t(realm); }
	std::string get_address_string() const;
	std::string get_sub_region_address() const;

	uint8_t region;
	uint8_t site;
	uint32_t realm;
};
}

enum realm_type
{
	REALM_TYPE_NORMAL =		0,
	REALM_TYPE_PVP =		1,
	REALM_TYPE_NORMAL2 =	4,
	REALM_TYPE_RP =			6,
	REALM_TYPE_RPPVP =		8,
	MAX_CLIENT_REALM_TYPE = 14,
	REALM_TYPE_FFA_PVP =	16,
};

struct realm
{
	void set_name(std::string& name);
	address get_address_fro_client(const address& client_addr) const;
	uint32_t get_config_id() const;

	static const uint32_t config_id_by_type[MAX_CLIENT_REALM_TYPE];

	battlenet::realm_handle id;
	uint32_t build;
	std::unique_ptr<address> external_address;
	std::unique_ptr<address> local_address;
	std::unique_ptr<address> local_subnet_mask;
	uint16_t port;
	std::string name;
	std::string normalized_name;
	uint8_t type;
	realm_flags flags;
	uint8_t timezone;
	account_types allowed_security_level;
	float population_level;
};

#endif
