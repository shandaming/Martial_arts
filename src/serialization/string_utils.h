/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <cassert>
#include <vector>
#include <algorithm>
#include <sstream>

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

template<typename T, typename... Args>
inline std::string string_format(T&& fmt, Args&&... args)
{
	return sprintf(std::forward<T>(fmt), std::forward<Args>(args)...);
}
}

#endif
