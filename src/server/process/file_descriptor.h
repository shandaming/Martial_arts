/*
 * Copyright (C) 2019
 */

#ifndef PRO_FILE_DESCRIPTOR_H
#define PRO_FILE_DESCRIPTOR_H

class file_descriptor
{
public:
	file_descriptor(const std::string& path, bool close_on_exit = true);
	file_descriptor(const int fd, bool close_on_exit = true) : 
		fd_(fd), close_on_exit_(close_on_exit) {}

	~file_descriptor(); 

	file_descriptor(const file_descriptor& r) : fd_(r.fd_), close_on_exit_(r.close_on_exit_) {}
	file_descriptor& operator=(const file_descriptor& fd);

	file_descriptor(file_descriptor&& fd) : 
		fd_(std::move(fd.fd_)), close_on_exit_(std::move(fd.close_on_exit_)) {}
	file_descriptor& operator=(file_descriptor&& fd);

	bool open_file(const std::string& path);
	void close();

	operator int() const { return fd_; }
private:
	int fd_;
	bool close_on_exit_;
};

#endif
