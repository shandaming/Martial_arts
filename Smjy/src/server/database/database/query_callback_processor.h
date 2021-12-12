/*
 * Copyright (C) 2019
 */

#ifndef DB_QUERY_CALLBACK_PROCESSOR_H
#define DB_QUERY_CALLBACK_PROCESSOR_H

#include "query_callback.h"

class query_callback_processor
{
public:
	query_callback_processor();
	~query_callback_processor();

	void add_query(query_callback&& query);

	void process_ready_queries();
private:
	query_callback_processor(const query_callback_processor&) = delete;
	query_callback_processor& operator=(const query_callback_processor&) = delete;

	std::vector<query_callback> callbacks_;
};

#endif
