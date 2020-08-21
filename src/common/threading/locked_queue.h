/*
 * Copyright (C) 2020
 */

#ifndef _LOCKED_QUEUE_H
#define _LOCKED_QUEUE_H

#include <deque>
#include <mutex>

template<typename T, typename StorageType = std::deque<T>>
class locked_queue
{
public:
	locked_queue() : canceled_(false) {}
	virtual ~locked_queue() {}

	void add(const T& item)
	{
		lock();

		queue_.push_back(item);

		unlock();
	}

	template<typename Iterator>
	void readd(Iterator begin, Iterator end)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		queue_.insert(queue_.begin(), begin, end);
	}

	bool next(T& result)
	{
		std::lock_guard<std::mutex> lock(mutex_);

		if(queue_.empty())
			return false;

		result = queue_.front();
		queue_.pop_front();

		return true;
	}

	template<typename Checker>
	bool next(T& result, Checker& check)
	{
		std::lock_guard<std::mutex> lock(mutex_);

		if(queue_.empty())
			return false;

		result = queue_.front();
		if(!check.process(result))
			return false;

		queue_.pop_front();

		return true;
	}

	T& peek(bool auto_unlock = false)
	{
		lock();

		T& result = queue_.front();
		if(auto_unlock)
			unlock();

		return result;
	}

	void cannel()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		canceled_ = true;
	}

	bool cancelled()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return canceled_;
	}

	void lock()
	{
		this->mutex_.lock();
	}

	void unlock()
	{
		this->mutex_->unlock();
	}

	void pop_front()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		queue_.pop_front();
	}

	bool empty() const 
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return queue_.empty();
	}
private:
	std::mutex mutex_;
	StorageType queue_;
	volatile bool canceled_;
};

#endif
