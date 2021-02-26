/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "condition_variable.h"

#ifdef PTHREAD_COND_INIT
Condition_variable::Condition_variable() = default;
#else
Condition_variable::Condition_variable()
{
	pthread_cond_init(&cond_, nullptr);
}
#endif
Condition_variable::~Condition_variable()
{
	pthread_cond_destroy(&cond_);
}

void Condition_variable::notify_one()
{		
	int e = pthread_cond_signal(&cond_);
	if(e)
		throw Error("Condition_variable::notify_one error: " + 
				std::to_string(e));
}

void Condition_variable::notify_all() 
{		
	int e = pthread_cond_broadcast(&cond_);
	if(e)
		throw Error("Condition_variable::notify_all error: " + 
				std::to_string(e));
}

void Condition_variable::wait(Unique_lock<Mutex>& lock)
{
	int e = pthread_cond_wait(&cond_, lock.mutex()->native_handle());
	if(e)
		throw Error("Condition_variable::wait error: " + std::to_string(e));
}

void at_thread_exit(At_thread_exit_elt* elt);

pthread_key_t key;

void run(void* p)
{
	auto elt = (At_thread_exit_elt*)p;
	while(elt)
	{
		auto next = elt->next;
		elt->cb(elt);
		elt = next;
	}
}

void run()
{
	auto elt = (At_thread_exit_elt*)pthread_getspecific(key);
	pthread_setspecific(key, nullptr);
	run(elt);
}

struct Notifier final : At_thread_exit_elt
{
	Notifier(Condition_variable& cv, Unique_lock<Mutex>& l) : cv(&cv),
		mx(l.release())
	{
		cb = &Notifier::run;
		at_thread_exit(this);
	}

	~Notifier()
	{
		mx->unlock();
		cv->notify_all();
	}

	Condition_variable* cv;
	Mutex* mx;

	static void run(void* p)
	{
		delete static_cast<Notifier*>(p);
	}
};

void key_init()
{
	struct key_s 
	{
		key_s() { pthread_key_create(&key, run); }
		~key_s() { pthread_key_delete(key); }
	};

	static key_s ks;
	// Also make sure the callbacks are run by std::exit.
	std::atexit(run);
}


void at_thread_exit(At_thread_exit_elt* elt)
{
	static pthread_once_t once = PTHREAD_ONCE_INIT;
	pthread_once(&once, key_init);

	elt->next = (At_thread_exit_elt*)pthread_getspecific(key);
	pthread_setspecific(key, elt);
}

void notify_all_at_thread_exit(Condition_variable& cv,
		Unique_lock<Mutex> l)
{ (void)new Notifier{cv, l}; }
