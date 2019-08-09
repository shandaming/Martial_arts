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
	LOG_INFO << "sql.update. Dtabase\"" << pool.get_connection_info()->database 
		<< "\" does not exist, do you want to create it? [yes (default) / no]:";

	// 从控制台获取是否更新
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

	// 文件里写入创建数据库sql
	file << "create database '" << pool.get_connection_info()->database 
		<< "' default character set utf8 collate utf8_general_ci\n\n";
	file.close();

	try
	{
		db_updater<T>::apply_file(pool, pool.get_connection_info()->host, pool.get_connection()->user, pool.get_connection_info()->password, pool.get_connection_info()->port_or_socket, "", temp);
	}
	catch(update_exception&)
	{
		LOG_FATAL << "sql.updates. Failed to create database " << pool.get_connection_info()->database 
			<< "! Does the user (named in *.cfg) have 'create', 'alter', 'drop', \
			'insert' and 'delete' privileges on the MySQL server?";
		remove(temp);
		return false;
	}

	LOG_INFO << "sql.updates. Done.";
	remove(temp); // 删除临时文集
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

	update_result result;
	try
	{
		result = update_fetcher.update(CONGIF_MGR->getbool(), CONFIG_MGR->getbool(), CONFIG_MGR->get_bool(), CONFIG_MGR->getint());
	}
	catch(update_exception&)
	{
		return false;
	}

	const std::string info = string_format("Containing "SZFNTD" new and "SZFNTD" archived updates.", result.recent, result.archived);

	if(!result.updated)
	{
		LOG_INFO << "sql.updates. >> " << db_update<T>::get_table_name() << " database is up-to-data! " << info;
	}
	else
	{
		LOG_INFO << "sql.updates. >> Applied " << result.updated << " " << result.updated == 1 ? "query" : "queries" << ". " << info;
	}
	return true;
}

template<typename T>
bool db_update<T>::populate(database_worker_pool<T>& pool)
{
	{
		query_result = retrieve(pool, "show tables");
		if(result && (result->get_row_count() > 0))
		{
			return true;
		}
	}

	if(!db_updater_util::check_executable())
	{
		return false;
	}

	LOG_INFO << "sql.updates. Database " << db_updater<T>::get_table_name() << " is empty. auto populating it...";

	const std::string p = db_updater<T>::get_base_file();
	if(p.empty())
	{
		LOG_INFO << "sql.updates. >> No base file provided, skipped!";
		return true;
	}

	const path base(p);
	if(!exists(base))
	{
		switch(db_updater<T>::get_base_location_type())
		{
			case LOCATION_REPOSITORY:
				{
					LOG_ERROR << "sql.updates. >> Base file \"" << base.generic_string() << "\" is missing. Try fixing it by cloning the source again.";
					break;
				}
			case LOCATION_DOWLOAD:
				{
					const std::string filename = base.filename().generic_string();
					const std::string workdir = fs::current_path().generic_string();
					LOG_ERROR << "sql.updates. >> File \"" << filename << "\" is missing, download it from \"https://\" uncompress it and place the file \"" << filename << "\" in the directory \"" << workdir << "\".";
					break;
				}
			default:
				break;
		}
		return true;
	}
	// 更新数据库
	LOG_INFO << "sql. updates. >> Applying \"" << base.generic_string() << "\"...";
	try
	{
		apply_file(pool, base);
	}
	catch(update_exception&)
	{
		return false;
	}
	LOG_INFO << "sql.updates. >> Done.";
	return true;
}

template<typename T>
query_result db_updater<T>::retrieve(database_worker_pool<T>& pool, const std::string& query)
{
	return pool.query(query);
}

template<typename T>
void db_updater<T>::apply(database_worker_pool<T>& pool, const std::string& query)
{
	pool.direct_execute(query);
}

template<typename T>
void db_updater<T>::apply_file(database_worker_pool<T>& pool, const path& path)
{
	db_updater<T>::apply_file(pool, pool.get_connection_info()->host, pool.get_connection_info()->user, pool.get_connection_info()->password, pool.get_connection_info()->port_or_socket, pool.get_connection_info()->database, path);
}

template<typename T>
void db_updater<T>:: apply_file(database_worker_pool<T>& pool, const std::string& host,
		const std::string& user, const std::string& password, const std::string& port_or_socket, 
		const std::string& database, const path& path)
{
	std::vector<std::string> args;
	args.reserve(8);

	// args[0] 是程序名
	args.push_back("mysql");
	// 客户端连接信息
	args.push_back("-h" + host); // 主机名
	args.push_back("-u" + user); // mysql用户名

	if(!password.empty())
	{
		args.push_back("-p" + password); // mysql秘密
	}
	
	// 检测是否通过IP或socket连接
	if(!isdigit(port_or_socket))
	{
		// 如果host =='.' 我们不能检测，因为开启socket会被命名为localhost
		args.push_back("-P0");
		args.push_back("-protocol=SOCKET");
		args.push_back("-S" + port_or_socket);
	}
	else
	{
		// 一般情况
		args.push_back("-P" + port_or_socket);
	}

	// 设置字符集为utf8
	args.push_back("--default-character-set=utf8");
	// 设置允许的最大数据包为1GB
	args.push_back("--max-allowed-packet=1GB");

	// 数据库
	if(!database.empty())
	{
		args.push_back(database);
	}
	// 调用一个不会泄露证书到日志里的mysql进程
	const int ret = trinity::start_pocess(db_updater_util::get_corrected_mysql_executable(), args, 
			"sql.updates", path.generic_string(), true);

	if(ret != EXIT_SUCCESS)
	{
		LOG_FATAL << "sql. updates. Applying of file '" << path.generic_string() 
			<< "' to database '" << pool.get_connection_info()->database 
			<< "' failed! If you are a user, please pull the latest revision from the repository.\
			Also make sure you have not applied any of the database with your sql client. \
			You cannot use auto-update system and import sql files from TrinityCore repository with your sql client.\
			If you are a developer, please fix you sql Query.";
		throw update_exception("update failed.");
	}
}
