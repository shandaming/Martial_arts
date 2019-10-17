/*
 * Copyright (C) 2018
 */

#include <cassert>
#include <iomanip>

#include "logging.h"
#include "common/thread.h"

namespace
{
constexpr int MAX_ERROR_BUF = 64;
 __thread char error_buf[MAX_ERROR_BUF];

__thread time_t t_lastSecond;
/*
  extern __thread int t_cachedTid;
  extern __thread char t_tidString[32];
  extern __thread int t_tidStringLength;
extern __thread const char* t_threadName;

void CurrentThread::cacheTid()
{
  if (t_cachedTid == 0)
  {
    t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));;
    t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
  }
}


 inline int tid()
  {
    if (__builtin_expect(t_cachedTid == 0, 0))
    {
      cacheTid();
    }
    return t_cachedTid;
	}
*/

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
/*
inline Log_stream& operator<<(Log_stream& s, const Logger::Source_file& v)
{
  s.append(v.data_, v.size_);
  return s;
}
*/

void defaultOutput(const char* msg, int len)
{
  size_t n = fwrite(msg, 1, len, stdout);
}

Logger::Output_func g_output = defaultOutput;


void Logger::init(Logger::Log_level level, int old_errno, const fs::path& file, int line)
{
  format_time();
  //CurrentThread::tid();
  stream_ << get_current_thread_id();
  stream_ << Log_level_name[level];
  if (old_errno != 0)
  {
    stream_ << strerror_tl(old_errno) << " (errno=" << old_errno << ") ";
  }
}

void Logger::format_time()
{
  int64_t microSecondsSinceEpoch = Timestamp::now().micro_seconds_since_epoch();
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / Timestamp::kMicro_seconds_per_second);
  int microseconds = static_cast<int>(microSecondsSinceEpoch % Timestamp::kMicro_seconds_per_second);

std::ostringstream os;
  if (seconds != t_lastSecond)
  {
    t_lastSecond = seconds;
    struct tm tm_time;
	time_t t = time(nullptr);
os << std::put_time(std::localtime(&t), "&F &T");
    //assert(os.str().size() == 19);
  }

	char buf[16];
	sprintf(buf, ".%06d", microseconds);

	//os << format_string(".%06d", microseconds);
	os << buf;
	stream_ << os.str();
}

void Logger::finish(const fs::path& file, int line)
{
  stream_ << " - " << file.string() << ':' << line << '\n';
}


Logger::Logger(const fs::path& file, int line)
{
init(INFO, 0, file, line);
}

Logger::Logger(const fs::path& file, int line, Logger::Log_level level)
{
init(level, 0, file, line);
}

Logger::Logger(const fs::path& file, int line, Logger::Log_level level, const char* func)
{
  init(level, 0, file, line);
	stream_ << func << ' ';
}

Logger::Logger(const fs::path& file, int line, bool to_abort)
{
init(to_abort?FATAL:ERROR, errno, file, line);
}

Logger::~Logger()
{
  finish(basename_, line_);
  //const Log_stream::Buffer& buf(stream().buffer());
  //g_output(buf.data(), buf.length());
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
