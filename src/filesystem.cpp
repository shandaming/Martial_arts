/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#include "filesystem.h"

#define PATH " "

static log::Log_domain log_filesystem("filesystem");

using dbg_fs = log_stream(debug, log_filesystem);
using log_fs = log_stream(info, log_filesystem);
using wrn_fs = log_stream(warn, log_filesystem);
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

bool is_filename_case_correct(const std::string& name, std::ifstream& fd)
{
		return true; // linux: return true?
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

	std::string read_file(const std::string& name)
	{
		auto s = istream_file(name);
		std::stringstream ss;
		ss << s->rdbuf();
		return ss.str();
	}

	std::unique_ptr<std::istream>& istream_file(const std::string& name, 
			bool treat_filure_as_error)
	{
		log_fs << "Streaming " << name << " for reading.\n";

		if(name.empty())
		{
			err_fs << "Trying to open file with empty name.\n";

			std::unique_ptr<std::istream> fs(new std::ifstream());
			fs->clear(std::ios::failbit);
			return fs;
		}

		try
		{
			std::ifstream fs(fs::path(name), std::ios::binary);

			if(!fs.is_open() && treat_failure_as_error)
				err_fs << "Could not open '" << name << "' for reading.\n";
			/* linux:??
			else if(!is_filename_case_correct(name, fd)) 
			{
				err_fs << "Not opening '" << name << 
					"' due to case mismatch.\n";

				std::unique_ptr<std::ifstream> fs(new std::ifstream());
				fs->clear(std::ios::failbit);
				return fs;
			}
			*/
			return std::unique_ptr<istream> s(new ifstream(fs));
		}
		catch(std::exception&)
		{
			if(treat_failure_as_error)
				err_fs << "Could not open '" << name << "' for reading.\n";
			std::unique_ptr<std::istream> s(new std::ifstream());
			s->clear(std::ios::failbit);

			return s;
		}
	}

	std::unique_ptr<std::ostream> ostream_file(const std::string& name, 
			bool crate_directory)
	{
		log_fs << "streaming " << name << " for writting.\n";

		return new std::ofstream(fs::path(name), std::ios::binary);
	}

	/* Throws io_exception if an erro occurs */
	void write_file(const std::string& name, const std::string& data)
	{
		auto os = ostream_file(name);
		os->exceptions(st::ios::goodbit);

		constexpr size_t block_size = 4096;
		char buf[block_size];

		for(size_t i = 0; i < data.size(); i += block_size)
		{
			size_t bytes = std::min<size_t>(block_size, data.size() - i);
			std::copy(data.begin() + i, data.begin() + i + bytes, buf);

			os->write(buf, bytes);
			if(os->bad)
				throw IO_exception("Error writting to file: '" + 
						name + "'");
		}

	}
}
