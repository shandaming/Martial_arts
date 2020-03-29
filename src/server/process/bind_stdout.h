/*
 * Copyright (C) 2019
 */

#ifndef PRO_BIND_STDOUT_H
#define PRO_BIND_STDOUT_H

#include <unistd.h>

#include "initializer.h"

class bind_stdout : public initializer_base
{
public:
	explicit bind_stdout(const int fd) : fd_(fd) {}

	template<typename T>
	void on_exec_setup(T&) const
	{
		::dunp2(fd_, STDOUT_FILENO);
	}
private:
	int fd_;
};

#endif
