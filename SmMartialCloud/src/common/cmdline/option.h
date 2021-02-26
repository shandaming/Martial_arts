/*
 * Copyright (C) 2019
 */

#ifndef CMD_OPTION_H
#define CMD_OPTION_H

#include <vector>
#include <string>

struct option
{
	option() : position_key(-1) {}

	option(const std::string& key, const std::vector<std::string>& value) :
		key(key), position_key(-1), value(value) {}

	std::string key;
	int position_key;
	std::vector<std::string> value;
	std::vector<std::string> original_tokens;
};

#endif
