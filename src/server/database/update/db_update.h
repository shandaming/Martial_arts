/*
 * Copyright (C) 2019
 */

#ifndef DATABASE_DB_UPDATE_H
#define DATABASE_DB_UPDATE_H

#include <string>
#include <filesystem>

class update_exception : public std::exception
{
public:
	update_exception(const std::string& msg) : msg_(msg) {}
	~update_exception() throw() {}

	const char* what() const throw() override { return msg_.c_str(); }
private:
	const std::string msg_;
};

enum base_location
{
	LOCATION_REPOSITORY, // 位置存储
	LOCATION_DOWNLOAD // 位置下载
};

class DB_updater_util
{
public:
	// 获取mysql可执行程序的路径
	static std::string get_corrected_mysql_executable();
	static bool check_executable();
private:
	// 正确的路径
	static std::string& corrected_path();
};

template<typename T>
class DB_updater
{
public:
	using path = std::filesystem::path;

	static inline std::string get_config_entry();
	static inline std::string get_table_name();

	static std::string get_base_file();
	static bool is_enabled(const uint32_t update_mask);

	static base_location get_base_location_type();

	static bool create(database_worker_pool<T>& pool);
	static bool update(database_worker_pool<T>& pool);
	static bool populate(database_worker_pool<T>& pool);
private:
	static query_result retrieve(database_worker_pool<T>& pool, const std::string& query);
	static void apply(database_worker_pool<T>& pool, const std::string& query);
	static void apply_file(database_worker_pool<T>& pool, const path& path);
	// 开启一个mysql进程并连接数据库执行文件里的sql
	static void apply_file(database_worker_pool<T>& pool, const std::string& host, const std::string& user, const std::string& password, const std::string& port_or_socket, const std::string& database, const path& path);
};

#endif
