/*
 * Copyright (C) 2020
 */

#ifndef _IP_LOCATION_H
#define _IP_LOCATION_H

#include <string>
#include <vector>
#include <cstdint>

struct ip_location_record
{
	ip_location_record() : ip_from(0), ip_to(0) {}
	ip_location_record(uint32_t ip_from_, uint32_t ip_to_, const std::string& country_code_, const std::string& country_name_) :
		ip_from(ip_from_), ip_to(ip_to_), country_code(country_code_), country_name(country_name_) {}

	uint32_t ip_from;
	uint32_t ip_to;
	std::string country_code;
	std::string country_name;
};

class ip_location_store
{
public:
	ip_location_store();
	~ip_location_store();

	static ip_location_store* instance();

	void load();
	const ip_location_record* get_location_record(const std::string& ip_address) const;
private:
	std::vector<ip_location_record> ip_location_store_;
};

#define IP_LOCATION ip_location_store::instance()

#endif
