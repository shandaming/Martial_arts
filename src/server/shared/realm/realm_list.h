/*
 * Copyright (C) 2020
 */

#ifndef _REALM_LIST_H
#define _REALM_LIST_H

#include <array>
#include <shared_mutex>
#include <unordered_set>

#include "realm.h"

struct realm_build_info
{
	uint32_t build;
	uint32_t major_version;
	uint32_t minor_version;
	uint32_t bugfix_version;
	std::array<int, 4> hotfix_version;
	std::array<uint8_t, 16> win64_auth_seed;
	std::array<uint8_t, 16> mac64_auth_seed;
};

class realm_list
{
public:
	typedef std::map<battlenet::realm_handle, realm> realm_map;

	static realm_list* instance();

	~realm_list();

	void initialize(event_loop* loop, uint32_t update_interval);
	void close();

	const realm* get_realm(const battlenet::realm_handle& id) const;

	const realm_build_info* get_build_info(uint32_t build) const;
	uint32_t get_minor_major_bugfix_version_for_build(uint32_t build) const;
	//void write_sub_regions()
	std::vector<uint8_t> get_realm_entry_json(const battlenet::realm_handle& id, uint32_t build) const;
	std::vector<uint8_t> get_realm_list(uint32_t build, const std::string& sub_region) const;
	//uint32_t join_realm(uint32_t realm_address, uint32_t build, const address& client_address, const std::array<uint8_t, 32>& client_secret, locale_constant locale, const std::string& os, std::string& account_name, )
private:
	realm_list();

	void load_build_info();
	void update_realms(const std::error_code& ec);
	void update_realm(realm& realm, const battlenet::realm_handle& id, 
			uint32_t build, const std::string& name, address&& addr, 
			address&& local_addr, address&& local_submask, uint16_t port, 
			uint8_t icon, realm_flags flag, uint8_t timezone, 
			account_types allowed_security_level, float population);

	std::vector<realm_build_info> builds_;
	std::shared_mutex realms_mutex_;
	realm_map realms_;
	std::unordered_set<std::string> sub_regions_;
	uint32_t update_interval_;
	//
	//
};

#define REALM_LIST realm_list::instance()

#endif
