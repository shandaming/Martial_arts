/*
 * Copyright (C) 2018 
 */

#ifndef NET_EVENT_LOOP_THREADPOOL_H
#define NET_EVENT_LOOP_THREADPOOL_H

#include "event_loop_thread.h"

namespace net
{
	class Event_loop_threadpool
	{
		public:
			typedef std::function<void(Event_loop*)> Thread_init_callback;

			Event_loop_threadpool(Event_loop* baseLoop);
			~Event_loop_threadpool();
			void set_thread_num(size_t num_threads) { num_threads_ = num_threads; }
			void start(const Thread_init_callback& cb = Thread_init_callback());

  // valid after calling start()
  /// round-robin
			Event_loop* get_next_loop();

  /// with the same hash code, it will always return the same Event_loop
			Event_loop* get_loop_for_hash(size_t hash_code);

			std::vector<Event_loop*> get_all_loops();

			bool started() const { return started_; }
		private:
			Event_loop* base_loop_;
			bool started_;
			size_t num_threads_;
			int next_;
			std::vector<std::unique_ptr<Event_loop_thread>> threads_;
			std::vector<Event_loop*> loops_;
	};
}

#endif
