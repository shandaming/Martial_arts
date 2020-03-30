/*
 * Copyright (C) 2018-2019
 */

#ifndef LOGGING_LOG_H
#define LOGGING_LOG_H

#include <memory>
#include <vector>

#include "logger.h"
#include "string_format.h"

#define LOGGER_ROOT "root"

typedef appender*(*appender_creator_fn)(uint8_t id, std::string const& name, log_level level, appender_flags flags, std::vector<char const*>&& extra_args);

template<typename AppenderImpl>
appender* create_appender(uint8_t id, std::string const& name, log_level level, appender_flags flags, std::vector<char const*>&& extra_args)
{
    return new AppenderImpl(id, name, level, flags, std::forward<std::vector<char const*>>(extra_args));
}

class event_loop;

class log
{
public:
	static log* instance();

	void initialize(event_loop* event_loop);
	void set_synchronous();
	void load_from_config();
	void close();
	bool should_log(std::string const& type, log_level level) const;
	bool set_log_level(std::string const& name, char const* level, bool is_logger = true);

	template<typename Fmt, typename... Args>
	inline void out_message(std::string const& filter, log_level const level, Fmt&& fmt, Args&&... args)
	{
		out_message(filter, level, string_format(std::forward<Fmt>(fmt), std::forward<Args>(args)...));
	}

	template<typename Format, typename... Args>
	void out_command(uint32_t account, Format&& fmt, Args&&... args)
	{
		if (!should_log("commands.gm", LOG_LEVEL_INFO))
		return;

		out_command(string_format(std::forward<Format>(fmt), std::forward<Args>(args)...), std::to_string(account));
	}

	void out_char_dump(char const* str, uint32_t account_id, uint64_t guid, char const* name);

	void set_realm_id(uint32_t id);

	template<typename AppenderImpl>
	void register_appender()
	{
		using index = typename AppenderImpl::type_index;
		register_appender(index::value, &create_appender<AppenderImpl>);
	}

	std::string const& get_logs_dir() const { return logs_dir_; }
	std::string const& get_logs_timestamp() const { return logs_timestamp_; }
private:
	log();
	~log();
	log(log const&) = delete;
	log(log&&) = delete;
	log& operator=(log const&) = delete;
	log& operator=(log&&) = delete;

	static std::string get_timestamp_str();
	void write(std::unique_ptr<log_message>&& msg) const;

	logger const* get_logger_by_type(std::string const& type) const;
	appender* get_appender_by_name(std::string const& name);
	uint8_t next_appender_id();
	void create_appender_from_config(std::string const& name);
	void create_logger_from_config(std::string const& name);
	void read_appenders_from_config();
	void read_loggers_from_config();
	void register_appender(uint8_t index, appender_creator_fn appender_create_fn);
	void out_message(std::string const& filter, log_level const level, std::string&& message);
	void out_command(std::string&& message, std::string&& param1);

	std::unordered_map<uint8_t, appender_creator_fn> appender_factory_;
	std::unordered_map<uint8_t, std::unique_ptr<appender>> appenders_;
	std::unordered_map<std::string, std::unique_ptr<logger>> loggers_;
	uint8_t appender_id_;
	log_level lowest_log_level_;

	std::string logs_dir_;
	std::string logs_timestamp_;

	event_loop* event_loop_;
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
