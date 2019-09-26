/*
 * Copyright (C) 2019
 */

#ifndef VARIABLES_MAP_H
#define VARIABLES_MAP_H

#include <map>
#include <string>
#include <any>

#include "option.h"

class variable_value;
typedef std::map<std::string, variable_value> variables_map;

class variable_value
{
	friend void store(const std::vector<option>& options, variables_map& vm);
public:
	variable_value() {}
	variable_value(const std::any& v) : v_(v) {}

	template<typename T>
	const T& as() const { return std::any_cast<const T&>(v_); }

	template<typename T>
	T& as() { return std::any_cast<T&>(v_); }

	bool empty() const { return v_.has_value() ? false : true; }


private:
	std::any v_;
};
/*
class variables_map : public std::map<std::string, variable_value>
{
public:
	variables_map() {}
	
private:
};
*/


void store(const std::vector<option>& options, variables_map& vm);

#endif
