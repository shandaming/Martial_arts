/*
 * Copyright (C) 2018
 */

#ifndef LOG_LOG_FILE_H
#define LOG_LOG_FILE_H

#include <string>
#include <memory>

namespace lg
{
class Append_file
{
public:
	explicit Append_file(const std::string& filename);

	~Append_file();

	void append(const char* logline, const size_t len);

	void flush();

	off_t written_bytes() const { return written_bytes_; }
private:
	size_t write(const char* logline, size_t len);

	FILE* fp_;
	char buffer_[64*1024];
	off_t written_bytes_;
};

class Log_file
{
public:
	Log_file(const std::string& basename, off_t roll_size,
			int flush_interval = 3,
			int check_every_n = 1024); //默认分割行数1024

	void append(const char* logline, int len);
	void flush();
	bool roll_file();
private:
	void append_unlocked(const char* logline, int len);

	static std::string get_log_filename(const std::string& basename, time_t* now);

	const std::string basename_;
	const off_t roll_size_;
	const int flush_interval_;
	const uint64_t check_every_n_;

	uint64_t count_;

	//开始记录日志时间（调整至零点时间，如12.04:11.24和 11.12.04:12.50，调整零点都是12.04:00.00，是同一天，只用来比较同一天，和日志名无关
	time_t start_of_period_;
	time_t last_roll_;
	time_t last_flush_;
	std::unique_ptr<Append_file> file_;

	const static int k_roll_per_seconds_ = 60*60*24; // 一天
};
} // namespace lg

#endif
