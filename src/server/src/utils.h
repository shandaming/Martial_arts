/*
 * Copyright (C) 2018
 */

#ifndef UTILS_H
#define UTILS_H

#include <memory>
#include <netinet/in.h>
#include "byte_buffer.h"

int open_clientfd(const char* hostname, const char* port);
int open_listenfd(const char* port);

void int2byte(char* data, int num);
int byte2int(char* data);

Byte_buffer encode(const std::string& str);
int decode(const char* buf);

namespace net
{
	// @inet_pton
	std::unique_ptr<char> ntop(struct sockaddr_in& sin, int domain = AF_INET);
	// @inet_ntop
	int pton(const char* str, struct sockaddr_in& sin, int domain = AF_INET);
}
#endif
