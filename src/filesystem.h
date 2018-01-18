/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <fcntl.h>
#include <cassert>
#include <mutex>
#include "exceptions.h"

int file_exists(const std::string& file);

std::string get_file_location(const std::string& file);

std::string read_file(const std::string& file);
const char* read_file(const char* file);

void write_file(const std::string& file, const std::string& text);
bool write_file(const char* file, const char* text);

int get_file_bytes_count(std::ifstream& fd);
long get_file_bytes_count(FILE* fd);

namespace filesystem
{
	/* An exception object used when an IO erro occurs. */
	struct IO_excetpion : public Error
	{
		IO_exception() : Error("") {}
		IO_exception(const std::string& msg) : Error(msg) {}
	};

	std::string get_cwd();

	std::string get_exe_dir();

	std::string directory_name(const std::string& file);

	/* Basic disk I/O - read file. */
	std::string read_file(const std::string& name);
	std::unique_ptr<std::istream>& istream_file(const std::string& fname, 
			bool treat_failure_as_error = true);
	std::unique_ptr<std::ostream>& ostream_file(const std::string& name, 
			bool create_firectory = true);
	/* Throws io_exection if an erro occurs. */
	void write_file(const std::string& name, const std::string& data);
}

#endif
