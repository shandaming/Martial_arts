/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "commandline_options.h"

Commandline_options::Commandline_options(
		const std::vector<std::string>& args)
{}

std::ostream& operator<<(std::ostream& os, 
		const Commandline_options& cmdline_opts)
{
	os << "Usage: " << cmdline_opts.args0_ << 
		"[<options>] [<data-directory>]\n";
	//os << cmdline_opts.visible_;
	return os;
}
