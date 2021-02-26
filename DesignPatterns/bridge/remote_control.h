/*
 * Copyright (C) 2018 
 */

#ifndef REMOTE_CONTROL_H
#define REMOTE_CONTROL_H

#include "tv.h"

class Remote_control
{
	public:
		virtual void on() = 0;
		virtual void off() = 0;
		virtual void set_channel() = 0;
	protected:
		Tv* tv_;
};

class Concrete_remote : public Remote_control
{
	public:
		Concrete_remote(Tv* t) { tv_ = t;}

		void on() override { tv_->on(); }
		void off() override { tv_->off(); }
		void set_channel() { tv_->turn_channel(); }
};

#endif
