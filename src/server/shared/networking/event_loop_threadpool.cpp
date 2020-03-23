/*
 * Copyright (C) 2018
 */

#include <cassert>
//#include <memory>

#include "event_loop_threadpool.h"

event_loop_threadpool::event_loop_threadpool(event_loop* base_loop) : 
	base_loop_(base_loop),
	started_(false),
	num_threads_(0),
	next_(0) {}

event_loop_threadpool::~event_loop_threadpool()
{
  // Don't delete loop, it's stack variable
}

void event_loop_threadpool::start()
{
	ASSERT(!started_);
	base_loop_->assert_in_loop_thread();

	started_ = true;

	for (size_t i = 0; i < num_threads_; ++i)
	{
		std::unique_ptr<event_loop_thread> event_loop_thread = std::make_unique<event_loop_thread>();
		threads_.push_back(std::move(event_loop_thread));
		loops_.push_back(threads_[i]->start_loop());
	}
}

event_loop* event_loop_threadpool::get_next_loop()
{
	base_loop_->assert_in_loop_thread();
	ASSERT(started_);
	event_loop* loop = base_loop_;

	if (!loops_.empty())
	{
    // round-robin
		loop = loops_[next_];
		++next_;
		if (static_cast<size_t>(next_) >= loops_.size())
			next_ = 0;
 
	}
	return loop;
}

event_loop* event_loop_threadpool::get_loop_for_hash(size_t hash_code)
{
	base_loop_->assert_in_loop_thread();
	event_loop* loop = base_loop_;

	if (!loops_.empty())
		loop = loops_[hash_code % loops_.size()];

	return loop;
}

std::vector<event_loop*> event_loop_threadpool::get_all_loops()
{
	base_loop_->assert_in_loop_thread();
	ASSERT(started_);
	if (loops_.empty())
		return std::vector<event_loop*>(1, base_loop_);
	else
		return loops_;
}
