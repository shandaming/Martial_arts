/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#include "filesystem.h"

#define PATH ""

static log::Log_domain log_filesystem("filesystem");

using err_fs = log_stream(err, log_filesystem);

using fs = std::filesystem;

static std::mutex mutex;

int file_exists(const std::string& file)
{
	struct stat buffer;
	
	return (stat(file.c_str(), &buffer) == 0);
}

std::string get_file_location(const std::string& file)
{
	if(file_exists(file))
		return file;

	return PATH + file;
}

std::string read_file(const std::string& file)
{
	std::ifstream fd(get_file_location(file), std::ios::binary | std::ios::in);
	if(!fd.is_open())
		throw Error("Fail to open the \"" + file + "\".");

	std::lock_guard<std::mutex> lock(mutex);
	int count = get_file_bytes_count(fd);
	char* data = new char[count];
	assert(data);

	fd.read(data, count);
	std::string out(data);
	delete data;

	return out;
}

const char* read_file(const char* file)
{
	char* buff = 0;

	FILE* fd = fopen(get_file_location(std::string(file)).c_str(), "rb");
	if(fd)
	{
		struct flock fl;
		fl.l_type = F_RDLCK;
		fl.l_whence = SEEK_SET;
		fl.l_start = 0;
		fl.l_len = 0;

		if(!fcntl(fileno(fd), F_SETLK, &fl)) // locked file
		{
			long count = get_file_bytes_count(fd);
		    buff = (char*)malloc(sizeof(char) * count);
		    assert(buff);
		    memset(buff, 0, sizeof(buff));
			fscanf(fd, "%s", buff);
		}
		fclose(fd);
	}

	return buff;
}

void write_file(const std::string& file, const std::string& text)
{
	std::ofstream out(get_file_location(file), std::ios::binary | 
			std::ios::out);
	if(!out.is_open())
		throw Error("Fail to open the \"" + file + "\".");

	std::lock_guard<std::mutex> lock(mutex);
	out.write(text.c_str(), text.size());
}

bool write_file(const char* file, const char* text)
{
	int e = EOF;

	FILE* fd = fopen(get_file_location(file).c_str(), "wb");
	if(fd)
	{
		struct flock fl;
		fl.l_type = F_WRLCK;
		fl.l_whence = SEEK_SET;
		fl.l_start = 0;
		fl.l_len = 0;

		if(!fcntl(fileno(fd), F_SETLK, &fl))
			e = fputs(text, fd);
	}
	return (e != EOF);
}

int get_file_bytes_count(std::ifstream& fd)
{
	fd.seekg(0, std::ios::end);
	int count = fd.tellg();
	fd.seekg(0, std::ios::beg);

	return count;
}

long get_file_bytes_count(FILE* fd)
{
	fseek(fd, 0, SEEK_END);
	long count = ftell(fd);
	rewind(fd); // Move to the beginning of the file stream.
	return count;
}

namespace filesystem
{
	std::string get_cwd()
	{
		fs::error_code ec;
		fs::path cwd = fs::current_path(ec);
		if(ec)
		{
			err_fs << "Failed to get current directory: "<< ec.message()
				<< "\n";
			return "";
		}
		return cwd.generic_string();
	}

	std::string get_exe_dir()
	{
		if(fs::exists("/proc/"))
		{
			fs::error_code ec;
			fs::path exe = fs::read_symlink(fs::path("/proc/self/exe"), ec);
			if(ec)
				return std::string();

			return exe.parent_path().string();
		}
		else
			return 
	}

	std::string directory_name(const std::string& file)
	{
		return fs::path(file).parent_path().string();
	}
}
