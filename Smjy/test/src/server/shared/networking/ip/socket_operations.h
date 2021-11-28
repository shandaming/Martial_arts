/*
 * Copyright (C) 2021
 */

#ifndef _SOCKET_OPERATION_H
#define _SOCKET_OPERATION_H

#include <system_error>

const char* inet_ntop(int domain, const void* addrptr, char* dst_str, size_t len, uint64_t scope_id, std::error_code& ec);

#endif
