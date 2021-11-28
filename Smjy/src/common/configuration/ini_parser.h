/*
 * Copyright (C) 2019
 */

#ifndef CFG_INI_PARSER_H
#define CFG_INI_PARSER_H

#include <fstream>
#include <memory>
#include <vector>

class ini_parser
{
public:
	ini_parser() = default;
	~ini_parser() = default;

	ini_parser(const ini_parser&) = delete;
	ini_parser& operator=(const ini_parser&) = delete;

	bool read_ini(const std::string& file, std::string& error);
	bool write_ini(const std::string& file, std::string& error);
	void print();

	std::string get_value(const std::string& section, const std::string& key) const;
	std::vector<std::string> get_keys_by_string(const std::string& section, const std::string& name);
private:
	bool has_section(const std::string& section);

	using section_property = std::pair<std::string, std::string>;
	struct section
	{
		bool has_key(const std::string& key)
		{
			for(auto& i : properties)
			{
				if(i.first == key)
					return true;
			}
			return false;
		}

		std::string section;
		std::vector<section_property> properties;
	};

	std::string filename_;
	std::vector<std::shared_ptr<section>> sections_;
	std::fstream fs_;
};

#endif
