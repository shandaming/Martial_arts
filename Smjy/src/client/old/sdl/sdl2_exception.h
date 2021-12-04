/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#ifndef SDL2_EXCEPTION_H
#define SDL2_EXCEPTION_H

#include <exception>

class sdl2_exception : public exception
{
public:
	sdl2_exception(const std::string& message) : message_(message) {}
	~sdl2_exception() noexcept {}

	const char* what() noexcept;
private:
	std::string message_;
};

#endif
