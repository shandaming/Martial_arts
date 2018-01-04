/*
 * Copyright (C) 2017 by ShanDaMing <shandaming@hotmail.com>
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>

/*
 * Base class for all the errors encountered.
 */
struct Error : std::exception
{
	Error() : message() {}
	Error(const std::string& msg) : message(msg) {}

	~Error() noexcept {}

	const char* what() noexcept
	{
		return message.c_str();
	}

	std::string message;
};

#endif
