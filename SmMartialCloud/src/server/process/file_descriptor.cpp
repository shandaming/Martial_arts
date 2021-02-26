/*
 * Copyright (C) 2019
 */

#include "file_descriptor.h"

file_descriptor::file_descriptor(const std::string& path) : fd_(-1)
{
	open_file(path);
}
	
file_descriptor::~file_descriptor() 
{
	if(close_on_exit_)
		close(); 
}
	
file_descriptor::file_descriptor& operator=(const file_descriptor& fd) 
{
	fd_ = fd.fd_; 
	close_on_exit_ = fd.close_on_exit_;
}
	
file_descriptor::file_descriptor& operator=(file_descriptor&& fd)
{
	fd_ = std::move(fd.fd_);
	fd.fd_ = -1;
	close_on_exit_ = std::move(fd.close_on_exit_);
	fd.close_on_exit_ = false;
}

bool file_descriptor::open_file(const std::string& path)
{
	if(fd_!= -1)
		return true;

	fd_ = ::open(path.c_str(), O_RDWR);
	if(fd_ == -1)
	{
		PROCESS_RETURN_THROW_LAST_SYSTEM_ERROR("open(1) failed.")
		return false;
	}
	return true;
}

void file_descriptor::close()
{
	if(fd_ > -1)
	{
		::close(fd_);
	}
}
