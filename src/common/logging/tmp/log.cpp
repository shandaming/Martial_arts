/*
 * Copyright (C) 2018 
 */

#include "log.h"

log::log() : AppenderId(0), lowestlogLevel(LOG_LEVEL_FATAL), _ioContext(nullptr), _strand(nullptr)
{
	m_logsTimestamp = "_" + GetTimestampStr();
	RegisterAppender<AppenderConsole>();
	RegisterAppender<AppenderFile>();
}

log::~log()
{
	delete _strand;
	Close();
}

uint8 log::NextAppenderId()
{
	return AppenderId++;
}

Appender* log::GetAppenderByName(std::string const& name)
{
	auto it = appenders.begin();
	while (it != appenders.end() && it->second && it->second->getName() != name)
	++it;

	return it == appenders.end() ? nullptr : it->second.get();
}

void log::CreateAppenderFromConfig(std::string const& appenderName)
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
	fprintf(stderr, "log::CreateAppenderFromConfig: Wrong configuration for appender %s. Config line: %s\n", name.c_str(), options.c_str());
	return;
	}

	AppenderFlags flags = APPENDER_FLAGS_NONE;
	AppenderType type = AppenderType(atoi(*iter++));
	logLevel level = logLevel(atoi(*iter++));

	if (level > LOG_LEVEL_FATAL)
	{
	fprintf(stderr, "log::CreateAppenderFromConfig: Wrong log Level %d for appender %s\n", level, name.c_str());
	return;
	}

	if (size > 2)
	flags = AppenderFlags(atoi(*iter++));

	auto factoryFunction = appenderFactory.find(type);
	if (factoryFunction == appenderFactory.end())
	{
	fprintf(stderr, "log::CreateAppenderFromConfig: Unknown type %d for appender %s\n", type, name.c_str());
	return;
	}

	try
	{
	Appender* appender = factoryFunction->second(NextAppenderId(), name, level, flags, std::vector<char const*>(iter, tokens.end()));
	appenders[appender->getId()].reset(appender);
	}
	catch (InvalidAppenderArgsException const& iaae)
	{
	fprintf(stderr, "%s", iaae.what());
	}
}

void log::CreateloggerFromConfig(std::string const& appenderName)
{
	if (appenderName.empty())
	return;

	logLevel level = LOG_LEVEL_DISABLED;
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

	level = logLevel(atoi(*iter++));
	if (level > LOG_LEVEL_FATAL)
	{
	fprintf(stderr, "log::CreateloggerFromConfig: Wrong log Level %u for logger %s\n", type, name.c_str());
	return;
	}

	if (level < lowestlogLevel)
	lowestlogLevel = level;

	logger = Trinity::make_unique<logger>(name, level);
	//fprintf(stdout, "log::CreateloggerFromConfig: Created logger %s, Level %u\n", name.c_str(), level);

	std::istringstream ss(*iter);
	std::string str;

	ss >> str;
	while (ss)
	{
	if (Appender* appender = GetAppenderByName(str))
	{
		logger->addAppender(appender->getId(), appender);
		//fprintf(stdout, "log::CreateloggerFromConfig: Added Appender %s to logger %s\n", appender->getName().c_str(), name.c_str());
	}
	else
		fprintf(stderr, "Error while configuring Appender %s in logger %s. Appender does not exist", str.c_str(), name.c_str());
	ss >> str;
	}
}

void log::ReadAppendersFromConfig()
{
	std::vector<std::string> keys = sConfigMgr->GetKeysByString("Appender.");
	for (std::string const& appenderName : keys)
	CreateAppenderFromConfig(appenderName);
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

	Close(); // Clean any logger or Appender created

	AppenderConsole* appender = new AppenderConsole(NextAppenderId(), "Console", LOG_LEVEL_DEBUG, APPENDER_FLAGS_NONE, std::vector<char const*>());
	appenders[appender->getId()].reset(appender);

	logger* rootlogger = new logger(LOGGER_ROOT, LOG_LEVEL_ERROR);
	rootlogger->addAppender(appender->getId(), appender);
	loggers[LOGGER_ROOT].reset(rootlogger);

	logger* serverlogger = new logger("server", LOG_LEVEL_INFO);
	serverlogger->addAppender(appender->getId(), appender);
	loggers["server"].reset(serverlogger);
	}
}

void log::RegisterAppender(uint8 index, AppenderCreatorFn appenderCreateFn)
{
	auto itr = appenderFactory.find(index);
	ASSERT(itr == appenderFactory.end());
	appenderFactory[index] = appenderCreateFn;
}

void log::out_message(std::string const& filter, log_level const level, std::string&& message)
{
	write(std::make_unique<log_message>(level, filter, std::move(message)));
}

void log::outCommand(std::string&& message, std::string&& param1)
{
	write(Trinity::make_unique<logMessage>(LOG_LEVEL_INFO, "commands.gm", std::move(message), std::move(param1)));
}

void log::write(std::unique_ptr<logMessage>&& msg) const
{
	logger const* logger = GetloggerByType(msg->type);

	if (_ioContext)
	{
	std::shared_ptr<logOperation> logOperation = std::make_shared<logOperation>(logger, std::move(msg));
	Trinity::Asio::post(*_ioContext, Trinity::Asio::bind_executor(*_strand, [logOperation]() { logOperation->call(); }));
	}
	else
		logger->write(msg.get());
}

const logger* log::Get_logger_by_type(const std::string& type) const
{
	auto it = loggers.find(type);
	if (it != loggers.end())
	return it->second.get();

	if (type == LOGGER_ROOT)
	return NULL;

	std::string parentlogger = LOGGER_ROOT;
	size_t found = type.find_last_of(".");
	if (found != std::string::npos)
	parentlogger = type.substr(0, found);

	return Get_logger_by_type(parentlogger);
}

std::string log::GetTimestampStr()
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

bool log::SetlogLevel(std::string const& name, const char* newLevelc, bool islogger /* = true */)
{
	logLevel newLevel = logLevel(atoi(newLevelc));
	if (newLevel < 0)
	return false;

	if (islogger)
	{
	auto it = loggers.begin();
	while (it != loggers.end() && it->second->getName() != name)
		++it;

	if (it == loggers.end())
		return false;

	it->second->setlogLevel(newLevel);

	if (newLevel != LOG_LEVEL_DISABLED && newLevel < lowestlogLevel)
		lowestlogLevel = newLevel;
	}
	else
	{
	Appender* appender = GetAppenderByName(name);
	if (!appender)
		return false;

	appender->setlogLevel(newLevel);
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

	std::unique_ptr<logMessage> msg(new logMessage(LOG_LEVEL_INFO, "entities.player.dump", ss.str()));
	std::ostringstream param;
	param << guid << '_' << name;

	msg->param1 = param.str();

	write(std::move(msg));
}

void log::SetRealmId(uint32 id)
{
	for (auto it = appenders.begin(); it != appenders.end(); ++it)
	it->second->setRealmId(id);
}

void log::Close()
{
	loggers.clear();
	appenders.clear();
}

bool log::Shouldlog(const std::string& type, log_level level) const
{
	// TODO: Use cache to store "Type.sub1.sub2": "Type" equivalence, should
	// Speed up in cases where requesting "Type.sub1.sub2" but only configured
	// logger "Type"

	// Don't even look for a logger if the logLevel is lower than lowest log levels across all loggers
	if (level < lowestlogLevel)
	return false;

	const logger* logger = Get_logger_by_type(type);
	if (!logger)
	return false;

	logLevel logLevel = logger->getlogLevel();
	return logLevel != LOG_LEVEL_DISABLED && logLevel <= level;
}

log* log::instance()
{
	static log instance;
	return &instance;
}

void log::Initialize(Trinity::Asio::IoContext* ioContext)
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
	Close();

	lowestlogLevel = LOG_LEVEL_FATAL;
	AppenderId = 0;
	m_logsDir = sConfigMgr->GetStringDefault("logsDir", "");
	if (!m_logsDir.empty())
	if ((m_logsDir.at(m_logsDir.length() - 1) != '/') && (m_logsDir.at(m_logsDir.length() - 1) != '\\'))
		m_logsDir.push_back('/');

	ReadAppendersFromConfig();
	ReadloggersFromConfig();
}
