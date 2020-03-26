/*
 * Copyright (C) 2018
 */

#include "timer.h"

std::atomic<int64_t> timer::num_created_ = 0;

void timer::restart(Timestamp now)
{
	if (repeat_)
	{
		expiration_ = add_time(now, interval_);
	}
	else
	{
		expiration_ = Timestamp::invalid();
	}
}
