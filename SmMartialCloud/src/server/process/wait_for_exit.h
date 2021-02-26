/*
 * Copyright (C) 2019
 */

#ifndef PRO_WAIT_FOR_EXIT_H
#define PRO_WAIT_FOR_EXIT_H

#include <sys/wait.h>
#include <sys/types.h>

#include "exceptions.h"

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
		PROCESS_THROW_LAST_SYSTEM_ERROR("waitpid(2) failed.");
	return status;
}

template<typename T>
int wait_for_exit(const T& p, std::error_code& ec)
{
	pid_t ret;
	int status;
	do
	{
		ret = ::waitpid(p.pid, &status, 0);
	}while((ret == -1 && errno == EINTR) ||
			(ret != -1 && !WIFEXITED(status)));
	if(ret == -1)
		PROCESS_RETURN_LAST_SYSTEM_ERROR(ec);
	else
		ec.clear();
	return status;
}

#endif
