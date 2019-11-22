/*
 * Copyright (C) 2019
 */

#include <sstream>
#include <iostream>

#include "ini_parser.h"
#include "common/serialization/string_utils.h"

bool ini_parser::has_section(const std::string& section)
{
	for(auto& i : sections_)
	{
		if(i->section == section)
			return true;
	}
	return false;
}

bool ini_parser::read_ini(const std::string& file, std::string& error)
{
	std::string line;
	std::string key;
	std::string value;
	size_t line_no = 0;
	section* section = 0;

	std::fstream fs;
	fs.open(file, std::ios::in);
	if(!fs.is_open())
	{
		error = "open file '" + file + "' failed.";
		return false;
	}

	while(fs.good())
	{
		++line_no;
		std::getline(fs, line);
		if(!fs_.good() && !fs.eof())
		{
			error = ("Read error. Line: " + line_no);
			return false;
		}
		trim(line);
		if(!line.empty())
		{
			if(line[0] == ';' || line[0] == '#')
			{}
			else if(line[0] == '[')
			{
				if(section && section->properties.empty())
				{
					// section如果没有ke-value,则忽略这section.
					sections_.pop_back();
				}

				auto pos = line.find(']');
				if(pos == std::string::npos)
				{
					error = ("Unmatched '['. Line:" + line_no);
					return false;
				}
				key = line.substr(1, pos - 1);
				trim(key);
				
				if(has_section(key))
				{
					error = ("Has the same section. Line:" + line_no);
					return false;
				}

				std::shared_ptr<ini_parser::section> s(new ini_parser::section);
				section = s.get();
				s->section = key;
				sections_.push_back(s);
			}
			else
			{
				if(!section)
				{
					// 没有section则跳过
					continue;
				}
				auto pos = line.find('=');
				if(pos == std::string::npos)
				{
					error = ("'=' character not found. Line:" + line_no);
					return false;
				}
				if(pos == 0)
				{
					error = ("Key expected. Line:" + line_no);
					return false;
				}
				key = line.substr(0, pos);
				value = line.substr(pos + 1);
				trim(key);
				trim(value);

				if(section->has_key(key))
				{
					error = ("Has the same key in a section. Line:" + line_no);
					return false;
				}

				section->properties.push_back(std::make_pair(key, value));
			}
		}
	}
	return true;
}

bool ini_parser::write_ini(const std::string& file, std::string& error)
{
	std::fstream fs;
	fs.open(file, std::ios::out);
	if(!fs.is_open())
	{
		error = "open file '" + file + "' failed.";
		return false;
	}

	for(auto& i : sections_)
	{
		fs_ << "[" << i->section << "]" << std::endl;
		for(auto& j : i->properties)
		{
			fs_ << j.first << " = " << j.second << std::endl;
		}
	}
	return true;
}

void ini_parser::print()
{
	std::stringstream os;
	for(auto& i : sections_)
	{
		os << "[" << i->section << "]" << std::endl;
		for(auto& j : i->properties)
		{
			os << j.first << " = " << j.second << std::endl;
		}
	}
	std::cout << os.str();
}

std::string ini_parser::get_value(const std::string& section, const std::string& key) const
{
	for(auto& i : sections_)
	{
		if(i->section == section)
		{
			for(auto& j : i->properties)
			{
				if(j.first == key)
				{
					return j.second;
				}
			}
		}
	}
	return "";
}

std::vector<std::string> ini_parser::get_keys_by_string(const std::string& section, const std::string& name)
{
	std::vector<std::string> keys;

	for(auto& i : sections_)
	{
		if(i->section == section)
		{
			for(auto& j : i->properties)
			{
				if(j.first.find(name) != std::string::npos)
					keys.push_back(j.first);
			}
		}
	}
	return keys;
}
