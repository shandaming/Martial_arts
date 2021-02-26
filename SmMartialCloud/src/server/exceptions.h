/*
 * Copyright (C) 2017 by ShanDaMing <shandaming@hotmail.com>
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

#include <system_error>

#define PROCESS_THROW(EX) throw EX

#define DO_STRINGIZE(x) #x

#define STRINGIZE(x) DO_STRINGIZE(x)

#define PROCESS_SOURCE_LOCATION \
    "in file '" __FILE__ "', line " STRINGIZE(__LINE__) ": "

#define PROCESS_THROW_LAST_SYSTEM_ERROR(what) \
    PROCESS_THROW(std::system_error( \
        std::error_code(errno, std::system_category()), \
        PROCESS_SOURCE_LOCATION what))

#define PROCESS_RETURN_LAST_SYSTEM_ERROR(ec) \
    ec = std::error_code(errno, std::system_category())

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
