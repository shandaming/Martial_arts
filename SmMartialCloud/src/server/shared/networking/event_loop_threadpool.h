/*
 * Copyright (C) 2018 
 */

#ifndef NET_EVENT_LOOP_THREADPOOL_H
#define NET_EVENT_LOOP_THREADPOOL_H

#include "event_loop_thread.h"

class event_loop_threadpool
{
public:
	event_loop_threadpool(event_loop* baseLoop, uint32_t thread_count);
	~event_loop_threadpool();

	void start();

  // valid after calling start()
  /// round-robin
	event_loop* get_next_loop();

  /// with the same hash code, it will always return the same event_loop
	event_loop* get_loop_for_hash(size_t hash_code);

	std::vector<event_loop*> get_all_loops();

	bool started() const { return started_; }
private:
	event_loop* base_loop_;
	bool started_;
	uint32_t thread_count_;
	int next_;
	std::vector<std::unique_ptr<event_loop_thread>> threads_;
	std::vector<event_loop*> loops_;
};

#endif
