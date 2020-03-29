/*
 * Copyright (C) 2019
 */

#ifndef DB_DATABASE_LOADER_H
#define DB_DATABASE_LOADER_H

#include <stack>

#include "database_worker_pool.h"

/* 一个辅助类，用于启动所有数据库工作池，处理更新，延迟准备语句并清除故障。 */
class database_loader
{
public:
	database_loader(const std::string& logger, const uint32_t default_update_mask);

	// 将数据库注册到加载器（延迟实现）
	template<typename T>
	database_loader& add_database(database_worker_pool<T>& pool, const std::string& name);

	// 加载所有数据库
	bool load();

	enum database_type_flags
	{
		DATABASE_NONE = 0,
		DATABASE_LOGIN = 1,
		DATABASE_CHARACTER = 2,
		DATABASE_WORLD = 4,
		DATABASE_HOTFIX = 8,
		DATABASE_MASK_ALL = DATABASE_LOGIN | DATABASE_CHARACTER | DATABASE_WORLD | DATABASE_HOTFIX
	};
private:
	// 打开数据库
	bool open_databse();
	// 填充数据库
	bool populate_database();
	// 更新数据库
	bool update_database();
	// 准备sql
	bool prepare_statements();

	using predicate_func = std::function<bool()>;
	using close_func = std::function<void()>;

	// 调用给定队列中的所有函数，并在出错时关闭数据库。 出现错误时返回false。
	bool process(std::queue<predicate_func>& queue);

	const std::string& logger_;
	const bool auto_setup_;
	const uint32_t update_flags_;

	std::queue<predicate_func> open_;
	std::queue<predicate_func> populate_;
	std::queue<predicate_func> update_;
	std::queue<predicate_func> prepare_;
	std::stack<close_func> close_;
};

#endif
