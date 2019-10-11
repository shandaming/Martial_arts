/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef CONDITION_VARIABLE_H
#define CONDITION_VARIABLE_H

#include "mutex.h"

enum class Status { no_timeout, timeout };

class Condition_variable
{
	private:
		typedef std::chrono::system_clock clock_type;
		typedef pthread_cond_t native_type;
#ifdef PTHREAD_COND_INIT
		native_type cond_ = PTHREAD_COND_INITIALIZER;
#else
		native_type cond_;
#endif
	public:
		typedef native_type* native_handle_type;

		Condition_variable();
		~Condition_variable();

		Condition_variable(const Condition_variable&) = delete;
		Condition_variable& operator=(const Condition_variable&) = delete;

		void notify_one();

		void notify_all();

		void wait(Unique_lock<Mutex>& lock);

		template<typename Predicate>
		void wait(Unique_lock<Mutex>& lock, Predicate p)
		{
			while(!p())
				wait(lock);
		}

		template<typename Duration>
		Status wait_until(Unique_lock<Mutex>& lock, 
			const std::chrono::time_point<clock_type, Duration>& atime)
		{ return wait_until_impl(lock, atime); }

		template<typename Clock, typename Duration>
		Status wait_until(Unique_lock<Mutex>& lock,
			const std::chrono::time_point<Clock, Duration>& atime)
		{	
			const typename Clock::time_point c_entry = Clock::now();
			const clock_type::time_point s_entry = clock_type::now();
			const auto delta = atime - c_entry;
			const auto s_atime = s_entry + delta;

			return wait_until_impl(lock, s_atime);
		}

		template<typename Clock, typename Duration, typename Predicate>
		bool wait_until(Unique_lock<Mutex>& lock, 
			const std::chrono::time_point<Clock, Duration>& atime,
			Predicate p)
		{
			while(!p())
				if(wait_until(lock, atime) == Status::timeout)
					return p();
			return true;
		}	

		template<typename Lock, typename Rep, typename Period>
		Status wait_for(Lock& lock, 
			const std::chrono::duration<Rep, Period>& rtime)
		{
			return wait_until(lock, clock_type::now() + rtime());
		}

		template<typename Lock, typename Rep, typename Period, 
			typename Predicate>
		bool wait_for(Lock& lock, 
			const std::chrono::duration<Rep, Period>& rtime,
			Predicate p)
		{
			return wait_until(lock, clock_type::now() + rtime, 
					std::move(p));
		}

		native_handle_type native_handle() { return &cond_; }
	private:
		template<typename Dur>
		Status wait_until_impl(Unique_lock<Mutex>& lock, 
			const std::chrono::time_point<clock_type, Dur>& atime)
		{
			auto s = 
				std::chrono::time_point_cast<std::chrono::seconds>(atime);
			auto ns = std::chrono::duration_cast<std::chrono::nanoseconds> 
				(atime - s);

			struct timespec ts = 
			{
				static_cast<std::time_t>(s.time_since_epoch().count()),
				static_cast<long>(ns.count())
			};

			pthread_cond_timedwait(&cond_, lock.mutex()->native_handle(),
				&ts);

			return (clock_type::now() < atime ? Status::no_timeout : 
				Status::timeout);
		}
};

void notify_all_at_thread_exit(Condition_variable&, Unique_lock<Mutex>);

struct At_thread_exit_elt
{
	At_thread_exit_elt* next;
	void (*cb)(void*);
};

#endif
