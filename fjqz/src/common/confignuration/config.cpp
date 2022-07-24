//
// Created by mylw on 2022/4/30.
//
#include <fstream>

#include "config.h"
#include "log.h"
#include "uitl.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace
{
json object_;
std::string filename_;
}

bool config_mgr::load_init(const std::string& file) 
{
	filename_ = file;

	std::ifstream in(filename_, std::ifstream::in);
	if (!in.is_open())
	{
		LOG_ERROR("Open config file {} failed.", filename_);
		return false;
	}

	try
	{
		in >> object_;
		return true;
	}
	catch (std::exception &e) 
	{
		LOG_ERROR("%s", e.what());
		return false;
	}
}

void config_mgr::save_json()
{
	std::ofstream out(filename_, std::ofstream::out | std::ofstream::trunc);
	if (!out.is_open())
	{
		LOG_ERROR("Open config file {} failed.", filename_);
		return;
	}

	try
	{
		object_ >> out;
	}
	catch (std::exception& e)
	{
		LOG_ERROR("%s", e.what());
	}
}

int get_window_value(const std::string& key) const
{
	auto& window = object_.at("window");
	return window.at(key);
}

void set_window_value(const std::string& key, int value)
{
	object_["window"][key] = value;
}

template<typename T, typename Format, typename... Args>
void get_json_value(T& value, Format&& fmt, Args&&... args)
{
	value = value[fmt];
	if constexpr (sizeof...(arg_left) > 0)
	{
		get_json_value(value, std::forward<Args>(args)...)];
	}
}

template<typename Format, typename... Args>
auto get_value_default(Format&& fmt, Args&&... args)
{
	auto value = object_[fmt];
	get_json_value(value, std::forward<Args>(args)...);
	return value;
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