/*
 * Copyright (C) 2019
 */

#ifndef DATABASE_WORKER_POOL_H
#define DATABASE_WORKER_POOL_H

template<typename T>
class database_worker_pool
{
public:
	database_worker_pool();
	~database_worker_pool();

	void set_connection_infO(const std::string& info_string, const uint8_t async_threads, const uint8_t synch_threads);

	uint32_t open();
	void close();

	//
	bool prepare_statements();

	inline const* mysql_connection_info get_connection_info() const
	{
		return connection_info_.get();
	}

	//
	void execute(const char* sql);

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
	void execute(prepared_statement<T>* stmt);

	//
	void direct_execute(const char* sql);

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
	void direct_execute(prepared_statement<T>* stmt);

	//
	query_result query(const char* sql, T* connection = nullptr);

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
	prepared_query_result query(prepared_statement<T>* stmt);

	//
	query_callback async_query(const char* sql);

	//
	query_callback async_query(prepared_statement<T>* stmt);

	//
	query_result_holder_future delay_query_holder(sql_query_holder<T>* holder);

	//
	sql_transaction<T> begin_transaction();

	//
	void commit_transaction(sql_transaction<T> transaction);

	//
	void direct_commit_transaction(sql_transaction<T>& transaction);

	//
	void execute_or_append(sql_transaction<T>& trans, const char* sql);

	//
	void execute_or_append(sql_transaction<T>& trans, prepared_statement<T>* stmt);

	//
	typedef typename T::statements prepared_statement_index;

	//
	prepared_statement<T>* get_prepared_statement(prepared_statement_index index);

	//
	void escape_string(std::string& str);

	//
	void keep_alive();
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

	//
	T* get_free_connection() const;

	//
	std::unique_ptr<producer_consumer_queue<sql_operation*>> queue_;
	std::array<std::vector<std::unique_ptr<T>>, IDX_SIZE> connections_;
	std::unique_ptr<mysql_connection_info> connection_info_;
	uint8_t async_threads_;
	uint8_t synch_threads_;
};

#endif
