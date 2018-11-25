/*
 * Copyright (C) 2018
 */

#ifndef NET_TIMER_QUEUE_H
#define NET_TIMER_QUEUE_H

#include <set>

#include "net/channel.h"
#include "net/timer.h"

namespace net
{
class Event_loop;

class Timer_queue
{
public:
	explicit Timer_queue(Event_loop* loop);
	~Timer_queue();

	Timer_queue(const Timer_queue&) = delete;
	Timer_queue& operator=(const Timer_queue&) = delete;

	Timer_id add_timer(Timer_callback&& cb, Timestamp when, 
			double interval);

	void cancel(const Timer_id& timerId);
private:
	typedef std::pair<Timestamp, Timer*> Entry;
	typedef std::set<Entry> Timer_list;
	typedef std::pair<Timer*, int64_t> Active_timer;
	typedef std::set<Active_timer> Active_timer_set;

	void add_timer_in_loop(Timer* timer);
	void cancel_in_loop(const Timer_id timerId);
	// called when timerfd alarms
	void handleRead();
	// move out all expired timers
	std::vector<Entry> get_expired(Timestamp now);
	void reset(const std::vector<Entry>& expired, Timestamp now);

	bool insert(Timer* timer);

	Event_loop* loop_;
	const int timerfd_;
	Channel timerfd_channel_;
	// Timer list sorted by expiration
	Timer_list timers_;

	// for cancel()
	Active_timer_set active_timers_;
	bool calling_expired_timers_; /* atomic */
	Active_timer_set canceling_timers_;
};
}

#endif
