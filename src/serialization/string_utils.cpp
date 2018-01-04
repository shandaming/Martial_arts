/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "string_utils.h"

void trim(std::string& str)
{
	auto it = std::find_if(str.begin(), str.end(), 
			[](char c) { return !isspace(c); });
	if(it == str.end())
		return;

	str.erase(str.begin(), it);
	str.erase(std::find_if(str.rbegin(), str.rend(), 
				[](char c) { return !isspace(c); }).base(), str.end());
}

std::vector<std::string> split(const std::string& val, char c, int flags)
{
	std::vector<std::string> res;
	std::stringstream ss;
	ss.str(val);

	std::string item;
	while(std::getline(ss, item, c))
	{
		if(flags & STRIP_SPACES)
			trim(item);
		if(!(flags & REMOVE_EMPTY) || !item.empty())
			res.push_back(std::move(item));
	}
	return res;
}
