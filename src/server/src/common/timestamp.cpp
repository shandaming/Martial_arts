/*
 * Copyright (C) 2018
 */

#include <sys/time.h>

#include "timestamp.h"

std::string Timestamp::to_string() const
{
	char buf[32] = {0};
	int64_t seconds = micro_seconds_since_epoch_ / 
		kMicro_seconds_per_second;
	int64_t microseconds = micro_seconds_since_epoch_ % 
		kMicro_seconds_per_second;
	snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", 
			seconds, microseconds);
	return buf;
}

std::string Timestamp::to_formatted_string(bool show_microseconds) const
{
	time_t seconds = static_cast<time_t>(micro_seconds_since_epoch_ /
			kMicro_seconds_per_second);
	struct tm tm_time;
	gmtime_r(&seconds, &tm_time);

	std::string buf;
	if (show_microseconds)
	{
		int microseconds = static_cast<int>(micro_seconds_since_epoch_ %
				kMicro_seconds_per_second);
		buf = string_format("%4d%02d%02d %02d:%02d:%02d.%06d",
				tm_time.tm_year + 1900, 
				tm_time.tm_mon + 1, 
				tm_time.tm_mday,
				tm_time.tm_hour,
				tm_time.tm_min, 
				tm_time.tm_sec,
				microseconds);
	}
	else
	{
		buf = string_format("%4d%02d%02d %02d:%02d:%02d",
				tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
				tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
	}
	return buf;
}

Timestamp Timestamp::now()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	int64_t seconds = tv.tv_sec;
	return Timestamp(seconds * kMicro_seconds_per_second + tv.tv_usec);
}
