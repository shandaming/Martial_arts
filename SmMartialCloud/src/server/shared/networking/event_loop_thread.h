/*
 * Copyright (C) 2018
 */

#ifndef NET_EVENT_LOOP_THREAD_H
#define NET_EVENT_LOOP_THREAD_H

#include <thread>

class event_loop;

class event_loop_thread
{
public:
	event_loop_thread(event_loop* loop);
	~event_loop_thread();
	event_loop* start_loop();
private:
	event_loop* loop_;
	std::unique_ptr<std::thread> thread_;
};

#endif
