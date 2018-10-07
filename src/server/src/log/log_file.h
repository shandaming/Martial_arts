/*
 * Copyright (C) 2018
 */

#ifndef LOG_LOG_FILE_H
#define LOG_LOG_FILE_H

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
	Log_file(const string& basename, off_t roll_size,
			bool thread_safe = true,
			int flush_interval = 3,
			int check_every_n = 1024);

	void append(const char* logline, int len);
	void flush();
	bool roll_file();
private:
	void append_unlocked(const char* logline, int len);

	static string get_log_filename(const string& basename, time_t* now);

	const string basename_;
	const off_t roll_size_;
	const int flush_interval_;
	const int check_every_n_;

	uint64_t count_;

	bool thread_safe_;
	std::mutex mutex_;

	time_t start_of_period_;
	time_t last_roll_;
	time_t last_flush_;
	Scoped_ptr<Append_file> file_;

	const static int k_roll_per_seconds_ = 60*60*24;
};
} // namespace lg

#endif
