/*
 * Copyright (C) 2018
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>

#include "net_utils.h"
#include "log/logging.h"

namespace
{
constexpr int buf_len = 128;
} // end namespace

namespace net
{
bool set_nonblock(int sockfd)
{
	int flags = fcntl(sockfd, F_GETFL, 0);
	if(flags == -1)
	{
		LOG_SYSFATAL << "set_nonblock() failed! 'F_GETFL' " << std::strerror(errno);
		return false;
	}
	flags |= O_NONBLOCK;
	if(fcntl(sockfd, F_SETFL, flags) == -1)
	{
		LOG_SYSFATAL << "set_nonblock() failed! 'F_SETFL' " << std::strerror(errno);
		return false;
	}
	return true;
}

bool set_close_on_exec(int sockfd)
{
	int flags = fcntl(sockfd, F_GETFD, 0);
	if(flags == -1)
	{
		LOG_SYSFATAL << "set_close_on_exec() failed! 'F_GETFD' " << std::strerror(errno);
		return false;
	}

	flags |= FD_CLOEXEC;
	if(fcntl(sockfd, F_SETFD, flags) == -1)
	{
		LOG_SYSFATAL << "set_close_on_exec() failed! 'F_SETFD' " << std::strerror(errno);
		return false;
	}
	return true;
}

//int create_tcp_socket(int domain)
int socket(int domain)
{
	int sockfd = ::socket(domain, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
	if(sockfd == -1)
	{
		LOG_SYSFATAL << "socket() failed! " << std::strerror(errno);
	}
	return sockfd;
}

//bool bind_socket(int sockfd, const sockaddr_in& addr)
bool bind(int sockfd, const sockaddr_in& addr)
{
	if(::bind(sockfd, reinterpret_cast<const sockaddr*>(&addr), sizeof(sockaddr_in)) == -1)
	{
		LOG_SYSFATAL << "bind() failed! " << std::strerror(errno);
		return false;
	}
	return true;
}

//bool listen_socket(int sockfd)
bool listen(int sockfd)
{
	if(::listen(sockfd, SOMAXCONN) == -1)
	{
		LOG_SYSFATAL << "listen() failed! " << std::strerror(errno);
		return false;
	}
	return true;
}

//bool connect_server(int sockfd, const sockaddr_in& addr)
bool connect(int sockfd, const sockaddr_in& addr)
{
	if(::connect(sockfd, reinterpret_cast<const sockaddr*>(&addr), 
				static_cast<socklen_t>(sizeof(sockaddr_in))) == -1)
	{
		LOG_SYSFATAL << "connect() failed! " << std::strerror(errno);
		return false;
	}
	return true;
}

//int accept_net_connection(int sockfd, sockaddr_in& addr)
int accept(int sockfd, sockaddr_in& addr)
{
	socklen_t addrlen = sizeof(addr);
	int connfd = ::accept4(sockfd, reinterpret_cast<sockaddr*>(&addr), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
	if(connfd < 0)
	{
		int saved_errno = errno;
		
		LOG_SYSERR << "accept() failed!";

		switch(saved_errno)
		{
			case EAGAIN:
			case ECONNABORTED:
			case EINTR:
			case EPROTO:
			case EPERM:
			case EMFILE:
				errno = saved_errno;
				break;
			case EBADF:
			case EFAULT:
			case EINVAL:
			case ENOBUFS:
			case ENOMEM:
			case ENOTSOCK:
			case EOPNOTSUPP:
				LOG_FATAL << "unexpected error of ::accept " << saved_errno;
				break;
			default:
				LOG_FATAL << "unknown error of ::accept " << saved_errno;
				break;
		}
	}
	return connfd;
}

ssize_t read(int fd, void *buf, size_t count) {}
ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {}
ssize_t write(int fd, const void *buf, size_t count) {}

//void close_sockfd(int sockfd)
void close(int sockfd)
{
	if(::close(sockfd) < 0)
	{
		LOG_SYSERR << "close() failed! fd = " << sockfd << ", " << std::strerror(errno);
	}
}

void shutdown_write(int sockfd)
{
	if(::shutdown(sockfd, SHUT_WR) < 0)
	{
		LOG_SYSERR << "shutdown_write() failed! sockfd = " << sockfd << ", " << std::strerror(errno);
	}
}

std::string get_hostname()
{
	char buf[buf_len] = {0};
	if(::gethostname(buf, sizeof buf) == 0)
	{
		return buf;
	}
	return "unknown host";
}

int get_socket_error(int sockfd)
{
  int optval;
  socklen_t optlen = static_cast<socklen_t>(sizeof optval);

  if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
  {
    return errno;
  }
  else
  {
    return optval;
  }
}

sockaddr_in get_local_addr(int sockfd)
{
  sockaddr_in localaddr;
  memset(&localaddr, 0, sizeof localaddr);
  socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
  if (::getsockname(sockfd, reinterpret_cast<sockaddr*>(&localaddr), &addrlen) < 0)
  {
    LOG_SYSERR << "sockets::get_local_addr";
  }
return localaddr;
}

sockaddr_in get_peer_addr(int sockfd)
{
  sockaddr_in peeraddr;
  memset(&peeraddr, 0, sizeof peeraddr);
  socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
  if (::getpeername(sockfd, reinterpret_cast<sockaddr*>(&peeraddr), &addrlen) < 0)
  {
    LOG_SYSERR << "sockets::getPeerAddr";
  }
return peeraddr;
}
} // namespace net
