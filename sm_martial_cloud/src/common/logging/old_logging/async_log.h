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
