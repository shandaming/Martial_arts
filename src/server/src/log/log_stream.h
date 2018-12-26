/*
 * Copyright (C) 2018
 */

#ifndef LOG_LOG_STREAM_H
#define LOG_LOG_STREAM_H

#include <string>
#include <cstdint>

#include "log_buffer.h"

namespace lg
{
class Async_log;

class Log_stream
{
public:
	typedef Log_buffer<small_buffer> Buffer;
	
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
	static const int max_numeric_size = 32;
};

void init_log(const std::string& file, int rolle_size);

} // end namespace

#define LOG_INIT(filename, rolle_size) lg::init_log(filename, rolle_size);

#endif
