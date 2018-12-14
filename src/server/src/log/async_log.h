/*
 * Copyright (C) 2018
 */

#ifndef LOG_ASYNC_LOG_H
#define LOG_ASYNC_LOG_H

#include <string>
#include <atomic>
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>

#include "log/log_buffer.h"

namespace lg
{
#if 0
class Count_down_latch
{
public:
	explicit Count_down_latch(int count) : count_(count) {}

	void wait()
{
	std::unique_lock<std::mutex> lock(mutex_);
	condition_.wait(lock, [this]{ return count_ > 0; });
}

	void count_down()
{
	--count_;
	if (count_ == 0)
	{
		condition_.notify_all();
	}
}

	int get_count() const { return count_; }
private:
	mutable std::mutex mutex_;
	std::condition_variable condition_;
	std::atomic<int> count_;
};

#endif

class Async_log
{
public:
	static Async_log* instance();

	void init(const std::string& filename, off_t roll_size, 
			int flush_interval = 3);

	void append(const char* logline, int len);

	void start()
	{
		running_ = true;
		thread_ = std::make_unique<std::thread>([this]{ this->thread_func(); });
		//latch_.wait();
	}

	void stop()
	{
		running_ = false;
		cond_.notify_one();
		thread_->join();
	}
private:
	Async_log();

	~Async_log()
	{
		if (running_)
		{
			stop();
		}
	}

	Async_log(const Async_log&) = delete;  
	Async_log& operator=(const Async_log&) = delete;

	void thread_func();

	typedef Log_buffer<large_buffer> Buffer;
	typedef std::vector<std::unique_ptr<Buffer>> Buffer_vector;
	typedef Buffer_vector::value_type Buffer_ptr;

	const int flush_interval_ = 3;
	bool running_;
	std::string filename_;
	off_t roll_size_ = 0;
  
	//Count_down_latch latch_;

	std::unique_ptr<std::thread> thread_;
	std::mutex mutex_;
	std::condition_variable cond_;

	Buffer_ptr current_buffer_;
	Buffer_ptr next_buffer_;
	Buffer_vector buffers_;
};
}

#define ASYNC_LOG lg::Async_log::instance()

#endif
