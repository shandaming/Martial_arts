﻿#include "option_description.h"
#include "cmdline.h"
#include "variables_map.h"

#include <iostream>
#if 0
int main(int argc, char* argv[])
{
	options_description general_opts("Options");
	general_opts.add_options()
		("version,v", "output the version number")
		 ("include-path,I", "include path", true)
		("log-dir", "", true)
		("data-dir", "overrides the data directory with the one specified.", true)
		("num-threads", "the initial number of threads", true)
		("debug,d", "enables additional command mode options in-game.")
		("help,h", "print description");

	variables_map vm;
	store(cmdline(argc, argv).options(general_opts).run(), vm);
#if 1
	if (vm.count("help"))
		std::cout << general_opts << std::endl;
	if (vm.count("include-path"))
		std::cout << vm["include-path"].as<std::string>();
	if (vm.count("version"))
		std::cout << "version 5.2" << std::endl;
	if (vm.count("v"))
		std::cout << "version 5.3" << std::endl;
	if (vm.count("log-dir"))
		std::cout << vm["log-dir"].as<std::string>() << std::endl;
	if (vm.count("num-threads"))
		std::cout << vm["num-threads"].as<int>() << std::endl;
	if (vm.count("I"))
		std::cout << vm["I"].as<int>() << std::endl;
	if (vm.count("data-dir"))
		std::cout << vm["data-dir"].as<std::string>() << std::endl;
	if (vm.count("debug"))
		std::cout << "Debug" << std::endl;
#endif
}
#endif
