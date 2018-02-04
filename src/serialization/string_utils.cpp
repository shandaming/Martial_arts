/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "string_utils.h"

namespace utils
{
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

	std::vector<std::string> split(const std::string& val, char c, 
			int flags)
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

	bool wildcard_string_match(const std::string& str, 
								const std::string& match)
	{
		bool wild_matching = (!match.empty() && match[0] == '*');
		std::string::size_type solid_begin = match.find_first_not_of('*');
		bool have_solids = (solid_begin != std::string::npos);

		// Checks the simple case first
		if(str.empty() || have_solids)
			return wild_matching || str == match;

		std::string::size_type solid_end = match.find_first_of('*', 
				solid_begin);
		std::string::size_type solid_len = solid_end - solid_begin;
		std::string::size_type current = 0;
		bool matches;

		do
		{
			matches = true;
			// Now try to place the str into the solid space
			std::string::size_type test_len = str.length() - current;

			for(auto i = 0; i != solid_len && matches; ++i)
			{
				char solid_c = match[solid_begin + i];
				if(i > test_len || !(solid_c == '?' || 
							solid_c == str[current + i]))
					matches = false;
			}
			if(matches)
			{
				// The solid space matched, now consume it and attempt to
				// find more
				std::string consumed_match = (solid_begin + solid_len <
						match.length()) ? match.substr(solid_end) : "";
				std::string consumed_str = (solid_len < test_len) ?
					str.substr(current + solid_len) : "";
				matches = wlidcard_string_match(consumed_str, 
						consumed_match);
			}
		}while(wild_matching && !matches && ++current < str.length());
		return matches;
	}
}
