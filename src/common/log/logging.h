/*
 * Copyright (C)
 */

#ifndef LOG_LOGING_H
#define LOG_LOGING_H

#include <string>

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

	Logger(const std::string& file, int line, const std::string& msg);
	Logger(const std::string& file, int line, Log_level level, const std::string& msg);
	Logger(const std::string& file, int line, Log_level level, const char* func, 
			const std::string& msg);
	Logger(const std::string& file, int line, bool toAbort, const std::string& msg);
	~Logger();

	static Log_level log_level();
	static void set_log_level(Log_level level);

	typedef void (*Output_func)(const char* msg, int len);
	typedef void (*Flush_func)();
	static void set_output(Output_func);
	static void set_flush(Flush_func);
private:
  //typedef Logger::Log_level Log_level;
	void init(Log_level level, int old_errno, const std::string& file, int line);
	const char* format_time(time_t time);
	void finish(const std::string& file, int line);

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
  CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

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

void init_log(const std::string& file, int rolle_size);
#define LOG_INIT(filename, rolle_size) init_log(filename, rolle_size);

#define LOG_TRACE(X) if (lg::Logger::log_level() <= lg::Logger::TRACE) \
  lg::Logger(__FILE__, __LINE__, lg::Logger::TRACE, __func__, X)

#define LOG_DEBUG(X) if (lg::Logger::log_level() <= lg::Logger::DEBUG) \
  lg::Logger(__FILE__, __LINE__, lg::Logger::DEBUG, __func__, X)

#define LOG_INFO(X) if (lg::Logger::log_level() <= lg::Logger::INFO) \
  lg::Logger(__FILE__, __LINE__, X)

#define LOG_WARN(X) lg::Logger(__FILE__, __LINE__, lg::Logger::WARN, X)
#define LOG_ERROR(X) lg::Logger(__FILE__, __LINE__, lg::Logger::ERROR, X)
#define LOG_FATAL(X) lg::Logger(__FILE__, __LINE__, lg::Logger::FATAL, X)
#define LOG_SYSERR(X) lg::Logger(__FILE__, __LINE__, false, X)
#define LOG_SYSFATAL(X) lg::Logger(__FILE__, __LINE__, true, X)

#endif
