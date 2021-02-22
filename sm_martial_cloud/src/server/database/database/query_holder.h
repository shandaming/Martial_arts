/*
 * Copyright (C) 2019
 */

#ifndef DB_QUERY_HOLDER_H
#define DB_QUERY_HOLDER_H

#include <vector>

#include "sql_operation.h"

class sql_query_holder_base
{
	friend class sql_query_holder_task;
public:
	sql_query_holder_base() {}

	virtual ~sql_query_holder_base();

	void set_size(size_t size);

	prepared_query_result get_prepared_result(size_t index);

	void set_prepared_result(size_t index, prepared_result_set* result);
protected:
	bool set_prepared_query_impl(size_t index, prepared_statement_base* stmt);
private:
	std::vector<std::pair<prepared_statement_base*, prepared_query_result>> queries_;
};

template<typename T>
class prepared_statement;

template<typename T>
class sql_query_holder : public sql_query_holder_base
{
public:
	bool set_prepared_query(size_t index, prepared_statement<T>* stmt)
	{
		return set_prepared_query_impl(index, stmt);
	}
};

class sql_query_holder_task : public sql_operation
{
public:
	sql_query_holder_task(sql_query_holder_base* holder) : holder_(holder), executed_(false) {}

	~sql_query_holder_task();

	bool execute() override;

	query_result_holder_future get_future() { return result_.get_future(); }
private:
	sql_query_holder_base* holder_;
	query_result_holder_promise result_;
	bool executed_;
};

#endif
