/*
 * Copyright (C) 2019
 */

#ifndef PIPE_H
#define PIPE_H

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

pipe create_pipe();

#endif
