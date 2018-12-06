/*
 * Copyright (C) 2018
 */

#ifndef LOG_LOG_STREAM_H
#define LOG_LOG_STREAM_H

#include <cstring>
#include <string>
#include <cstdint>

namespace lg
{

namespace detail
{
constexpr int small_buffer = 4096;
constexpr int large_buffer = 4194304;

template<int SIZE>
class Log_buffer
{
public:
	Log_buffer() : current_(data_) { /*set_cookie(cookie_start);*/ }

	~Log_buffer() { /*set_cookie(cookie_end);*/ }

	void append(const char* buf, size_t len)
	{
		// FIXME: append partially
		if (static_cast<size_t>(avail()) > len)
		{
			memcpy(current_, buf, len);
			current_ += len;
		}
	}

	const char* data() const { return data_; }
	int length() const { return static_cast<int>(current_ - data_); }

	// write to data_ directly
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

	//void set_cookie(void (*cookie)()) { cookie_ = cookie; }

	// for used by unit test
	std::string to_string() const { return string(data_, length()); }
	//StringPiece toStringPiece() const { return StringPiece(data_, length()); }
private:
	const char* end() const { return data_ + sizeof data_; }
	// Must be outline function for cookies.
	static void cookie_start();
	static void cookie_end();

	//void (*cookie_)();

	char data_[SIZE] = {0};
	char* current_;
};

template<int SIZE>
void Log_buffer<SIZE>::cookie_start() {}

template<int SIZE>
void Log_buffer<SIZE>::cookie_end() {}

} // namespace detail

class Async_log;

class Log_stream
{
public:
	typedef detail::Log_buffer<detail::small_buffer> Buffer;
	
	Log_stream();

	Log_stream(const Log_stream&) = delete;
	Log_stream& operator=(const Log_stream&) = delete;

	Log_stream& operator<<(bool v);
	Log_stream& operator<<(int8_t v);
	Log_stream& operator<<(uint8_t v);
	Log_stream& operator<<(int16_t);
	Log_stream& operator<<(uint16_t);
	Log_stream& operator<<(int32_t);
	Log_stream& operator<<(uint32_t);
	Log_stream& operator<<(int64_t);
	Log_stream& operator<<(uint64_t);
	Log_stream& operator<<(float v);
	Log_stream& operator<<(double);

	Log_stream& operator<<(const char* str);
	Log_stream& operator<<(const unsigned char* str);

	Log_stream& operator<<(const std::string& v);
private:
	static Async_log* log_;

	static const int max_numeric_size = 32;
};

void init_log(const std::string& file, int rolle_size);

} // end namespace lg

#define LOG_INIT(filename, rolle_size) lg::init_log(filename, rolle_size)

#endif
