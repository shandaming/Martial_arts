/*
 * Copyright (C) 2018
 */

#include <algorithm>

#include "log_stream.h"
#include "async_log.h"

namespace lg
{
namespace
{
Async_log* log = nullptr;

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
static_assert(sizeof(digits) == 20);

const char digitsHex[] = "0123456789ABCDEF";
static_assert(sizeof digitsHex == 17);

// Efficient Integer to String Conversions, by Matthew Wilson.
template<typename T>
size_t convert(char buf[], T value)
{
  T i = value;
  char* p = buf;

  do
  {
    int lsd = static_cast<int>(i % 10);
    i /= 10;
    *p++ = zero[lsd];
  } while (i != 0);

  if (value < 0)
  {
    *p++ = '-';
  }
  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}

size_t convertHex(char buf[], uintptr_t value)
{
  uintptr_t i = value;
  char* p = buf;

  do
  {
    int lsd = static_cast<int>(i % 16);
    i /= 16;
    *p++ = digitsHex[lsd];
  } while (i != 0);

  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}

//template class detail::Log_buffer<detail::small_buffer>;
//template class detail::Log_buffer<detail::large_buffer>;

}


void init_log(const std::string& file, int rolle_size)
{
	if(log == nullptr)
	{
		log = new Async_log(file, rolle_size);
		log->start();
	}
}

/*
void Log_stream::static_check()
{
  static_assert(max_numeric_size - 10 > std::numeric_limits<double>::digits10);
  static_assert(max_numeric_size - 10 > std::numeric_limits<long double>::digits10);
  static_assert(max_numeric_size - 10 > std::numeric_limits<long>::digits10);
  static_assert(max_numeric_size - 10 > std::numeric_limits<long long>::digits10);
}*/

template<typename T>
void Log_stream::format_integer(T v)
{
  //if (buffer_.avail() >= max_numeric_size)
  {
    //size_t len = convert(buffer_.current(), v);
   // log_.add(len);
  }
}

Log_stream::Log_stream()
{
	if(log_ == nullptr)
	{
		log_ = log;
	}
}

inline Log_stream& Log_stream::operator<<(bool v)
{
	log_->append(v ? "1" : "0", 1);
	return *this;
}

inline Log_stream& Log_stream::operator<<(int8_t v)
{
	log_->append(std::to_string(v).c_str(), 1);
	return *this;
}

inline Log_stream& Log_stream::operator<<(uint8_t v)
{
	*this << static_cast<char>(v);
	return *this;
}

inline Log_stream& Log_stream::operator<<(int16_t v)
{
  *this << static_cast<int>(v);
  return *this;
}

inline Log_stream& Log_stream::operator<<(uint16_t v)
{
  *this << static_cast<unsigned int>(v);
  return *this;
}

inline Log_stream& Log_stream::operator<<(int32_t v)
{
  //format_integer(v);
log_->append(std::to_string(v).c_str(), sizeof(int32_t));
  return *this;
}

inline Log_stream& Log_stream::operator<<(uint32_t v)
{
  //format_integer(v);
log_->append(std::to_string(v).c_str(), sizeof(uint32_t));
  return *this;
}

inline Log_stream& Log_stream::operator<<(int64_t v)
{
  //format_integer(v);
log_->append(std::to_string(v).c_str(), sizeof(int64_t));
  return *this;
}

inline Log_stream& Log_stream::operator<<(uint64_t v)
{
  //format_integer(v);
log_->append(std::to_string(v).c_str(), sizeof(uint64_t));
  return *this;
}

inline Log_stream& Log_stream::operator<<(float v)
{
	*this << static_cast<double>(v);
	return *this;
}

// FIXME: replace this with Grisu3 by Florian Loitsch.
inline Log_stream& Log_stream::operator<<(double v)
{
  //if (buffer_.avail() >= max_numeric_size)
  {
    //int len = snprintf(buffer_.current(), max_numeric_size, "%.12g", v);
    //log_.add(len);
  }
log_->append(std::to_string(v).c_str(), sizeof(double));
  return *this;
}

inline Log_stream& Log_stream::operator<<(const char* str)
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

inline Log_stream& Log_stream::operator<<(const unsigned char* str)
{
	return operator<<(reinterpret_cast<const char*>(str));
}

inline Log_stream& Log_stream::operator<<(const std::string& v)
{
	log_->append(v.c_str(), v.size());
	return *this;
}

void Log_stream::append(const char* data, int len) { log_->append(data, len); }

template<typename T>
Fmt::Fmt(const char* fmt, T val)
{
  static_assert(std::is_arithmetic<T>::value == true);

  length_ = snprintf(buf_, sizeof buf_, fmt, val);
  //assert(static_cast<size_t>(length_) < sizeof buf_);
}

// Explicit instantiations

template Fmt::Fmt(const char* fmt, char);

template Fmt::Fmt(const char* fmt, short);
template Fmt::Fmt(const char* fmt, unsigned short);
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);
template Fmt::Fmt(const char* fmt, long);
template Fmt::Fmt(const char* fmt, unsigned long);
template Fmt::Fmt(const char* fmt, long long);
template Fmt::Fmt(const char* fmt, unsigned long long);

template Fmt::Fmt(const char* fmt, float);
template Fmt::Fmt(const char* fmt, double);
} // end namespac lg
