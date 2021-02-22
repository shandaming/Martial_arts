/*
 * Copyright (C) 2018
 */

#include <cstdio>
#include <cstring>
#include <algorithm>

#include "appender_file.h"
#include "log.h"

appender_file::appender_file(uint8_t id, const std::string& name, log_level level, 
		appender_flags flags, std::vector<const char*> extra_args) :
	appender(id, name, level, flags),
	logfile_(NULL),
	log_dir_(LOG->get_logs_dir()),
	max_file_size_(0),
	file_size_(0)
{
	if (extra_args.empty())
		throw invalid_appender_args_exception(string_format("log::create_appender_from_config: "
					"Missing file name for appender %s\n", name.c_str()));

	filename_ = extra_args[0];

	char const* mode = "a";
	if (extra_args.size() > 1)
		mode = extra_args[1];

	if (flags & APPENDER_FLAGS_USE_TIMESTAMP)
	{
		size_t dot_pos = filename_.find_last_of(".");
		if (dot_pos != std::string::npos)
			filename_.insert(dot_pos, LOG->get_logs_timestamp());
		else
			filename_ += LOG->get_logs_timestamp();
	}

	if (extra_args.size() > 2)
		max_file_size_ = atoi(extra_args[2]);

	dynamic_name_ = std::string::npos != filename_.find("%s");
	backup_ = (flags & APPENDER_FLAGS_MAKE_FILE_BACKUP) != 0;

	if (!dynamic_name_)
		logfile_ = open_file(filename_, mode, !strcmp(mode, "w") && backup_);
}

void appender_file::_write(const log_message* message)
{
	bool exceed_max_size = max_file_size_ > 0 && (file_size_.load() + message->size()) > 
		max_file_size_;

	if (dynamic_name_)
	{
		char namebuf[PATH_MAX];
		snprintf(namebuf, PATH_MAX, filename_.c_str(), message->param1.c_str());
	// always use "a" with dynamic name otherwise it could delete the log we wrote in last _write() call
		FILE* file = open_file(namebuf, "a", backup_ || exceed_max_size);
		if (!file)
			return;
		fprintf(file, "%s%s\n", message->prefix.c_str(), message->text.c_str());
		fflush(file);
		file_size_ += uint64_t(message->size());
		fclose(file);
		return;
	}
	else if (exceed_max_size)
	logfile_ = open_file(filename_, "w", true);

	if (!logfile_)
		return;

	fprintf(logfile_, "%s%s\n", message->prefix.c_str(), message->text.c_str());
	fflush(logfile_);
	file_size_ += uint64_t(message->size());
}

FILE* appender_file::open_file(const std::string& filename, const std::string& mode, bool backup)
{
	std::string full_name(log_dir_ + filename);
	if (backup)
	{
		close_file();
		std::string new_name(full_name);
		new_name.push_back('.');
		new_name.append(log_message::get_time_str(time(NULL)));
		std::replace(new_name.begin(), new_name.end(), ':', '-');

		// 没有错误处理...如果无法进行备份，只需忽略
		rename(full_name.c_str(), new_name.c_str()); 
	}

	if (FILE* ret = fopen(full_name.c_str(), mode.c_str()))
	{
		file_size_ = ftell(ret);
		return ret;
	}

	return nullptr;
}

void appender_file::close_file()
{
	if (logfile_)
	{
		fclose(logfile_);
		logfile_ = nullptr;
	}
}
