/*
 * Copyright (C) 2019
 */

#ifndef PRO_PIPE_H
#define PRO_PIPE_H

#include <unistd.h>

#include "exceptions.h"

struct pro_pipe
{
	pro_pipe(int source, int sink) : source(source), sink(sink) {}

	int source;
	int sink;
};

pro_pipe make_pipe(int source, int sink)
{
	return pro_pipe(source, sink);
}

pro_pipe create_pipe()
{
	int fds[2];
	if(::pipe(fds) == -1)
	{
		PROCESS_THROW_LAST_SYSTEM_ERROR("pipe(2) failed.")
	}
	return pro_pipe(fds[0], fds[1]);
}

#endif
