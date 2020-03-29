/*
 * Copyright (C) 2018 
 */

#include <sstream>
#include <memory>

#include "log.h"
#include "appender_console.h"
#include "appender_file.h"
#include "log_operation.h"
#include "common/debugging/errors.h"
#include "common/configuration/config.h"
#include "common/utility/util.h"
#include "event_loop.h"

log::log() : appender_id_(0), lowest_log_level_(LOG_LEVEL_FATAL), event_loop_(nullptr)
{
	logs_timestamp_ = "_" + get_timestamp_str();
	register_appender<appender_console>();
	register_appender<appender_file>();
}

log::~log()
{
	close();
}

uint8_t log::next_appender_id()
{
	return appender_id_++;
}

appender* log::get_appender_by_name(std::string const& name)
{
	auto it = appenders_.begin();
	while (it != appenders_.end() && it->second && it->second->get_name() != name)
		++it;

	return it == appenders_.end() ? nullptr : it->second.get();
}

void log::create_appender_from_config(std::string const& appender_name)
{
	if (appender_name.empty())
		return;

	// Format=type, level, flags, optional1, optional2
	// if type = File. optional1 = file and option2 = mode
	// if type = Console. optional1 = Color
	//std::string options = sConfigMgr->GetStringDefault(appender_name.c_str(), "");
	std::string options = CONFIG_MGR->get_value_default("worldserver", appender_name.c_str(), "");

	tokenizer tokens(options, ',');
	auto iter = tokens.begin();

	size_t size = tokens.size();
	std::string name = appender_name.substr(9);

	if (size < 2)
	{
		fprintf(stderr, "log::create_appender_from_config: Wrong configuration for appender %s. Config line: %s\n", name.c_str(), options.c_str());
		return;
	}

	appender_flags flags = APPENDER_FLAGS_NONE;
	appender_type type = appender_type(atoi(*iter++));
	log_level level = log_level(atoi(*iter++));

	if (level > LOG_LEVEL_FATAL)
	{
		fprintf(stderr, "log::create_appender_from_config: Wrong log Level %d for appender %s\n", level, name.c_str());
		return;
	}

	if (size > 2)
		flags = appender_flags(atoi(*iter++));

	auto factoryFunction = appender_factory_.find(type);
	if (factoryFunction == appender_factory_.end())
	{
		fprintf(stderr, "log::create_appender_from_config: Unknown type %d for appender %s\n", type, name.c_str());
		return;
	}

	try
	{
		appender* appender = factoryFunction->second(next_appender_id(), name, level, flags, std::vector<char const*>(iter, tokens.end()));
		appenders_[appender->get_id()].reset(appender);
	}
	catch (invalid_appender_args_exception const& iaae)
	{
		fprintf(stderr, "%s", iaae.what());
	}
}

void log::create_logger_from_config(std::string const& appender_name)
{
	if (appender_name.empty())
		return;

	log_level level = LOG_LEVEL_DISABLED;
	uint8_t type = uint8_t(-1);

	//std::string options = sConfigMgr->GetStringDefault(appender_name.c_str(), "");
	std::string options = CONFIG_MGR->get_value_default("worldserver", appender_name.c_str(), "");
	std::string name = appender_name.substr(7);

	if (options.empty())
	{
		fprintf(stderr, "log::create_logger_from_config: Missing config option logger.%s\n", name.c_str());
		return;
	}

	tokenizer tokens(options, ',');
	tokenizer::const_iterator iter = tokens.begin();

	if (tokens.size() != 2)
	{
		fprintf(stderr, "log::create_logger_from_config: Wrong config option logger.%s=%s\n", name.c_str(), options.c_str());
		return;
	}

	std::unique_ptr<logger>& logger_ref = loggers_[name];
	if (logger_ref)
	{
		fprintf(stderr, "Error while configuring logger %s. Already defined\n", name.c_str());
		return;
	}

	level = log_level(atoi(*iter++));
	if (level > LOG_LEVEL_FATAL)
	{
		fprintf(stderr, "log::create_logger_from_config: Wrong log Level %u for logger %s\n", type, name.c_str());
		return;
	}

	if (level < lowest_log_level_)
		lowest_log_level_ = level;

	logger_ref = std::make_unique<logger>(name, level);

	std::istringstream ss(*iter);
	std::string str;

	ss >> str;
	while (ss)
	{
		if (appender* appender = get_appender_by_name(str))
			logger_ref->add_appender(appender->get_id(), appender);
		else
			fprintf(stderr, "Error while configuring appender %s in logger %s. appender does not exist", str.c_str(), name.c_str());
		ss >> str;
	}
}

void log::read_appenders_from_config()
{
	//std::vector<std::string> keys = sConfigMgr->GetKeysByString("appender.");
	std::vector<std::string> keys = CONFIG_MGR->get_keys_by_string("log", "appender.");
	for (std::string const& appender_name : keys)
		create_appender_from_config(appender_name);
}

void log::read_loggers_from_config()
{
	//std::vector<std::string> keys = sConfigMgr->GetKeysByString("logger.");
	std::vector<std::string> keys = CONFIG_MGR->get_keys_by_string("log", "logger.");
	for (std::string const& loggerName : keys)
		create_logger_from_config(loggerName);

	// Bad config configuration, creating default config
	if (loggers_.find(LOGGER_ROOT) == loggers_.end())
	{
		fprintf(stderr, "Wrong loggers_ configuration. Review your logger config section.\n"
					"Creating default loggers_ [root (Error), server (Info)] to console\n");

		close(); // Clean any logger or appender created

		appender_console* appender = new appender_console(next_appender_id(), "Console", LOG_LEVEL_DEBUG, APPENDER_FLAGS_NONE, std::vector<char const*>());
		appenders_[appender->get_id()].reset(appender);

		logger* root_logger = new logger(LOGGER_ROOT, LOG_LEVEL_ERROR);
		root_logger->add_appender(appender->get_id(), appender);
		loggers_[LOGGER_ROOT].reset(root_logger);

		logger* server_logger = new logger("server", LOG_LEVEL_INFO);
		server_logger->add_appender(appender->get_id(), appender);
		loggers_["server"].reset(server_logger);
	}
}

void log::register_appender(uint8_t index, appender_creator_fn appender_create_fn)
{
	auto itr = appender_factory_.find(index);
	ASSERT(itr == appender_factory_.end());
	appender_factory_[index] = appender_create_fn;
}

void log::out_message(std::string const& filter, log_level const level, std::string&& message)
{
	write(std::make_unique<log_message>(level, filter, std::move(message)));
}

void log::out_command(std::string&& message, std::string&& param1)
{
	write(std::make_unique<log_message>(LOG_LEVEL_INFO, "commands.gm", std::move(message), std::move(param1)));
}

void log::write(std::unique_ptr<log_message>&& msg) const
{
	logger const* logger = get_logger_by_type(msg->type);

	if (event_loop_)
	{
		std::shared_ptr<log_operation> new_log_operation = std::make_shared<log_operation>(logger, std::move(msg));
		//event_loop_.run_in_loop(std::bind(&log_operation::call, new_log_operation));
		event_loop_->run_in_loop(std::bind([new_log_operation]() { new_log_operation->call(); }));
	}
	else
		logger->write(msg.get());
}

const logger* log::get_logger_by_type(const std::string& type) const
{
	auto it = loggers_.find(type);
	if (it != loggers_.end())
		return it->second.get();

	if (type == LOGGER_ROOT)
		return NULL;

	std::string parent_logger = LOGGER_ROOT;
	size_t found = type.find_last_of(".");
	if (found != std::string::npos)
		parent_logger = type.substr(0, found);

	return get_logger_by_type(parent_logger);
}

std::string log::get_timestamp_str()
{
	time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	std::tm aTm;
	localtime_r(&tt, &aTm);

	//       YYYY   year
	//       MM     month (2 digits 01-12)
	//       DD     day (2 digits 01-31)
	//       HH     hour (2 digits 00-23)
	//       MM     minutes (2 digits 00-59)
	//       SS     seconds (2 digits 00-59)
	try
	{
		return string_format("%04d-%02d-%02d_%02d-%02d-%02d",
		aTm.tm_year + 1900, aTm.tm_mon + 1, aTm.tm_mday, aTm.tm_hour, aTm.tm_min, aTm.tm_sec);
	}
	catch (std::exception const& ex)
	{
		fprintf(stderr, "Failed to initialize timestamp part of log filename! %s", ex.what());
		fflush(stderr);
		ABORT();
	}
	return "";
}

bool log::set_log_level(std::string const& name, const char* new_levelc, bool is_logger /* = true */)
{
	log_level new_level = log_level(atoi(new_levelc));
	if (new_level < 0)
		return false;

	if (is_logger)
	{
		auto it = loggers_.begin();
		while (it != loggers_.end() && it->second->get_name() != name)
			++it;

	if (it == loggers_.end())
		return false;

	it->second->set_log_level(new_level);

	if (new_level != LOG_LEVEL_DISABLED && new_level < lowest_log_level_)
		lowest_log_level_ = new_level;
	}
	else
	{
		appender* appender = get_appender_by_name(name);
		if (!appender)
			return false;

		appender->set_log_level(new_level);
	}

	return true;
}

void log::out_char_dump(char const* str, uint32_t account_id, uint64_t guid, char const* name)
{
	if (!str || !should_log("entities.player.dump", LOG_LEVEL_INFO))
		return;

	std::ostringstream ss;
	ss << "== START DUMP == (account: " << account_id << " guid: " << guid << " name: " << name
	   << ")\n" << str << "\n== END DUMP ==\n";

	std::unique_ptr<log_message> msg(new log_message(LOG_LEVEL_INFO, "entities.player.dump", ss.str()));
	std::ostringstream param;
	param << guid << '_' << name;

	msg->param1 = param.str();

	write(std::move(msg));
}

void log::set_realm_id(uint32_t id)
{
	for (auto it = appenders_.begin(); it != appenders_.end(); ++it)
		it->second->set_realm_id(id);
}

void log::close()
{
	loggers_.clear();
	appenders_.clear();
}

bool log::should_log(const std::string& type, log_level level) const
{
	// TODO: Use cache to store "Type.sub1.sub2": "Type" equivalence, should
	// Speed up in cases where requesting "Type.sub1.sub2" but only configured
	// logger "Type"

	// Don't even look for a logger if the log_level is lower than lowest log levels across all loggers_
	if (level < lowest_log_level_)
		return false;

	const logger* logger = get_logger_by_type(type);
	if (!logger)
		return false;

	log_level log_level = logger->get_log_level();
	return log_level != LOG_LEVEL_DISABLED && log_level <= level;
}

log* log::instance()
{
	static log instance;
	return &instance;
}

void log::initialize(event_loop* event_loop)
{
	if (event_loop)
		event_loop_ = event_loop;

	load_from_config();
}

void log::set_synchronous()
{
	event_loop_ = nullptr;
}

void log::load_from_config()
{
	close();

	lowest_log_level_ = LOG_LEVEL_FATAL;
	appender_id_ = 0;
	//logs_dir_ = sConfigMgr->GetStringDefault("logsDir", "");
	logs_dir_ = CONFIG_MGR->get_value_default("log", "logsDir", "");
	if (!logs_dir_.empty())
		if ((logs_dir_.at(logs_dir_.length() - 1) != '/') && (logs_dir_.at(logs_dir_.length() - 1) != '\\'))
			logs_dir_.push_back('/');

	read_appenders_from_config();
	read_loggers_from_config();
}
