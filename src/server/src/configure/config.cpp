/*
 * Copyright (C) 2019
 */

#include "config.h"

static std::string filename;

bool config_mgr::load_initial(const std::string& file, std::string& error)
{
	filename = file;
	return ini_.read_ini(file, error);
}

bool config_mgr::reload(std::string& error)
{
	return load_initial(filename, error);
}

config_mgr* config_mgr::instance()
{
	static config_mgr instance;
	return &instance;
}
