/*
 * Copyright (C) 2019
 */

#include <cstring>
#include <mysql/mysqld_error.h>

#include "transaction.h"
#include "mysql_connection.h"
#include "sql_operation.h"
#include "prepared_statement.h"

std::mutex transaction_task::dead_lock_lock_;

// 将原始查询附加到事务
void transaction_base::append(const char* sql)
{
	sql_element_data data;
	data.type = SQL_ELEMENT_RAW;
	data.element.query = strdup(sql);
	queries_.push_back(data);
}

//
void transaction_base::append_prepared_statement(prepared_statement_base* stmt)
{
	sql_element_data data;
	data.type = SQL_ELEMENT_PREPARED;
	data.element.stmt = stmt;
	queries_.push_back(data);
}

void transaction_base::cleanup()
{
	// 可以通过显式调用Cleanup或自动析构函数来调用
	if(cleaned_up_)
		return;

	for(auto& data : queries_)
	{
		switch(data.type)
		{
			case SQL_ELEMENT_PREPARED:
				delete data.element.stmt;
				break;
			case SQL_ELEMENT_RAW:
				free((void*)(data.element.query));
				break;
			default:
				break;
		}
	}
	queries_.clear();
	cleaned_up_ = true;
}

bool transaction_task::execute()
{
	int error_code = conn->execute_transaction(trans_);
	if(!error_code)
	{
		return true;
	}

	if(error_code == ER_LOCK_DEADLOCK)
	{
		// 确保只有1个异步线程重试一个事务，这样它们就不会彼此死锁
		std::lock_guard<std::mutex> lock(dead_lock_lock_);
		uint8_t loop_breaker = 5;// 处理MySQL错误1213，而无需将死锁扩展到内核本身
		for(uint8_t i = 0; i < loop_breaker; ++i)
		{
			if(conn->execute_transaction(trans_))
			{
				return true;
			}
		}
	}
	trans_->cleanup();
	return false;
}
