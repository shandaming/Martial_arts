/*
 * Copyright (C) 2018
 */

#include <cassert>
#include <iomanip>

#include "logging.h"
#include "common/thread.h"
#include "async_log.h"
#include "commont/serialization/string_utils.h"

namespace
{
constexpr int MAX_ERROR_BUF = 64;
 __thread char error_buf[MAX_ERROR_BUF];

__thread time_t t_lastSecond;

}

namespace lg
{
const char* strerror_tl(int saved_errno)
{
  return strerror_r(saved_errno, error_buf, sizeof error_buf);
}

Logger::Log_level initLogLevel()
{
  if (getenv("LOG_TRACE"))
    return Logger::TRACE;
  else if (getenv("LOG_DEBUG"))
    return Logger::DEBUG;
  else
    return Logger::INFO;
}

Logger::Log_level g_log_level = initLogLevel();

const char* Log_level_name[Logger::NUM_LOG_LEVELS] =
{
  "TRACE ",
  "DEBUG ",
  "INFO  ",
  "WARN  ",
  "ERROR ",
  "FATAL ",
};


inline Log_stream& operator<<(Log_stream& s, std::string& v)
{
  s << v;
  return s;
}

void defaultOutput(const char* msg, int len)
{
  size_t n = fwrite(msg, 1, len, stdout);
}

Logger::Output_func g_output = defaultOutput;


void Logger::init(Logger::Log_level level, int old_errno, const std::string& file, int line, 
		const std::string& msg)
{
  format_time();
  //CurrentThread::tid();
  stream_ << get_current_thread_id();
  stream_ << Log_level_name[level];
  std::string log_msg;
  if (old_errno != 0)
  {
    stream_ << strerror_tl(old_errno) << " (errno=" << old_errno << ") ";
	log_msg = msg + std::string(strerror_tl(old_errno)) + " (errno=" + old_errno + ") ";
  }
  log_msg = string_format("%s[%d]%s: %s", format_time(NULL), get_current_pthread_id(), 
		  log_level_name[level], log_msg.c_str());
	ASYNC_LOG->append(log_msg.c_str(), log_msg.size());
}

const char* Logger::format_time(time_t time)
{
	tm atm;
	localtime_t(&time, &atm);
	char buf[20];
	snprintf(buf, 20, "%04d-%02d-%02d_%02d:%02d:%02d", atm.tm_year + 1900, atm.tm_mon + 1, 
			atm.tm_mday, atm.tm_hour, atm.tm_min, atm.tm_sec);
	return buf;
}

void Logger::finish(const std::string& file, int line)
{
  stream_ << " - " << file.string() << ':' << line << '\n';
}


Logger::Logger(const std::string& file, int line, const std::string& msg)
{
init(INFO, 0, file, line);
}

Logger::Logger(const std::string& file, int line, Logger::Log_level level, const std::string& msg)
{
init(level, 0, file, line);
}

Logger::Logger(const std::string& file, int line, Logger::Log_level level, const char* func,
		const std::string& msg)
{
  init(level, 0, file, line);
	stream_ << func << ' ';
}

Logger::Logger(const std::string& file, int line, bool to_abort, const std::string& msg)
{
init(to_abort?FATAL:ERROR, errno, file, line);
}

Logger::~Logger()
{
  finish(basename_, line_);
  if (level_ == FATAL)
  {
     fflush(stdout);
    abort();
  }
}

void Logger::set_log_level(Logger::Log_level level)
{
  g_log_level = level;
}

void Logger::set_output(Output_func out)
{
  //g_output = out;
}

void Logger::set_flush(Flush_func flush)
{
  //g_flush = flush;
}


} // end namespac

void init_log(const std::string& file, int rolle_size)
{
	ASYNC_LOG->init(file, rolle_size);
	ASYNC_LOG->start();
}
