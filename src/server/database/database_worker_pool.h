/*
 * Copyright (C) 2019
 */

#ifndef DATABASE_WORKER_POOL_H
#define DATABASE_WORKER_POOL_H

#include "sql_operation.h"
#include "mysql_connection.h"

template<typename T>
class database_worker_pool
{
public:
	database_worker_pool() : queue_(new producer_consumer_queue<sql_operation*>()), async_threads_(0), synch_threads_(0)
	{
		wp_fatal(mysql_thread_safe(), "Used MySQL library isn't thread-safe");
		wp_fatal(mysql_get_client_version() >= MIN_MYSQL_CLIENT_VERSION, 
				"TrinityCore does not support MySQL version below 5.1");
		wp_fatal(mysql_get_client_version() == MYSQL_VERSION_ID, 
				"Used MySQL library version (%s) does not match the version used to compile TrinityCore (%s). Search on forum for TCE00011",
				mysql_get_client_info(), MYSQL_SERVER_VERSION);
	}

	~database_worker_pool()
	{
		queue_->canel();
	}

	void set_connection_infO(const std::string& info_string, const uint8_t async_threads, const uint8_t synch_threads)
	{
		connection_info_ = std::make_unique<mysql_connection_info>(info_string);
		async_threads_ = async_threads;
		synch_threads_ = synch_threads;
	}

	uint32_t open()
	{
		wp_tatal(connection_info_.get(), "Connection info was not set!");

		LOG_INFO << "sql.driver. Opening Database pool " << get_database_name() << ". Asynchronous connections: " <<
			async_threads_ << ", synchronous connections: " << synch_threads_;

		uint32_t error = open_connections(IDX_ASYNC, async_threads_);
		if(error)
		{
			return error;
		}

		error = open_connections(IDX_SYNCH, synch_threads_);
		if(!error)
		{
			LOG_INFO << "sql.driver. Database pool " << get_database_name() << " opened successfully. " <<
				connections_[IDX_SYNCH].size() + connections_[IDX_ASYNC].size() << " total connections running.";
		}
		return error;
	}

	void close()
	{
		LOG_INFO << "sql.driver. Closing down database pool " << get_database_name();

		// 关闭实际的MySQL连接。
		connection_[IDX_ASYNC].clear();

		LOG_INFO << "sql.driver. Asynchronous connections on database pool '" << get_database_name() << 
			"' terminated. Proceeding with synchronous connections";

		// 关闭同步连接！ 无需锁定连接，因为DatabaseWorkerPool <> :: Close！ 仅应在内核中的任何其他线程任务退出后才调用！ 表示此时无法进行并发访问。
		connections_[IDX_SYNCH].clear();

		LOG_INFO << "sql.driver. All connections on database pool '" << get_database_name() << "'";
	}


	// 准备所有准备好的语句
	bool prepare_statements()
	{
		for(auto& connections : connections_)
		{
			for(auto& i : connections)
			{
				i->lock_if_ready();
				if(!i->prepare_statements())
				{
					i->unlock();
					close();
					return false;
				}
				else
				{
					i->unlock();
				}
			}
		}
	}


	inline mysql_connection_info const* get_connection_info() const
	{
		return connection_info_.get();
	}

	const char* get_database_name() const
	{
		return connection_info_->database.c_str();
	}

	// 以字符串格式排队将单向执行的SQL操作，该操作将异步执行。 ！ 此方法仅应用于仅执行一次（例如在启动期间）的查询。
	void execute(const char* sql)
	{
		if(trinity::is_format_empty_or_null(sql))
		{
			return;
		}
		basic_statement_task* task = new basic_statement_task(sql);
		enqueue(task);
	}

	// ！ 使具有变量args的字符串格式的单向SQL操作入队，该操作将异步执行。 ！ 此方法仅应用于仅执行一次（例如在启动期间）的查询。
	template<typename F, typename... Args>
	void p_execute(F&& sql, Args&&... args)
	{
		if(trinity::is_format_empty_or_null(sql))
		{
			return;
		}
		execute(trinity::string_format(std::forware<F>(sql, std::forward<Args>(args)...).c_str()));
	}

	// ！ 以准备好的语句格式排队单向SQL操作，该操作将异步执行。 ！ 语句必须带有CONNECTION_ASYNC标志。
	void execute(prepared_statement<T>* stmt)
	{
		prepared_statement_task* task = new prepared_statement_task(stmt);
		enqueue(task);
	}

	// 直接执行字符串格式的单向SQL操作，该操作将阻塞调用线程，直到完成。 ！ 此方法仅应用于仅执行一次（例如在启动期间）的查询。
	void direct_execute(const char* sql)
	{
		if(trinity::is_format_empty_or_null(sql))
		{
			return;
		}
		T* connection = get_free_connection();
		connection->execute(sql);
		connection->unlock();
	}

	// 使用变量args-直接以字符串格式执行单向SQL操作，该操作将阻塞调用线程，直到完成。 ！ 此方法仅应用于仅执行一次（例如在启动期间）的查询。
	template<typename F, typename... Args>
	void direct_execute(F&& sql, Args&&... args)
	{
		if(trinity::is_format_empty_or_null(sql))
		{
			return;
		}
		direct_execute(trinity::string_format(std::forward<F>(sql), std::forward<Args>(args)...).c_str());
	}

	// 以准备好的语句格式直接执行单向SQL操作，该操作将阻塞调用线程，直到完成。 ！ 语句必须带有CONNECTION_SYNCH标志。
	void direct_execute(prepared_statement<T>* stmt)
	{
		T* connection = get_free_connection();
		connection->execute(stmt);
		connection->unlock();
		//
		delete stmt;
	}

	// 直接执行字符串格式的SQL查询，该查询将阻塞调用线程，直到完成。 ！ 返回引用计数的自动指针，无需在上层代码中进行手动内存管理。
	query_result query(const char* sql, T* connection = nullptr)
	{
		if(!connection)
		{
			connection = get_free_connection();
		}
		result_set* result = connection->query(sql);
		connection->unlock();
		if(!result || !result->get_row_count() || !result->next_row())
		{
			delete result;
			return query_result(NULL);
		}
		return query_result(result);
	}

	// 使用变量args直接以字符串格式执行SQL查询，该查询将阻塞调用线程，直到完成。 ！ 返回引用计数的自动指针，无需在上层代码中进行手动内存管理。
	template<typename F, typename... Args>
	query_result p_query(F&& sql, T* conn, Args&&... args)
	{
		if(trinity::is_format_empty_or_null(sql))
		{
			return query_result(nullptr);
		}
		return query(trinity::string_format(std::forward<F>(sql), std::forward<Args>(args)...).c_str(), conn);
	}

	// 使用变量args直接以字符串格式执行SQL查询，该查询将阻塞调用线程，直到完成。 ！ 返回引用计数的自动指针，无需在上层代码中进行手动内存管理。
	template<typename F, typename... Args>
	query_result p_query(F&& sql, Args&& args)
	{
		if(trinity::is_format_empty_or_null(sql))
		{
			return query_result(nullptr);
		}
		return query(trinity::string_format(std::forward<F>(sql), std::forward<Args>(args)...).c_str());
	}

	// 以准备好的格式直接执行SQL查询，该查询将阻塞调用线程，直到完成。 ！ 返回引用计数的自动指针，无需在上层代码中进行手动内存管理。 ！ 语句必须带有CONNECTION_SYNCH标志。
	prepared_query_result query(prepared_statement<T>* stmt)
	{
		auto connection = get_fre_connection();
		prepared_result_set* ret = connection->query(stmt);
		connection->unlock();

		//
		delete stmt;

		if(!ret || !ret->get_row_count())
		{
			delete ret;
			return prepared_query_result(NULL);
		}
		return prepared_query_result(ret);
	}

	// 以字符串格式使查询排队，该查询将在执行查询后立即设置QueryResultFuture返回对象的值。 ！ 然后，在ProcessQueryCallback方法中处理返回值。
	query_callback async_query(const char* sql)
	{
		basic_statement_task* task = new basic_statement_task(sql, true);
		//
		query_result_future result = task->get_future();
		enqueue(task);
		return query_callback(std::move(result));
	}

	// 使查询以准备好的格式排队，该格式将在查询执行后立即设置PreparedQueryResultFuture返回对象的值。 ！ 然后，在ProcessQueryCallback方法中处理返回值。 ！ 语句必须带有CONNECTION_ASYNC标志。
	query_callback async_query(prepared_statement<T>* stmt)
	{
		prepared_statement_task* task = new prepared_statement_task(stmt, true);
		//
		prepared_query_result_future result = task->get_future();
		enqueue(task);
		return query_callback(std::move(result));
	}

	// 排队一个将设置QueryResultHolderFuture值的SQL操作向量（既可以是临时的也可以是已准备好的）！ 查询执行后立即返回对象。 ！ 然后，在ProcessQueryCallback方法中处理返回值。 ！ 添加到此持有人的任何准备好的语句都需要使用CONNECTION_ASYNC标志进行准备。
	query_result_holder_future delay_query_holder(sql_query_holder<T>* holder)
	{
		sql_query_holder_task* task = new sql_query_holder_task(holder);
		//
		query_result_holder_future result = task->get_future();
		enqueue(task);
		return result;
	}

	// 开始一个自动管理的事务指针，如果未提交，它将自动回滚。 （自动提交= 0）
	sql_transaction<T> begin_transaction()
	{
		return std::make_shared<transaction<T>>();
	}

	// 使单向SQL操作的集合入队（既可以即席又可以准备）。 这些操作的顺序！ 被附加到事务将在执行期间得到尊重。
	void commit_transaction(sql_transaction<T> transaction)
	{
		enqueue(new transaction_task(transaction));
	}

	// 直接执行单向SQL操作的集合（既可以即席又可以准备）。 这些操作的顺序！ 被附加到事务将在执行期间得到尊重。
	void direct_commit_transaction(sql_transaction<T>& transaction)
	{
		T* connection = get_free_connection();
		int error_code = connection->execute_transaction(transaction);
		if(!error_code)
		{
			connection->unlock(); //
			return;
		}

		//
		if(error_code == ER_LOCK_DEADLOCK)
		{
			uint8_t loop_breaker = 5;
			for(uint8_t i = 0; i < loop_breaker; ++i)
			{
				if(!connection->execute_transaction(transaction))
				{
					break;
				}
			}
		}
		//
		transaction->cleanup();
		connection->unlock();
	}

	// 用于在不同上下文中执行即席语句的方法。 ！ 如果存在有效对象，将包装在事务中，否则独立执行。
	void execute_or_append(sql_transaction<T>& trans, const char* sql)
	{
		if(!trans)
		{
			execute(sql);
		}
		else
		{
			trans->append(sql);
		}
	}

	// 用于在不同上下文中执行准备好的语句的方法。 ！ 如果存在有效对象，将包装在事务中，否则独立执行。
	void execute_or_append(sql_transaction<T>& trans, prepared_statement<T>* stmt)
	{
		if(!trans)
		{
			execute(stmt);
		}
		else
		{
			trans->append(stmt);
		}
	}

	//
	typedef typename T::statements prepared_statement_index;

	// 自动管理（内部）指向准备好的语句对象的指针，以供在上层代码中使用。 ！ 在this-> DirectExecute（PreparedStatement *），this-> Query（PreparedStatement *）或PreparedStatementTask ::〜PreparedStatementTask中删除指针。 ！ 在执行之前，该对象尚未绑定到MySQL上下文上的prepared语句。
	prepared_statement<T>* get_prepared_statement(prepared_statement_index index)
	{
		return new prepared_statement<T>(index);
	}

	// 为当前的排序规则应用转义字符串。 （utf8）
	void escape_string(std::string& str)
	{
		if(str.empty())
		{
			return;
		}

		char* buf = new char[str.size() * 2 + 1];
		escape_string(buf, str.c_str(), uint32_t(str.size()));
		str = buf;
		delete buf;
	}

	// 使我们所有的MySQL连接保持活动状态，防止服务器断开我们的连接。
	void keep_alive()
	{
		//
		for(auto& i : connections_[IDX_SYNCH])
		{
			if(i->lock_if_ready())
			{
				i->ping();
				i->unlock();
			}
		}
		//
		auto count = connections_[IDX_ASYNC].size();
		for(uint8_t i = 0; i < count; ++i)
		{
			enqueue(new ping_operation);
		}
	}
private:
	enum internal_index
	{
		IDX_ASYNC,
		IDX_SYNCH,
		IDX_SIZE
	};

	uint32_t open_connections(internal_index type, uint8_t num_connections)
	{
		for(uint8_t i = 0; i < num_connections; ++i)
		{
			//
			auto connection = [&]
				{
					switch(type)
					{
						case IDX_ASYNC:
							return trinity::make_unique<T>(queue_.get(), *connection_info_);
						case IDX_SYNCH:
							return trinity::make_unique<T>(*connection_info_);
						default:
							ABORT();
					}
				}();

			if(uint32_t error = connection->open())
			{
				//
				connections_[type].clear();
				return error;
			}
			else if(mysql_get_server_version(connection->get_handle()) < MIN_MYUSQL_SERVER_VERSION)
			{
				LOG_ERROR << "sql.driver. TrinityCore does not support MySQL versions below 5.1";
				return 1;
			}
			else
			{
				connections_[type].push_back(std::move(connection));
			}
		}
		//
		return 0;
	}

	unsigned long escape_string(char* to, const char* from, unsigned long length)
	{
		if(!to || !from || !length)
		{
			return 0;
		}
		return mysql_real_escape_string(connections_[IDX_SYNCH].front()->get_handle(), to, from, length);
	}

	void enqueue(sql_operation* op)
	{
		queue_->push(op);
	}

	// 在同步连接池中获取免费连接。 ！ 调用者在触摸MySQL上下文后必须调用t-> Unlock（）以防止死锁。
	T* get_free_connection() const
	{
		uint8_t i = 0;
		auto num_cons = connections_[IDX_SYNCH].size();
		T* connection = NULL;
		//
		while(true)
		{
			connection = connections_[IDX_SYNCH][i++ % num_cons].get();
			//
			if(connection->lock_if_ready())
			{
				break;
			}
		}
		return connection;
	}

	// 异步工作线程共享的队列。
	std::unique_ptr<producer_consumer_queue<sql_operation*>> queue_;
	std::array<std::vector<std::unique_ptr<T>>, IDX_SIZE> connections_;
	std::unique_ptr<mysql_connection_info> connection_info_;
	uint8_t async_threads_;
	uint8_t synch_threads_;
};

#endif
