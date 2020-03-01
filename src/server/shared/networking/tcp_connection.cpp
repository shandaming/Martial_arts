/*
 * Copyright (C) 2018
 */

#include "tcp_connection.h"
#include "net/event_loop.h"
#include "common/weak.h"
#include "logging/log.h"

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

tcp_connection::tcp_connection(event_loop* loop, const std::string& name, 
		int sockfd, const Inet_address& local_addr, const Inet_address& peer_addr) :
	loop_(loop),
    name_(name),
    state_(kConnecting),
    reading_(true),
    socket_(new Socket(sockfd)),
    channel_(new Channel(loop, sockfd)),
    local_addr_(local_addr),
    peer_addr_(peer_addr),
    high_water_mark_(64*1024*1024)
{
	channel_->set_read_callback(std::bind(&tcp_connection::handle_read, this, std::placeholders::_1));
	channel_->set_write_callback(std::bind(&tcp_connection::handle_write, this));
	channel_->set_close_callback(std::bind(&tcp_connection::handle_close, this));
	channel_->set_error_callback(std::bind(&tcp_connection::handle_error, this));

	LOG_DEBUG << "tcp_connection::ctor[" <<  name_ << "] at " << this
            << " fd=" << sockfd;
	socket_->set_keep_alive();
}

tcp_connection::tcp_connection(event_loop* loop, const std::string& name, std::shared_ptr<socket>& sockfd, const endpoint& peer_endpoint) :
	loop_(loop),
    name_(name),
    state_(kConnecting),
    reading_(true),
    socket_(sockfd),
    channel_(std::make_unique<channel>(loop, sockfd)),
    local_addr_(local_addr),
    peer_addr_(peer_addr),
    high_water_mark_(64*1024*1024),
	rempte_address_(peer_endpoint.address()),
	remote_port_(peer_endpoint.port),
	read_buffer(),
	closed_(false),
	closing_(false),
	iswriting_sync_(false)
{
	channel_->set_read_callback(std::bind(&tcp_connection::handle_read, this, std::placeholders::_1));
	channel_->set_write_callback(std::bind(&tcp_connection::handle_write, this));
	channel_->set_close_callback(std::bind(&tcp_connection::handle_close, this));
	channel_->set_error_callback(std::bind(&tcp_connection::handle_error, this));

	LOG_DEBUG << "tcp_connection::ctor[" <<  name_ << "] at " << this
            << " fd=" << sockfd;
	socket_->set_keep_alive();

	read_buffer_.resize(READ_BLOCK_SIZE);
}

tcp_connection::~tcp_connection()
{
	LOG_DEBUG << "tcp_connection::dtor[" <<  name_ << "] at " << this
            << " fd=" << channel_->fd()
            << " state=" << state_to_string();
	assert(state_ == kDisconnected);

	closed_ = true;
	socket_->close();
}

void tcp_connection::send(const void* data, int len)
{
	send(std::string(static_cast<const char*>(data), len));
}

void tcp_connection::send(const std::string& message)
{
  if (state_ == kConnected)
  {
    if (loop_->is_in_loop_thread())
    {
      send_in_loop(message);
    }
    else
    {
	void (tcp_connection::*fp)(const std::string& message) = &tcp_connection::send_in_loop;
      loop_->run_in_loop(
          std::bind(fp, this, 
                      message));
                    //std::forward<string>(message)));
    }
  }
}

// FIXME efficiency!!!
void tcp_connection::send(Buffer* buf)
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
void (tcp_connection::*fp)(const std::string& message) = &tcp_connection::send_in_loop;
      loop_->run_in_loop(
          std::bind(fp,
                      this,     // FIXME
                      buf->retrieve_all_as_string()));
                    //std::forward<string>(message)));
    }
  }
}

void tcp_connection::send_in_loop(const std::string& message)
{
	send_in_loop(message.data(), message.size());
}

void tcp_connection::send_in_loop(const void* data, size_t len)
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
				loop_->queue_in_loop(std::bind(write_complete_callback_, shared_from_this()));
		}
		else // nwrote < 0
		{
			nwrote = 0;
			if (errno != EWOULDBLOCK)
			{
				LOG_SYSERR << "tcp_connection::sendInLoop";
				if (errno == EPIPE || errno == ECONNRESET) // FIXME: any others?
					faultError = true;
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
			loop_->queue_in_loop(std::bind(high_water_mark_callback_, shared_from_this(), oldLen + remaining));

		output_buffer_.append(static_cast<const char*>(data)+nwrote, remaining);

		if (!channel_->is_write())
			channel_->enable_write();
	}
}

void tcp_connection::shutdown()
{
	// FIXME: use compare and swap
	if (state_ == kConnected)
	{
		set_state(kDisconnecting);
		// FIXME: shared_from_this()?
		loop_->run_in_loop(std::bind(&tcp_connection::shutdown_in_loop, this));
	}
}

void tcp_connection::shutdown_in_loop()
{
	loop_->assert_in_loop_thread();
	if (!channel_->is_write())
		// we are not writing
		socket_->shutdown_write();
}

void tcp_connection::force_close()
{
	// FIXME: use compare and swap
	if (state_ == kConnected || state_ == kDisconnecting)
	{
		set_state(kDisconnecting);
		loop_->queue_in_loop(std::bind(&tcp_connection::force_close_in_loop, shared_from_this()));
	}
}

void tcp_connection::force_close_with_delay(double seconds)
{
	if (state_ == kConnected || state_ == kDisconnecting)
	{
		set_state(kDisconnecting);
		loop_->run_after(seconds,
				make_weak_callback(shared_from_this(),
					&tcp_connection::force_close));  // not forceCloseInLoop to avoid race condition
  }
}

void tcp_connection::force_close_in_loop()
{
	loop_->assert_in_loop_thread();

	if (state_ == kConnected || state_ == kDisconnecting)
	{
		// as if we received 0 byte in handle_read();
		handle_close();
	}
}

const char* tcp_connection::state_to_string() const
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

void tcp_connection::set_tcp_no_delay(bool on)
{
	socket_->set_tcp_no_delay();
}

void tcp_connection::start_read()
{
	loop_->run_in_loop(std::bind(&tcp_connection::start_read_in_loop, this));
}

void tcp_connection::start_read_in_loop()
{
	loop_->assert_in_loop_thread();

	if (!reading_ || !channel_->is_read())
	{
		channel_->enable_read();
		reading_ = true;
	}
}

void tcp_connection::stop_read()
{
	loop_->run_in_loop(std::bind(&tcp_connection::stop_read_in_loop, this));
}

void tcp_connection::stop_read_in_loop()
{
	loop_->assert_in_loop_thread();

	if (reading_ || channel_->is_read())
	{
		channel_->disable_read();
		reading_ = false;
	}
}

void tcp_connection::connect_established()
{
	loop_->assert_in_loop_thread();
	assert(state_ == kConnecting);

	set_state(kConnected);
	channel_->tie(shared_from_this());
	channel_->enable_read();

	connection_callback_(shared_from_this());
}

void tcp_connection::connect_destroyed()
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

void tcp_connection::handle_read(Timestamp receive_time)
{
	loop_->assert_in_loop_thread();

	size_t tranferred_bytes = 0;
	std::error_code ec;
	size_t bytes_read = 0;
	size_t remaing_space = 0;
	int sockfd = *socket;

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
			LOG_ERROR("Networking", "socket[%d]-ip[%s] read failed. error %d: %s", 
					sockfd, remote_address_.to_string().c_str(), ec.value(), ec.message().c_str());
			handle_error();
			break;
		}
		else if(bytes_read == 0)
		{
			handle_close();
			break;
		}
		else if(bytes_read < remaining_space)
		{
			read_buffer_.write_complate(bytes_read);
			message_callback_(shared_from_this(), &input_buffer_, receive_time);
			break;
		}
		else
			read_buffer_.write_complate(bytes_read);
	}

	read_handler_callback_(ec, tranferred_bytes);
}

void tcp_connection::handle_write()
{
	loop_->assert_in_loop_thread();
#if 0

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
      LOG_SYSERR << "tcp_connection::handle_write";
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
#else
  iswriting_sync_ = false;
  handle_queue();
#endif
}

void tcp_connection::handle_close()
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

void tcp_connection::handle_error()
{
	//-----------------------------
	int sockfd = channel_->get_file_descriptor();
	std::error_code ec;
	int optvel;
	int result = getsockopt(sockfd), SOL_SOCKET, SO_ERROR, &optvel, sizeof(optvel, ec);
	if(ec)
		LOG_ERROR("Networking", "socket[%d] error %d: %s", 
				sockfd, ec.value(), ec.message().c_str());
	//-------------------------------

	int err = net::get_socket_error(channel_->fd());
	LOG_ERROR << "tcp_connection::handle_error [" << name_
            << "] - SO_ERROR = " << err << " " << strerror(err);
}

//------------------------------------------

void tcp_connection::queue_packet(message_buffer&& buffer)
{
	write_queue_.push(std::move(buffer));
}

bool tcp_connection::sync_process_queue()
{
	if(iswriting_sync_)
		return false;
	iswriting_sync_ = true;

	channel_->enable_write();
	return false;
}

bool tcp_connection::handle_queue()
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
