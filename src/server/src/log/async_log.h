/*
 * Copyright (C) 2018
 */

#ifndef LOG_ASYNC_LOG_H
#define LOG_ASYNC_LOG_H

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
	Async_log(const string& basename, off_t roll_size, 
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
		thread_.start();
		latch_.wait();
	}

	void stop()
	{
		running_ = false;
		cond_.notify();
		thread_.join();
	}
private:
	Async_log(const Async_log&) = delete;  
	Async_log& operator=(const Async_log&) = delete;

	void thread_func();

	typedef muduo::detail::FixedBuffer<muduo::detail::large_buffer> Buffer;
	typedef Ptr_vector<Buffer> BufferVector;
	typedef BufferVector::auto_type BufferPtr;

	const int flush_interval_;
	bool running_;
	string basename_;
	off_t roll_size_;
  
	muduo::Count_down_latch latch_;

	std::thread thread_;
	std::mutex mutex_;
	std::condition_variable cond_;

	BufferPtr current_buffer_;
	BufferPtr next_buffer_;
	BufferVector buffers_;
};
}

#endif
