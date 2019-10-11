/*
 * Copyright (C) 2019
 */

#ifndef _WAIT_FOR_EXIT_H
#define _WAIT_FOR_EXIT_H

#include <sys/wait.h>
#include <sys/types.h>

template<typename T>
int wait_for_exit(const T& p)
{
	pid_t ret;
	int status;
	do
	{
		ret = ::waitpid(p.pid, &status, 0);
	}while((ret == -1 && errno == EINTR) ||
			(ret != -1 && !WIFEXITED(status)));
	if(ret == -1)
	{
		LOG_ERROR << "wait_for_exit(), waitpid(2) failed.";
		return ret;
	}
	return status;
}

#endif
