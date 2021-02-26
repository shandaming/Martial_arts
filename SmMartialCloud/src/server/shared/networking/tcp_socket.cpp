/*
 * Copyright (C) 2018
 */

#include "tcp_socket.h"
#include "event_loop.h"
#include "weak.h"
#include "log.h"

tcp_socket::tcp_socket(event_loop* loop, tcp::socket&& sockfd) :
	loop_(loop),
	state_(kConnecting),
	socket_(std::move(sockfd)),
	channel_(loop, socket_),
	remote_address_(socket_.remote_endpoint().address()),
	remote_port_(socket_.remote_endpoint().port()),
	read_buffer(),
	closed_(false),
	closing_(false),
	iswriting_sync_(false)
{
	channel_.set_read_callback(std::bind(&tcp_socket::handle_read, this, std::placeholders::_1));
	channel_.set_write_callback(std::bind(&tcp_socket::write_handler_wrapper, this));
	channel_.set_close_callback(std::bind(&tcp_socket::close_socket, this));
	channel_.set_error_callback(std::bind(&tcp_socket::handle_error, this));

	LOG_DEBUG("network", "New tcp socket [%d:%s:%u]", socket_,  remote_address_.to_string().c_str(), remote_port_);

	socket_->set_keep_alive();

	read_buffer_.resize(READ_BLOCK_SIZE);
		
	connect_established();
}

tcp_socket::~tcp_socket()
{
	LOG_DEBUG("network", "Delete tcp socket [%d:%s:%u]", socket_,  remote_address_.to_string().c_str(), remote_port_);

	assert(state_ == kDisconnected);

	closed_ = true;
	socket_.close();
}

const char* tcp_socket::state_to_string() const
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

void tcp_socket::set_no_delay(bool enable)
{
	socket_->set_option(option::tcp_no_delay(enable));
	//socket_->set_no_delay();
}

void tcp_socket::connect_established()
{
	loop_->assert_in_loop_thread();
	assert(state_ == kConnecting);

	set_state(kConnected);
	//channel_.tie(shared_from_this());
	channel_.enable_read();

	//connection_callback_(shared_from_this());
}

void tcp_socket::connect_destroyed()
{
	loop_->assert_in_loop_thread();

	if (state_ == kConnected)
	{
		set_state(kDisconnected);
		channel_.disable_all();

		//connection_callback_(shared_from_this());
	}
	channel_.remove();
}

void tcp_socket::handle_read()
{
	loop_->assert_in_loop_thread();

	size_t tranferred_bytes = 0;
	std::error_code ec;
	size_t bytes_read = 0;
	size_t remaing_space = 0;
	int sockfd = socket_;

	while(1)
	{
		read_buffer.normalize();
		read_buffer.ensure_free_space();
		remaining_space = read_buffer_.get_remaining_space();

		iovec vec;
		vec.iov_base = read_buffer_.get_write_pointer();
		vec.iov_len = remaining_space;

		if(non_blocking_read(socket, &vec, remaining_space, ec, bytes_read))
				transferred_bytes += bytes_read;
		if(ec)
		{
			LOG_ERROR("networking", "tcp socket [%d:%s:%u] read failed. error %d: %s", 
					sockfd, remote_address_.to_string().c_str(), get_remote_port(), ec.value(), ec.message().c_str());
			handle_error();
			break;
		}
		else if(bytes_read == 0)
		{
			close_socket();//handle_close();
			break;
		}
		else if(bytes_read < remaining_space)
		{
			read_buffer_.write_complate(bytes_read);
			break;
		}
		else
			read_buffer_.write_complate(bytes_read);
	}

	read_handler_callback_(ec, tranferred_bytes);
}

void tcp_socket::write_handler_wrapper()
{
	loop_->assert_in_loop_thread();
	
	iswriting_sync_ = false;
	handle_queue();
}

void tcp_socket::handle_close()
{
	loop_->assert_in_loop_thread();

	LOG_TRACE << "fd = " << channel_.fd() << " state = " << state_to_string();
	ASSERT(state_ == kConnected || state_ == kDisconnecting);
	// we don't close fd, leave it to dtor, so we can find leaks easily.
	set_state(kDisconnected);
	channel_.disable_all();

	connect_destroyed();
}

void tcp_socket::close_socket()
{
	if(closed_.exchange(true))
		return;
	
	std::error_code ec;
	socket_.shutdown(socket::shutdown_send, ec);
	if(ec)
		LOG_ERROR("networking", "Close tcp socket %s error: %d %s", 
			get_remote_ip_address().to_string().c_str(), ec.value(), ec.message().c_str());
	on_close();
	handle_close();
}

void tcp_socket::handle_error()
{
	int sockfd = socket_;
	std::error_code ec;
	int optvel;
	int result = getsockopt(sockfd), SOL_SOCKET, SO_ERROR, &optvel, sizeof(optvel, ec);
	if(ec)
		LOG_ERROR("networking", "tcp socket[%d:%s:%u] error %d: %s", sockfd, 
				remote_address_.to_string().c_str(), remote_port_, 
				ec.value(), ec.message().c_str());
}

//------------------------------------------

void tcp_socket::queue_packet(message_buffer&& buffer)
{
	write_queue_.push(std::move(buffer));
}

bool tcp_socket::sync_process_queue()
{
	if(iswriting_sync_)
		return false;
	iswriting_sync_ = true;

	channel_.enable_write();
	return false;
}

bool tcp_socket::handle_queue()
{
	if(write_queue_.empty())
		return false;

	message_buffer& queue_message = write_queue_.front();
	size_t bytes_to_send = queue_message.get_active_size();
	std::error_code ec;
	size_t bytes_sent = 0;

	iovec vec;
	vec.iov_base = queue_message.get_read_pointer();
	vec.iov_len = bytes_to_send;

	non_blocking_write(*socket_, &vec, bytes_to_send, ec, bytes_sent);
	if(ec)
	{
		if(ec == std::errc::would_block || ec == std::errc::try_again)
			return sync_process_queue();

		write_queue_.pop();
		if(close && write_queue_.empty())
			close_socket();
		return false;
	}
	else if(bytes_sent == 0)
	{
		write_queue_.pop();
		if(close && write_queue_.empty())
			close_socket();
		return false;
	}
	else if(bytes_sent < bytes_to_send)
	{
		queue_message.read_complate(bytes_sent);
		return sync_process_queue();
	}

	write_queue_.pop();
	if(close && write_queue_.empty())
		close_socket();
	return write_queue.empty();
}
