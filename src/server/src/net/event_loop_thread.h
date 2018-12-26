/*
 * Copyright (C) 2018
 */

#ifndef NET_EVENT_LOOP_THREAD_H
#define NET_EVENT_LOOP_THREAD_H

#include <mutex>
#include <condition_variable>
#include <thread>

#include "event_loop.h"

namespace net
{
class Event_loop_thread
{
public:
	typedef std::function<void(Event_loop*)> Thread_init_callback;

	Event_loop_thread(const Thread_init_callback& cb = 
			Thread_init_callback());
	~Event_loop_thread();
	Event_loop* start_loop();
private:
	void thread_func();

	Event_loop* loop_;
	bool exiting_;
	std::thread* thread_;
	std::mutex mutex_;
	std::condition_variable cond_;
	Thread_init_callback callback_;
};
}

#endif
