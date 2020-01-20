/*
 * Copyright (C) 2018
 */

#include "event_loop_thread.h"

namespace net
{
Event_loop_thread::Event_loop_thread(const Thread_init_callback& cb) :
	loop_(NULL),
    exiting_(false),
    callback_(cb) {}

Event_loop_thread::~Event_loop_thread()
{
  exiting_ = true;
  if (loop_ != NULL) // not 100% race-free, eg. thread_func could be running callback_.
  {
    // still a tiny chance to call destructed object, if thread_func exits just now.
    // but when Event_loop_thread destructs, usually programming is exiting anyway.
    loop_->quit();
    thread_->join();
  }
}

Event_loop* Event_loop_thread::start_loop()
{
  //assert(!thread_.started());
  //thread_.start();
	thread_ = new std::thread([this]{ thread_func(); });

//std::thread thread(cb);
Event_loop* loop = nullptr;
{
	 std::unique_lock<std::mutex> lk(mutex_);
      cond_.wait(lk, [this] { return loop_ != nullptr; });
loop = loop_;
}

  return loop;
}

void Event_loop_thread::thread_func()
{
  Event_loop loop;

  if (callback_)
  {
    callback_(&loop);
  }

  {
    std::lock_guard lock(mutex_);
    loop_ = &loop;
    cond_.notify_one();
  }

  loop.loop();
  //assert(exiting_);
  loop_ = NULL;
}
}
