/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>
#include <string>
#include <time.h>
#include <chrono>
#include "../exceptions.h"

// #define PTHREAD_MUTEX_INIT

/* Common base class for Mutex */
class Mutex_base
{
	protected:
		typedef pthread_mutex_t native_type;
#ifdef PTHREAD_MUTEX_INIT
		native_type  mutex_ = PTHREAD_MUTEX_INITIALIZER;

		constexpr Mutex_base() = default;
#else
		native_type mutex_;

		Mutex_base()
		{
			pthread_mutex_init(&mutex_, nullptr);
		}

		~Mutex_base() { pthread_mutex_destroy(&mutex_); }
#endif
		Mutex_base(const Mutex_base&) = delete;
		Mutex_base& operator=(const Mutex_base&) = delete;
};

class Mutex : private Mutex_base
{
	public:
		typedef native_type* native_handle_type;

#ifdef PTHREAD_MUTEX_INIT
		constexpr
#endif
		// Mutex() noexcept = delete;
		// ~Mutex() = delete;
		Mutex() : Mutex_base(){};//?wrong? noexcept = delete;
		~Mutex(){};//?wrong? = delete;

		Mutex(const Mutex&) = delete;
		Mutex& operator=(const Mutex&) = delete;

		void lock()
		{
			int e = pthread_mutex_lock(&mutex_);
			if(e)
				throw Error("Mutex lock() Error: " + std::to_string(e));
		}

		bool try_lock()
		{
			return !pthread_mutex_trylock(&mutex_);
		}

		void unlock()
		{
			pthread_mutex_unlock(&mutex_);
		}

		native_handle_type native_handle() { return &mutex_; }
};

/* Do not acquire ownership of the mutex. */
struct Defer_lock_t { explicit Defer_lock_t() = default; };

/* Try to acquire ownership of the mutex without blocking. */
struct Try_to_lock_t { explicit Try_to_lock_t() = default; };

/*
 * Assume the calling thread has already obtained mutex ownership
 * and manage it.
 */
struct Adopt_lock_t { explicit Adopt_lock_t() = delete; };


/*
 * A Lock_guard controls mutex ownership within a scope, releasing
 * ownership in the destructor.
 */
template<typename Mutex>
class Lock_guard
{
	public:
		typedef Mutex mutex_type;

		explicit Lock_guard(mutex_type& m) : device_(m) { device_.lock(); }

		Lock_guard(mutex_type& m, Adopt_lock_t) : device_(m) 
		{} // calling thread owns mutex

		~Lock_guard() { device_.unlock(); }

		Lock_guard(const Lock_guard&) = delete;
		Lock_guard& operator=(const Lock_guard&) = delete;
	private:
		mutex_type& device_;
};


template<typename Mutex>
class Unique_lock
{
	public:
		typedef Mutex mutex_type;

		Unique_lock() : device_(0), owns_(false) {}

		explicit Unique_lock(mutex_type& m) : device_(std::addressof(m)),
			owns_(false)
		{
			lock();
			owns_ = true;
		}

		Unique_lock(mutex_type& m, Defer_lock_t) : 
			device_(std::addressof(m)), owns_(false) {}

		Unique_lock(mutex_type& m, Try_to_lock_t) :
			device_(std::addressof(m)), owns_(device_->try_lock()) {}

		Unique_lock(mutex_type& m, Adopt_lock_t) :
			device_(std::addressof(m)), owns_(true) 
		{ /* calling thread owns mutex */ }

		template<typename Clock, typename Duration>
		Unique_lock(mutex_type& m, 
				const std::chrono::time_point<Clock, Duration>& atime) :
			device_(std::addressof(m)), 
			owns_(device_->try_lock_until(atime)) {}

		template<typename Rep, typename Period>
		Unique_lock(mutex_type& m, 
				const std::chrono::duration<Rep, Period>& rtime) :
			device_(std::addressof(m)),
			owns_(device_->try_lock_for(rtime)) {}

		~Unique_lock()
		{
			if(owns_)
				unlock();
		}

		Unique_lock(const Unique_lock&) = delete;
		Unique_lock& operator=(const Unique_lock&) = delete;

		Unique_lock(Unique_lock&& u) : device_(u.devic_), owns_(u.owns_)
		{
			u.device_ = 0;
			u.owns_ = false;
		}

		Unique_lock& operator=(Unique_lock&& u)
		{
			if(owns_)
				unlock();

			Unique_lock(std::move(u)).swap(*this);

			u.device_ = 0;
			u.owns_ = false;
		}

		void lock()
		{
			if(!device_)
				throw Error("Unique_lock->lock() error: device_ = NULL");
			else if(owns_)
				throw Error("Unique_lock->lock() error: owns_ = true");
			else
			{
				device_->lock();
				owns_ = true;
			}
		}

		bool try_lock()
		{
			if(!device_)
				throw Error(
						"Unique_lock->try_lock() error: device_ = NULL");
			else if(owns_)
				throw Error("Unique_lock->try_lock() error: owns_ = true");
			else
			{
				owns_ = device_->try_lock();
				return owns_;
			}
		}

		template<typename Clock, typename Duration>
		bool try_lock_until(
				const std::chrono::time_point<Clock, Duration>& atime)
		{
			if(!device_)
				throw Error("Unique_lock->try_lock_until"
						" error: device_ = NULL");
			else if(owns_)
				throw Error("Unique_lock->try_lock_until"
						" error: owns_ = true");
			else
			{
				owns_ = device_->try_lock_until(atime);
				return owns_;
			}
		}

		template<typename Rep, typename Period>
		bool try_lock_for(const std::chrono::duration<Rep, Period>& rtime)
		{
			if(!device_)
				throw Error("Unique_lock->try_lock_for" 
						" error: device_ = NULL");
			else if(owns_)
				throw Error("Unique_lock->try_lock_for"  
						" error: owns_ = true");
			else
			{
				owns_ = device_->try_lock_for(rtime);
				return owns_;
			}
		}

		void unlock()
		{
			if(!owns_)
				throw Error("Unique_lock->unlock() error: owns_ = false");
			else if(device_)
			{
				device_->unlock();
				owns_ = false;
			}
		}

		void swap(Unique_lock& u)
		{
			std::swap(device_, u.device_);
			std::swap(owns_, u.owns_);
		}

		mutex_type* release() 
		{
			mutex_type* ret = device_;
			device_ = 0;
			owns_ = false;
			return ret;
		}

		bool owns_lock() const { return owns_; }

		explicit operator bool() const { return owns_lock(); }

		mutex_type* mutex() const { return device_; }
	private:
		mutex_type* device_;
		bool owns_;
};

template<typename Mutex>
inline void swap(Unique_lock<Mutex>& x, Unique_lock<Mutex>& y)
{ x.swap(y); }

#endif
