/*
 * Copyright (C) 2019
 */

#include <vector>

#include "variables_map.h"

void store(const std::vector<option>& options, variables_map& vm)
{
	for(auto& it : options)
	{
		std::string option_name = it.key;
		if(option_name.empty())
		{
			continue;
		}

		if(vm.count(option_name))
		{
			continue;
		}

		if(it.value.size() > 1)
		{
			throw std::logic_error("multiple_values.");
		}
		vm[option_name] = it.value.empty() ? 
			variable_value("") : 
			variable_value(it.value.front());
	}
}
