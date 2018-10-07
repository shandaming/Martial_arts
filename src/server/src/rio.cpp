/*
 * Copyright (C) 2018
 */

#include <cerrno>
#include <cstring>
#include <unistd.h>
#include "rio.h"

ssize_t rio_readn(int fd, void* usrbuf, size_t n)
{
	size_t nleft = n;
	ssize_t nread;
	char* bufp = static_cast<char*>(usrbuf);

	while(nleft > 0)
	{
		if((nread = read(fd, bufp, nleft)) < 0)
		{
			// 阻塞只需要检测EINTR ,异步还需要检测EWOULDBLOCK和EAGAIN
			if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)	// interrupted bu sig handler return
				nread = 0;		// and call read() again
			else
				return -1;	// errno set by read()
		}
		else if(nread == 0) 
			break;			// EOF
		nleft -= nread;
		bufp += nread;
	}
	return (n - nleft);
}

ssize_t rio_writen(int fd, void* usrbuf, size_t n)
{
	size_t nleft = n;
	ssize_t nwritten;
	char* bufp = static_cast<char*>(usrbuf);

	while(nleft > 0)
	{
		if((nwritten = write(fd, bufp, nleft)) <= 0)
		{
			// 阻塞与异步检测同read
			if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) // Interrupted by sig handler return
				nwritten = 0; // and call write() again
			else
				return -1;	// errno set by write()
		}
		nleft -= nwritten;
		bufp += nwritten;
	}
	return n;
}

//---------------------------------------------
void rio_readinitb(rio_t* rp, int fd)
{
	rp->rio_fd = fd;
	rp->rio_cnt = 0;
	rp->rio_bufptr = rp->rio_buf;
}

static ssize_t rio_read(rio_t* rp, char* usrbuf, size_t n)
{
	int cnt;
	while(rp->rio_cnt <= 0)
	{
		rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
		if(rp->rio_cnt < 0)
		{
			if(errno != EINTR)
				return -1;
		}
		else if(rp->rio_cnt == 0)
			return 0;
		else
			rp->rio_bufptr = rp->rio_buf;
	}

	cnt = n;
	if(rp->rio_cnt < n)
		cnt = rp->rio_cnt;
	memcpy(usrbuf, rp->rio_bufptr, cnt);
	rp->rio_bufptr += cnt;
	rp->rio_cnt -= cnt;

	return cnt;
}

ssize_t rio_readlineb(rio_t* rp, void* usrbuf, size_t maxlen)
{
	int n, rc;
	char c, *bufp = static_cast<char*>(usrbuf);

	for(n = 1; n < maxlen; ++n)
	{
		if((rc = rio_read(rp, &c, 1)) == 1)
		{
			*bufp++ = c;
			if(c == '\n')
			{
				++n;
				break;
			}
		}
		else if(rc == 0)
		{
			if(n == 1)
				return 0;
			else
				break;
		}
		else
			return -1;
	}
	*bufp = 0;
	return n - 1;
}

ssize_t rio_readnb(rio_t* rp, void* usrbuf, size_t n)
{
	size_t nleft = n;
	ssize_t nread;
	char* bufp = static_cast<char*>(usrbuf);

	while(nleft > 0)
	{
		if((nread = rio_read(rp, bufp, nleft)) < 0)
			return -1;
		else if(nread == 0)
			break;
		nleft -= nread;
		bufp += nread;
	}
	return n - nleft;
}
