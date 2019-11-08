/*
 * Copyright (C) 2017 - 2019 by Shan Daming
 */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <vector>
#include <cstdio>

#include "define.h"

enum 
{
	REMOVE_EMPTY = 0x01, // remove empty elements.
	STRIP_SPACES = 0x02  // strips leading and trailing blank spaces.
};

void trim(std::string& str);

/* Splits a (comma-)separated string into a vector of pieces. */
std::vector<std::string> split(const std::string& val, char c = ',',
		int flags = REMOVE_EMPTY | STRIP_SPACES);

/*
 * Match using '*' as any number of characters (including none), and 
 * '?' as any one character.
 */
bool wildcard_string_match(const std::string& str, const std::string& match);

template<typename... Args>
std::string string_format(const std::string& fmt, Args&&... args) ATTR_PRINTF(1, 2)
{
	int size = snprintf(nullptr, 0, fmt.c_str(), std::forward<Args>(args)...);
	std::string buffer;
	buffer.reserve(size + 1);
	buffer.resize(size);
	snprintf(&buffer[0], size + 1, fmt.c_str(), std::forward<Args>(args)...);
	return buffer;
};

#endif
