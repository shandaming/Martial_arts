/*
 * Copyright (C) 2019
 */

#ifndef CHILD_H
#define CHILD_H

#include <sys/types.h>

struct child
{
	explicit child(pid_t p) : pid(p) {}

	pid_t pid;
};

#endif
