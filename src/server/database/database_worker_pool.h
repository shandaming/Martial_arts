/*
 * Copyright (C) 2019
 */

#ifndef DATABASE_WORKER_POOL_H
#define DATABASE_WORKER_POOL_H

class ping_operation : public sql_operation
{
	//
	bool execute() override
	{
		conn_->ping();
		return true;
	}
};

template<typename T>
class database_worker_pool
{
public:
	database_worker_pool() : queue_(new producer_consumer_queue<sql_operation*>()), async_threads_(0), synch_threads_(0)
{
	wp_fatal(mysql_thread_safe(), "Used MySQL library isn't thread-safe");
	wp_fatal(mysql_get_client_version() >= MIN_MYSQL_CLIENT_VERSION, "TrinityCore does not support MySQL version below 5.1");
	wp_fatal(mysql_get_client_version() == MYSQL_VERSION_ID, "Used MySQL library version (%s) does not match the version used to compile TrinityCore (%s). Search on forum for TCE00011", mysql_get_client_info(), MYSQL_SERVER_VERSION);
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

	LOG_INFO << "sql.driver. Opening Database pool " << get_database_name() << ". Asynchronous connections: " << async_threads_ << ", synchronous connections: " << synch_threads_;

	uint32_t error = open_connections(IDX_ASYNC, async_threads_);
	if(error)
	{
		return error;
	}

	error = open_connections(IDX_SYNCH, synch_threads_);
	if(!error)
	{
		LOG_INFO << "sql.driver. Database pool " << get_database_name() << " opened successfully. " << connections_[IDX_SYNCH].size() + connections_[IDX_ASYNC].size() << " total connections running.";
	}
	return error;
}

	void close()
{
	LOG_INFO << "sql.driver. Closing down database pool " << get_database_name();

	//
	connection_[IDX_ASYNC].clear();

	LOG_INFO << "sql.driver. Asynchronous connections on database pool '" << get_database_name() << "' terminated. Proceeding with synchronous connections";

	//
	connections_[IDX_SYNCH].clear();

	LOG_INFO << "sql.driver. All connections on database pool '" << get_database_name() << "'";
}


	//
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


	inline const* mysql_connection_info get_connection_info() const
	{
		return connection_info_.get();
	}

const char* database_worker_pool<T>::get_database_name() const
{
	return connection_info_->database.c_str();
}

	//
	void execute(const char* sql)
{
	if(trinity::is_format_empty_or_null(sql))
	{
		return;
	}
	basic_statement_task* task = new basic_statement_task(sql);
	enqueue(task);
}

	//
	template<typename F, typename... Args>
	void p_execute(F&& sql, Args&&... args)
	{
		if(trinity::is_format_empty_or_null(sql))
		{
			return;
		}
		execute(trinity::string_format(std::forware<F>(sql, std::forward<Args>(args)...).c_str()));
	}

	//
	void execute(prepared_statement<T>* stmt)
{
	prepared_statement_task* task = new prepared_statement_task(stmt);
	enqueue(task);
}

	//
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

	//
	template<typename F, typename... Args>
	void direct_execute(F&& sql, Args&&... args)
	{
		if(trinity::is_format_empty_or_null(sql))
		{
			return;
		}
		direct_execute(trinity::string_format(std::forward<F>(sql), std::forward<Args>(args)...).c_str());
	}

	//
	void direct_execute(prepared_statement<T>* stmt)
{
	T* connection = get_free_connection();
	connection->execute(stmt);
	connection->unlock();
	//
	delete stmt;
}

	//
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

	//
	template<typename F, typename... Args>
	query_result p_query(F&& sql, T* conn, Args&&... args)
	{
		if(trinity::is_format_empty_or_null(sql))
		{
			return query_result(nullptr);
		}
		return query(trinity::string_format(std::forward<F>(sql), std::forward<Args>(args)...).c_str(), conn);
	}

	//
	template<typename F, typename... Args>
	query_result p_query(F&& sql, Args&& args)
	{
		if(trinity::is_format_empty_or_null(sql))
		{
			return query_result(nullptr);
		}
		return query(trinity::string_format(std::forward<F>(sql), std::forward<Args>(args)...).c_str());
	}

	//
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

	//
	query_callback async_query(const char* sql)
{
	basic_statement_task* task = new basic_statement_task(sql, true);
	//
	query_result_future result = task->get_future();
	enqueue(task);
	return query_callback(std::move(result));
}

	//
	query_callback async_query(prepared_statement<T>* stmt)
{
	prepared_statement_task* task = new prepared_statement_task(stmt, true);
	//
	prepared_query_result_future result = task->get_future();
	enqueue(task);
	return query_callback(std::move(result));
}

	//
	query_result_holder_future delay_query_holder(sql_query_holder<T>* holder)
{
	sql_query_holder_task* task = new sql_query_holder_task(holder);
	//
	query_result_holder_future result = task->get_future();
	enqueue(task);
	return result;
}

	//
	sql_transaction<T> begin_transaction()
{
	return std::make_shared<transaction<T>>();
}

	//
	void commit_transaction(sql_transaction<T> transaction)
{
	enqueue(new transaction_task(transaction));
}

	//
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

	//
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

	//
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

	//
	prepared_statement<T>* get_prepared_statement(prepared_statement_index index)
{
	return new prepared_statement<T>(index);
}

	//
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

	//
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

	//
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

	//
	std::unique_ptr<producer_consumer_queue<sql_operation*>> queue_;
	std::array<std::vector<std::unique_ptr<T>>, IDX_SIZE> connections_;
	std::unique_ptr<mysql_connection_info> connection_info_;
	uint8_t async_threads_;
	uint8_t synch_threads_;
};

#endif
