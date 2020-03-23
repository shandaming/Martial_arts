/*
 * Copyright (C) 2018
 */

#include "event_loop_thread.h"

event_loop_thread::event_loop_thread() :
	loop_(NULL),
	exiting_(false) {}

event_loop_thread::~event_loop_thread()
{
  exiting_ = true;
  if (loop_ != NULL) // not 100% race-free, eg. thread_func could be running callback_.
  {
    // still a tiny chance to call destructed object, if thread_func exits just now.
    // but when event_loop_thread destructs, usually programming is exiting anyway.
    loop_->quit();
    thread_->join();
  }
}

event_loop* event_loop_thread::start_loop()
{
  //assert(!thread_.started());
  //thread_.start();
	thread_ = std::make_unique<std::thread>([this]{ thread_func(); });

//std::thread thread(cb);
	event_loop* loop = nullptr;
	{
		std::unique_lock<std::mutex> lk(mutex_);
		cond_.wait(lk, [this] { return loop_ != nullptr; });
		loop = loop_;
	}

	return loop;
}

void event_loop_thread::thread_func()
{
	event_loop loop;

	{
		std::lock_guard lock(mutex_);
		loop_ = &loop;
		cond_.notify_one();
	}

	loop.loop();
  //assert(exiting_);
	loop_ = NULL;
}
