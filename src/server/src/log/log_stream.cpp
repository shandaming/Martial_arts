/*
 * Copyright (C) 2018
 */

#include <algorithm>

#include "log_stream.h"
#include "async_log.h"

namespace lg
{
void init_log(const std::string& file, int rolle_size)
{
	ASYNC_LOG->init(file, rolle_size);
	ASYNC_LOG->start();
}

Log_stream::Log_stream() {}

Log_stream& Log_stream::operator<<(bool v)
{
	ASYNC_LOG->append(v ? "1" : "0", 1);
	return *this;
}

Log_stream& Log_stream::operator<<(int8_t v)
{
	ASYNC_LOG->append(std::to_string(v).c_str(), 1);
	return *this;
}

Log_stream& Log_stream::operator<<(uint8_t v)
{
	*this << static_cast<char>(v);
	return *this;
}

Log_stream& Log_stream::operator<<(int16_t v)
{
	*this << static_cast<int>(v);
	return *this;
}

Log_stream& Log_stream::operator<<(uint16_t v)
{
  	*this << static_cast<unsigned int>(v);
  	return *this;
}

Log_stream& Log_stream::operator<<(int32_t v)
{
	ASYNC_LOG->append(std::to_string(v).c_str(), sizeof(int32_t));
  	return *this;
}

Log_stream& Log_stream::operator<<(uint32_t v)
{
	ASYNC_LOG->append(std::to_string(v).c_str(), sizeof(uint32_t));
  	return *this;
}

Log_stream& Log_stream::operator<<(int64_t v)
{
	ASYNC_LOG->append(std::to_string(v).c_str(), sizeof(int64_t));
  	return *this;
}

Log_stream& Log_stream::operator<<(uint64_t v)
{
	ASYNC_LOG->append(std::to_string(v).c_str(), sizeof(uint64_t));
  	return *this;
}

Log_stream& Log_stream::operator<<(float v)
{
	*this << static_cast<double>(v);
	return *this;
}

Log_stream& Log_stream::operator<<(double v)
{
	ASYNC_LOG->append(std::to_string(v).c_str(), sizeof(double));
  	return *this;
}

Log_stream& Log_stream::operator<<(const char* str)
{
	if (str)
	{
		ASYNC_LOG->append(str, strlen(str));
	}
	else
	{
		ASYNC_LOG->append("(null)", 6);
	}
	return *this;
}

Log_stream& Log_stream::operator<<(const unsigned char* str)
{
	return operator<<(reinterpret_cast<const char*>(str));
}

Log_stream& Log_stream::operator<<(const std::string& v)
{
	ASYNC_LOG->append(v.c_str(), v.size());
	return *this;
}
} // end namespac
