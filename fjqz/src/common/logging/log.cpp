/*
 * Copyright (C) 2018 
 */

#include <SDL2/SDL_log.h>

#include "log.h"

void out_message(std::string_view filter, log_level level, std::string&& message)
{
    SDL_LogPriority priority;

    switch(level)
    {
        case LOG_LEVEL_DEBUG:
            priority = SDL_LOG_PRIORITY_DEBUG;
            break;
        case LOG_LEVEL_INFO:
            priority = SDL_LOG_PRIORITY_INFO;
            break;
        case LOG_LEVEL_WARN:
            priority = SDL_LOG_PRIORITY_WARN;
            break;
        case LOG_LEVEL_ERROR:
            priority = SDL_LOG_PRIORITY_ERROR;
            break;
        case LOG_LEVEL_FATAL:
            priority = SDL_LOG_PRIORITY_CRITICAL;
            break;
        case LOG_LEVEL_TRACE:
        default:
            priority = SDL_LOG_PRIORITY_INFO;
            break;
    }

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, priority, filter.data(), " ", message.c_str());
}
