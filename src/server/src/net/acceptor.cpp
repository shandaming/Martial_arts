/*
 * Copyright (C) 2018
 */

#include <fcntl.h>
#include <unistd.h>

#include <cassert>

#include "acceptor.h"
#include "log/logging.h"

namespace net
{

Acceptor::Acceptor(Event_loop* loop, const Inet_address& listen_addr, bool reuseport)
  : loop_(loop),
    accept_socket_(socket(listen_addr.family())),
    accept_channel_(loop, accept_socket_),
    listenning_(false),
    idle_fd_(open("/dev/null", O_RDONLY | O_CLOEXEC))
{
  assert(idle_fd_ >= 0);
  accept_socket_.set_reuse_addr();
  accept_socket_.set_reuse_port();
  accept_socket_.bind(listen_addr);
  accept_channel_.set_read_callback(
      std::bind(&Acceptor::handle_read, this));
}

Acceptor::~Acceptor()
{
  accept_channel_.disable_all();
  accept_channel_.remove();
  close(idle_fd_);
}

void Acceptor::listen()
{
  loop_->assert_in_loop_thread();
  listenning_ = true;
  accept_socket_.listen();
  accept_channel_.enable_read();
}

void Acceptor::handle_read()
{
  loop_->assert_in_loop_thread();
  Inet_address peer_addr;

	int connfd = accept_socket_.accept(peer_addr);
  //FIXME loop until no more
  if (connfd >=0)
  {
    // string hostport = peerAddr.toIpPort();
    // LOG_TRACE << "Accepts of " << hostport;
    if (new_connection_callback_)
    {
      new_connection_callback_(connfd, peer_addr);
    }
    else
    {
      close(connfd);
    }
  }
  else
  {
    LOG_SYSERR << "in Acceptor::handle_read";
    // Read the section named "The special problem of
    // accept()ing when you can't" in libev's doc.
    // By Marc Lehmann, author of libev.
    if (errno == EMFILE)
    {
      close(idle_fd_);
      idle_fd_ = accept(accept_socket_, NULL, NULL);
      close(idle_fd_);
      idle_fd_ = open("/dev/null", O_RDONLY | O_CLOEXEC);
    }
  }
}
} // end namespace
