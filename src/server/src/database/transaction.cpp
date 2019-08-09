/*
 * Copyright (C) 2019
 */

#include "transaction.h"

std::mutex transaction_task::dead_lock_lock;

//
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
	//
	if(cleaned_up_)
	{
		return;
	}
	for(auto& i : queries_)
	{
		switch(i.type)
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
	int error_code = conn_->execute_transaction(trans_);
	if(!error_code)
	{
		return true;
	}

	if(error_code == ER_LOCK_DEADLOCK)
	{
		//
		std::lock_guard<std::mutex> lock(dead_lock_lock_);
		uint8_t loop_breaker = 5;//
		for(uint8_t i = 0; i < loop_breaker; ++i)
		{
			if(conn_->execute_transaction(trans_))
			{
				return true;
			}
		}
	}
	trans_->cleanup();
	return false;
}
