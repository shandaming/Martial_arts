/*
 * Copyright (C) 2019
 */

#include "query_holder.h"

bool sql_query_holder_base::set_prepared_query_imp(size_t index, prepared_statement_base* stmt)
{
	if(queries.size() <= index)
	{
		LOG_ERROR << "sql.sql. Query index (" << index << ") out of range (size:" << queries.size() << ") for prepared statement.";
		return false;
	}
	queries_[index].first = stmt;
	return true;
}

prepared_query_result sql_query_holder_base::get_prepared_result(size_t index)
{
	//
	if(index < queries.size())
	{
		return queries_[index].second;
	}
	else
	{
		return prepared_query_result(nullptr);
	}
}

void sql_query_holder_base::set_prepared_result(size_t index, prepared_result_set* result)
{
	if(result && !result->get_row_count())
	{
		delete result;
		result = nullptr;
	}
	//
	if(index < queries_.size())
	{
		queries_[index].second = prepared_query_result(result);
	}
}

sql_query_holder_base::~sql_query_holder_base()
{
	for(size_t i = 0; i < queries_.size(); ++i)
	{
		//
		delete queries_[i].first;
	}
}

void sql_query_holder_base::set_size(size_t size)
{
	//
	queries_.resize(size);
}

sql_query_holder_task::~sql_query_holder_task()
{
	if(!executed_)
	{
		delete holder_;
	}
}

bool sql_query_holder_task::execute()
{
	executed_ = true;
	if(!holder_)
	{
		return false;
	}

	//
	for(size_t i = 0; i < holder_->queries_.size(); ++i)
	{
		if(prepared_statement_base* stmt = holder_->queries_[i].first)
		{
			holder_->set_prepared_result(i, conn->query(stmt));
		}
	}
	result_.set_value(holder_);
	return true;
}
