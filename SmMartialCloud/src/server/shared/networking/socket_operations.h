/*
 * Copyright (C) 2020
 */

#ifndef NET_SOCKET_OPERATIONS_H
#define NET_SOCKET_OPERATIONS_H

#include <sys/uio.h>

#include "socket_types.h"

int non_blocking_socket(int domain, int type, int protocol, std::error_code& ec);
int bind(int sockfd, const socket_addr_type* addr, socklen_t addrlen, std::error_code& ec);
int listen(int sockfd, int backlog, std::error_code& ec);
bool non_blocking_accept(int sockfd, socket_addr_type* addr, socklen_t addrlen, std::error_code& ec, int new_socket);
int connect(int sockfd, const socket_addr_type* addr, socklen_t addrlen, std::error_code& ec);
int close(int sockfd, std::error_code& ec);
int shutdown(int sockfd, int what, std::error_code& ec);

bool set_user_non_blocking(int sockfd, std::error_code& ec);
bool set_close_on_exec(int sockfd, std::error_code& ec);

int getsockname(int sockfd, socket_addr_type* addr, socklen_t* addrlen, std::error_code& ec);
int getpeername(int sockfd, socket_addr_type* addr, socklen_t* addrlen, std::error_code& ec);

int setsockopt(int sockfd, int level, int optname, const char* optval, socklen_t optlen, std::error_code& ec);
int getsockopt(int sockfd, int level, int optname, void* optval, socklen_t* optlen, std::error_code& ec);

const char* inet_ntop(int domain, const void* addrptr, char* dst_str, size_t len, uint64_t scope_id, std::error_code& ec);
int inet_pton(int domain, const char* src_str, void* addrptr, uint64_t* scope_id, std::error_code& ec);

bool non_blocking_read(int sockfd, iovec* buf, size_t count, std::error_code& ec, size_t& bytes_transferred);
bool non_blocking_recv(int sockfd, iovec* buf, size_t count, int flags, bool is_stream, std::error_code& ec, size_t& bytes_transferred);
bool non_blocking_recvfrom(int sockfd, iovec* buf, size_t count, int flags, socket_addr_type* addr, socklen_t* addrlen, std::error_code& ec, size_t& bytes_transferred);
bool non_blocking_recvmsg(int sockfd, iovec* buf, size_t count, int in_flags, int out_flags, std::error_code& ec, size_t& bytes_transferred);

bool non_blocking_write(int sockfd, iovec* buf, size_t count, std::error_code& ec, size_t& bytes_transferred);
bool non_blocking_send(int sockfd, const iovec* buf, size_t count, int flags, std::error_code& ec, size_t bytes_transferred);
bool non_blockiing_sendto(int sockfd, const iovec* buf, size_t count, int flags, const socket_addr_type* addr, socklen_t addrlen, std::error_code& ec, size_t& bytes_transferred);

uint16_t host_to_network_short(uint16_t value);
uint16_t network_to_host_short(uint16_t value);

uint32_t host_to_network_long(uint32_t value);
uint32_t network_to_host_long(uint32_t value);

#endif
