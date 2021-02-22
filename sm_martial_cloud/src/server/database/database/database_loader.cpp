/*
 * Copyright (C) 2019
 */

#include <mysql/mysqld_error.h>

#include "database_loader.h"
#include "db_update.h"
#include "config.h"
#include "log.h"

database_loader::database_loader(const std::string& logger, const uint32_t default_update_mask) :
	logger_(logger), auto_setup_(true), update_flags_(default_update_mask) {}

template<typename T>
database_loader& database_loader::add_database(database_worker_pool<T>& pool, const std::string& name)
{
	const bool update_enabled_for_this = db_updater<T>::is_enabled(update_flags_);

	// 添加数据库打开操作
	open_.push([this, name, update_enabled_for_this, &pool]()->bool
			{
				// 从配置文集读取数据库信息
				const std::string& db_string = CONFIG_MGR->get_value_default("1", "2", "1");
				if(db_string.empty())
				{
					LOG_ERROR(logger_, "Database %s not specified in configuaration file!", name.c_str());
					return false;
				}

				const uint8_t async_threads = uint8_t(CONFIG_MGR->get_value_default("1", "1", 1));
				if(async_threads < 1 || async_threads > 32)
				{
					LOG_ERROR(logger_, "%s database; invalid number of worker threads specified. Please pick a value between 1 and 32", name.c_str());
					return false;
				}
				const uint8_t sync_threads = uint8_t(CONFIG_MGR->get_value_default("1", "2", 1));
				// 设置数据库连接信息
				pool.set_connection_info(db_string, async_threads, sync_threads);
				if(uint32_t error = pool.open())
				{
					// 数据库不存在
					if(error == ER_BAD_DB_ERROR && update_enabled_for_this && auto_setup_)
					{
						// 如果启用了自动设置，请尝试创建数据库并再次连接
						if(db_updater<T>::create(pool) && !pool.open())
							error = 0;
					}
					// 如果没有处理错误退出
					if(error)
					{
						LOG_ERROR("sql.driver", "\nDatabase_pool %s NOT opened. There were errors opening the MySQL connections. check your SQLDriverLogFile for specific errors. Read wiki at https://www.trinitycore.info/display/tc/TrinityCoreHome.", name.c_str());
						return false;
					}
				}

				// 添加数据库关闭操作
				close_.push([&pool]
						{
							pool.close();
						});
			});

	// 仅在为此池启用更新时才填充和更新
	if(update_enabled_for_this)
	{
		// 添加数据库填充操作
		populate_.push([this, name, &pool]()->bool
				{
					if(!db_updater<T>::populate(pool))
					{
						LOG_ERROR(logger_, "Could not populate the %s database. see log for details.", name.c_str());
						return false;
					}
					return true;
				});

		// 添加数据库更新操作
		update_.push([this, name, &pool]()->bool
				{
					if(!db_updater<T>::update(pool))
					{
						LOG_ERROR(logger_, "Could not update the %s database, see log for details.", name.c_str());
						return false;
					}
					return true;
				});
	}

	// 添加数据库准备语句操作
	prepare_.push([this, name, &pool]()->bool
			{
				if(!pool.prepare_statements())
				{
					LOG_ERROR(logger_, "Could not prepare statements of the %s database, see log for details.", name.c_str());
					return false;
				}
				return true;
			});

	return *this;
}

bool database_loader::load()
{
	if(!update_flags_)
		LOG_INFO("sql.updates", "Automatic database updates are disabled for all databases!");
	if(!open_databases())
		return false;
	if(!populate_databases())
		return false;
	if(!update_databases())
		return false;
	if(!prepare_statements())
		return false;
	return true;
}

bool database_loader::open_databases()
{
	return process(open_);
}

bool database_loader::populate_databases()
{
	return process(populate_);
}

bool database_loader::update_databases()
{
	return process(update_);
}

bool database_loader::prepare_statements()
{
	return process(prepare_);
}

bool database_loader::process(std::queue<predicate_func>& queue)
{
	while(!queue.empty())
	{
		if(!queue.front()()) // 处理open、populate、update、prepared操作
		{
			// 关闭所有已注册关闭操作的打开的数据库
			while(!close_.empty())
			{
				close_.top()();
				close_.pop();
			}
			return false;
		}
		queue.pop();
	}
	return true;
}
