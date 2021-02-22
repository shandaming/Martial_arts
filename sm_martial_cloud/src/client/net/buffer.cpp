/*
 * Copyright (C) 2018
 */

#include <sys/uio.h>

#include "net/buffer.h"

namespace net
{
const char Buffer::crlf[] = "\r\n";

const size_t Buffer::cheap_prepend = 8;
const size_t Buffer::initial_size = 1024;

ssize_t Buffer::read_fd(int fd, int* saved_errno)
{
	// saved an ioctl()/FIONREAD call to tell how much to read
	char extrabuf[65536];
	struct iovec vec[2];
	const size_t writable = writable_bytes();
	vec[0].iov_base = begin() + writer_index_;
	vec[0].iov_len = writable;
	vec[1].iov_base = extrabuf;
	vec[1].iov_len = sizeof extrabuf;
	// when there is enough space in this buffer, don't read into extrabuf.
	// when extrabuf is used, we read 128k-1 bytes at most.
	const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
	const ssize_t n = readv(fd, vec, iovcnt);
	if (n < 0)
	{
		*saved_errno = errno;
	}
	else if (static_cast<size_t>(n) <= writable)
	{
		writer_index_ += n;
	}
	else
	{
		writer_index_ = buffer_.size();
		append(extrabuf, n - writable);
	}

	return n;
}
}



void read_handler(int fd, int* errno)
{
	message_buffer read_buffer;
	read_buffer.normalize();
	read_buffer.ensure_free_space();

	size_t remaining_space = read_buffer.get_remaining_space();

	bool completed = false;

	// 应该更具报头和数据长度来读去
	do
	{
		struct iovec vec;
		vec.iov_base = read_buffer.get_write_pointer();
		vec.iov_len = remaining_space;

		ssize_t read_bytes = readv(fd, vec, 1);
		if(read_bytes < 0)
		{
			*errno = errno;
			return read_bytes;
		}
		else if(read_bytes < remaining_space)
			completed = false;
		else
		{
			read_buffer.normalize();
			read_buffer.ensure_free_space();

			remaining_space = read_buffer.get_remaining_space();

			completed = true;
		}
		read_buffer.write_completed(read_bytes);
	}while(completed);
}
