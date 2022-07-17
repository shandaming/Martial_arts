//
// Created by mylw on 2022/4/30.
//
#include <fstream>

#include "config.h"
#include "log.h"
#include "uitl.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

static json object;

bool config_mgr::load_init(const std::string& file) {
	std::ifstream in(file, std::ifstream::in);
	if (!in.is_open())
	{
		LOG_ERROR("Open config file {} failed.", file);
		return false;
	}

	try
	{
		in >> object;
		return true;
	}
	catch (std::exception &e) {
		LOG_ERROR("%s", e.what());
		return false;
	}
}

template<class T>
T config_mgr::get_value_default(std::string const& name, T def) const
{
	return object[name];
}

std::string config_mgr::get_string_default(const std::string& name, const std::string& def) const
{
	std::string val = get_value_default(name, def);
	val.erase(std::remove(val.begin(), val.end(), '"'), val.end());
	return val;
}

bool config_mgr::get_bool_default(const std::string& name, bool def) const
{
	std::string val = get_value_default(name, std::string(def ? "1" : "0"));
	val.erase(std::remove(val.begin(), val.end(), '"'), val.end());
	return string_to_bool(val);
}

int config_mgr::get_int_default(const std::string& name, int def) const
{
	return get_value_default(name, def);
}

float config_mgr::get_float_default(const std::string& name, float def) const
{
	return get_value_default(name, def);
}

config_mgr* config_mgr::instance()
{
	static config_mgr instance;
	return &instance;
}