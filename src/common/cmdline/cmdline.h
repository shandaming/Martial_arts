/*
 * Copyright (C) 2019
 */

#ifndef CMDLINE_H
#define CMDLINE_H

#include <functional>

#include "option_description.h"
#include "option.h"

class cmdline
{
public:
	cmdline(const std::vector<std::string>& args) { init(args); }
	cmdline(int argc, char* argv[]);

	void init(const std::vector<std::string>& args);

	cmdline& options(const options_description& desc_);

	std::vector<option> run();

	std::vector<option> parse_long_option(std::vector<std::string>& args);
	std::vector<option> parse_short_option(std::vector<std::string>& args);
	std::vector<option> parse_terminator(std::vector<std::string>& args);

	typedef std::function<std::vector<option>(std::vector<std::string>&)> style_parser;

	void finish_option(option& opt, std::vector<std::string>& other_tokens, 
			const std::vector<style_parser>& style_parsers);

	std::vector<std::string> args;
	const options_description* desc;
	//const positional_options_description* positional;
};

#endif
