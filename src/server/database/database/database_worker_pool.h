/*
 * Copyright (C) 2019
 */

#ifndef DB_DATABASE_WORKER_POOL_H
#define DB_DATABASE_WORKER_POOL_H

#include "sql_operation.h"
#include "mysql_connection.h"
#include "common/utility/string_format.h"
#include "query_callback.h"
#include "query_holder.h"

template<typename T>
class database_worker_pool
{
public:
	database_worker_pool();
	~database_worker_pool();

	void set_connection_info(const std::string& info_string, const uint8_t async_threads, const uint8_t synch_threads);

	uint32_t open();

	void close();


	// 准备所有准备好的语句
	bool prepare_statements();


	inline mysql_connection_info const* get_connection_info() const
	{
		return connection_info_.get();
	}

	const char* get_database_name() const;

	// 以字符串格式排队将单向执行的SQL操作，该操作将异步执行。 ！ 此方法仅应用于仅执行一次（例如在启动期间）的查询。
	void execute(const char* sql);

	// ！ 使具有变量args的字符串格式的单向SQL操作入队，该操作将异步执行。 ！ 此方法仅应用于仅执行一次（例如在启动期间）的查询。
	template<typename F, typename... Args>
	void p_execute(F&& sql, Args&&... args)
	{
		if(is_format_empty_or_null(sql))
		{
			return;
		}
		execute(string_format(sql, std::forward<Args>(args)...).c_str());
	}

	// ！ 以准备好的语句格式排队单向SQL操作，该操作将异步执行。 ！ 语句必须带有CONNECTION_ASYNC标志。
	void execute(prepared_statement<T>* stmt);

	// 直接执行字符串格式的单向SQL操作，该操作将阻塞调用线程，直到完成。 ！ 此方法仅应用于仅执行一次（例如在启动期间）的查询。
	void direct_execute(const char* sql);

	// 使用变量args-直接以字符串格式执行单向SQL操作，该操作将阻塞调用线程，直到完成。 ！ 此方法仅应用于仅执行一次（例如在启动期间）的查询。
	template<typename F, typename... Args>
	void direct_execute(F&& sql, Args&&... args)
	{
		if(is_format_empty_or_null(sql))
		{
			return;
		}
		direct_execute(string_format(std::forward<F>(sql), std::forward<Args>(args)...).c_str());
	}

	// 以准备好的语句格式直接执行单向SQL操作，该操作将阻塞调用线程，直到完成。 ！ 语句必须带有CONNECTION_SYNCH标志。
	void direct_execute(prepared_statement<T>* stmt);

	// 直接执行字符串格式的SQL查询，该查询将阻塞调用线程，直到完成。 ！ 返回引用计数的自动指针，无需在上层代码中进行手动内存管理。
	query_result query(const char* sql, T* connection = nullptr);

	// 使用变量args直接以字符串格式执行SQL查询，该查询将阻塞调用线程，直到完成。 ！ 返回引用计数的自动指针，无需在上层代码中进行手动内存管理。
	template<typename F, typename... Args>
	query_result p_query(F&& sql, T* conn, Args&&... args)
	{
		if(is_format_empty_or_null(sql))
		{
			return query_result(nullptr);
		}
		return query(string_format(std::forward<F>(sql), std::forward<Args>(args)...).c_str(), conn);
	}

	// 使用变量args直接以字符串格式执行SQL查询，该查询将阻塞调用线程，直到完成。 ！ 返回引用计数的自动指针，无需在上层代码中进行手动内存管理。
	template<typename F, typename... Args>
	query_result p_query(F&& sql, Args&&... args)
	{
		if(is_format_empty_or_null(sql))
		{
			return query_result(nullptr);
		}
		return query(string_format(std::forward<F>(sql), std::forward<Args>(args)...).c_str());
	}

	// 以准备好的格式直接执行SQL查询，该查询将阻塞调用线程，直到完成。 ！ 返回引用计数的自动指针，无需在上层代码中进行手动内存管理。 ！ 语句必须带有CONNECTION_SYNCH标志。
	prepared_query_result query(prepared_statement<T>* stmt);

	// 以字符串格式使查询排队，该查询将在执行查询后立即设置QueryResultFuture返回对象的值。 ！ 然后，在ProcessQueryCallback方法中处理返回值。
	query_callback async_query(const char* sql);

	// 使查询以准备好的格式排队，该格式将在查询执行后立即设置PreparedQueryResultFuture返回对象的值。 ！ 然后，在ProcessQueryCallback方法中处理返回值。 ！ 语句必须带有CONNECTION_ASYNC标志。
	query_callback async_query(prepared_statement<T>* stmt);

	// 排队一个将设置QueryResultHolderFuture值的SQL操作向量（既可以是临时的也可以是已准备好的）！ 查询执行后立即返回对象。 ！ 然后，在ProcessQueryCallback方法中处理返回值。 ！ 添加到此持有人的任何准备好的语句都需要使用CONNECTION_ASYNC标志进行准备。
	query_result_holder_future delay_query_holder(sql_query_holder<T>* holder);

	// 开始一个自动管理的事务指针，如果未提交，它将自动回滚。 （自动提交= 0）
	sql_transaction<T> begin_transaction();

	// 使单向SQL操作的集合入队（既可以即席又可以准备）。 这些操作的顺序！ 被附加到事务将在执行期间得到尊重。
	void commit_transaction(sql_transaction<T> transaction);

	// 直接执行单向SQL操作的集合（既可以即席又可以准备）。 这些操作的顺序！ 被附加到事务将在执行期间得到尊重。
	void direct_commit_transaction(sql_transaction<T>& transaction);

	// 用于在不同上下文中执行即席语句的方法。 ！ 如果存在有效对象，将包装在事务中，否则独立执行。
	void execute_or_append(sql_transaction<T>& trans, const char* sql);

	// 用于在不同上下文中执行准备好的语句的方法。 ！ 如果存在有效对象，将包装在事务中，否则独立执行。
	void execute_or_append(sql_transaction<T>& trans, prepared_statement<T>* stmt);

	//
	typedef typename T::statements prepared_statement_index;

	// 自动管理（内部）指向准备好的语句对象的指针，以供在上层代码中使用。 ！ 在this-> DirectExecute（PreparedStatement *），this-> Query（PreparedStatement *）或PreparedStatementTask ::〜PreparedStatementTask中删除指针。 ！ 在执行之前，该对象尚未绑定到MySQL上下文上的prepared语句。
	prepared_statement<T>* get_prepared_statement(prepared_statement_index index);

	// 为当前的排序规则应用转义字符串。 （utf8）
	void escape_string(std::string& str);

	// 使我们所有的MySQL连接保持活动状态，防止服务器断开我们的连接。
	void keepalive();
private:
	enum internal_index
	{
		IDX_ASYNC,
		IDX_SYNCH,
		IDX_SIZE
	};

	uint32_t open_connections(internal_index type, uint8_t num_connections);

	unsigned long escape_string(char* to, const char* from, unsigned long length);

	void enqueue(sql_operation* op);

	// 在同步连接池中获取免费连接。 ！ 调用者在触摸MySQL上下文后必须调用t-> Unlock（）以防止死锁。
	T* get_free_connection() const;

	// 异步工作线程共享的队列。
	std::unique_ptr<producer_consumer_queue<sql_operation*>> queue_;
	std::array<std::vector<std::unique_ptr<T>>, IDX_SIZE> connections_;
	std::unique_ptr<mysql_connection_info> connection_info_;
	std::vector<uint8_t> prepared_statement_size_;
	uint8_t async_threads_;
	uint8_t synch_threads_;
};

#endif
