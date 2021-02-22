/*
 * Copyright (C) 2018
 */

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <cstdio>

#include <cstdlib>

#include "utils.h"

int open_clientfd(const char* hostname, const char* port)
{
	struct sockaddr_in clientaddr;
	int clientfd = socket(AF_INET, SOCK_STREAM, 0);
	if(clientfd < 0)
	{
		close(clientfd);
		return -1;
	}

	bzero(&clientaddr, sizeof(struct sockaddr_in));
	clientaddr.sin_family = AF_INET;
	//serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	clientaddr.sin_port = htons(atoi(port));
    inet_pton(AF_INET, hostname, &clientaddr.sin_addr.s_addr);

	printf("------ IP:%s Port:%s ------\n", hostname, port);

    if(connect(clientfd, reinterpret_cast<struct sockaddr*>(&clientaddr), sizeof(struct sockaddr)) == -1)
    {
		close(clientfd);
        return -1;
    }
    return clientfd;
}

int open_listenfd(const char* port)
{
	
	struct sockaddr_in serveraddr;
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0)
	{
		close(listenfd);
		return -1;
	}

	int optval;
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, static_cast<const void*>(&optval), sizeof(int)) == -1)
	{
		close(listenfd);
		return -1;
	}

	bzero(&serveraddr, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(atoi(port));

	printf("------ IP:%u Port:%s ------\n", INADDR_ANY, port);

	if(bind(listenfd, reinterpret_cast<struct sockaddr*>(&serveraddr), sizeof(serveraddr)) == -1)
	{
		close(listenfd);
		return -1;
	}

	if(listen(listenfd, 128) == -1)
	{
		close(listenfd);
		return -1;
	}

	return listenfd;
}

void int2byte(char* data, int num)
{
	if(data)
	{
		memset(data, 0, sizeof(data));
		data[0] = (char)(0xff & num);
		data[1] = (char)((0xff00 & num) >> 8);
		data[2] = (char)((0xff0000 & num) >> 16);
		data[3] = (char)((0xff000000 & num) >> 24);
	}
}

int byte2int(char* data)
{
	int res = -1;
	if(data)
	{
		data[0] & 0xFF;
		res |= ((data[1] << 8) & 0xFF00);
		res |= ((data[2] << 16) & 0xFF0000);
		res |= ((data[3] << 24) & 0xFF000000);
	}
	return res;
}

Byte_buffer encode(const std::string& str)
{
	Byte_buffer b;
	b << "mBdT" << str.size() << str;
	assert(b.size() == (str.size() + 8));
	return b;
}

int decode(const char* buf)
{
	const char* str = buf;
	if(strncmp(buf, "mBdT", 4) != 0)
		return 0;
	str = buf + 4;
	Byte_buffer b(16);
	int len;
	b << str;
	b >> len;
	return len;
}

namespace net
{
	constexpr int ip_len = 128;

	std::unique_ptr<char> ntop(struct sockaddr_in& sin, int domain)
	{
		std::unique_ptr<char> ip(new char[ip_len]);
		if(inet_ntop(domain, &sin, ip.get(), ip_len))
			return ip;
		return nullptr;
	}

	int pton(const char* ip, struct sockaddr_in& sin, int domain)
	{
		return inet_pton(domain, ip, &sin);
	}
}
