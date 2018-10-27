/*
 * Copyright (C) 2018
 */

#include "logger.h"

namespace lg
{
Logger::Logger(const std::string& name, Log_level level): name_(name), level_(level) { }

void Logger::add_appender(uint8 id, Appender* appender)
{
    appenders_[id] = appender;
}

void Logger::del_appender(uint8 id)
{
    appenders_.erase(id);
}

void Logger::set_log_level(Log_level level)
{
    level_ = level;
}

void Logger::write(Log_message* message) const
{
    if (!level_ || level_ > message->level || message->text.empty())
    {
        //fprintf(stderr, "Logger::write: Logger %s, Level %u. Msg %s Level %u WRONG LEVEL MASK OR EMPTY MSG\n", getName().c_str(), getLogLevel(), message.text.c_str(), message.level);
        return;
    }

    for (auto it = appenders_.begin(); it != appenders_.end(); ++it)
        if (it->second)
            it->second->write(message);
}
}
