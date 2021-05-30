/*
 * Copyright (C) 2021
 */

#ifndef SERVER_WORKER_THREAD_H
#define SERVER_WORKER_THREAD_H

#include "producer_consumer_queue.h"

template<typename SocketType>
class socket_worker
{
public:
	socket_worker(producer_consumer_queue<sql_operation*>* new_queue) : queue_(new_queue), cancell_(false) {}
	
	void worker_thread()
	{
		if(!queue_)
			return;

		while(true)
		{
			socket_bytes_read sock;
			queue_->wait_and_pop(op);
			if(cancell_ || !op)
			{
				return;
			}

			op->set_connection(connection_);
			op->call();
			delete op;
		}
	}
	
	void cancell() { cancell_ = true; }
private:
	socket_worker(const socket_worker&) = delete;
	socket_worker& operator=(const socket_worker&) = delete;

	producer_consumer_queue<socket_bytes_read>* queue_;
	bool cancell_;
};

#endif
