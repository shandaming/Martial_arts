/*
 * Copyright (C) 2018
 */

#include <sys/syscall.h> 

#include <string>
#include <type_traits>

#include "thread.h"

namespace
{
int thread_local cached_tid = 0;
std::string thread_local tid_string;
std::string thread_local thread_name = "unknown";

static_assert(std::is_same<int, pid_t>::value);

inline pid_t gettid() { return static_cast<pid_t>(syscall(SYS_gettid)); }
}

pid_t get_current_thread_id()
{
	char buf[16];
	if(cached_tid == 0)
	{
		cached_tid = gettid();
		sprintf(buf, "%5d", cached_tid);
		//tid_string = std::move(format_string("%5d ", cached_tid));
		tid_string = buf;
	}
	else
	{
		return cached_tid;
	}
}
