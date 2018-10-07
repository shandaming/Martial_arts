/*
 * Copyright (C) 2018
 */

#include "log_file.h"

namespace lg
{
Append_file::Append_file(const std::string& filename) : 
	fp_(fopen(filename.c_str(), "ae")), written_bytes_(0)
{
	assert(fp_);
	setbuffer(fp_, buffer_, sizeof buffer_);
}

Append_file::~Append_file()
{
	fclose(fp_);
}

void Append_file::append(const char* logline, const size_t len)
{
	size_t n = write(logline, len);
	size_t remain = len - n;
	while (remain > 0)
	{
		size_t x = write(logline + n, remain);
		if (x == 0)
		{
			int err = ferror(fp_);
			if (err)
			{
				fprintf(stderr, "Append_file::append() failed %s\n", 
						strerror_tl(err));
			}
			break;
		}
		n += x;
		remain = len - n; // remain -= x
	}

	written_bytes_ += len;
}

void Append_file::flush()
{
	fflush(fp_);
}

size_t Append_file::write(const char* logline, size_t len)
{
	return fwrite_unlocked(logline, 1, len, fp_);
}


Log_file::Log_file(const string& basename, off_t roll_size, 
		bool thread_safe, int flush_interval, int check_every_n) : 
	basename_(basename),
	roll_size_(roll_size),
    flush_interval_(flush_interval),
    check_every_n_(check_every_n),
    count_(0),
    thread_safe_(thread_safe),
    start_of_period_(0),
    last_roll_(0),
    last_flush_(0)
{
	assert(basename.find('/') == string::npos);
	roll_file();
}

void Log_file::append(const char* logline, int len)
{
	if (thread_safe_)
	{
		std::scoped lock(mutex_);
		append_unlocked(logline, len);
	}
	else
	{
		append_unlocked(logline, len);
	}
}

void Log_file::flush()
{
	if (thread_safe_)
	{
		std::scoped lock(mutex_);
		file_->flush();
	}
	else
	{
		file_->flush();
	}
}

void Log_file::append_unlocked(const char* logline, int len)
{
	file_->append(logline, len);

	if (file_->written_bytes() > roll_size_)
	{
		roll_file();
	}
	else
	{
		++count_;
		if (count_ >= check_every_n_)
		{
			count_ = 0;
			time_t now = time(nullptr);
			time_t thisPeriod_ = now / k_roll_per_seconds_ * 
				k_roll_per_seconds_;
			if (thisPeriod_ != start_of_period_)
			{
				roll_file();
			}
			else if (now - last_flush_ > flush_interval_)
			{
				last_flush_ = now;
				file_->flush();
			}
		}
	}
}

bool Log_file::roll_file()
{
	time_t now = 0;
	string filename = get_log_filename(basename_, &now);
	time_t start = now / k_roll_per_seconds_ * k_roll_per_seconds_;

	if (now > last_roll_)
	{
		last_roll_ = now;
		last_flush_ = now;
		start_of_period_ = start;
		file_.reset(new Append_file(filename));
		return true;
	}
	return false;
}

string Log_file::get_log_filename(const string& basename, time_t* now)
{
	string filename;
	filename.reserve(basename.size() + 64);
	filename = basename;

	char timebuf[32];
	struct tm tm;
	*now = time(NULL);
	gmtime_r(now, &tm); // FIXME: localtime_r ?
	//strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
	filename += put_time(&tm, ".%Y%m%d-%H%M%S.");

	//filename += timebuf;

	filename += get_hostname();

	//char pidbuf[32];
	//snprintf(pidbuf, sizeof pidbuf, ".%d", getpid());
	//filename += pidbuf;
	filename += string_format(".%d", getpid());

	filename += ".log";

	return filename;
}
} // namespace lg
