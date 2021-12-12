/*
 * Copyright (C) 2019
 */

#ifndef PRO_BIND_STDERR_H
#define PRO_BIND_STDERR_H

#include <unistd.h>

#include "initializer.h"

class bind_stderr : public initializer_base
{
public:
	explicit bind_stderr(const int fd) : fd_(fd) {}

	template<typename T>
	void on_exec_setup(T&) const
	{
		::dup2(fd_, STDERR_FILENO);
	}
private:
	const int fd_;
};

#endif
