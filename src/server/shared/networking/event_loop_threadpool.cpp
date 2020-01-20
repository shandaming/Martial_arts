/*
 * Copyright (C) 2018
 */

#include <cassert>
//#include <memory>

#include "event_loop_threadpool.h"

namespace net
{
Event_loop_threadpool::Event_loop_threadpool(Event_loop* base_loop)
  : base_loop_(base_loop),
    started_(false),
    num_threads_(0),
    next_(0)
{
}

Event_loop_threadpool::~Event_loop_threadpool()
{
  // Don't delete loop, it's stack variable
}

void Event_loop_threadpool::start(const Thread_init_callback& cb)
{
  assert(!started_);
  base_loop_->assert_in_loop_thread();

  started_ = true;

  for (size_t i = 0; i < num_threads_; ++i)
  {
    Event_loop_thread* t = new Event_loop_thread(cb);
    threads_.emplace_back(std::unique_ptr<Event_loop_thread>(t));
    loops_.push_back(t->start_loop());
  }
  if (num_threads_ == 0 && cb)
  {
    cb(base_loop_);
  }
}

Event_loop* Event_loop_threadpool::get_next_loop()
{
  base_loop_->assert_in_loop_thread();
  assert(started_);
  Event_loop* loop = base_loop_;

  if (!loops_.empty())
  {
    // round-robin
    loop = loops_[next_];
    ++next_;
    if (static_cast<size_t>(next_) >= loops_.size())
    {
      next_ = 0;
    }
  }
  return loop;
}

Event_loop* Event_loop_threadpool::get_loop_for_hash(size_t hash_code)
{
  base_loop_->assert_in_loop_thread();
  Event_loop* loop = base_loop_;

  if (!loops_.empty())
  {
    loop = loops_[hash_code % loops_.size()];
  }
  return loop;
}

std::vector<Event_loop*> Event_loop_threadpool::get_all_loops()
{
  base_loop_->assert_in_loop_thread();
  assert(started_);
  if (loops_.empty())
  {
    return std::vector<Event_loop*>(1, base_loop_);
  }
  else
  {
    return loops_;
  }
}
}
