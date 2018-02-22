/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#include "filesystem.h"
#include "log.h"

#define PATH " "

static lg::Log_domain log_filesystem("filesystem");

#define DBG_FS LOG_STREAM(lg::debug, log_filesystem)
#define LOG_FS LOG_STREAM(lg::info, log_filesystem)
#define WRN_FS LOG_STREAM(lg::warn, log_filesystem)
#define ERR_FS LOG_STREAM(lg::err, log_filesystem)

//using fs = std::filesystem;
namespace fs = std::experimental::filesystem;

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
	static fs::path user_data_dir, user_config_dir, cache_dir;

	static const std::string& get_version_path_suffix()
	{
		static std::string suffix;

		// We only really need to generate this one since
		// the version number cannot change during runtinme.

		if(suffix.empty())
		{
			std::ostringstream os;
			os << game_config::game_version.major_version() << '.' <<
				game_config::game_version.minor_version();
			suffix = os.str();
		}

		return suffix;
	}

	static inline bool error_except_not_found(const std::error_code& ec)
	{
		return (ec && ec.value() != static_cast<int>(std::errc::no_such_file_or_directory));
	}
	
	static bool file_exists(const fs::path& fpath)
	{
		std::error_code ec;
		bool exists = fs::exists(fpath, ec);
		if(error_except_not_found(ec))
			ERR_FS << "Failed to check existence of file " << fpath.string()
				<< ": " << ec.message() << '\n';
		return exists;
	}

	bool looks_like_pbl(const std::string& file)
	{
		return utils::wildcard_string_match(file/*utf8::lowercase(file)*/, "*.pbl");
	}

	static bool is_legal_file(const std::string& filename)
	{
		DBG_FS << "Looking for '" << filename << "'.\n";

		if(filename.empty())
		{
			LOG_FS << " invalid filename\n";
			return false;
		}

		if(filename.find("..") != std::string::npos)
		{
			ERR_FS << "Illegal path '" << filename << 
				"' (\"..\" not allowed).\n";
			return false;
		}

		if(filename.find('\\') != std::string::npos)
		{
			ERR_FS << "Illegal path '" << filename <<
				R"end(' ("\" not allowed, for compatibility with GUN/Linux \
				and macOS).)end" << std::endl;
			return false;
		}

		if(looks_like_pbl(filename))
		{
			ERR_FS << "Illegal path '" << filename << "' (.pbl files are \
				not allowed)." << std::endl;
			return false;
		}
		return true;
	}

	std::string get_cwd()
	{
		std::error_code ec;
		fs::path cwd = fs::current_path(ec);
		if(ec)
		{
			ERR_FS << "Failed to get current directory: "<< ec.message()
				<< "\n";
			return "";
		}
		return cwd.generic_string();
	}

	std::string get_exe_dir()
	{
		if(fs::exists("/proc/"))
		{
			std::error_code ec;
			fs::path exe = fs::read_symlink(fs::path("/proc/self/exe"), ec);
			if(ec)
				return std::string();

			return exe.parent_path().string();
		}
		else
			return get_cwd();
	}

	std::string directory_name(const std::string& file)
	{
		return fs::path(file).parent_path().string();
	}

	static bool create_directory_if_missing(const fs::path& dirpath)
	{
		std::error_code ec;
		fs::file_status fs = fs::status(dirpath, ec);
		if(error_except_not_found(ec))
		{
			ERR_FS << "Failed to retrieve file status for " << 
				dirpath.string() << ": " << ec.message() << '\n';
			return false;
		}
		else if(fs::is_directory(fs))
		{
			ERR_FS << "directory " << dirpath.string() << 
				" exists, not creating\n";
			return true;
		}
		else if(fs::exists(fs))
		{
			ERR_FS << "cannot create directory " << dirpath.string() <<
				": file exists\n";
			return false;
		}

		bool created = fs::create_directory(dirpath, ec);
		if(ec)
			ERR_FS << "Failed to create directory " << dirpath.string() <<
				": " << ec.message() << "\n";
		return created;
	}

	static bool create_directory_if_missing_recursive(
			const fs::path& dirpath)
	{
		DBG_FS << "creating recursive directory: " << dirpath.string() <<
			"\n";

		if(dirpath.empty())
			return false;
		std::error_code ec;
		fs::file_status fs = fs::status(dirpath);
		if(error_except_not_found(ec))
		{
			ERR_FS << "Failed to retrieve file status for " << 
				dirpath.string() << ": " << ec.message() << '\n';
			return false;
		}
		else if(fs::is_directory(fs))
			return true;
		else if(fs::exists(fs))
			return false;

		if(!dirpath.has_parent_path() || 
				create_directory_if_missing_recursive(
					dirpath.parent_path()))
			return create_directory_if_missing(dirpath);
		else
		{
			ERR_FS << "Counld not create parents to " << dirpath.string() <<
				'\n';
			return false;
		}
	}

	bool file_exists(const std::string& name)
	{
		return file_exists(fs::path(name));
	}

	std::string normalize_path(const std::string& path, 
			bool normalize_separators,
			bool resolve_dot_entries)
	{
		if(path.empty())
			return path;

		std::error_code ec;
		fs::path p = resolve_dot_entries ? fs::canonical(path, ec) :
			fs::absolute(path);

		if(ec)
			return "";
		if(normalize_separators)
			return p.make_preferred().string();
		else
			return p.string();
	}

	std::string read_file(const std::string& name)
	{
		std::unique_ptr<std::istream> s = istream_file(name);
		std::stringstream ss;
		ss << s->rdbuf();
		return ss.str();
	}

	scoped_istream istream_file(const std::string& name, 
			bool treat_failure_as_error)
	{
		LOG_FS << "Streaming " << name << " for reading.\n";

		if(name.empty())
		{
			ERR_FS << "Trying to open file with empty name.\n";

			std::unique_ptr<std::istream> fs(new std::ifstream());
			fs->clear(std::ios::failbit);
			return fs;
		}

		try
		{
			std::ifstream fs(fs::path(name), std::ios::binary);

			if(!fs.is_open() && treat_failure_as_error)
				ERR_FS << "Could not open '" << name << "' for reading.\n";
			/* linux:????
			else if(!is_filename_case_correct(name, fd)) 
			{
				err_fs << "Not opening '" << name << 
					"' due to case mismatch.\n";

				std::unique_ptr<std::ifstream> fs(new std::ifstream());
				fs->clear(std::ios::failbit);
				return fs;
			}
			*/
                        scoped_istream is(new std::ifstream(std::move(fs)));
			return is;
		}
		catch(std::exception&)
		{
			if(treat_failure_as_error)
				ERR_FS << "Could not open '" << name << "' for reading.\n";
			std::unique_ptr<std::istream> s(new std::ifstream());
			s->clear(std::ios::failbit);

			return s;
		}
	}

	scoped_ostream ostream_file(const std::string& name, 
			bool crate_directory)
	{
		LOG_FS << "streaming " << name << " for writting.\n";

                scoped_ostream os(new std::ofstream(fs::path(name), std::ios::binary));
		return os;
	}

	/* Throws io_exception if an erro occurs.. */
	void write_file(const std::string& name, const std::string& data)
	{
		auto os = ostream_file(name);
		os->exceptions(std::ios::goodbit);

		constexpr size_t block_size = 4096;
		char buf[block_size];

		for(size_t i = 0; i < data.size(); i += block_size)
		{
			size_t bytes = std::min<size_t>(block_size, data.size() - i);
			std::copy(data.begin() + i, data.begin() + i + bytes, buf);

			os->write(buf, bytes);
			if(os->bad())
				throw Io_exception("Error writting to file: '" + 
						name + "'");
		}

	}

	static void setup_user_data_dir()
	{
		if(create_directory_if_missing_recursive(user_data_dir))
		{
			ERR_FS << "could not open or create user data directory at " <<
				user_data_dir.string() << '\n';
			return;
		}
		// TOOO: this may not print the error message if the directory
		// exists but we don't have the proper permissions

		// create user data and add-on directories
		create_directory_if_missing(user_data_dir / "data");
		create_directory_if_missing(user_data_dir / "data" / "add-ons");
		create_directory_if_missing(user_data_dir / "saves");
		create_directory_if_missing(user_data_dir / "persist");
	}

	void set_user_data_dir(std::string newprefdir)
	{
	#ifdef PREFERENCES_DIR
		if(newprefdir.empty())
			newprefdir = PREFERENCES_DIR;
	#endif
		std::string backupprefdir = ".game" + get_version_path_suffix();

		const char* home_dir = getenv("HOME");

	#ifdef _X11
		if(newprefdir.empty())
		{
			const char* xdg_data = getenv("XDG_DATA_HOME");
			if(!xdg_data || xdg_data[0] == '\0')
			{
				if(!home_str)
				{
					newprefdir = backupprefdir;
					goto other;
				}
				user_data_dir = home_dir;
				user_data_dir /= ".local/share";
			}
			else
				user_data_dir = xdg_data;
			user_data_dir /= "game";
			user_data_dir /= get_version_path_suffix();
		}
		else
		{
			other:
			fs::path home = home_str ? home_str : ".";

			if(newprefdir[0] == '/')
				user_data_dir = newprefdir;
			else
				user_data_dir = home / newprefdir;
		}
	#else
		if(newprefdir.empty())
			newprefdir = backupprefdir;

		const char* home_str = getenv("HOME");
		fs::path home = home_str ? home_str : ".";

		if(newprefdir[0] == '/')
			user_data_dir = newprefdir;
		else
			user_data_dir = home / newprefdir;
	#endif
		setup_user_data_dir();
		user_data_dir = normalize_path(user_data_dir.string(), true, true);
	}

	static const fs::path& get_user_data_path()
	{
		if(user_data_dir.empty())
			set_user_data_dir(std::string());
		return user_data_dir;
	}

	std::string get_json_location(const std::string& filename,
			const std::string& current_dir)
	{
		if(!is_legal_file(filename))
			return std::string();

		assert(!game_config::path.empty());

		fs::path fpath(filename);
		fs::path result;

		if(filename[0] == '-')
		{
			result /= get_user_data_path() / "data" / filename.substr(1);
			DBG_FS << " trying '" << result.string() << '\n';
		}
		else if(*fpath.begin() == ".")
		{
			if(!current_dir.empty())
				result /= fs::path(current_dir);
			else
				result /= fs::path(game_config::path) / "data";
			result /= filename;
		}
		else if(!game_config::path.empty())
			result /= fs::path(game_config::path) / "data" / filename;

		if(result.empty() || !file_exists(result))
		{
			DBG_FS << " not found\n";
			result.clear();
		}
		else
			DBG_FS << " found: '" << result.string() << '\n';

		return result.string();
	}
}
