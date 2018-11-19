/*
 * Copyright (C) 2018
 */

#ifndef NET_TCP_CONNECTION_H
#define NET_TCP_CONNECTION_H

#include "buffer.h"

namespace net
{
class Tcp_connection
{
public:
Tcp_connection(Event_loop* loop,
                const std::string& name,
                int sockfd,
                const Inet_address& local_addr,
                const Inet_address& peer_addr);
  ~Tcp_connection();

  Event_loop* get_loop() const { return loop_; }
  const std::string& name() const { return name_; }
  const Inet_address& local_address() const { return local_addr_; }
  const Inet_address& peer_address() const { return peer_addr_; }
  bool connected() const { return state_ == kConnected; }
  bool disconnected() const { return state_ == kDisconnected; }
  // return true if success.
  bool getTcpInfo(struct tcp_info*) const;
  std::string getTcpInfoString() const;

  // void send(string&& message); // C++11
  void send(const void* message, int len);
  void send(const std::string& message);
  // void send(Buffer&& message); // C++11
  void send(Buffer* message);  // this one will swap data
  void shutdown(); // NOT thread safe, no simultaneous calling
  // void shutdownAndForceCloseAfter(double seconds); // NOT thread safe, no simultaneous calling
  void force_close();
  void force_close_with_delay(double seconds);
  void set_tcp_no_delay(bool on);
  // reading or not
  void start_read();
  void stop_read();
  bool is_reading() const { return reading_; }; // NOT thread safe, may race with start/stopReadInLoop

  void set_context(const std::any& context)
  { context_ = context; }

  const std::any& get_context() const
  { return context_; }

  std::any* get_mutable_context()
  { return &context_; }

  void set_connection_callback(const Connection_callback& cb)
  { connection_callback_ = cb; }

  void set_message_callback(const Message_callback& cb)
  { message_callback_ = cb; }

  void set_write_complete_callback(const Write_complete_callback& cb)
  { write_complete_callback_ = cb; }

  void set_high_water_mark_callback(const High_water_mark_callback& cb, size_t high_water_mark)
  { high_water_mark_callback_ = cb; high_water_mark_ = high_water_mark; }

  /// Advanced interface
  Buffer* input_buffer()
  { return &input_buffer_; }

  Buffer* output_buffer()
  { return &output_buffer_; }

  /// Internal use only.
  void set_close_callback(const Close_callback& cb)
  { close_callback_ = cb; }

  // called when TcpServer accepts a new connection
  void connect_established();   // should be called only once
  // called when TcpServer has removed me from its map
void connect_destroyed(); // should be called only once
private:
  enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
  void handle_read(Timestamp receive_time);
  void handle_write();
  void handle_close();
  void handle_error();
  // void sendInLoop(string&& message);
  void send_in_loop(const std::string& message);
  void send_in_loop(const void* message, size_t len);
  void shutdown_in_loop();
  // void shutdownAndForceCloseInLoop(double seconds);
  void force_close_in_loop();
  void set_state(StateE s) { state_ = s; }
  const char* stateToString() const;
  void start_read_in_loop();
  void stop_read_in_loop();

  Event_loop* loop_;
  const std::string name_;
  StateE state_;  // FIXME: use atomic variable
  bool reading_;
  // we don't expose those classes to client.
  Scoped_ptr<Socket> socket_;
  Scoped_ptr<Channel> channel_;
  const Inet_address local_addr_;
  const Inet_address peer_addr_;

  Connection_callback connection_callback_;
  Message_callback message_callback_;
  Write_complete_callback write_complete_callback_;
  High_water_mark_callback high_water_mark_callback_;
  Close_callback close_callback_;

  size_t high_water_mark_;
  Buffer input_buffer_;
  Buffer output_buffer_; // FIXME: use list<Buffer> as output buffer.
std::any context_;
};

typedef std::shared_ptr<Tcp_connection> Tcp_connection_ptr;
} // end namespace net

#endif
