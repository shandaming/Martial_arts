/*
 * Copyright (C) 2019
 */

#include "query_holder.h"

bool sql_query_holder_base::set_prepared_query_imp(size_t index, prepared_statement_base* stmt)
{
	if(queries.size() <= index)
	{
		LOG_ERROR("sql.sql", "Query index (%u) out of range (size:%u) for prepared statement.", index, queries.size());
		return false;
	}
	queries_[index].first = stmt;
	return true;
}

prepared_query_result sql_query_holder_base::get_prepared_result(size_t index)
{
	// 如果索引是预准备语句，则不要调用此函数
	if(index < queries.size())
		return queries_[index].second;
	else
		return prepared_query_result(nullptr);
}

void sql_query_holder_base::set_prepared_result(size_t index, prepared_result_set* result)
{
	if(result && !result->get_row_count())
	{
		delete result;
		result = nullptr;
	}
	// 将结果存储在持有人中
	if(index < queries_.size())
		queries_[index].second = prepared_query_result(result);
}

sql_query_holder_base::~sql_query_holder_base()
{
	for(size_t i = 0; i < queries_.size(); ++i)
	{
		// 如果从未使用过结果，则释放已经使用过的资源结果（称为getresult）将被删除
		delete queries_[i].first;
	}
}

void sql_query_holder_base::set_size(size_t size)
{
	// 要优化push_back，请保留要执行的查询数
	queries_.resize(size);
}

sql_query_holder_task::~sql_query_holder_task()
{
	if(!executed_)
		delete holder_;
}

bool sql_query_holder_task::execute()
{
	executed_ = true;
	if(!holder_)
		return false;

	// 执行持有人中的所有查询并传递结果
	for(size_t i = 0; i < holder_->queries_.size(); ++i)
	{
		if(prepared_statement_base* stmt = holder_->queries_[i].first)
			holder_->set_prepared_result(i, conn->query(stmt));
	}
	result_.set_value(holder_);
	return true;
}
