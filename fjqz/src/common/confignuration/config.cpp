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

bool config_mgr::load_init(std::string_view& file)
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
		out << object_;
	}
	catch (std::exception& e)
	{
		LOG_ERROR("%s", e.what());
	}
}

int config_mgr::get_window_value(std::string_view& key)
{
	auto& window = object_.at("window");
	return window.at(key.data());
}

void config_mgr::set_window_value(std::string_view& key, int value)
{
	object_["window"][key.data()] = value;
}

std::string config_mgr::get_resource_path(std::string_view& key)
{
	auto& window = object_.at("resource_path");
	return window.at(key.data());
}

config_mgr* config_mgr::instance()
{
	static config_mgr instance;
	return &instance;
}