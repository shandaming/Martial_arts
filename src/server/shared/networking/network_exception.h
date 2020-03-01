/*
 * Copyright (C) 2017 by ShanDaMing <shandaming@hotmail.com>
 */

#ifndef NETWORK_EXCEPTIONS_H
#define NETWORK_EXCEPTIONS_H

#include <exception>
#include <string>
#include <system_error>

struct networking_exception : std::exception
{
	networking_exception() : message() {}
	networking_exception(const std::string& msg) : message(msg) {}
	networking_exception(const std::error_code& ec) : message(ec.message) {}

	~networking_exception() noexcept {}

	const char* what() noexcept
	{
		return ("Networking exception. " + message).c_str();
	}

	std::string message;
};

#endif
