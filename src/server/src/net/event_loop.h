/*
 * Copyright (C) 2018
 */

#ifndef NET_EVENT_LOOP_H
#define NET_EVENT_LOOP_H

#include <functional>
#include <any>
#include <mutex>

#include "poller.h"
#include "timer.h"
#include "channel.h"
#include "timer_queue.h"
#include "common/thread.h"
#include "common/timestamp.h"
#include "common/scoped_ptr.h"

namespace net
{
class Event_loop
{
public:
	typedef std::function<void()> Functor;

  	Event_loop();
  	~Event_loop();  // force out-line dtor, for scoped_ptr members.

	Event_loop(const Event_loop&) = delete;
	Event_loop& operator=(const Event_loop&) = delete;

	//永远循环。必须在与创建对象相同的线程中调用。
  	void loop();

	// 退出循环。
	///
	///如果通过原始指针调用，这不是100％线程安全的，
	///更好地通过shared_ptr <Event_loop>调用100％安全性。
	void quit();

	///轮询返回的时间通常意味着数据到达。
	Timestamp poll_return_time() const { return poll_return_time_; }

	int64_t iteration() const { return iteration_; }

	size_t queue_size();

	void run_in_loop(Functor&& cb);
	void queue_in_loop(Functor&& cb);

	void cancel(Timer_id timerId);

	Timer_id run_at(const Timestamp& time, Timer_callback&& cb);
	Timer_id run_after(double delay, Timer_callback&& cb);
	Timer_id run_every(double interval, Timer_callback&& cb);

	// 内部使用
	void wakeup();
	void update_channel(Channel* channel);
	void remove_channel(Channel* channel);
	bool has_channel(Channel* channel);

	void assert_in_loop_thread()
	{
		if (!is_in_loop_thread())
		{
			abort_not_in_loop_thread();
		}
	}

  	bool is_in_loop_thread() const 
	{
		return thread_id_ == get_current_thread_id(); 
	}

	bool event_handling() const { return event_handling_; }

	void set_context(const std::any& context) { context_ = context; }

	const std::any& get_context() const { return context_; }

	std::any* get_mutable_context() { return &context_; }

	static Event_loop* get_event_loop_of_current_thread();
private:
	void abort_not_in_loop_thread();
	void handle_read();  // waked up
	void do_pending_functors();

  	void print_active_channels() const; // DEBUG

  	typedef std::vector<Channel*> Channel_list;

  	bool looping_; /* atomic */
  	bool quit_; /* atomic and shared between threads, okay on x86, I guess. */
  	bool event_handling_; /* atomic */
  	bool calling_pending_functors_; /* atomic */
  	int64_t iteration_;
  	const pid_t thread_id_;
  	Timestamp poll_return_time_;
  	Scoped_ptr<Poller> poller_;
  	Scoped_ptr<Timer_queue> timer_queue_;
  	int wakeup_fd_;

  	Scoped_ptr<Channel> wakeup_channel_;
  	std::any context_;

  	// scratch variables
  	Channel_list active_channels_;
  	Channel* current_active_channel_;

  	std::mutex mutex_;
	std::vector<Functor> pending_functors_; // @GuardedBy mutex_
};
}

#endif
