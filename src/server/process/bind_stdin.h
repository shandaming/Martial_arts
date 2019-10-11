/*
 * Copyright (C) 2019
 */

#ifndef BIND_STDIN_H
#define BIND_STDIN_H

#include <unistd.h>

#include "initializer.h"

class bind_stdin : public initializer_base
{
public:
	explicit bind_stdin(const int fd) : fd_(fd) {}

	template<typename T>
	void on_exec_setup(T&) const
	{
		::dup2(fd_, STDIN_FILENO);
	}
private:
	const int fd_;
};

#endif
