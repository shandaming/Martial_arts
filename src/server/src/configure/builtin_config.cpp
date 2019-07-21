/*
 * Copyright (C) 2019
 */

#include "builtin_config.h"
#include "revision.h"
#include "config.h"

template<typename Func>
static std::string get_string_with_default_value(const std::string& section,
		const std::string& key, Func func)
{
	const std::string& val = CONFIG_MGR->get_string_default(section, key, "");
	return val.empty() ? func() : val;
}

std::string get_source_directory()
{
	return get_string_with_default_value("path", "source_directory", revision::get_source_directory());
}

std::string get_mysql_executable()
{
	return get_string_with_default_value("mysql", "mysql_executable", revision::get_mysql_executable);
}
