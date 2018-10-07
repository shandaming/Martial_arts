/*
 * Copyright (C) 2018
 */

#include "logging.h"

namespace
{
  extern __thread int t_cachedTid;
  extern __thread char t_tidString[32];
  extern __thread int t_tidStringLength;
extern __thread const char* t_threadName;

 inline int tid()
  {
    if (__builtin_expect(t_cachedTid == 0, 0))
    {
      cacheTid();
    }
    return t_cachedTid;
	}

void CurrentThread::cacheTid()
{
  if (t_cachedTid == 0)
  {
    t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));;
    t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
  }
}

}

namespace lg
{

__thread char t_errnobuf[512];
__thread char t_time[64];
__thread time_t t_lastSecond;

const char* strerror_tl(int savedErrno)
{
  return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
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

const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
  "TRACE ",
  "DEBUG ",
  "INFO  ",
  "WARN  ",
  "ERROR ",
  "FATAL ",
};

// helper class for known string length at compile time
struct T
{
  T(const char* str, unsigned len)
    :str_(str),
     len_(len)
  {
    assert(strlen(str) == len_);
  }

  const char* str_;
  const unsigned len_;
};

inline Log_stream& operator<<(Log_stream& s, T v)
{
  s.append(v.str_, v.len_);
  return s;
}

inline Log_stream& operator<<(Log_stream& s, const Logger::Source_file& v)
{
  s.append(v.data_, v.size_);
  return s;
}

void defaultOutput(const char* msg, int len)
{
  size_t n = fwrite(msg, 1, len, stdout);
  //FIXME check n
  (void)n;
}

void defaultFlush()
{
  fflush(stdout);
}

Logger::Output_func g_output = defaultOutput;
Logger::Flush_func g_flush = defaultFlush;
Time_zone g_logTimeZone;

}


Logger* Logger::instance() { static Logger logger; return &logger; }

void Logger::init(Log_level level, int old_errno, const fs::path& file, int line)
{
	time_ = Timestamp::now();
	level_ = level;
	line_ = line;
	basename_ = file.c_string();

  format_time();
  CurrentThread::tid();
  stream_ << T(CurrentThread::tidString(), CurrentThread::tidStringLength());
  stream_ << T(LogLevelName[level], 6);
  if (old_errno != 0)
  {
    stream_ << strerror_tl(old_errno) << " (errno=" << old_errno << ") ";
  }
}

void Logger::format_time()
{
  int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / Timestamp::kMicroSecondsPerSecond);
  int microseconds = static_cast<int>(microSecondsSinceEpoch % Timestamp::kMicroSecondsPerSecond);
  if (seconds != t_lastSecond)
  {
    t_lastSecond = seconds;
    struct tm tm_time;
    if (g_logTimeZone.valid())
    {
      tm_time = g_logTimeZone.toLocalTime(seconds);
    }
    else
    {
      ::gmtime_r(&seconds, &tm_time); // FIXME Time_zone::fromUtcTime
    }

    int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    assert(len == 17); (void)len;
  }

  if (g_logTimeZone.valid())
  {
    Fmt us(".%06d ", microseconds);
    assert(us.length() == 8);
    stream_ << T(t_time, 17) << T(us.data(), 8);
  }
  else
  {
    Fmt us(".%06dZ ", microseconds);
    assert(us.length() == 9);
    stream_ << T(t_time, 17) << T(us.data(), 9);
  }
}

void Logger::finish()
{
  stream_ << " - " << __FILE__ << ':' << __LINE__ << '\n';
}

void Logger::write(const fs::path& file, int line)
  : impl_(INFO, 0, __FILE__, __LINE__)
{
  Timestamp time_;
  Log_stream stream_;
  Log_level level_;
  int line_;
  fs::path basename_;
}

void Logger::write(const fs::path& file, int line, Log_level level, const char* func)
{
  init(leve, 0, file, line);
}

void Logger::write(const fs::path& file, int line, Log_level level)
{
init(level, 0, file, line);
}

void Logger::write(const fs::path& file, int line, bool to_abort)
{
init(to_abort?FATAL:ERROR, errno, file, line);
}

Logger::~Logger()
{
  impl_.finish();
  const Log_stream::Buffer& buf(stream().buffer());
  g_output(buf.data(), buf.length());
  if (impl_.level_ == FATAL)
  {
    g_flush();
    abort();
  }
}

void Logger::set_log_level(Logger::Log_level level)
{
  g_log_level = level;
}

void Logger::set_output(Output_func out)
{
  g_output = out;
}

void Logger::set_flush(Flush_func flush)
{
  g_flush = flush;
}

void Logger::set_time_zone(const Time_zone& tz)
{
  g_logTimeZone = tz;
}

} // end namespac lg
