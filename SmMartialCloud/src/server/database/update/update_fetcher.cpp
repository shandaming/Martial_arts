/*
 * Copyright (C) 2019
 */

#include <chrono>
#include <fstream>
#include <sstream>

#include "update_fetcher.h"
#include "db_update.h"
#include "field.h"
#include "query_result.h"
#include "log.h"
#include "sha1.h"

struct update_fetcher::directory_entry
{
	directory_entry(const fs::path& path_, state state_) : path(path_), _state(state_) {}

	const fs::path path;
	const state _state;
};

update_fetcher::update_fetcher(const fs::path& source_directory, 
		const std::function<void(const std::string&)>& apply,
		const std::function<void(const fs::path&)>& apply_file,
		const std::function<query_result(const std::string&)>& retrieve) :
	source_directory_(std::make_unique<fs::path>(source_directory)), 
	apply_(apply), apply_file_(apply_file), retrieve_(retrieve) {}

update_fetcher::locale_file_storage update_fetcher::get_file_list() const
{
	locale_file_storage files;
	directory_storage directories = receive_include_directories();
	for(auto& entry : directories)
	{
		fill_file_list_recursively(entry.path, files, entry._state, 1);
	}
	return files;
}

void update_fetcher::fill_file_list_recursively(const fs::path& path, 
		locale_file_storage& storage, 
		const state state, 
		const uint32_t depth) const
{
	static const uint32_t max_depth = 10;
	static const fs::directory_iterator end;

	for(fs::directory_iterator it(path); it != end; ++it)
	{
		if(fs::is_directory(it->path()))
		{
			if(depth< max_depth)
			{
				fill_file_list_recursively(it->path(), storage, state, depth - 1);
			}
		}
		else if(it->path().extension() == ".sql")
		{
			LOG_TRACE("sql.updates", "Added locale file \"%s\".", it->path().filename().generic_string().c_str());

			const locale_file_entry entry = {it->path(), state};

			// 检查加倍的文件名。 因为元素只是通过文件名进行比较，所以这没关系
			if(storage.find(entry) != storage.end())
			{
				LOG_FATAL("sql.updates", "Duplicate filename \"%s\" occurred. Because updates are ordered by their filenames, every name needs to be unique!", it->path().generic_string().c_str());

				throw update_exception("Updating failed. see the log for details.");
			}
			storage.insert(entry);
		}
	}
}

update_fetcher::directory_storage update_fetcher::receive_include_directories() const
{
	directory_storage directories;
	const query_result result = retrieve_("SELECT 'path', 'state' from updates_include");
	if(!result)
	{
		return directories;
	}

	do
	{
		field* fields = result->fetch();
		std::string path = fields[0].get_string();
		if(path.substr(0, 1) == "S")
		{
			path = source_directory_->generic_string() + path.substr(1);
		}

		const fs::path p(path);
		if(!is_directory(p))
		{
			LOG_WARN("sql.updates", "db_updater: given update include directory \"%s\" does not exist, skipped.", p.generic_string().c_str());
			continue;
		}

		const directory_entry entry = {p, applied_file_entry::state_convert(fields[1].get_string())};
		directories.push_back(entry);

		LOG_TRACE("sql.updates", "Added applied file \"%s\" from remote.", p.filename().generic_string().c_str());
	}
	while(result->next_row());

	return directories;
}

update_fetcher::applied_file_storage update_fetcher::receive_applied_files() const
{
	applied_file_storage map;
	query_result result = retrieve_("select 'name', 'hash', 'state', unix_timestamp('timestamp') from updates order by 'name' asc");
	if(!result)
	{
		return map;
	}

	do
	{
		field* fields = result->fetch();
		const applied_file_entry entry = {fields[0].get_string(), fields[1].get_string(), applied_file_entry::state_convert(fields[2].get_string()), fields[3].get_uint64()};

		map.insert(std::make_pair(entry.name, entry));
	}
	while(result->next_row());

	return map;
}

std::string update_fetcher::read_sql_update(const fs::path& file) const
{
	std::ifstream in(file);
	if(!in.is_open())
	{
		LOG_FATAL("sql.updates", "Failed to open the sql update \"%s\" for reading! Stopping the server to keep the database intergrity. try to identify and solve the issue or disable the database updater.", file.generic_string().c_str());

		throw update_exception("Opening the sql update failed!");
	}

	auto update = [&in]
		{
			std::ostringstream os;
			os << in.rdbuf();
			return os.str();
		}();

	in.close();
	return update;
}

update_result update_fetcher::update(const bool redundancy_checks,
		const bool allow_rehash, const bool archived_redundancy, 
		const int32_t clean_dead_references_max_count) const
{
	const locale_file_storage available = get_file_list();
	applied_file_storage applied = receive_applied_files();

	size_t count_recent_updates = 0;
	size_t count_archived_updates = 0;

	for(const auto& i : applied)
	{
		if(i.second._state == RELEASED)
		{
			++count_recent_updates;
		}
		else
		{
			++count_archived_updates;
		}
	}

	// 填充哈希到名称缓存
	hash_to_file_name_storage hash_to_name;
	for(auto i : applied)
	{
		hash_to_name.insert(std::make_pair(i.second.hash, i.first));
	}

	size_t imported_updates = 0;

	for(auto& i : available)
	{
		LOG_DEBUG("sql.updates", "Checking update \"%s\"...", i.first.filename().generic_string().c_str());

		auto it = applied.find(i.first.filename().string());
		if(it != applied.end())
		{
			// 如果禁用冗余，请跳过它，因为已应用更新。
			if(!redundancy_checks)
			{
				LOG_DEBUG("sql.updates", ">> Update is already applied, skipping redundancy checks.");
				applied.erase(it);
				continue;
			}

			// 如果更新位于存档目录中并在我们的数据库中标记为已存档，则跳过冗余检查（存档更新永远不会更改）。
			if(!archived_redundancy && (it->second._state == ARCHIVED) && (i.second == ARCHIVED))
			{
				LOG_DEBUG("sql.updates", ">> Update is archived and marked as archived in database, skipping redundancy checks.");
				applied.erase(it);
				continue;
			}
		}

		// 根据查询内容计算Sha1哈希值。
		const std::string hash = calculate_sha1_hash(read_sql_update(i.first));

		update_mode mode = MODE_APPLY;

		// 更新不在我们的应用列表中
		if(it != applied.end())
		{
			// 捕获重命名（不同的文件名，但相同的哈希）
			auto hash_it = hash_to_name.find(hash);
			if(hash_it != hash_to_name.end())
			{
				// 检查原始文件是否已删除。 如果没有，我们就遇到了问题。
				locale_file_storage::const_iterator locale_it;
				// 向前推送localeIter
				for(locale_it = available.begin(); (locale_it != available.end()) && 
						(locale_it->first.filename().string() != hash_it->second); ++locale_it)
				{
					;
				}

				// 冲突！
				if(locale_it != available.end())
				{
					LOG_WARN("sql updates", ">> It seems like the update \"%s\" \"%d\" was renamed, but the old file is still there! Treating it as a new file! (It is probably an unmodified copy of the file \"%s\")", i.first.filename().string().c_str(), hash.substr(0, 7), locale_it->first.filename().string().c_str());
				}
				// 将文件视为此处重命名是安全的
				else
				{
					LOG_INFO("sql.updates", ">> Renaming update\"%s\" to \"%s\" \"%s\".", hash_it->second.c_str(), i.first.filename().string().c_str(), hash.substr(0, 7).c_str());

					rename_entry(hash_it->second, i.first.filename().string());
					applied.erase(hash_it->second);
					continue;
				}
			}
			// 如果以前从未见过，请应用更新。
			else
			{
				LOG_INFO("sql.updates", ">> Applying update \"%s\" \"%s\"...", i.first.filename().string().c_str(), hash.substr(0, 7).c_str());
			}
		}
		// 如果更新条目存在于我们的数据库中，则使用空哈希重新更新更新条目。
		else if(allow_rehash && it->second.hash.empty())
		{
			mode = MODE_REHASH;

			LOG_INFO("sql.updates", ">> Re-hashing update \"%s\" \"%s\"...", i.first.filename().string().c_str(), hash.substr(0, 7).c_str());
		}
		else
		{
			// 如果文件的哈希值与我们数据库中存储的哈希值不同，请重新应用更新（因为它已更改）。
			if(it->second.hash != hash)
				LOG_INFO("sql.updates", ">> Reapplying update \"%s\" \"%s\" -> \"%s\" (it changed)...", i.first.filename().string().c_str(), it->second.hash.substr(0, 7).c_str(), hash.substr(0, 7).c_str());
			else
			{
				// 如果文件未更改且刚刚移动，请更新其状态（如有必要）。
				if(it->second._state != i.second)
				{
					LOG_DEBUG("sql.update", ">> Updating the state of \"%s\" to \"%s\"...", i.first.filename().string().c_str(), applied_file_entry::state_convert(i.second).c_str());

					update_state(i.first.filename().string(), i.second);
				}

				LOG_DEBUG("sql.updates", ">> Update is already applied and matches the hash \"%s\"", hash.substr(0, 7).c_str());

				applied.erase(it);
				continue;
			}
		}

		uint32_t speed = 0;
		const applied_file_entry file = {i.first.filename().string(), hash, i.second, 0};
		switch(mode)
		{
			case MODE_APPLY:
				speed = apply(i.first);
			case MODE_REHASH:
				update_entry(file, speed);
				break;
			default:
				break;
		}

		if(it != applied.end())
		{
			applied.erase(it);
		}
		if(mode == MODE_APPLY)
		{
			++imported_updates;
		}
	}

	// 清理孤立的条目（如果已启用）
	if(!applied.empty())
	{
		const bool do_cleanup = (clean_dead_references_max_count < 0) ||
			(applied.size() <= static_cast<size_t>(clean_dead_references_max_count));

		for(auto& i : applied)
		{
			LOG_WARN("sql.updates", "The file \"%s\" was applied to the database, but is missing in your update directory now!", i.first.c_str());
			if(do_cleanup)
			{
				LOG_INFO("sql.updates", "Deleting orphaned entry \"%s\"", i.first.c_str());
			}
		}

		if(do_cleanup)
		{
			clean_up(applied);
		}
		else
		{
			LOG_ERROR("sql.updates", "Cleanup is disabled! There were %u dirty files applied to ypur database, but they are now missing in your source directory", applied.size());
		}
	}
	return update_result(imported_updates, count_recent_updates, count_archived_updates);
}

uint32_t update_fetcher::apply(const fs::path& path) const
{
	using time = std::chrono::high_resolution_clock;

	// 基准查询速度
	auto begin = time::now();
	// 更新数据库
	apply_file_(path);

	// 返回查询应用的时间
	return uint32_t(std::chrono::duration_cast<std::chrono::milliseconds>(time::now() - begin).count());
}

void update_fetcher::update_entry(const applied_file_entry& entry, const uint32_t speed) const
{
	const std::string update = "replace into updates (name, hash, state, speed) values('" + entry.name + "', '" + entry.hash + "', '" + entry.get_state_as_string() + "', '" + std::to_string(speed) + "')";
	// 更新数据库
	apply_(update);
}

void update_fetcher::rename_entry(const std::string& from, const std::string& to) const
{
	// 删除目标（如果存在）
	std::string update = "delete from updates where name = '" + to + "'";
	apply_(update);

	// 改名
	update = "update updates set name = '" + to + "' where name = '" + from + "'";
	apply_(update);
}

void update_fetcher::clean_up(const applied_file_storage& storage) const
{
	if(storage.empty())
	{
		return;
	}

	std::stringstream update;
	size_t remaining = storage.size();

	update << "delete from updates where name in(";
	for(auto& i : storage)
	{
		update << "'" << i.first << "'";
		if((--remaining) > 0)
		{
			update << ",";
		}
	}
	update << ")";

	// 更新数据库
	apply_(update.str());
}

void update_fetcher::update_state(const std::string& name, const state state) const
{
	const std::string update = "update updates set state = '" + applied_file_entry::state_convert(state) + "' + where name = '" + name + "'";

	// 更新数据库
	apply_(update);
}

bool update_fetcher::path_compare::operator()(const locale_file_entry& l, 
		const locale_file_entry& r) const
{
	return l.first.filename().string() < r.first.filename().string();
}
