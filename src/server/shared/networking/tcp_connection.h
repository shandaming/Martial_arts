/*
 * Copyright (C) 2018
 */

#ifndef NET_TCP_CONNECTION_H
#define NET_TCP_CONNECTION_H

#include <any>

#include "buffer.h"
#include "net/inet_address.h"
#include "net/net_utils.h"
#include "net/socket.h"
#include "net/channel.h"
#include "common/scoped_ptr.h"

#define READ_BLOCK_SIZE 4096

class event_loop;

class tcp_connection : public std::enable_shared_from_this<tcp_connection>
{
public:
	tcp_connection(event_loop* loop,
                const std::string& name,
                int sockfd,
                const Inet_address& local_addr,
                const Inet_address& peer_addr);
	tcp_connection(event_loop* loop, const std::string& name, socket& sockfd, const endpoint& peer_endpoint);
	virtual ~tcp_connection();

	tcp_connection(const tcp_connection&) = delete;
	tcp_connection& operator=(const tcp_connection&) = delete;

	event_loop* get_loop() const { return loop_; }
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

	void set_context(const std::any& context) { context_ = context; }

	const std::any& get_context() const { return context_; }

	std::any* get_mutable_context() { return &context_; }

	void set_connection_callback(const Connection_callback& cb)
	{
		connection_callback_ = cb; 
	}

	void set_message_callback(const Message_callback& cb)
	{
		message_callback_ = cb; 
	}

	void set_write_complete_callback(const Write_complete_callback& cb)
	{ 
		write_complete_callback_ = cb; 
	}

	void set_high_water_mark_callback(const High_water_mark_callback& cb, size_t high_water_mark)
	{ 
		high_water_mark_callback_ = cb; 
		high_water_mark_ = high_water_mark; 
	}

	void set_read_handler_callback(std::function<void(std::error_code&, size_t)> cb)
	{
		read_handler_callback_ = cb;
	}

	/// Advanced interface
	Buffer* input_buffer() { return &input_buffer_; }

	Buffer* output_buffer() { return &output_buffer_; }

	/// Internal use only.
	void set_close_callback(const Close_callback& cb) 
	{
		close_callback_ = cb; 
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

	void close_tcp_connection();

	void delayed_close_tcp_connection() { closing_ = true; }

	message_buffer& get_read_buffer() { return read_buffer_; }
protected:
	virtual bool void on_close() {}

	virtual void read_handler() = 0;

	bool sync_process_queue();

	socket& underlying_stream() { return *socket_; }
private:
	void read_handler_internal(std::error_code& ec, size_t transferred_bytes)
	{
		if(!ec)
			read_handler();
	}

	void write_handler_wrapper(std::error_code& ec, size_t);

	bool handle_queue();



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
	const char* state_to_string() const;
	void start_read_in_loop();
	void stop_read_in_loop();

	event_loop* loop_;
	const std::string name_;
	StateE state_;  // FIXME: use atomic variable
	bool reading_;
	// we don't expose those classes to client.
	std::shared_ptr<socket> socket_;
	std::shared_ptr<Channel> channel_;
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

	address remote_address_;
	uint16_t remote_port_;

	std::function<void(std::error_code&, size_t)> read_handler_callback_;
	message_buffer read_buffer_;
	std::queue<message_buffer> write_queue_;

	std::atomi<bool> closed_;
	std::atomi<bool> closing_;
	bool iswriting_sync_;
};

typedef std::shared_ptr<tcp_connection> Tcp_connection_ptr;

#endif
