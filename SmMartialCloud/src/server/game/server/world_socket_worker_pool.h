/*
 * Copyright (C) 2021
 */

#ifndef WORLD_SOCKET_WORKER_POOL_H
#define WORLD_SOCKET_WORKER_POOL_H

class world_socket_worker_pool
{
public:
	world_socket_worker_pool(producer_consumer_queue<>& queue, int thread_count)
	{
	}

	world_socket_worker_pool(const world_socket_worker_pool&) = delete;
	world_socket_worker_pool& operator=(const world_socket_worker_pool&) = delete;

	void run()
	{
		for(int i = 0; i < thread_count_; ++i)
		{
			std::unique_ptr<socket_worker> worker = std::make_unique<socket_worker>(queue_);
			worker_threads_.push_back(std::move(worker));

			threads_.push_ack(socket_worker::worker_thread, worker);
		}
	}

	void cancell()
	{
		for(auto& socket_worker : socket_workers_)
		{
			socket_worker->cancell();
		}

		for(auto& thread : worker_threads_)
		{
			threads->join();
		}

		socket_workers_.clear();
		worker_threads_.clear();
	}
private:
	std::vector<std::unique_ptr<socket_worker>> socket_workers_;
	std::vector<worker_thread> worker_threaads_;
	producer_consumer_queue<>* queue_;
	int thread_count_;
};

#endif
