/*
 * Copyright (C)
 */

#ifndef LOG_LOGING_H
#define LOG_LOGING_H

#include <experimental/filesystem>

#include "log_stream.h"

namespace lg
{
namespace fs = std::experimental::filesystem;

class Logger
{
public:
  enum Log_level
  {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    NUM_LOG_LEVELS,
  };

static Logger* instance();


  Log_stream& stream() { return stream_; }

  static Log_level log_level();
  static void set_log_level(Log_level level);

  typedef void (*Output_func)(const char* msg, int len);
  typedef void (*Flush_func)();
  static void set_output(Output_func);
  static void set_flush(Flush_func);
  //static void set_time_zone(const Time_zone& tz);

void write(const fs::path& file, int line);
void write(const fs::path& file, int line, Log_level level);
void write(const fs::path& file, int line, Log_level level, const char* func);
void write(const fs::path& file, int line, bool to_abort);

private:
	Logger();
  ~Logger();


  //typedef Logger::Log_level Log_level;
  void init(Log_level level, int old_errno, const fs::path& file, int line);
  void format_time();
  void finish();

  Timestamp time_;
  Log_stream stream_;
  Log_level level_;
  int line_;
  fs::path basename_;
};

extern Logger::Log_level g_log_level;

inline Logger::Log_level Logger::log_level()
{
  return g_log_level;
}

const char* strerror_tl(int savedErrno);


#define CHECK_NOTNULL(val) \
  ::muduo::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

// A small helper for CHECK_NOTNULL().
template <typename T>
T* CheckNotNull(const fs::path& file, int line, const char *names, T* ptr)
{
  if (ptr == NULL)
  {
   Logger(file, line, Logger::FATAL).stream() << names;
  }
  return ptr;
}

} // end namespace lg

#define LOG_TRACE if (lg::Logger::log_level() <= lg::Logger::TRACE) \
  lg::Logger::instance()->write(__FILE__, __LINE__, lg::Logger::TRACE, __func__).stream()

#define LOG_DEBUG if (lg::Logger::log_level() <= lg::Logger::DEBUG) \
  lg::Logger::instance()->write(__FILE__, __LINE__, lg::Logger::DEBUG, __func__).stream()

#define LOG_INFO if (lg::Logger::log_level() <= lg::Logger::INFO) \
  lg::Logger::instance()->write(__FILE__, __LINE__).stream()

#define LOG_WARN lg::Logger::instance()->write(__FILE__, __LINE__, lg::Logger::WARN).stream()
#define LOG_ERROR lg::Logger::instance()->write(__FILE__, __LINE__, lg::Logger::ERROR).stream()
#define LOG_FATAL lg::Logger::instance()->write(__FILE__, __LINE__, lg::Logger::FATAL).stream()
#define LOG_SYSERR lg::Logger::instance()->write(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL lg::Logger::instance()->write(__FILE__, __LINE__, true).stream()

#endif
