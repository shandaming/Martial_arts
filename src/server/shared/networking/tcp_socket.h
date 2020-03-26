/*
 * Copyright (C) 2018
 */

#ifndef NET_TCP_SOCKET_H
#define NET_TCP_SOCKET_H

#include <any>

#include "buffer.h"
#include "socket.h"
#include "channel.h"

#define READ_BLOCK_SIZE 4096

class event_loop;

class tcp_socket : public std::enable_shared_from_this<tcp_socket>
{
public:
	tcp_socket(event_loop* loop, socket&& sockfd);
	virtual ~tcp_socket();

	tcp_socket(const tcp_socket&) = delete;
	tcp_socket& operator=(const tcp_socket&) = delete;

	event_loop* get_loop() const { return loop_; }
	bool connected() const { return state_ == kConnected; }
	bool disconnected() const { return state_ == kDisconnected; }

	void set_tcp_no_delay(bool on);

	void set_connection_callback(const Connection_callback& cb)
	{
		connection_callback_ = cb; 
	}

	void set_read_handler_callback(std::function<void(std::error_code&, size_t)> cb)
	{
		read_handler_callback_ = cb;
	}

	// called when TcpServer accepts a new connection
	void connect_established();   // should be called only once
	// called when TcpServer has removed me from its map
	void connect_destroyed(); // should be called only once

	virtual void start() = 0;

	virtual void update();

	void queue_packet(message_buffer&& buffer);

	address get_remote_ip_address() const { return remote_address_; }
	uint16_t get_remote_port() const { return remote_port_; }

	bool is_open() const { return !closed_ && !closing_; }

	void close_socket();

	void delayed_close_tcp_socket() { closing_ = true; }

	message_buffer& get_read_buffer() { return read_buffer_; }
protected:
	virtual bool void on_close() {}

	virtual void read_handler() = 0;

	bool sync_process_queue();

	socket& underlying_stream() { return socket_; }
private:
	void read_handler_internal(std::error_code& ec, size_t transferred_bytes)
	{
		if(!ec)
			read_handler();
	}

	void write_handler_wrapper();

	bool handle_queue();


	enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
	void handle_read();
	void handle_close();
	void handle_error();

	void set_state(StateE s) { state_ = s; }
	const char* state_to_string() const;

	event_loop* loop_;
	StateE state_;  // FIXME: use atomic variable
	// we don't expose those classes to client.
	socket socket_;
	channel channel_;

	address remote_address_;
	uint16_t remote_port_;

	std::function<void(std::error_code&, size_t)> read_handler_callback_;
	message_buffer read_buffer_;
	std::queue<message_buffer> write_queue_;

	std::atomi<bool> closed_;
	std::atomi<bool> closing_;
	bool iswriting_sync_;
};

typedef std::shared_ptr<tcp_socket> tcp_socket_ptr;

#endif
