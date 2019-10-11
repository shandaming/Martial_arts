/*
 * Copyright (C) 2018
 */

#ifndef LOG_UTILS_H
#define LOG_UTILS_H

namespace lg
{
	enum Log_level
	{
	    LOG_LEVEL_DISABLED                           = 0,
	    LOG_LEVEL_TRACE                              = 1,
	    LOG_LEVEL_DEBUG                              = 2,
	    LOG_LEVEL_INFO                               = 3,
	    LOG_LEVEL_WARN                               = 4,
	    LOG_LEVEL_ERROR                              = 5,
	    LOG_LEVEL_FATAL                              = 6,

	    NUM_ENABLED_LOG_LEVELS                       = 6
	};

	enum Appender_type : uint8
	{
	    APPENDER_NONE,
	    APPENDER_CONSOLE,
	    APPENDER_FILE,
	    APPENDER_DB
	};

	enum Appender_flags
	{
	    APPENDER_FLAGS_NONE                          = 0x00,
	    APPENDER_FLAGS_PREFIX_TIMESTAMP              = 0x01,
	    APPENDER_FLAGS_PREFIX_LOGLEVEL               = 0x02,
	    APPENDER_FLAGS_PREFIX_LOGFILTERTYPE          = 0x04,
	    APPENDER_FLAGS_USE_TIMESTAMP                 = 0x08, // only used by File_appender
	    APPENDER_FLAGS_MAKE_FILE_BACKUP              = 0x10  // only used by File_appender
	};
}

#endif
