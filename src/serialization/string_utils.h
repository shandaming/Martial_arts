/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

enum 
{
	REMOVE_EMPTY = 0x01, // remove empty elements.
	STRIP_SPACES = 0x02  // strips leading and trailing blank spaces.
};

void trim(std::string str);

/* Splits a (comma-)separated string into a vector of pieces. */
std::vector<std::string> split(const std::string& val, char c = ',',
		int flags = REMOVE_EMPTY | STRIP_SPACES);

#endif
