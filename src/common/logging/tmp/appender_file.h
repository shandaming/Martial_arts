/*
 * Copyright (C) 2018
 */

#ifndef APPENDER_FILE_H
#define APPENDER_FILE_H

class appender_file : public appender
{
public:
	typedef std::integral_constant<appender_type, APPENDER_FILE>::type type_index;

	appender_file(uint8 id, const std::string& name, log_level level, appender_flags flags, 
			std::vector<const char*> extra_args);
	~appender_file() { close_file(); }

	FILE* open_file(const std::string& filename, const std::string& mode, bool backup);
	appender_type get_type() const override { return type_index::value; }
private:
	void close_file();
	void write_stream(const log_message* message) override;

	FILE* logfile_;
	std::string filename_;
	std::string log_dir_;
	bool dynamic_name_;
	bool backup_;
	uint64 max_file_size_;
	std::atomic<uint64> file_size_;
};

#endif
