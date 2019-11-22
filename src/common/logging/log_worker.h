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

typedef std::function<int()> log_task;

constexpr int maximum_tasks = 8192;

template<int Size>
struct log_task_list
{
	log_task_list() { tasks.reserve(Size); }
	~log_task_list() { clear(); }

	void push(const log_task& task)
	{
		if (avail())
			tasks.push_back(task);
	}

	void execute()
	{
		for(auto& t : tasks)
		{
			t();
		}
	}

	size_t size() const { return tasks.size(); }
	bool avail() const { return (Size - tasks.size()) > 0; }
	void clear() { tasks.clear(); }

	std::vector<log_task> tasks;
};

class log_worker
{
public:
	log_worker();
	~log_worker();

	void add_task(const log_task& task);

	void working();
	void stop();
private:
	log_worker(const log_worker&) = delete;  
	log_worker& operator=(const log_worker&) = delete;

	void do_work();

	typedef log_task_list<maximum_tasks> task_list;
	typedef std::vector<std::unique_ptr<task_list>> tasks_list_vector;
	typedef tasks_list_vector::value_type task_list_ptr;

	const int flush_interval_ = 3;
	bool is_working_;
  
	std::unique_ptr<std::thread> thread_;
	std::mutex mutex_;
	std::condition_variable cond_;

	task_list_ptr current_task_list_;
	task_list_ptr next_task_list_;
	tasks_list_vector tasks_list_;
};

#define ASYNC_LOG lg::log_worker::instance()

#endif
