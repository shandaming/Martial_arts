/*
 * Copyright (C) 2019
 */

#include <unistd.h>

#include "pipe.h"
#include "log/loggin.h"

pipe create_pipe()
{
	int fds[2];
	if(::pipe(fds[2]) == -1)
	{
		LOG_ERROR << "pipe(2) failed. " << strerror(errno);
		return pipe(-1, -1);
	}
	return pipe(fds[0], fds[1]);
}
