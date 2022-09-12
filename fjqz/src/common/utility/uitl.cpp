//
// Created by mylw on 2022/5/1.
//
#include <algorithm>
#include <cctype>
#include <fstream>
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

int get_file_data_length(std::fstream& in)
{
	in.seekg(std::ios::end);
	int length = in.tellg();
	in.seekg(std::ios::beg);
	return length;
}

auto load_file_data(std::string_view file)
{
	std::vector<char> result;

	std::fstream in(file.data(), std::ios::in | std::ios::binary);
	if(!in.is_open())
		return result;

	int length = get_file_data_length(in);

	result.resize(length);
	in.read(result.data(), length);

	return result;
}
