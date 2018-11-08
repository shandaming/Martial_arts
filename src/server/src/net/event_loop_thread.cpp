/*
 * Copyright (C) 2018
 */

#include "event_loop_thread.h"

namespace net
{
Event_loop_thread::Event_loop_thread(const Thread_init_callback& cb,
                                 const std::string& name)
  : loop_(NULL),
    exiting_(false),
    //thread_(cb),
    callback_(cb) {}

Event_loop_thread::~Event_loop_thread()
{
  exiting_ = true;
  if (loop_ != NULL) // not 100% race-free, eg. thread_func could be running callback_.
  {
    // still a tiny chance to call destructed object, if thread_func exits just now.
    // but when Event_loop_thread destructs, usually programming is exiting anyway.
    loop_->quit();
    thread_.join();
  }
}

EventLoop* Event_loop_thread::start_loop()
{
  //assert(!thread_.started());
  thread_.start();

std::thread thread(cb);

  {
    std::lock_guard lock(mutex_);
    while (loop_ == NULL)
    {
      cond_.wait();
    }
  }

  return loop_;
}

void Event_loop_thread::thread_func()
{
  EventLoop loop;

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
