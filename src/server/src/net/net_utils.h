/*
 * Copyright (C) 2018
 */

#ifndef NET_UTILS_H
#define NET_UTILS_H

#include <netinet/in.h>

#include <functional>
#include <memory>

#include "common/timestamp.h"

namespace net
{	
int create_tcp_socket(int domain);

bool bind_socket(int sockfd, const sockaddr_in& addr);
bool listen_socket(int sockfd);
bool connect_server(int sockfd, const sockaddr_in& addr);
int accept_net_connection(int sockfd, sockaddr_in& addr);

ssize_t read(int fd, void *buf, size_t count);
ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t write(int fd, const void *buf, size_t count);

void close_sockfd(int sockfd);
void shutdown_write(int sockfd);

int get_socket_error(int sockfd);

sockaddr_in get_local_addr(int sockfd);
sockaddr_in get_peer_addr(int sockfd);

class Buffer;
class Tcp_connection;
typedef std::shared_ptr<Tcp_connection> Tcp_connection_ptr;
typedef std::function<void()> Timer_callback;
typedef std::function<void (const Tcp_connection_ptr&)> Connection_callback;
typedef std::function<void (const Tcp_connection_ptr&)> Close_callback;
typedef std::function<void (const Tcp_connection_ptr&)> Write_complete_callback;
typedef std::function<void (const Tcp_connection_ptr&, size_t)> High_water_mark_callback;

// the data has been read to (buf, len)
typedef std::function<void (const Tcp_connection_ptr&, Buffer*, Timestamp)> Message_callback;

void default_connection_callback(const Tcp_connection_ptr& conn);
void default_message_callback(const Tcp_connection_ptr& conn,

Buffer* buffer, Timestamp receiveTime);

std::string get_hostname();
} // end namespace net

#endif
