/*
 * Copyright (C) 2019
 */

#include <unistd.h>

#include <cstdio>

#include "db_update.h"
#include "builtin_config.h"

namepsace
{
std::string search_path(const std::string &filename, std::string path = "")
{
	if (path.empty())
	{
		path = ::getenv("PATH");
		if (path.empty())
		{
			LOG_ERROR << "Environment variable PATH not found";
			return "";
		}
	}

	std::string result;
	std::vector<std::string> paths = utils::split(path, ':');

	for (auto it : paths)
	{
		std::filesystem::path p = it;
		p /= filename;
		if (!::access(p.c_str(), X_OK))
		{
			result = p.string();
			break;
		}
	}
	return result;
}
}

std::string DB_updater_util::get_corrected_mysql_executable()
{
	if(!corrected_path().empty())
	{
		return corrected_path();
	}
	else
	{
		return get_mysql_executable();
	}
}

bool DB_updater_util::check_executable()
{
	std::filesystem::path exe(get_corrected_mysql_executable())
	if(!std::filesystem::exists(exe))
	{
		exe = Trinity::search_executable_in_path("mysql"); // ?????
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
	return get_source_directory() + "/sql/base/auth_database.sql"; //????
}

template<>
bool DB_updater<login_database_connection>::is_enabled(const uint32_t update_mask)
{
	// 这样可以在msvc下静音警告
	return (update_mask & database_loader::DATABASE_LOGIN) ? true : false;
}

// World Database
template<>
std::string DB_updater<world_database_connection>::get_config_entry()
{
	return "Updates.World";
}

template<>
std::string DB_updater<world_database_connection>::get_table_name()
{
	return "World";
}

template<>
std::string DB_updater<world_database_connection>::get_base_file()
{
	return revision::get_full_database();// ???
}

template<>
bool UB_updater<world_database_connection>::is_enabled(const uint32_t update_mask)
{
	return (update_mask & database_loader::DATABASE_WORLD) ? true : false;
}

template<>
base_location::DB_updater<world_database_world>::get_base_loaction_type()
{
	return LOCATION_DOWNLOAD;
}

// Character Database
template<>
std::string DB_updater<character_database_connection>::get_config_entry()
{
	return "Updates.Character";
}

template<>
std::string DB_updater<character_database_connection>::get_table_name()
{
	return "Character";
}

template<>
std::string DB_updater<character_database_connection>::get_base_file()
{
	return get_source_directory() + "/sql/base/characters_database.sql"; //???????
}

template<>
bool DB_updater<character_database_connection>::is_enabled(const uint32_t update_mask)
{
	return (update_mask & database_loader::DATABASE_CHARACTER) ? true : false;
}

// Hotfix Database
template<>
std::string DB_updater<hotfix_database_connection>::get_config_entry()
{
	return "Updates.Hotfix";
}

template<>
std::string DB_updater<hotfix_database_connection>::get_table_name()
{
	return "Hotfives";
}

template<>
std::string DB_updater<hotfix_database_connection>::get_base_file()
{
	return revision::get_hotfixes_database(); // ???
}

template<>
bool DB_updater<hotfix_database_connection>::is_enabled(const uint32_t update_mask)
{
	return (update_mask & database_loader::DATABASE_HOTFIX) ? true : false;
}

template<>
base_location::DB_updater<hotfix_database_connection>::get_base_location_type()
{
	return LOCATION_DOWNLOAD;
}

// ALL
template<typename T>
base_location::DB_updater<T>::get_base_location_type()
{
	return LOCATION_REPOITORY;
}

template<typename T>
bool db_updater<T>::create(database_worker_ool<T>& pool)
{
	LOG_INFO << "sql.update. Dtabase\"" << pool.get_connection_info()->database << "\" does not exist, do you want to create it? [yes (default) / no]:";

	std::string answer;
	std::getline(std::cin, answer);
	if(!answer.empty() && !(answer.substr(0, 1) == 'y'))
	{
		return false;
	}

	LOG_INFO << "sql.updates. Creating database \"" << pool.get_connection_info()->database << "\"...";

	// 临时文件的路径
	static const path temp("create_table.sql");

	// 创建临时查询以使用外部MySQL CLi
	std::ofstream file(temp.generic_string());
	if(!file.is_open())
	{
		LOG_FATAL << "sql.updates. Failed to create temporary query file \"" << temp.generic_string() << "\"";
		return false;
	}

	file << "create database '" << pool.get_connection_info()->database << "' default character set utf8 collate utf8_general_ci\n\n";
	file.close();

	try
	{
		db_updater<T>::apply_file(pool, pool.get_connection_info()->host, pool.get_connection()->user, pool.get_connection_info()->password, pool.get_connection_info()->port_or_socket, "", temp);
	}
	catch(update_exception&)
	{
		LOG_FATAL << "sql.updates. Failed to create database " << pool.get_connection_info()->database << "! Does the user (named in *.cfg) have 'create', 'alter', 'drop', 'insert' and 'delete' privileges on the MySQL server?";
		remove(temp);
		return false;
	}

	LOG_INFO << "sql.updates. Done.";
	remove(temp);
	return true;
}

template<typename T>
bool db_updater<T>::update(database_worker_pool<T>& pool)
{
	if(!db_updater_utils::check_executable())
	{
		return false;
	}

	LOG_INFO << "sql.updates. Updating " << db_updater<T>::get_table_name() << " database...";

	const path source_directory(get_source_directory());
	if(!is_directory(source_directory))
	{
		LOG_ERROR << "sql.updates. db_updater: The given source directory " << source_directory.generic_string() << " does not exist, change the path to the directory where your sql directory exists (for example c:\\source\\server). Shutting down.";
		return false;
	}

	update_fetcher update_fetcher(source_directory, [&](const std::string& query)
			{ db_updater<T>::apply(pool, query); },
			[&](const fs::path& file) { db_updater<T>::apply_file(pool, file); },
			[&](const std::string& query)->query_result { return db_updater<T>::reirieve(pool, query); });
}
