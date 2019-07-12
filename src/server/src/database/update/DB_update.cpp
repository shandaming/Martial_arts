/*
 * Copyright (C) 2019
 */

#include "DB_update.h"

std::string DB_updater_util::get_corrected_mysql_executable()
{
	if(!corrected_path().empty())
		return corrected_path();
	else
		return Built_in_config::get_mysql_executable();
}

bool DB_updater_util::check_executable()
{
	std::filesystem::path exe(get_corrected_mysql_executable())
	if(!std::filesystem::exists(exe))
	{
		exe = Trinity::search_executable_in_path("mysql");
		if(!exe.empty() && std::filesystem::exists(exe))
		{
			// 纠正cli的路径
			corrected_path() = std::filesystem::absolute(exe).generic_string();
			return true;
		}

		LOG_FATAL << "sql.updates. Didn't find any executable MySQL binary at \'" << std::filesystem::absolute(exe).generic_string() << "\' or in path, correct the path in the *.conf (\'MySQLExecutable\')";

		return false;
	}
	return true;
}

std::string& DB_updater_util::corrected_path()
{
	static std::string path;
	return path;
}

// Auth Database
template<>
std::string DB_updater<login_database_connection>::get_config_entry()
{
	return "Updates.Auth";
}

template<>
std::string DB_updater<login_database_connection>::get_table_name()
{
	return "Auth";
}

template<>
std::string DB_updater<login_database_connection>::get_base_file()
{
	return Built_in_config::get_source_directory() + "/sql/base/auth_database.sql";
}

template<>
bool DB_updater<login_database_connection>::is_enabled(const uint32_t update_mask)
{
	// 这样可以在msvc下静音警告
	return (update_mask & database_loader::DATABASE_LOGIN) ? true : false;
}
