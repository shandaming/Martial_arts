/*
 * Copyright (C) 2018-2019
 */

#ifndef LOG_H
#define LOG_H

#include "common/serialization/string_utils.h"

class log
{
public:
	static log* instance();

	void Initialize(Trinity::Asio::IoContext* ioContext);
	void SetSynchronous();
	void LoadFromConfig();
	void Close();
	bool ShouldLog(std::string const& type, LogLevel level) const;
	bool SetLogLevel(std::string const& name, char const* level, bool isLogger = true);

	template<typename Fmt, typename... Args>
	inline void out_message(std::string const& filter, log_level const level, Fmt&& fmt, Args&&... args)
	{
		out_message(filter, level, string_format(std::forward<Format>(fmt), std::forward<Args>(args)...));
	}

	template<typename Format, typename... Args>
	void outCommand(uint32 account, Format&& fmt, Args&&... args)
	{
		if (!ShouldLog("commands.gm", LOG_LEVEL_INFO))
		return;

		outCommand(Trinity::StringFormat(std::forward<Format>(fmt), std::forward<Args>(args)...), std::to_string(account));
	}

	void outCharDump(char const* str, uint32 account_id, uint64 guid, char const* name);

	void SetRealmId(uint32 id);

	template<class AppenderImpl>
	void RegisterAppender()
	{
		using Index = typename AppenderImpl::TypeIndex;
		RegisterAppender(Index::value, &CreateAppender<AppenderImpl>);
	}

	std::string const& GetLogsDir() const { return m_logsDir; }
	std::string const& GetLogsTimestamp() const { return m_logsTimestamp; }
private:
	Log();
	~Log();
	Log(Log const&) = delete;
	Log(Log&&) = delete;
	Log& operator=(Log const&) = delete;
	Log& operator=(Log&&) = delete;

	static std::string GetTimestampStr();
	void write(std::unique_ptr<LogMessage>&& msg) const;

	Logger const* GetLoggerByType(std::string const& type) const;
	Appender* GetAppenderByName(std::string const& name);
	uint8 NextAppenderId();
	void CreateAppenderFromConfig(std::string const& name);
	void CreateLoggerFromConfig(std::string const& name);
	void ReadAppendersFromConfig();
	void ReadLoggersFromConfig();
	void RegisterAppender(uint8 index, AppenderCreatorFn appenderCreateFn);
	void out_message(std::string const& filter, LogLevel const level, std::string&& message);
	void outCommand(std::string&& message, std::string&& param1);

	std::unordered_map<uint8, AppenderCreatorFn> appenderFactory;
	std::unordered_map<uint8, std::unique_ptr<Appender>> appenders;
	std::unordered_map<std::string, std::unique_ptr<Logger>> loggers;
	uint8 AppenderId;
	LogLevel lowestLogLevel;

	std::string m_logsDir;
	std::string m_logsTimestamp;

	Trinity::Asio::IoContext* _ioContext;
	Trinity::Asio::Strand* _strand;
};

#define LOG log::instance()

#define LOG_EXCEPTION_FREE(filter_type, level, ...)	\
	{	\
		try	\
		{	\
			LOG->out_message(filter_type, level, __VA_ARGS__);	\
		}	\
		catch(std::exception& e)	\
		{	\
			LOG->out_message("server", LOG_LEVEL_ERROR,	\
					"Wrong format occurred (%s) at %s:%u.", e.what(), __FILE__, __LINE__);	\
		}	\
	}

#ifdef PERFORMANCE_PROFILING
#define LOG_MESSAGE_BODY(filter_type, level, ...) (void(0))
#else
void check_args(const char*, ...) ATTR_PRINTF(1, 2);
void check_args(const std::string&, ...);

#define LOG_MESSAGE_BODY(filter_type, level, ...)					\
	do																\
	{																\
		if(LOG->should_log(filter_type, level))						\
		{															\
			if(false)												\
				check_args(__VA_ARGS__)								\
																	\
			LOG_EXCEPTION_FREE(filter_type, level, __VA_ARGS__);	\
		}															\
	}while(0)
#endif

#define LOG_TRACE(filter_type, ...)	\
	LOG_MESSAGE_BODY(filter_type, LOG_LEVEL_TRACE, __VA_ARGS__)

#define LOG_DEBUG(filter_type, ...)	\
	LOG_MESSAGE_BODY(filter_type, LOG_LEVEL_DEBUG, __VA_ARGS__)

#define LOG_INFO(filter_type, ...)	\
	LOG_MESSAGE_BODY(filter_type, LOG_LEVEL_INFO, __VA_ARGS__)

#define LOG_WARN(filter_type, ...)	\
	LOG_MESSAGE_BODY(filter_type, LOG_LEVEL_WARN, __VA_ARGS__)

#define LOG_ERROR(filter_type, ...)	\
	LOG_MESSAGE_BODY(filter_type, LOG_LEVEL_ERROR, __VA_ARGS__)

#define LOG_FATAL(filter_type, ...)	\
	LOG_MESSAGE_BODY(filter_type, LOG_LEVEL_FATAL, __VA_ARGS__)

#endif
