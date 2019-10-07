/*
 * Copyright (C) 2019
 */

#include <iostream>

#include "variables_map.h"
#include "option_description.h"

void store(const std::vector<option>& options, variables_map& vm)
{
	try
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
				throw options_error("multiple_values.");
			}
			vm[option_name] = it.value.empty() ? 
				variable_value("") : 
				variable_value(it.value.front());
		}
	}
	catch(options_error& e)
	{
		std::cerr << e.what() << std::endl;
		return;
	}
}
