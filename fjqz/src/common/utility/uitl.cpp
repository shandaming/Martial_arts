//
// Created by mylw on 2022/5/1.
//

#include <cctype>

#include <vector>
#include "uitl.h"

bool string_equal(std::string_view a, std::string_view b)
{
	return std::equal(a.begin(), a.end(), b.begin(), b.end(),
					  [](char c1, char c2) { return std::tolower(c1) == std::tolower(c2); });
}

bool string_to_bool(std::string_view str)
{
	if ((str == "1") || string_equal(str, "y") || string_equal(str, "on") || string_equal(str, "yes") || string_equal(str, "true"))
		return true;
	if ((str == "0") || string_equal(str, "n") || string_equal(str, "off") || string_equal(str, "no") || string_equal(str, "false"))
		return false;
	return false;
}


