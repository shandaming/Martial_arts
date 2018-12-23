/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <vector>

namespace utils
{
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
bool wildcard_string_match(const std::string& str, 
							const std::string& match);

#if 0
template<typename T, typename... Args, 
	typename std::enable_if_t<
		std::is_same_v<std::remove_cv_t<T>, std::string> or
		std::is_same_v<std::remove_cv_t<T>, char*>, 
	int> = 0>
std::string string_format(T&& fmt, Args&&... args)
{
}
#endif

std::string string_format(const char* fmt, ...);
}

#endif
