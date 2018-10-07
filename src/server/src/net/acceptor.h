/*
 * Copyright (C) 2018
 */

#ifndef NET_ACCEPTOR_H
#define NET_ACCEPTOR_H

#include "socket.h"
#include "event_loop.h"

namespace net
{
class Acceptor
{
public:
  typedef std::function<void (int sockfd,
                                const Inet_address&)> New_connection_callback;

  Acceptor(Event_loop* loop, const Inet_address& listen_addr, bool reuseport);
  ~Acceptor();

  void set_new_connection_callback(const New_connection_callback& cb)
  { new_connection_callback_ = cb; }

  bool listenning() const { return listenning_; }
void listen();
private:
 void handle_read();

  Event_loop* loop_;
  Socket accept_socket_;
  Channel accept_channel_;
  New_connection_callback new_connection_callback_;
  bool listenning_;
int idle_fd_;
};
} // end namespace net

#endif
