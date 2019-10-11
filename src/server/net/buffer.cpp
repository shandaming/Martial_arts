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
