/*
 * Copyright (C) 2018
 */

#include "appender_file.h"

appender_file::appender_file(uint8 id, const std::string& name, log_level level, 
		appender_flags flags, std::vector<const char*> extra_args) :
	appender(id, name, level, flags),
	logfile_(NULL),
	logdir_(sLog->get_logs_dir()),
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
			filename_.insert(dot_pos, sLog->GetLogsTimestamp());
		else
			filename_ += sLog->GetLogsTimestamp();
	}

	if (extraArgs.size() > 2)
		_maxFileSize = atoi(extraArgs[2]);

	dynamic_name_ = std::string::npos != _fileName.find("%s");
	backup_ = (flags & APPENDER_FLAGS_MAKE_FILE_BACKUP) != 0;

	if (!dynamic_name_)
		logfile_ = open_file(_fileName, mode, !strcmp(mode, "w") && backup_);
}

void appender_file::write_stream(const log_message* message)
{
	bool exceed_max_size = max_file_size_ > 0 && (file_size_.load() + message->size()) > 
		max_file_size_;

	if (dynamic_name_)
	{
		char namebuf[TRINITY_PATH_MAX];
		snprintf(namebuf, TRINITY_PATH_MAX, _fileName.c_str(), message->param1.c_str());
	// always use "a" with dynamic name otherwise it could delete the log we wrote in last _write() call
		FILE* file = open_file(namebuf, "a", backup_ || exceed_max_size);
		if (!file)
			return;
		fprintf(file, "%s%s\n", message->prefix.c_str(), message->text.c_str());
		fflush(file);
		file_size_ += uint64(message->size());
		fclose(file);
		return;
	}
	else if (exceed_max_size)
	logfile_ = open_file(_fileName, "w", true);

	if (!logfile_)
		return;

	fprintf(logfile_, "%s%s\n", message->prefix.c_str(), message->text.c_str());
	fflush(logfile_);
	file_size_ += uint64(message->Size());
}

FILE* appender_file::open_file(const std::string& filename, const std::string& mode, bool backup)
{
	std::string full_name(log_dir_ + filename);
	if (backup)
	{
		close_file();
		std::string new_name(full_name);
		new_name.push_back('.');
		new_name.append(log_message::getTimeStr(time(NULL)));
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
