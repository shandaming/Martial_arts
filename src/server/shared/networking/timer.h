/*
 * Copyright (C) 2018
 */

#ifndef NET_TIMER_H
#define NET_TIMER_H

#include <atomic>

#include "common/timestamp.h"

class Timer
{
public:
	Timer(const Timer_callback& cb, Timestamp when, double interval) :
		callback_(cb), expiration_(when), interval_(interval), 
		repeat_(interval > 0.0), sequence_(++num_created_)
	{}

	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;

	void run() const { callback_(); }

	Timestamp expiration() const  { return expiration_; }
	bool repeat() const { return repeat_; }
	int64_t sequence() const { return sequence_; }

	void restart(Timestamp now);

	static int64_t num_created() { return num_created_; }
private:
	const Timer_callback callback_;
	Timestamp expiration_;
	const double interval_;
	const bool repeat_;
	const int64_t sequence_;

	static std::atomic<int64_t> num_created_;
};

class Timer_id
{
public:
	friend class Timer_queue;

	Timer_id() : timer_(nullptr), sequence_(0) {}

	explicit Timer_id(Timer* timer, int64_t sequence) : 
		timer_(timer), sequence_(sequence) {}

	Timer_id(const Timer_id&) = delete;
	Timer_id& operator=(const Timer_id&) = delete;
private:
	Timer* timer_;
	int64_t sequence_;
};

#endif
