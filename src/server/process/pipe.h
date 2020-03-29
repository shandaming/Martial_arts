/*
 * Copyright (C) 2019
 */

#ifndef PRO_PIPE_H
#define PRO_PIPE_H

#include "exceptions.h"

struct pipe
{
	pipe(int source, int sink) : source(source), sink(sink) {}

	int source;
	int sink;
};

pipe make_pipe(int source, int sink)
{
	return pipe(source, sink);
}

pipe create_pipe()
{
	int fds[2];
	if(::pipe(fds[2]) == -1)
	{
		PROCESS_THROW_LAST_SYSTEM_ERROR("pipe(2) failed.")
	}
	return pipe(fds[0], fds[1]);
}

#endif
