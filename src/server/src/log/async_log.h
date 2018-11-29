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

#include "log/log_stream.h"

namespace lg
{
class Count_down_latch
{
public:
	explicit Count_down_latch(int count);

	void wait();

	void count_down();

	int get_count() const;
private:
	mutable std::mutex mutex_;
	std::condition_variable condition_;
	std::atomic<int> count_;
};

class Async_log
{
public:
	Async_log(const std::string& basename, off_t roll_size, 
			int flush_interval = 3);

	~Async_log()
	{
		if (running_)
		{
			stop();
		}
	}

	void append(const char* logline, int len);

	void start()
	{
		running_ = true;
		//thread_.start();
		latch_.wait();
	}

	void stop()
	{
		running_ = false;
		cond_.notify_one();
		thread_->join();
	}
private:
	Async_log(const Async_log&) = delete;  
	Async_log& operator=(const Async_log&) = delete;

	void thread_func();

	typedef detail::Log_buffer<detail::large_buffer> Buffer;
	typedef std::vector<std::unique_ptr<Buffer>> Buffer_vector;
	typedef Buffer_vector::value_type Buffer_ptr;

	const int flush_interval_;
	bool running_;
	std::string basename_;
	off_t roll_size_;
  
	Count_down_latch latch_;

	std::thread* thread_;
	std::mutex mutex_;
	std::condition_variable cond_;

	Buffer_ptr current_buffer_;
	Buffer_ptr next_buffer_;
	Buffer_vector buffers_;
};

#define LOG_INIT(filename, roll_size) static Async_log async_log(filename, roll_size);
}

#endif
