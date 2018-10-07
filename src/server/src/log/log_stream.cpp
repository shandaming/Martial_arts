/*
 * Copyright (C) 2018
 */

#include "log_stream.h"

namespace lg
{
namespace detail
{

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

template class Log_buffer<small_buffer>;
template class Log_buffer<large_buffer>;

}
}




void Log_stream::static_check()
{
  static_assert(max_numeric_size - 10 > std::numeric_limits<double>::digits10);
  static_assert(max_numeric_size - 10 > std::numeric_limits<long double>::digits10);
  static_assert(max_numeric_size - 10 > std::numeric_limits<long>::digits10);
  static_assert(max_numeric_size - 10 > std::numeric_limits<long long>::digits10);
}

template<typename T>
void Log_stream::format_integer(T v)
{
  if (buffer_.avail() >= max_numeric_size)
  {
    size_t len = convert(buffer_.current(), v);
    buffer_.add(len);
  }
}

inline Log_stream& Log_stream::operator<<(bool v)
{
	buffer_.append(v ? "1" : "0", 1);
	return *this;
}

inline Log_stream& Log_stream::operator<<(int8_t v)
{
	buffer_.append(&v, 1);
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
  format_integer(v);
  return *this;
}

inline Log_stream& Log_stream::operator<<(uint32_t v)
{
  format_integer(v);
  return *this;
}

inline Log_stream& Log_stream::operator<<(int64_t v)
{
  format_integer(v);
  return *this;
}

inline Log_stream& Log_stream::operator<<(uint64_t v)
{
  format_integer(v);
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
  if (buffer_.avail() >= max_numeric_size)
  {
    int len = snprintf(buffer_.current(), max_numeric_size, "%.12g", v);
    buffer_.add(len);
  }
  return *this;
}

inline Log_stream& Log_stream::operator<<(const char* str)
{
	if (str)
	{
		buffer_.append(str, strlen(str));
	}
	else
	{
		buffer_.append("(null)", 6);
	}
	return *this;
}

inline Log_stream& Log_stream::operator<<(const unsigned char* str)
{
	return operator<<(reinterpret_cast<const char*>(str));
}

inline Log_stream& Log_stream::operator<<(const std::string& v)
{
	buffer_.append(v.c_str(), v.size());
	return *this;
}

inline Log_stream& Log_stream::operator<<(const void* p)
{
  uintptr_t v = reinterpret_cast<uintptr_t>(p);
  if (buffer_.avail() >= max_numeric_size)
  {
    char* buf = buffer_.current();
    buf[0] = '0';
    buf[1] = 'x';
    size_t len = convertHex(buf+2, v);
    buffer_.add(len+2);
  }
  return *this;
}



template<typename T>
Fmt::Fmt(const char* fmt, T val)
{
  static_assert(boost::is_arithmetic<T>::value == true);

  length_ = snprintf(buf_, sizeof buf_, fmt, val);
  assert(static_cast<size_t>(length_) < sizeof buf_);
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
