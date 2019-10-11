/*
 * Copyright (C) 2017 by Shan Daming
 */

#include <unistd.h>
#include <string>
#include "../exceptions.h"
#include "thread.h"

#define GLIBXCC_NPROCS sysconf(_SC_NPROCESSORS_ONLN)

static void* execute_native_thread_routine(void* p)
{
	Thread::State_ptr t(static_cast<Thread::State*>(p));

	try
	{
		t->run();
	}
	catch(...)
	{
		std::terminate();
	}

	return nullptr;
}

Thread::State::~State() = default;

void Thread::join()
{
	int e = EINVAL;

	if(id_ != Id())
		e = pthread_join(id_.thread_, 0);

	if(e)
		throw Error("Thread::join() error: " + std::to_string(e));
	id_ = Id();
}

void Thread::detach()
{
	int e = EINVAL;

	if(id_ != Id())
		e = pthread_detach(id_.thread_);
	if(e)
		throw Error("Thread::detach() error: " + std::to_string(e));

	id_ = Id();
}

void Thread::start_thread(State_ptr state, void (*)())
{
	const int err = pthread_create(&id_.thread_, nullptr, 
			&execute_native_thread_routine,
			state.get());
	if(err)
		throw Error("Thread::start_thread error " + std::to_string(err));
	state.release();
}

unsigned int Thread::hardware_concurrency() 
{
	int n = GLIBXCC_NPROCS;
	if(n < 0)
		n = 0;
	return n;
}

void sleep_for(std::chrono::seconds s, std::chrono::nanoseconds ns)
{
//#ifdef USE_NANOSLEEP
	struct timespec ts =
	{
		static_cast<std::time_t>(s.count()),
		static_cast<long>(ns.count())
	};
	while(nanosleep(&ts, &ts) == -1 && errno == EINTR) {}

	sleep(s.count());
	if(ns.count() > 0)
	{
		long us = ns.count() / 1000;
		if(us == 0)
			us = 1;
		usleep(us);
	}
//#else
//	sleep(s.count() + (ns.count() >= 1000000));
}
