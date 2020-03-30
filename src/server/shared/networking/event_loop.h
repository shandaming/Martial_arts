/*
 * Copyright (C) 2018
 */

#ifndef NET_EVENT_LOOP_H
#define NET_EVENT_LOOP_H

#include <mutex>

#include "epoll.h"
#include "timer.h"
#include "channel.h"
#include "timer_queue.h"

class event_loop
{
public:
	typedef std::function<void()> functor;

  	event_loop();
  	~event_loop();  // force out-line dtor, for scoped_ptr members.

	event_loop(const event_loop&) = delete;
	event_loop& operator=(const event_loop&) = delete;

	//永远循环。必须在与创建对象相同的线程中调用。
  	void loop();

	void quit();

	int64_t iteration() const { return iteration_; }

	size_t queue_size();

	void run_in_loop(functor&& cb);
	void queue_in_loop(functor&& cb);

	void cancel(Timer_id timerId);

	Timer_id run_at(const Timestamp& time, Timer_callback&& cb);
	Timer_id run_after(double delay, Timer_callback&& cb);
	Timer_id run_every(double interval, Timer_callback&& cb);

	// 内部使用
	void wakeup();
	void update_channel(channel* channel);
	void remove_channel(channel* channel);
	bool has_channel(channel* channel);

	void assert_in_loop_thread();

  	bool is_in_loop_thread() const 
	{
		return thread_id_ == get_current_thread_id(); 
	}

	bool event_handling() const { return event_handling_; }

	static event_loop* get_event_loop_of_current_thread();
private:
	void handle_read();  // waked up
	void do_pending_functors();

  	void print_active_channels() const; // DEBUG

  	typedef std::vector<channel*> channel_list;

  	bool looping_; /* atomic */
  	bool quit_; /* atomic and shared between threads, okay on x86, I guess. */
  	bool event_handling_; /* atomic */
  	bool calling_pending_functors_; /* atomic */
  	const int thread_id_;
  	std::unique_ptr<epoll> epoll_;
  	std::unique_ptr<timer_queue> timer_queue_;
  	int wakeup_fd_;

  	std::unique_ptr<channel> wakeup_channel_;

  	// scratch variables
  	channel_list active_channels_;
  	channel* current_active_channel_;

  	std::mutex mutex_;
	std::vector<functor> pending_functors_; // @GuardedBy mutex_
};

#endif
