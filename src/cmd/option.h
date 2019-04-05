/*
 * Copyright (C) 2019
 */

#ifndef UTIL_OPTION_H
#define UTIL_OPTION_H

#include <string>
#include <vector>

class option
{
public:
	option() : position_key(-1), unregistered(false), case_insensitive(false) {}

	option(const std::string& xstring_key, const std::vector< std::string> &xvalue) : 
		string_key(xstring_key) , position_key(-1) , value(xvalue), unregistered(false)
        , case_insensitive(false) {}

    std::string string_key;
    int position_key;
        /** Option's value */
	std::vector<std::string> value;
    std::vector< std::basic_string<char> > original_tokens;
    bool unregistered;
    bool case_insensitive;
};

#endif
