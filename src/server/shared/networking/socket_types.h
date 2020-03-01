/*
 * Copyright (C) 2020
 */

#ifndef IP_SOCKET_TYPES_H
#define IP_SOCKET_TYPES_H

#include <sys/socket.h>

typedef int socket_type;
typedef sockaddr socket_addr_type;

typedef in_addr in4_addr_type;
typedef sockaddr_in sockaddr_in4_type;

typedef sockaddr_in6 sockaddr_in6_type;
typedef in6_addr in6_addr_type;

typedef sockaddr_storage sockaddr_storage_type;

constexpr int invalid_socket = -1;
constexpr int max_addr_v4_str_len = INET_ADDRSTRLEN;
#if defined(INET6_ADDRSTRLEN)
constexpr int max_addr_v6_str_len = INET6_ADDRSTRLEN + 1 + IF_NAMESIZE;
#else
constexpr int max_addr_v6_str_len = 256
#endif

#endif
