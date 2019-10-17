/*
 * Copyright (C) 2018
 */

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <string>

class Timestamp
{
public:
	Timestamp() : micro_seconds_since_epoch_(0) {}
		
	explicit Timestamp(int64_t micro_seconds_since_epoch) : 
		micro_seconds_since_epoch_(micro_seconds_since_epoch) {}

	void swap(Timestamp& other)
	{
		std::swap(micro_seconds_since_epoch_, 
				other.micro_seconds_since_epoch_);
	}

	std::string to_string() const;
	std::string to_formatted_string(bool show_microseconds = true) const;

	bool valid() const { return micro_seconds_since_epoch_ > 0; }

	int64_t micro_seconds_since_epoch() const 
	{
		return micro_seconds_since_epoch_; 
	}

	time_t seconds_since_epoch() const
	{ 
		return static_cast<time_t>(micro_seconds_since_epoch_ 
				/ kMicro_seconds_per_second); 
	}

	static Timestamp now();
	static Timestamp invalid()
	{
		return Timestamp();
	}

	static Timestamp from_unix_time(time_t t)
	{
		return from_unix_time(t, 0);
	}

	static Timestamp from_unix_time(time_t t, int microseconds)
	{
		return Timestamp(static_cast<int64_t>(t) * 
				kMicro_seconds_per_second + microseconds);
	}

	static const int kMicro_seconds_per_second = 1000 * 1000;
private:
	int64_t micro_seconds_since_epoch_;
};

inline bool operator<(Timestamp l, Timestamp r)
{
	return l.micro_seconds_since_epoch() < r.micro_seconds_since_epoch();
}

inline bool operator==(Timestamp l, Timestamp r)
{
	return l.micro_seconds_since_epoch() 
		== r.micro_seconds_since_epoch();
}

///
/// 获取两个时间戳的时间差，以秒为单位。
///
/// @param high, low
/// @return (high-low) in seconds
/// @c double has 52-bit precision, enough for one-microsecond
/// resolution for next 100 years.
inline double time_difference(Timestamp high, Timestamp low)
{
	int64_t diff = high.micro_seconds_since_epoch() 
		- low.micro_seconds_since_epoch();
	return static_cast<double>(diff) / Timestamp::kMicro_seconds_per_second;
}

///
/// Add @c seconds to given timestamp.
///
/// @return timestamp+seconds as Timestamp
///
inline Timestamp add_time(Timestamp timestamp, double seconds)
{
	int64_t delta = static_cast<int64_t>(seconds 
			* Timestamp::kMicro_seconds_per_second);
	return Timestamp(timestamp.micro_seconds_since_epoch() + delta);
}

#endif
