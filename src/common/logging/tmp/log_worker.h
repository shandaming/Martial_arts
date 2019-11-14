/*
 * Copyright (C) 2018
 */

#ifndef LOG_WORKER_H
#define LOG_WORKER_H

#include <string>
#include <vector>
#include <thread>
#include <functional>
#include <condition_variable>
#include <mutex>

typedef std::function<void()> log_task;

constexpr int maximum_tasks = 8192;

template<int Size>
struct log_task_list
{
	log_task_list() { tasks_.reserve(SIZE); }
	~log_task_list() { clear(); }

	void push(const log_task& task)
	{
		if (avail())
			tasks_.push_back(task);
	}

	void execute()
	{
		for(auto& t : tasks)
		{
			t();
		}
	}

	bool avail() const { return (SIZE - tasks_.size()) > 0; }
	void clear() { tasks_.clear(); }

	std::vector<log_task> tasks;
};

class log_worker
{
public:
	static log_worker* instance();

	void add_log_task(const log_task& task);

	void working()
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
	log_worker();

	~log_worker()
	{
		if (running_)
		{
			stop_working();
		}
	}

	log_worker(const log_worker&) = delete;  
	log_worker& operator=(const log_worker&) = delete;

	void thread_func();

	typedef log_task_list<large_buffer> buffer;
	typedef std::vector<std::unique_ptr<buffer>> buffer_vector;
	typedef buffer_vector::value_type buffer_ptr;

	const int flush_interval_ = 3;
	bool running_ = false;
  
	std::unique_ptr<std::thread> thread_;
	std::mutex mutex_;
	std::condition_variable cond_;

	buffer_ptr current_buffer_;
	buffer_ptr next_buffer_;
	buffer_vector buffers_;
};

#define ASYNC_LOG lg::log_worker::instance()

#endif
