/*
 * Copyright (C) 2018
 */

#include "event_loop_thread.h"
#include "event_loop.h"

event_loop_thread::event_loop_thread(event_loop* loop) : 
	loop_(loop) {}

event_loop_thread::~event_loop_thread()
{
	if(loop_)
	{
		loop_->quit();
		thread_->join();
	}
}

event_loop* event_loop_thread::start_loop()
{
	if(!loop_)
		return nullptr;

	thread_ = std::make_unique<std::thread>([&loop_]()
			{
				loop_.loop();
			});

	return loop_;
}
