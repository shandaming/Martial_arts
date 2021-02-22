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
struct error : std::exception
{
	error() : message() {}
	error(const std::string& msg) : message(msg) {}

	~error() noexcept {}

	const char* what() noexcept
	{
		return message.c_str();
	}

	std::string message;
};

#endif
