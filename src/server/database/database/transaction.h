/*
 * Copyright (C) 2019
 */

#ifndef TRANSACTION_H
#define TRANSACTIOn_H

#include "sql_operation.h"
#include "common/serialization/string_utils.h"

class prepared_statement_base;
template<typename T>
class prepared_statement;
struct sql_element_data;

class transaction_base
{
	friend class transaction_task;
	friend class mysql_connection;

	template<typename T>
	friend class database_worker_pool;
public:
	transaction_base() : cleaned_up_(false) {}

	virtual ~transaction_base() { cleanup(); }

	void append(const char* sql);

	template<typename F, typename... Args>
	void append(F&& sql, Args&&... args)
	{
		append(string_format(std::forward<F>(sql), std::forward<Args>(args)...).c_str());
	}

	size_t get_size() const { return queries_.size(); }
protected:
	void append_prepared_statement(prepared_statement_base* statement);
	void cleanup();
	std::vector<sql_element_data> queries_;
private:
	bool cleaned_up_;
};

template<typename T>
class transaction : public transaction_base
{
public:
	using transaction_base::append;

	void append(prepared_statement<T>* statement)
	{
		append_prepared_statement(statement);
	}
};

template<typename T>
using sql_transaction = std::shared_ptr<transaction<T>>;

class transaction_task : public sql_operation
{
	template<typename T>
	friend class database_worker_pool;

	friend class database_worker;
public:
	transaction_task(std::shared_ptr<transaction_base> trans) : trans_(trans) {}
	~transaction_task() {}
protected:
	bool execute() override;

	std::shared_ptr<transaction_base> trans_;
	static std::mutex dead_lock_lock_;
};

#endif
