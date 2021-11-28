/*
 * Copyright (C) 2019
 */

#include <cstring>

#include "adhoc_statement.h"
#include "mysql_connection.h"

basic_statement_task::basic_statement_task(const char* sql, bool async) :
	result_(nullptr)
{
	sql_ = strdup(sql);
	has_result_ = async; // 如果操作是异步，则会有个result
	if(async)
		result_ = new query_result_promise();
}

basic_statement_task::~basic_statement_task()
{
	free((void*)sql_);
	if(has_result_ && result_ != nullptr)
		delete result_;
}

bool basic_statement_task::execute()
{
	if(has_result_)
	{
		result_set* result = conn->query(sql_);
		if(!result || !result->get_row_count() || !result->next_row())
		{
			delete result;
			result_->set_value(query_result(NULL));
			return false;
		}

		result_->set_value(query_result(result));
		return true;
	}
	return conn->execute(sql_);
}
