/*
 * Copyright (C) 2019
 */

#ifndef ADHOC_STATEMENT_H
#define ADHOC_STATEMENT_H

#include "sql_operation.h"

class basic_statement_task : public sql_operation
{
public:
	basic_statement_task(const char* sql, bool async = false);
	~basic_statement_task();

	bool execute() override;
	query_result_future get_future() const { return result_->get_future(); }
private:
	const char* sql_;
	bool has_result_;
	query_result_promise* result_;
};

#endif
