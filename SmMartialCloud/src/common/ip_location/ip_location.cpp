/*
 * Copyright (C) 2020
 */

#include <fstream>
#include <cstdlib>

#include "config.h"
#include "errors.h"
#include "log.h"
#include "ip_location.h"
ip_location_store::ip_location_store() {}

ip_location_store::~ip_location_store() {}

void ip_location_store::load()
{
	ip_location_store_.clear();
	LOG_INFO("server.loading", "Loading IP Location Database...");

	std::string database_file_path = CONFIG_MGR->get_value_default("IP", "ip_location_file", "");
	if(database_file_path.empty())
		return;

	std::ifstream database_file(database_file_path);
	if(!database_file)
	{
		LOG_ERROR("server.loading", "ip_location: No ip database file exists (%s)", database_file_path.c_str());
		return;
	}

	if(!database_file.is_open())
	{
		LOG_ERROR("server.loading", "ip_location: Ip database file (%s) can not be opened.", database_file_path.c_str());
		return;
	}

	std::string ip_from;
	std::string ip_to;
	std::string country_code;
	std::string country_name;

	while(database_file.good())
	{
		if(!std::getline(database_file, ip_from, ','))
			break;
		if(!std::getline(database_file, ip_to, ','))
			break;
		if(!std::getline(database_file, country_code, ','))
			break;
		if(!std::getline(database_file, country_name, '\n'))
			break;

		country_name.erase(std::remove(country_name.begin(), country_name.end(), '\r'), country_name.end());
		country_name.erase(std::remove(country_name.begin(), country_name.end(), '\n'), country_name.end());

		ip_from.erase(std::remove(ip_from.begin(), ip_from.end(), '"'), ip_from.end());
		ip_to.erase(std::remove(ip_to.begin(), ip_to.end(), '"'), ip_to.end());
		country_code.erase(std::remove(country_code.begin(), country_code.end(), '"'), country_code.end());
		country_name.erase(std::remove(country_name.begin(), country_name.end(), '"'), country_name.end());

		std::transform(country_code.begin(), country_code.end(), country_code.begin(), ::tolower);

		ip_location_store_.emplace_back(uint32_t(std::stoul(ip_from.c_str())), uint32_t(std::stoul(ip_to.c_str())), std::move(country_code), std::move(country_name));
	}

	std::sort(ip_location_store_.begin(), ip_location_store_.end(), [](const ip_location_record& a, const ip_location_record& b) { return a.ip_from < b.ip_from; });
	ASSERT(std::is_sorted(ip_location_store_.begin(), ip_location_store_.end(), [](const ip_location_record& a, const ip_location_record& b) { return a.ip_from < b.ip_from; }), "Overlapping IP ranges detected in database file");

	database_file.close();

	LOG_INFO("server.loading", ">> loaded %u ip location entries.", ip_location_store_.size());
}

const ip_location_record* ip_location_store::get_location_record(const std::string& ip_address) const
{
	uint32_t ip = 0; //
	auto iter = std::upper_bound(ip_location_store_.begin(), ip_location_store_.end(), ip, [](uint32_t ip, const ip_location_record& loc) { return ip < loc.ip_to; });
	if(iter == ip_location_store_.end())
		return nullptr;

	if(ip < iter->ip_from)
		return nullptr;

	return &(*iter);
}

ip_location_store* ip_location_store::instance()
{
	static ip_location_store instance;
	return &instance;
}
