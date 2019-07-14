/*
 * Copyright (C) 2019
 */

#include "ini_parser.h"

ini_parser::ini_parser(const std::string& filename) : filename_(filename), it_(line_.begin()), ch_(' ')
{
	open_file(filename_);
}

ini_parser::~ini_parser()
{
	close_file();
}

bool ini_parser::open_file(const std::string& filename)
{
	close_file();

	fs_.open(filename, std::ios::in | std::ios::out);
	if(!fs_.is_open())
	{
		std::stderr << "Failed to open " << filename;
		return false;
	}
	return true;
}

void ini_parser::close_file()
{
	if(fs_.is_open())
	{
		fs_.close();
	}
}

bool ini_parser::reload_file(const std::string& filename)
{
	filename_ = filename;
	return open_file(filename_);
}

char ini_parser::getchar()
{
	if(it_ != line_.end())
	{
		*it_++;
	}
	else
	{
		if(std::getline(fs_, line_, '#'))
		{
			it_ = line_.begin();
			return *it_++;
		}
	}
	return EOF;
}

bool ini_parser::scan(const char need)
{
	ch_ = getchar();
	if(need)
	{
		if(ch_ != need)
			return false;

		ch_ = getchar();
		return true;
	}
	return true;
}

void ini_parser::tokenize()
{
	std::string section;
	std::string name;
	std::string value;
	std::pair<token, std::string> res;

	while(ch_ != EOF)
	{
		while(ch_ == ' ' || ch_ == '\t')
			scan();

		{
			switch(ch_)
			{
					// 跳过注释
				case '#':
				case ';':
					while(ch_ != EOF)
						scan();
					break;
					// section
				case '[': 
					whle(!scan(']'))
					{
						if(ch_ == '\0')
							std::stderr << "section: error!";
						section.push(ch);
					}
					res = std::make_pair(SECTION, section);
					section.clear();
				default:

					break;
			}
		}
		if(res.empty())
			return res;
	}
}

void ini_parser::analyse()
{
	auto value = tokenize();
	switch(value.first)
	{

	}
}

void ini_parser::parse()
{
	std::string line;
	std::string section;
	std::string key;
	std::string value;
	int line = 1;

	while(std::getline(fs_, line, '#'))
	{
		for(int i = 0, i < line.size(); ++i)
		{
			char ch = line[i];
			switch(ch)
			{
				case '#':
				case ';':

					break;
				case '[':
					break;
				case ' ':
				case '\t':
					continue;

			}
		}
		++line;
	}
}
