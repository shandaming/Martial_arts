/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <fstream>
#include <iostream>
#include <experimental/filesystem>
#include <system_error>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <fcntl.h>
#include <cassert>
#include <mutex>
#include "config.h"
#include "exceptions.h"
#include "game_config.h"
#include "version.h"

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
	using scoped_istream = std::unique_ptr<std::istream>;
	using scoped_ostream = std::unique_ptr<std::ostream>;

	typedef unique_ptr<SDL_RWops, void(*)(SDL_RWops*)> Rwops_ptr;
	Rwops_ptr make_read_RWops(const std::string& path);
	Rwops_ptr make_write_RWops(const std::string& path);

	/* An exception object used when an IO erro occurs. */
	struct Io_exception : public Error
	{
		Io_exception() : Error("") {}
		Io_exception(const std::string& msg) : Error(msg) {}
	};

	void set_user_data_dir(std::string newprefdir);

	std::string get_user_data_dir();

	std::string get_cwd();
	std::string get_exe_dir();

	/*
	 * 返回文件的基本文件名，并删除目录名，相当于一个可以移植的basename()
	 * 函数。
	 * 如果@remove_extension = true，则文件扩展名将从返回值中除去
	 */
	std::string base_name(const std::string& file, const bool remove_extension = false);

	// 返回文件的目录名，并剥离文件名，等同于便携式dirname()函数
	std::string directory_name(const std::string& file);

	/*
	 * Create a recursive directory tree if it does exist already
	 */
	bool create_directory_if_missing_recursive(const std::string& dirname);

	bool file_exists(const std::string& name);

	bool looks_like_pbl(const std::string& file);

	/*
	 * Returns the absolute path of a file
	 */
	std::string normalize_path(const std::string& path, 
			bool normalize_separators = false, 
			bool resolve_dot_entries = false);

	/* Basic disk I/O - read file. */
	std::string read_file(const std::string& name);
	scoped_istream istream_file(const std::string& fname, 
			bool treat_failure_as_error = true);
	scoped_ostream ostream_file(const std::string& name, 
			bool create_firectory = true);
	/* Throws io_exection if an erro occurs. */
	void write_file(const std::string& name, const std::string& data); //

	/*
	 * Returns a complete path to the actual JSON file or directory
	 * or an empty string if the file isn't present.
	 */
	std::string get_json_location(const std::string& filename,
			const std::string& current_dir = std::string());

	/*
	 * The paths manager is responsible for recording the various paths that
	 * binary files may be located at.
	 * It should be passed a config boejct which holds binary path 
	 * information.
	 * This is in the format
	 *
	 * binary_path
	 * {
	 *		path = <path>
	 * }
	 */
	struct Binary_paths_manager
	{
			Binary_paths_manager();
			Binary_paths_manager(const Config& cfg);
			~Binary_paths_manager();

			Binary_paths_manager(const Binary_paths_manager&) = delete;
			Binary_paths_manager& operator=(
					const Binary_paths_manager&) = delete;

			void set_paths(const Config& cfg);
		private:
			void cleanup();

			std::vector<std::string> paths_;
	};

	void clear_binary_paths_cache();

	/*
	 * Returns a vector with all possible paths to a given type of binary,
	 * e.g. 'images', 'sounds', etc.
	 */
	const std::vector<std::string>& get_binary_paths(
			const std::string& type);

	/*
	 * Returns a complete path to the actual file of a given @a type or an 
	 * empty string if the file isn't present.
	 */
	std::string get_binary_file_location(const std::string& type,
										const std::string& filename);
}

#endif
