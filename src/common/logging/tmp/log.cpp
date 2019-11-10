/*
 * Copyright (C) 2018 
 */

#include "log.h"

log::log() : appender_id_(0), lowestlog_level(LOG_LEVEL_FATAL), _ioContext(nullptr), _strand(nullptr)
{
	logs_timestamp_ = "_" + get_timestamp_str();
	register_appender<appenderConsole>();
	register_appender<appenderFile>();
}

log::~log()
{
	delete _strand;
	close();
}

uint8 log::next_appender_id()
{
	return appender_id++;
}

appender* log::get_appender_by_name(std::string const& name)
{
	auto it = appenders.begin();
	while (it != appenders.end() && it->second && it->second->getName() != name)
		++it;

	return it == appenders.end() ? nullptr : it->second.get();
}

void log::CreateappenderFromConfig(std::string const& appenderName)
{
	if (appenderName.empty())
	return;

	// Format=type, level, flags, optional1, optional2
	// if type = File. optional1 = file and option2 = mode
	// if type = Console. optional1 = Color
	std::string options = sConfigMgr->GetStringDefault(appenderName.c_str(), "");

	Tokenizer tokens(options, ',');
	auto iter = tokens.begin();

	size_t size = tokens.size();
	std::string name = appenderName.substr(9);

	if (size < 2)
	{
	fprintf(stderr, "log::CreateappenderFromConfig: Wrong configuration for appender %s. Config line: %s\n", name.c_str(), options.c_str());
	return;
	}

	appenderFlags flags = APPENDER_FLAGS_NONE;
	appenderType type = appenderType(atoi(*iter++));
	log_level level = log_level(atoi(*iter++));

	if (level > LOG_LEVEL_FATAL)
	{
	fprintf(stderr, "log::CreateappenderFromConfig: Wrong log Level %d for appender %s\n", level, name.c_str());
	return;
	}

	if (size > 2)
	flags = appenderFlags(atoi(*iter++));

	auto factoryFunction = appenderFactory.find(type);
	if (factoryFunction == appenderFactory.end())
	{
	fprintf(stderr, "log::CreateappenderFromConfig: Unknown type %d for appender %s\n", type, name.c_str());
	return;
	}

	try
	{
	appender* appender = factoryFunction->second(Nextappender_id(), name, level, flags, std::vector<char const*>(iter, tokens.end()));
	appenders[appender->getId()].reset(appender);
	}
	catch (InvalidappenderArgsException const& iaae)
	{
	fprintf(stderr, "%s", iaae.what());
	}
}

void log::CreateloggerFromConfig(std::string const& appenderName)
{
	if (appenderName.empty())
	return;

	log_level level = LOG_LEVEL_DISABLED;
	uint8 type = uint8(-1);

	std::string options = sConfigMgr->GetStringDefault(appenderName.c_str(), "");
	std::string name = appenderName.substr(7);

	if (options.empty())
	{
	fprintf(stderr, "log::CreateloggerFromConfig: Missing config option logger.%s\n", name.c_str());
	return;
	}

	Tokenizer tokens(options, ',');
	Tokenizer::const_iterator iter = tokens.begin();

	if (tokens.size() != 2)
	{
	fprintf(stderr, "log::CreateloggerFromConfig: Wrong config option logger.%s=%s\n", name.c_str(), options.c_str());
	return;
	}

	std::unique_ptr<logger>& logger = loggers[name];
	if (logger)
	{
	fprintf(stderr, "Error while configuring logger %s. Already defined\n", name.c_str());
	return;
	}

	level = log_level(atoi(*iter++));
	if (level > LOG_LEVEL_FATAL)
	{
	fprintf(stderr, "log::CreateloggerFromConfig: Wrong log Level %u for logger %s\n", type, name.c_str());
	return;
	}

	if (level < lowestlog_level)
	lowestlog_level = level;

	logger = Trinity::make_unique<logger>(name, level);
	//fprintf(stdout, "log::CreateloggerFromConfig: Created logger %s, Level %u\n", name.c_str(), level);

	std::istringstream ss(*iter);
	std::string str;

	ss >> str;
	while (ss)
	{
	if (appender* appender = GetappenderByName(str))
	{
		logger->addappender(appender->getId(), appender);
		//fprintf(stdout, "log::CreateloggerFromConfig: Added appender %s to logger %s\n", appender->getName().c_str(), name.c_str());
	}
	else
		fprintf(stderr, "Error while configuring appender %s in logger %s. appender does not exist", str.c_str(), name.c_str());
	ss >> str;
	}
}

void log::ReadappendersFromConfig()
{
	std::vector<std::string> keys = sConfigMgr->GetKeysByString("appender.");
	for (std::string const& appenderName : keys)
	CreateappenderFromConfig(appenderName);
}

void log::ReadloggersFromConfig()
{
	std::vector<std::string> keys = sConfigMgr->GetKeysByString("logger.");
	for (std::string const& loggerName : keys)
	CreateloggerFromConfig(loggerName);

	// Bad config configuration, creating default config
	if (loggers.find(LOGGER_ROOT) == loggers.end())
	{
	fprintf(stderr, "Wrong loggers configuration. Review your logger config section.\n"
					"Creating default loggers [root (Error), server (Info)] to console\n");

	close(); // Clean any logger or appender created

	appenderConsole* appender = new appenderConsole(Nextappender_id(), "Console", LOG_LEVEL_DEBUG, APPENDER_FLAGS_NONE, std::vector<char const*>());
	appenders[appender->getId()].reset(appender);

	logger* rootlogger = new logger(LOGGER_ROOT, LOG_LEVEL_ERROR);
	rootlogger->addappender(appender->getId(), appender);
	loggers[LOGGER_ROOT].reset(rootlogger);

	logger* serverlogger = new logger("server", LOG_LEVEL_INFO);
	serverlogger->addappender(appender->getId(), appender);
	loggers["server"].reset(serverlogger);
	}
}

void log::register_appender(uint8 index, appender_creator_fn appender_create_fn)
{
	auto itr = appender_factory.find(index);
	ASSERT(itr == appender_factory.end());
	appender_factory[index] = appender_create_fn;
}

void log::out_message(std::string const& filter, log_level const level, std::string&& message)
{
	write(std::make_unique<log_message>(level, filter, std::move(message)));
}

void log::outCommand(std::string&& message, std::string&& param1)
{
	write(Trinity::make_unique<log_message>(LOG_LEVEL_INFO, "commands.gm", std::move(message), std::move(param1)));
}

void log::write(std::unique_ptr<log_message>&& msg) const
{
	logger const* logger = get_logger_by_type(msg->type);

	if (_ioContext)
	{
	std::shared_ptr<logOperation> logOperation = std::make_shared<logOperation>(logger, std::move(msg));
	Trinity::Asio::post(*_ioContext, Trinity::Asio::bind_executor(*_strand, [logOperation]() { logOperation->call(); }));
	}
	else
		logger->write(msg.get());
}

const logger* log::get_logger_by_type(const std::string& type) const
{
	auto it = loggers.find(type);
	if (it != loggers.end())
		return it->second.get();

	if (type == LOGGER_ROOT)
		return NULL;

	std::string parent_logger = LOGGER_ROOT;
	size_t found = type.find_last_of(".");
	if (found != std::string::npos)
		parent_logger = type.substr(0, found);

	return get_logger_by_type(parentlogger);
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
		return Trinity::StringFormat("%04d-%02d-%02d_%02d-%02d-%02d",
		aTm.tm_year + 1900, aTm.tm_mon + 1, aTm.tm_mday, aTm.tm_hour, aTm.tm_min, aTm.tm_sec);
	}
	catch (std::exception const& ex)
	{
		fprintf(stderr, "Failed to initialize timestamp part of log filename! %s", ex.what());
		fflush(stderr);
		ABORT();
	}
}

bool log::set_log_level(std::string const& name, const char* new_levelc, bool is_logger /* = true */)
{
	log_level new_level = log_level(atoi(new_levelc));
	if (newLevel < 0)
		return false;

	if (islogger)
	{
		auto it = loggers.begin();
		while (it != loggers.end() && it->second->get_name() != name)
			++it;

	if (it == loggers.end())
		return false;

	it->second->set_log_level(new_level);

	if (new_level != LOG_LEVEL_DISABLED && new_level < lowest_log_level)
		lowest_log_level = new_level;
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

void log::outCharDump(char const* str, uint32 accountId, uint64 guid, char const* name)
{
	if (!str || !Shouldlog("entities.player.dump", LOG_LEVEL_INFO))
	return;

	std::ostringstream ss;
	ss << "== START DUMP == (account: " << accountId << " guid: " << guid << " name: " << name
	   << ")\n" << str << "\n== END DUMP ==\n";

	std::unique_ptr<log_message> msg(new log_message(LOG_LEVEL_INFO, "entities.player.dump", ss.str()));
	std::ostringstream param;
	param << guid << '_' << name;

	msg->param1 = param.str();

	write(std::move(msg));
}

void log::set_realm_id(uint32 id)
{
	for (auto it = appenders.begin(); it != appenders.end(); ++it)
		it->second->set_realm_id(id);
}

void log::close()
{
	loggers.clear();
	appenders.clear();
}

bool log::should_log(const std::string& type, log_level level) const
{
	// TODO: Use cache to store "Type.sub1.sub2": "Type" equivalence, should
	// Speed up in cases where requesting "Type.sub1.sub2" but only configured
	// logger "Type"

	// Don't even look for a logger if the log_level is lower than lowest log levels across all loggers
	if (level < lowest_log_level)
		return false;

	const logger* logger = Get_logger_by_type(type);
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

void log::initialize(Trinity::Asio::IoContext* ioContext)
{
	if (ioContext)
	{
	_ioContext = ioContext;
	_strand = new Trinity::Asio::Strand(*ioContext);
	}

	LoadFromConfig();
}

void log::SetSynchronous()
{
	delete _strand;
	_strand = nullptr;
	_ioContext = nullptr;
}

void log::LoadFromConfig()
{
	close();

	lowestlog_level = LOG_LEVEL_FATAL;
	appender_id = 0;
	m_logsDir = sConfigMgr->GetStringDefault("logsDir", "");
	if (!m_logsDir.empty())
	if ((m_logsDir.at(m_logsDir.length() - 1) != '/') && (m_logsDir.at(m_logsDir.length() - 1) != '\\'))
		m_logsDir.push_back('/');

	ReadappendersFromConfig();
	ReadloggersFromConfig();
}
