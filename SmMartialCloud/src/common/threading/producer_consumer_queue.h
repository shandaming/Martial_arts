/*
 * Copyright (C) 2019
 */

#ifndef PRODUCER_CONSUMER_QUEUE_H
#define PRODUCER_CONSUMER_QUEUE_H

#include <queue>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <type_traits>

template<typename T>
class producer_consumer_queue
{
public:
	producer_consumer_queue() : shutdown_(false) {}

	void push(const T& v)
	{
		std::lock_guard<std::mutex> lock(mutex_);

		queue_.push(std::move(v));
		cv_.notify_one();
	}

	bool empty()
	{
		std::lock_guard<std::mutex> lock(mutex_);

		return queue_.empty();
	}

	bool pop(T& v)
	{
		std::lock_guard<std::mutex> lock(mutex_);

		if(queue_.empty() || shutdown_)
			return false;

		v = queue_.front();
		queue_.pop();

		return true;
	}

	void wait_and_pop(T& v)
	{
		std::unique_lock<std::mutex> lock(mutex_);

		while(queue_.empty() || shutdown_)
			cv_.wait(lock);

		if(queue_.empty() || shutdown_)
			return;

		v = queue_.front();
		queue_.pop();
	}

	void cancel()
	{
		std::unique_lock<std::mutex> lock(mutex_);

		while(!queue_.empty())
		{
			T& value = queue_.front();
			delete_queue_object(value);
			queue_.pop();
		}

		shutdown_ = true;
		cv_.notify_all();
	}
private:
	template<typename E = T>
	typename std::enable_if<std::is_pointer<E>::value>::type delete_queue_object(E& obj) { delete obj; }

	template<typename E = T>
	typename std::enable_if<std::is_pointer<E>::value>::type delete_queue_object(const E&) {}

	std::mutex mutex_;
	std::condition_variable cv_;
	std::atomic<bool> shutdown_;
	std::queue<T> queue_;
};

#endif
