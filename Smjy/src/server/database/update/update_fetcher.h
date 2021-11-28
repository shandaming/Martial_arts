/*
 * Copyright (C) 2019
 */

#ifndef DB_UPDATE_FETCHER_H
#define DB_UPDATE_FETCHER_H

#include <string>
#include <functional>
#include <set>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <filesystem>

#include "database_env_fwd.h"

namespace fs = std::filesystem;

struct update_result
{
	update_result() : updated(0), recent(0), archived(0) {}

	update_result(const size_t updated_, const size_t recent_, const size_t archived_) : updated(updated_), recent(recent_), archived(archived_) {}

	size_t updated;
	size_t recent; // 最近
	size_t archived; // 存档
};

class update_fetcher
{
public:
	update_fetcher(const fs::path& update_directory, 
			const std::function<void(const std::string&)>& apply, 
			const std::function<void(const fs::path&)>& apply_file, 
			const std::function<query_result(const std::string&)>& retrieve);

	update_result update(const bool redundancy_checks, 
			const bool allow_rehash, 
			const bool archived_redundancy, 
			const int32_t clean_dead_references_max_count) const;
private:
	enum update_mode
	{
		MODE_APPLY,
		MODE_REHASH
	};

	enum state
	{
		RELEASED,
		ARCHIVED // 已封存
	};

	struct applied_file_entry // 应用文件条目
	{
		applied_file_entry(const std::string& name_, const std::string& hash_, state state_, uint64_t timestamp_) : 
			name(name_), hash(hash_), _state(state_), timestamp(timestamp_) {}

		static inline state state_convert(const std::string& state)
		{
			return (state == "RELEASED") ? RELEASED : ARCHIVED;
		}

		static inline std::string state_convert(const state state)
		{
			return (state == RELEASED) ? "RELEASED" : "ARCHIVED";
		}

		std::string get_state_as_string() const { return state_convert(_state); }

		const std::string& name;
		const std::string& hash;
		const state _state;
		const uint64_t timestamp;
	};

	struct directory_entry;

	typedef std::pair<fs::path, state> locale_file_entry;

	struct path_compare
	{
		bool operator()(const locale_file_entry& l, const locale_file_entry& r) const;
	};

	typedef std::set<locale_file_entry, path_compare> locale_file_storage;
	typedef std::unordered_map<std::string, std::string> hash_to_file_name_storage;
	typedef std::unordered_map<std::string, applied_file_entry> applied_file_storage;
	typedef std::vector<update_fetcher::directory_entry> directory_storage;

	locale_file_storage get_file_list() const;
	// 第归查找更新sql文件存储到storage中
	void fill_file_list_recursively(const fs::path& path, locale_file_storage& storage, const state state, const uint32_t depth) const;

	// 从数据库里查询所有要更新的包含目录
	directory_storage receive_include_directories() const;
	// 从数据库里查询文件的详细信息
	applied_file_storage receive_applied_files() const;

	// 读取文件里的sql语句
	std::string read_sql_update(const fs::path& file) const;

	// 更新数据库
	uint32_t apply(const fs::path& path) const;

	void update_entry(const applied_file_entry& entry, const uint32_t speed = 0) const;
	void rename_entry(const std::string& from, const std::string& to) const;
	void clean_up(const applied_file_storage& storage) const;

	void update_state(const std::string& name, const state state) const;

	const std::unique_ptr<fs::path> source_directory_;

	const std::function<void(const std::string&)> apply_;
	const std::function<void(const fs::path&)> apply_file_;
	const std::function<query_result(const std::string&)> retrieve_;
};

#endif
