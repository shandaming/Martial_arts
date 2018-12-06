/*
 * Copyright (C) 2018
 */

#include <algorithm>

#include "log_stream.h"
#include "async_log.h"

namespace lg
{
static Async_log* log;


void init_log(const std::string& file, int rolle_size)
{
	if(log == nullptr)
	{
		log = new Async_log(file, rolle_size);
		log->start();
	}
}

Async_log* Log_stream::log_ = nullptr;

Log_stream::Log_stream()
{
	if(log_ == nullptr)
	{
		log_ = log;
	}
}

Log_stream& Log_stream::operator<<(bool v)
{
	log_->append(v ? "1" : "0", 1);
	return *this;
}

Log_stream& Log_stream::operator<<(int8_t v)
{
	log_->append(std::to_string(v).c_str(), 1);
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
log_->append(std::to_string(v).c_str(), sizeof(int32_t));
  return *this;
}

Log_stream& Log_stream::operator<<(uint32_t v)
{
log_->append(std::to_string(v).c_str(), sizeof(uint32_t));
  return *this;
}

Log_stream& Log_stream::operator<<(int64_t v)
{
log_->append(std::to_string(v).c_str(), sizeof(int64_t));
  return *this;
}

Log_stream& Log_stream::operator<<(uint64_t v)
{
log_->append(std::to_string(v).c_str(), sizeof(uint64_t));
  return *this;
}

Log_stream& Log_stream::operator<<(float v)
{
	*this << static_cast<double>(v);
	return *this;
}

Log_stream& Log_stream::operator<<(double v)
{
log_->append(std::to_string(v).c_str(), sizeof(double));
  return *this;
}

Log_stream& Log_stream::operator<<(const char* str)
{
	if (str)
	{
		log_->append(str, strlen(str));
	}
	else
	{
		log_->append("(null)", 6);
	}
	return *this;
}

Log_stream& Log_stream::operator<<(const unsigned char* str)
{
	return operator<<(reinterpret_cast<const char*>(str));
}

Log_stream& Log_stream::operator<<(const std::string& v)
{
	log_->append(v.c_str(), v.size());
	return *this;
}
} // end namespac
