/*
 * Copyright (C) 2019
 */

#ifndef CFG_INI_PARSER_H
#define CFG_INI_PARSER_H

#include <fstream>

class ini_parser
{
public:
	ini_parser(const std::string& filename);
	~ini_parser();

	ini_parser(const ini_parser&);
	ini_parser& operator=(const ini_parser&);

	ini_parser(ini_parser&&);
	ini_parser& operator=(ini_parser&&);

	bool open_file(const std::string& filename);
	void close_file();

	bool reload_file(const std::string& filename);

private:
	enum token
	{
		SECTION,
		KEY,
		VALUE
	};

	using section_property = std::pair<std::string, std::string>;
	struct section
	{
		std::string name;
		std::vector<section_property> properties;
	};

	char getchar();
	bool scan(const char need = 0);
	void tokenize();

	void analyse();

	std::string line_;
	char ch_;
	std::string::iterator it_;

	std::string filename_;
	std::vector<section> sections_;
	std::fstream fs_;
};

#endif
