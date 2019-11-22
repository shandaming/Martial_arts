/*
 * Copyright (C) 2019
 */

#ifndef INI_CONFIG_H
#define INI_CONFIG_H

#include <algorithm>
#include <type_traits>

#include "ini_parser.h"

namespace tlp
{
// 将字符串转换为对应的类型的直
template<typename T>
struct convert_string
{
	constexpr typename std::enable_if_t<std::is_same_v<std::remove_const_t<T>, std::string>, std::string> operator()(const std::string& str) { return str; }
};

template<>
struct convert_string<const char*>
{
	std::string operator()(const std::string& str) { return str; }
};

template<>
struct convert_string<int8_t>
{
	int8_t operator()(const std::string& str) { return atoi(str.c_str()); }
};

template<>
struct convert_string<int16_t>
{
	int16_t operator()(const std::string& str) { return atoi(str.c_str()); }
};

template<>
struct convert_string<int32_t>
{
	int32_t operator()(const std::string& str) { return atol(str.c_str()); }
};

template<>
struct convert_string<int64_t>
{
	int64_t operator()(const std::string& str) { return atoll(str.c_str()); }
};

template<>
struct convert_string<uint8_t>
{
	uint8_t operator()(const std::string& str) { return strtoul(str.c_str(), 0, 10); }
};

template<>
struct convert_string<uint16_t>
{
	uint16_t operator()(const std::string& str) { return strtoul(str.c_str(), 0, 10); }
};

template<>
struct convert_string<uint32_t>
{
	uint32_t operator()(const std::string& str) { return strtoul(str.c_str(), 0, 10); }
};

template<>
struct convert_string<uint64_t>
{
	uint64_t operator()(const std::string& str) { return strtoull(str.c_str(), 0, 10); }
};

template<>
struct convert_string<float>
{
	float operator()(const std::string& str) { return strtof(str.c_str(), 0); }
};

template<>
struct convert_string<double>
{
	double operator()(const std::string& str) { return atof(str.c_str()); }
};

template<>
struct convert_string<bool>
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
		tlp::convert_string<T> c;
		return c(res);
	}

	std::string get_value_default(const std::string& section, const std::string& key, const char* def) const
	{
		std::string res = ini_.get_value(section, key);
		if(res.empty())
			return def;
		res.erase(std::remove(res.begin(), res.end(), '"'), res.end());
		tlp::convert_string<const char*> c;
		return c(res);
	}

	std::vector<std::string> get_keys_by_string(const std::string& section, const std::string& name)
	{
		return ini_.get_keys_by_string(section, name);
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
