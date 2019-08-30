/*
 * Copyright (C) 2019
 */

#include "start_process.h"

template<typename T>
int create_process(T waiter, const std::string& executable, const std::vector<std::string>& args, const std::string& logger, const std::string& input, bool secure)
{
	pipe out_pipe = create_pipe();
	pipe err_pipe = create_pipe();

	if(!secure)
	{
		std::string args_str;
		for(auto& i : args)
		{
			args_str += i;
			args_str += " ";
		}
		LOG_TRAGE << "Starting process \"" << executable << "\" with arguments: \"" << args_str << "\".";
	}

	if(input.empty())
	{
		int fd = open()file;

		child res = execute(std::filesystem::absolute(executable), args, int, out_pipe[1], err_pipe[1]);
	}
}
