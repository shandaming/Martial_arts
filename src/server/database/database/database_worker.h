/*
 * Copyright (C) 2019
 */

#ifndef DB_DATABASE_WORKER_H
#define DB_DATABASE_WORKER_H

#include <atomic>
#include <thread>

#include "sql_operation.h"
#include "common/threading/producer_consumer_queue.h"

class database_worker
{
public:
	database_worker(producer_consumer_queue<sql_operation*>* new_queue, mysql_connection* connection);
	~database_worker();
private:
	database_worker(const database_worker&) = delete;
	database_worker& operator=(const database_worker&) = delete;

	void worker_thread();

	std::thread worker_thread_;
	std::atomic<bool> cancelation_token_;

	producer_consumer_queue<sql_operation*>* queue_;
	mysql_connection* connection_;
};

#endif
