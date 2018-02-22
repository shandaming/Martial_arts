/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "commandline_options.h"

Commandline_options::Commandline_options(const std::vector<std::string>& 
										args) :                                                                     args_(args)
{
	if(args.size() > 0)
		args0_ = args[0];

	cmdline_.add_group("General options:");
	cmdline_.add_option("--help", "-h", "print this help text and exit",
						Commandline::exclusive);
	cmdline_.add_option("--version", "", "print version information and exit", Commandline::exclusive);
	
	cmdline_.add_group("Display options:");
	cmdline_.add_option("--fps", "displays the number of frames per second the game is currently running at, in a corner of the screen.");
	cmdline_.add_option("--fullscreen", "-f", "runs the game in full screen mode.");
}

bool Commandline_options::parse_commandline( 
									const int flags,
									const int start_pos)
{
        Commandline::Parse_status status  = cmdline_.parse_line(args_, flags, start_pos);
	if(check_parse_status(status))
	{
		if(cmdline_["--fps"])
			fps = true;
		if(cmdline_["--fullscreen"])
			fullscreen = true;
	}
}

bool Commandline_options::check_parse_status(
		const Commandline::Parse_status& status)
{
	bool result = false;
	std::string str;

	switch(status)
	{
		case PS::exclusive_option:
			// check whether to print the "usage text" or not
			if(cmdline_["--help"])
				print_usage();
			else
				result = true;
			break;
		case PS::normal:
			result = true;
			break;
		default:
			// an erro occurred while parsing the command line
			//cmdline_.get_status_string(status, str);
			//print_error(str);
			break;
	}
	return result;
}

void Commandline_options::print_usage()
{
	print_header();
	std::cout << "usage: " << args0_ << " "
                << "[OPTION]... " << "[VAR=VALUE]...";

	std::string str;
	//cmdline_.get_syntax_string(str);
	if(!str.empty())
		std::cout << std::endl << str;
	cmdline_.get_option_string(str);
	if(!str.empty())
		std::cout << std::endl << str << std::endl;
	exit(0);
}

void Commandline_options::print_header()
{
	//std::out << identification << std::endl << std::endl << program_name_ 
		//<< ": " << description << std::endl;
}

std::ostream& operator<<(std::ostream& os, 
		const Commandline_options& cmdline_opts)
{
	os << "Usage: " << cmdline_opts.args0_ <<
		" [<options>] [<data-directory>]\n";
	//os << cmdline_opts.visible_;
	return os;
}
