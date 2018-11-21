/*
 * Copyright (C) 2018
 */

#include "tcp_connection.h"
#include "log/logging.h"
#include "net/event_loop.h"

namespace net
{
void default_connection_callback(const Tcp_connection_ptr& conn)
{
  LOG_TRACE << conn->local_address().to_ip_port() << " -> "
            << conn->peer_address().to_ip_port() << " is "
            << (conn->connected() ? "UP" : "DOWN");
  // do not call conn->forceClose(), because some users want to register message callback only.
}

void default_message_callback(const Tcp_connection_ptr&,
                                        Buffer* buf,
                                        Timestamp)
{
  buf->retrieve_all();
}

Tcp_connection::Tcp_connection(Event_loop* loop,
                             const std::string& name,
                             int sockfd,
                             const Inet_address& local_addr,
                             const Inet_address& peer_addr)
  : loop_(loop),
    name_(name),
    state_(kConnecting),
    reading_(true),
    socket_(new Socket(sockfd)),
    channel_(new Channel(loop, sockfd)),
    local_addr_(local_addr),
    peer_addr_(peer_addr),
    high_water_mark_(64*1024*1024)
{
  channel_->set_read_callback(
      std::bind(&Tcp_connection::handle_read, this, std::placeholders::_1));
  channel_->set_write_callback(
      std::bind(&Tcp_connection::handle_write, this));
  channel_->set_close_callback(
      std::bind(&Tcp_connection::handle_close, this));
  channel_->set_error_callback(
      std::bind(&Tcp_connection::handle_error, this));
  LOG_DEBUG << "Tcp_connection::ctor[" <<  name_ << "] at " << this
            << " fd=" << sockfd;
  socket_->set_keep_alive();
}

Tcp_connection::~Tcp_connection()
{
  LOG_DEBUG << "Tcp_connection::dtor[" <<  name_ << "] at " << this
            << " fd=" << channel_->fd()
            << " state=" << state_to_string();
  assert(state_ == kDisconnected);
}
/*
bool Tcp_connection::getTcpInfo(struct tcp_info* tcpi) const
{
  return socket_->getTcpInfo(tcpi);
}

std::string Tcp_connection::getTcpInfoString() const
{
  char buf[1024];
  buf[0] = '\0';
  socket_->getTcpInfoString(buf, sizeof buf);
  return buf;
}*/

void Tcp_connection::send(const void* data, int len)
{
  send(std::string(static_cast<const char*>(data), len));
}

void Tcp_connection::send(const std::string& message)
{
  if (state_ == kConnected)
  {
    if (loop_->is_in_loop_thread())
    {
      send_in_loop(message);
    }
    else
    {
	void (Tcp_connection::*fp)(const std::string& message) = &Tcp_connection::send_in_loop;
      loop_->run_in_loop(
          std::bind(fp, this, 
                      message));
                    //std::forward<string>(message)));
    }
  }
}

// FIXME efficiency!!!
void Tcp_connection::send(Buffer* buf)
{
  if (state_ == kConnected)
  {
    if (loop_->is_in_loop_thread())
    {
      send_in_loop(buf->peek(), buf->readable_bytes());
      buf->retrieve_all();
    }
    else
    {
void (Tcp_connection::*fp)(const std::string& message) = &Tcp_connection::send_in_loop;
      loop_->run_in_loop(
          std::bind(fp,
                      this,     // FIXME
                      buf->retrieve_all_as_string()));
                    //std::forward<string>(message)));
    }
  }
}

void Tcp_connection::send_in_loop(const std::string& message)
{
  send_in_loop(message.data(), message.size());
}

void Tcp_connection::send_in_loop(const void* data, size_t len)
{
  loop_->assert_in_loop_thread();
  ssize_t nwrote = 0;
  size_t remaining = len;
  bool faultError = false;
  if (state_ == kDisconnected)
  {
    LOG_WARN << "disconnected, give up writing";
    return;
  }
  // if no thing in output queue, try writing directly
  if (!channel_->is_write() && output_buffer_.readable_bytes() == 0)
  {
    nwrote = net::write(channel_->fd(), data, len);
    if (nwrote >= 0)
    {
      remaining = len - nwrote;
      if (remaining == 0 && write_complete_callback_)
      {
        loop_->queue_in_loop(std::bind(write_complete_callback_, shared_from_this()));
      }
    }
    else // nwrote < 0
    {
      nwrote = 0;
      if (errno != EWOULDBLOCK)
      {
        LOG_SYSERR << "Tcp_connection::sendInLoop";
        if (errno == EPIPE || errno == ECONNRESET) // FIXME: any others?
        {
          faultError = true;
        }
      }
    }
  }

  assert(remaining <= len);
  if (!faultError && remaining > 0)
  {
    size_t oldLen = output_buffer_.readable_bytes();
    if (oldLen + remaining >= high_water_mark_
        && oldLen < high_water_mark_
        && high_water_mark_callback_)
    {
      loop_->queue_in_loop(std::bind(high_water_mark_callback_, shared_from_this(), oldLen + remaining));
    }
    output_buffer_.append(static_cast<const char*>(data)+nwrote, remaining);
    if (!channel_->is_write())
    {
      channel_->enable_write();
    }
  }
}

void Tcp_connection::shutdown()
{
  // FIXME: use compare and swap
  if (state_ == kConnected)
  {
    set_state(kDisconnecting);
    // FIXME: shared_from_this()?
    loop_->run_in_loop(std::bind(&Tcp_connection::shutdown_in_loop, this));
  }
}

void Tcp_connection::shutdown_in_loop()
{
  loop_->assert_in_loop_thread();
  if (!channel_->is_write())
  {
    // we are not writing
    socket_->shutdown_write();
  }
}

// void Tcp_connection::shutdownAndForceCloseAfter(double seconds)
// {
//   // FIXME: use compare and swap
//   if (state_ == kConnected)
//   {
//     set_state(kDisconnecting);
//     loop_->runInLoop(boost::bind(&Tcp_connection::shutdownAndForceCloseInLoop, this, seconds));
//   }
// }

// void Tcp_connection::shutdownAndForceCloseInLoop(double seconds)
// {
//   loop_->assert_in_loop_thread();
//   if (!channel_->is_writing())
//   {
//     // we are not writing
//     socket_->shutdownWrite();
//   }
//   loop_->runAfter(
//       seconds,
//       makeWeakCallback(shared_from_this(),
//                        &Tcp_connection::forceCloseInLoop));
// }

void Tcp_connection::force_close()
{
  // FIXME: use compare and swap
  if (state_ == kConnected || state_ == kDisconnecting)
  {
    set_state(kDisconnecting);
    loop_->queue_in_loop(std::bind(&Tcp_connection::force_close_in_loop, shared_from_this()));
  }
}

void Tcp_connection::force_close_with_delay(double seconds)
{
  if (state_ == kConnected || state_ == kDisconnecting)
  {
    set_state(kDisconnecting);
    loop_->run_after(
        seconds,
        make_weak_callback(shared_from_this(),
                         &Tcp_connection::force_close));  // not forceCloseInLoop to avoid race condition
  }
}

void Tcp_connection::force_close_in_loop()
{
  loop_->assert_in_loop_thread();
  if (state_ == kConnected || state_ == kDisconnecting)
  {
    // as if we received 0 byte in handle_read();
    handle_close();
  }
}

const char* Tcp_connection::state_to_string() const
{
  switch (state_)
  {
    case kDisconnected:
      return "kDisconnected";
    case kConnecting:
      return "kConnecting";
    case kConnected:
      return "kConnected";
    case kDisconnecting:
      return "kDisconnecting";
    default:
      return "unknown state";
  }
}

void Tcp_connection::set_tcp_no_delay(bool on)
{
  socket_->set_tcp_no_delay();
}

void Tcp_connection::start_read()
{
  loop_->run_in_loop(std::bind(&Tcp_connection::start_read_in_loop, this));
}

void Tcp_connection::start_read_in_loop()
{
  loop_->assert_in_loop_thread();
  if (!reading_ || !channel_->is_read())
  {
    channel_->enable_read();
    reading_ = true;
  }
}

void Tcp_connection::stop_read()
{
  loop_->run_in_loop(std::bind(&Tcp_connection::stop_read_in_loop, this));
}

void Tcp_connection::stop_read_in_loop()
{
  loop_->assert_in_loop_thread();
  if (reading_ || channel_->is_read())
  {
    channel_->disable_read();
    reading_ = false;
  }
}

void Tcp_connection::connect_established()
{
  loop_->assert_in_loop_thread();
  assert(state_ == kConnecting);
  set_state(kConnected);
  channel_->tie(shared_from_this());
  channel_->enable_read();

  connection_callback_(shared_from_this());
}

void Tcp_connection::connect_destroyed()
{
  loop_->assert_in_loop_thread();
  if (state_ == kConnected)
  {
    set_state(kDisconnected);
    channel_->disable_all();

    connection_callback_(shared_from_this());
  }
  channel_->remove();
}

void Tcp_connection::handle_read(Timestamp receive_time)
{
  loop_->assert_in_loop_thread();
  int savedErrno = 0;
  ssize_t n = input_buffer_.read_fd(channel_->fd(), &savedErrno);
  if (n > 0)
  {
    message_callback_(shared_from_this(), &input_buffer_, receive_time);
  }
  else if (n == 0)
  {
    handle_close();
  }
  else
  {
    errno = savedErrno;
    LOG_SYSERR << "Tcp_connection::handle_read";
    handle_error();
  }
}

void Tcp_connection::handle_write()
{
  loop_->assert_in_loop_thread();
  if (channel_->is_write())
  {
    ssize_t n = net::write(channel_->fd(),
                               output_buffer_.peek(),
                               output_buffer_.readable_bytes());
    if (n > 0)
    {
      output_buffer_.retrieve(n);
      if (output_buffer_.readable_bytes() == 0)
      {
        channel_->disable_write();
        if (write_complete_callback_)
        {
          loop_->queue_in_loop(std::bind(write_complete_callback_, shared_from_this()));
        }
        if (state_ == kDisconnecting)
        {
          shutdown_in_loop();
        }
      }
    }
    else
    {
      LOG_SYSERR << "Tcp_connection::handle_write";
      // if (state_ == kDisconnecting)
      // {
      //   shutdown_in_loop();
      // }
    }
  }
  else
  {
    LOG_TRACE << "Connection fd = " << channel_->fd()
              << " is down, no more writing";
  }
}

void Tcp_connection::handle_close()
{
  loop_->assert_in_loop_thread();
  LOG_TRACE << "fd = " << channel_->fd() << " state = " << state_to_string();
  assert(state_ == kConnected || state_ == kDisconnecting);
  // we don't close fd, leave it to dtor, so we can find leaks easily.
  set_state(kDisconnected);
  channel_->disable_all();

  Tcp_connection_ptr guard_this(shared_from_this());
  connection_callback_(guard_this);
  // must be the last line
  close_callback_(guard_this);
}

void Tcp_connection::handle_error()
{
  int err = net::get_socket_error(channel_->fd());
  LOG_ERROR << "Tcp_connection::handle_error [" << name_
            << "] - SO_ERROR = " << err << " " << strerror(err);
}
} // end namespace net
