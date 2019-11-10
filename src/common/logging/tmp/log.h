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

	void initialize(Trinity::Asio::IoContext* ioContext);
	void SetSynchronous();
	void LoadFromConfig();
	void close();
	bool should_log(std::string const& type, log_level level) const;
	bool set_log_level(std::string const& name, char const* level, bool islogger = true);

	template<typename Fmt, typename... Args>
	inline void out_message(std::string const& filter, log_level const level, Fmt&& fmt, Args&&... args)
	{
		out_message(filter, level, string_format(std::forward<Format>(fmt), std::forward<Args>(args)...));
	}

	template<typename Format, typename... Args>
	void outCommand(uint32 account, Format&& fmt, Args&&... args)
	{
		if (!Shouldlog("commands.gm", LOG_LEVEL_INFO))
		return;

		outCommand(Trinity::StringFormat(std::forward<Format>(fmt), std::forward<Args>(args)...), std::to_string(account));
	}

	void outCharDump(char const* str, uint32 account_id, uint64 guid, char const* name);

	void set_realm_id(uint32 id);

	template<class appenderImpl>
	void register_appender()
	{
		using Index = typename appenderImpl::TypeIndex;
		register_appender(Index::value, &Createappender<appenderImpl>);
	}

	std::string const& get_logs_dir() const { return logs_dir_; }
	std::string const& get_logs_timestamp() const { return log_timestamp_; }
private:
	log();
	~log();
	log(log const&) = delete;
	log(log&&) = delete;
	log& operator=(log const&) = delete;
	log& operator=(log&&) = delete;

	static std::string get_timestamp_str();
	void write(std::unique_ptr<log_message>&& msg) const;

	logger const* GetloggerByType(std::string const& type) const;
	appender* GetappenderByName(std::string const& name);
	uint8 Nextappender_id();
	void CreateappenderFromConfig(std::string const& name);
	void CreateloggerFromConfig(std::string const& name);
	void ReadappendersFromConfig();
	void ReadloggersFromConfig();
	void register_appender(uint8 index, appenderCreatorFn appenderCreateFn);
	void out_message(std::string const& filter, log_level const level, std::string&& message);
	void outCommand(std::string&& message, std::string&& param1);

	std::unordered_map<uint8, appenderCreatorFn> appenderFactory;
	std::unordered_map<uint8, std::unique_ptr<appender>> appenders;
	std::unordered_map<std::string, std::unique_ptr<logger>> loggers;
	uint8 appender_id_;
	log_level lowest_log_level;

	std::string logs_dir_;
	std::string log_timestamp_;

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
