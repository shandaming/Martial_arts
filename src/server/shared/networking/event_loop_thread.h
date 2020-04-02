/*
 * Copyright (C) 2018
 */

#ifndef NET_EVENT_LOOP_THREAD_H
#define NET_EVENT_LOOP_THREAD_H

#include <mutex>
#include <condition_variable>
#include <thread>

class event_loop;

class event_loop_thread
{
public:
	event_loop_thread();
	~event_loop_thread();
	event_loop* start_loop();
private:
	void thread_func();

	event_loop* loop_;
	bool exiting_;
	std::unique_ptr<std::thread> thread_;
	std::mutex mutex_;
	std::condition_variable cond_;
};

#endif
