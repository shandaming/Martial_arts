/*
 * Copyright (C) 2018
 */

#ifndef NET_TIMER_QUEUE_H
#define NET_TIMER_QUEUE_H

#include <set>

#include "channel.h"
#include "timer.h"

class event_loop;

class timer_queue
{
public:
	explicit timer_queue(event_loop* loop);
	~timer_queue();

	timer_queue(const timer_queue&) = delete;
	timer_queue& operator=(const timer_queue&) = delete;

	timer_id add_timer(Timer_callback&& cb, Timestamp when, 
			double interval);

	void cancel(const timer_id& timerId);
private:
	typedef std::pair<Timestamp, timer*> Entry;
	typedef std::set<Entry> Timer_list;
	typedef std::pair<timer*, int64_t> Active_timer;
	typedef std::set<Active_timer> Active_timer_set;

	void add_timer_in_loop(timer* timer);
	void cancel_in_loop(const timer_id& timerId);
	// called when timerfd alarms
	void handleRead();
	// move out all expired timers
	std::vector<Entry> get_expired(Timestamp now);
	void reset(const std::vector<Entry>& expired, Timestamp now);

	bool insert(timer* timer);

	event_loop* loop_;
	const int timerfd_;
	channel timerfd_channel_;
	// timer list sorted by expiration
	Timer_list timers_;

	// for cancel()
	Active_timer_set active_timers_;
	bool calling_expired_timers_; /* atomic */
	Active_timer_set canceling_timers_;
};

#endif
