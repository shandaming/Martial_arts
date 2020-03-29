/*
 * Copyright (C) 2019
 */

#ifndef UTILITY_TIMER_H
#define UTILITY_TIMER_H

#include <chrono>
#include <ctime>
#include <string>

std::string get_time_str(time_t time)
{
	tm atm;
	localtime_r(&time, &atm);
	char buf[20];
	snprintf(buf, 20, "%04d-%02d-%02d_%02d:%02d:%02d", atm.tm_year + 1900, atm.tm_mon + 1, atm.tm_mday, atm.tm_hour, atm.tm_min, atm.tm_sec);
	return buf;
}

inline uint32_t get_ms_time()
{
	using namespace std::chrono;
	static const steady_clock::time_point application_start_time = steady_clock::now();
	return uint32_t(duration_cast<milliseconds>(steady_clock::now() - application_start_time).count());
}

inline uint32_t get_ms_time_diff(uint32_t old_ms_time, uint32_t new_ms_time)
{
	if(old_ms_time > new_ms_time)
		return (0xFFFFFFFF - old_ms_time) + new_ms_time;
	else
		return new_ms_time - old_ms_time;
}

inline uint32_t get_ms_time_diff_to_now(uint32_t old_ms_time)
{
	return get_ms_time_diff(old_ms_time, get_ms_time());
}

#endif
