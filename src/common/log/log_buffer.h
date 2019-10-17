/*
 * Copyright (C) 2018
 */

#ifndef LOG_LOG_BUFFER_H
#define LOG_LOG_BUFFER_H

#include <cstring>

namespace lg
{
constexpr int small_buffer = 4096;
constexpr int large_buffer = 4194304;

template<int SIZE>
class Log_buffer
{
public:
	Log_buffer() : current_(data_) {}

	~Log_buffer() {}

	void append(const char* buf, size_t len)
	{
		if (static_cast<size_t>(avail()) > len)
		{
			memcpy(current_, buf, len);
			current_ += len;
		}
	}

	const char* data() const { return data_; }
	int length() const { return static_cast<int>(current_ - data_); }

	char* current() { return current_; }
	int avail() const { return static_cast<int>(end() - current_); }
	void add(size_t len) { current_ += len; }

	void reset() { current_ = data_; }
	void bzero() { memset(data_, 0, sizeof data_); }

	// for used by GDB
	const char* debug_string()
	{
		*current_ = '\0';
		return data_;
	}


	// for used by unit test
	std::string to_string() const { return string(data_, length()); }
private:
	const char* end() const { return data_ + sizeof data_; }

	char data_[SIZE] = {0};
	char* current_;
};
}

#endif
