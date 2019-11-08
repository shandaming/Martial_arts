/*
 * Copyright (C) 2018
 */

#include "logger.h"

logger::logger(const std::string& name, log_level level): name_(name), level_(level) { }

void logger::add_appender(uint8 id, appender* appender)
{
    appenders_[id] = appender;
}

void logger::del_appender(uint8 id)
{
    appenders_.erase(id);
}

void logger::set_log_level(log_level level)
{
    level_ = level;
}

void logger::write(log_message* message) const
{
    if (!level_ || level_ > message->level || message->text.empty())
    {
        //fprintf(stderr, "logger::write: logger %s, Level %u. Msg %s Level %u WRONG LEVEL MASK OR EMPTY MSG\n", getName().c_str(), getLogLevel(), message.text.c_str(), message.level);
        return;
    }

    for (auto it = appenders_.begin(); it != appenders_.end(); ++it)
        if (it->second)
            it->second->write(message);
}
