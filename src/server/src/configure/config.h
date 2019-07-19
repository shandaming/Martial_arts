/*
 * Copyright (C) 2019
 */

#ifndef INI_CONFIG_H
#define INI_CONFIG_H

#include <algorithm>

#include "ini_parser.h"

namespace tlp
{
template<typename T>
struct conversion
{
	T operator()(const std::string& str) { return str; }
};
/*
template<>
struct conversion<const char*>
{
	const char* operator()(const std::string& str) { return str; }
};
*/

template<>
struct conversion<int8_t>
{
	int8_t operator()(const std::string& str) { return atoi(str.c_str()); }
};

template<>
struct conversion<int16_t>
{
	int16_t operator()(const std::string& str) { return atoi(str.c_str()); }
};

template<>
struct conversion<int32_t>
{
	int32_t operator()(const std::string& str) { return atol(str.c_str()); }
};

template<>
struct conversion<int64_t>
{
	int64_t operator()(const std::string& str) { return atoll(str.c_str()); }
};

template<>
struct conversion<uint8_t>
{
	uint8_t operator()(const std::string& str) { return strtoul(str.c_str(), 0, 10); }
};

template<>
struct conversion<uint16_t>
{
	uint16_t operator()(const std::string& str) { return strtoul(str.c_str(), 0, 10); }
};

template<>
struct conversion<uint32_t>
{
	uint32_t operator()(const std::string& str) { return strtoul(str.c_str(), 0, 10); }
};

template<>
struct conversion<uint64_t>
{
	uint64_t operator()(const std::string& str) { return strtoull(str.c_str(), 0, 10); }
};

template<>
struct conversion<float>
{
	float operator()(const std::string& str) { return strtof(str.c_str(), 0); }
};

template<>
struct conversion<double>
{
	double operator()(const std::string& str) { return atof(str.c_str()); }
};

template<>
struct conversion<bool>
{
	bool operator()(const std::string& str) 
	{
		std::string lower_str = str;
		std::transform(str.begin(), str.end(), lower_str.begin(), ::tolower);
		return lower_str == "1" || lower_str == "true" || lower_str == "yes";
	}
};
}

class config_mgr
{
public:
	static config_mgr* instance();

	bool load_initial(const std::string& file, std::string& error);
	bool reload(std::string& error);

	template<typename T>
	T get_value_default(const std::string& section, const std::string& key, T def) const
	{
		std::string res = ini_.get_value(section, key);
		if(res.empty())
			return def;
		res.erase(std::remove(res.begin(), res.end(), '"'), res.end());
		tlp::conversion<T> c;
		return c(res);
	}

	void print() { ini_.print(); }
private:
	config_mgr() = default;
	~config_mgr() = default;

	config_mgr(const config_mgr&) = delete;
	config_mgr& operator=(const config_mgr&) = delete;

	ini_parser ini_;
};

#define CONFIG_MGR config_mgr::instance()

#endif
