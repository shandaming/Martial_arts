/*
 * Copyright (C) 2019
 */

#include "database_worker.h"

database_worker::database_worker(producer_consumer_queue<sql_operation*>* new_queue, mysql_connection* connection) : connection_(connection), queue_(new_queue), cancelation_token_(false), worker_thread_(std::thread(&database_worker::worker_thread_, this)) {}

database_worker::~database_worker()
{
	cancelation_token_ = true;
	queue_->cancel();
	worker_thread_.join();
}

void database_worker::worker_thread()
{
	if(!queue_)
	{
		return;
	}

	while(true)
	{
		sql_operation* op = nullptr;
		queue_->wait_and_pop(op);
		if(cancelation_token_ || !op)
		{
			return;
		}

		op->set_connection(connection_);
		op->call();
		delete op;
	}
}
