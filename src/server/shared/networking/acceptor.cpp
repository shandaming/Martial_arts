/*
 * Copyright (C) 2018
 */

#include <fcntl.h>
#include <unistd.h>

#include <cassert>

#include "acceptor.h"
#include "socket_operations.h"
#include "logging/log.h"

acceptor::acceptor(Event_loop* loop, const std::string& bind_ip, uint16_t port, bool reuseport) : 
	loop_(loop),
	endpoint_(make_address(bind_ip), port),
	//accept_socket_(socket(listen_addr.family())),
	//accept_channel_(loop, accept_socket_),
	is_open_(false),
	idle_fd_(open("/dev/null", O_RDONLY | O_CLOEXEC))
{
	assert(idle_fd_ >= 0);
	//accept_channel_.set_read_callback(std::bind(&acceptor::handle_read, this));
}

acceptor::~acceptor()
{
	close();
}

void acceptor::set_new_connection_callback(const min_connection_thread_callback& cb) 
{
	new_connection_callback_ = cb; 
}

void acceptor::set_min_connection_thread_callback(const new_connection_callback& cb) 
{
	min_connection_thread_callback_ = cb; 
}

bool bind()
{
	std::error_code ec;
	int sockfd = open(endpoint_.protocol(), ec);
	if(ec)
	{
		is_open_ = false;
		LOG_ERROR("Networking", "open() failed. error %d: %s", ec.value(), ec.message().c_str());
		return false;
	}

	accept_channel_(loop, accept_socket_),
	accept_channel_.set_read_callback(std::bind(&acceptor::handle_read, this));

	accept_socket_.open(sockfd);
	accept_socket_.set_reuse_addr();
	accept_socket_.set_reuse_port();

	is_open_ = true;

	bind(ec);
	if(ec)
	{
		accept_socket_.close();
		is_open_ = false;

		LOG_ERROR("Networking", "bind() failed. error %d: %s", ec.value(), ec.message().c_str());
		return false;
	}

	listen(ec);
	if(ec)
	{
		accept_socket_.close();
		is_open_ = false;

		LOG_ERROR("Networking", "listen() failed. error %d: %s", ec.value(), ec.message().c_str());
		return false;
	}

	loop_->assert_in_loop_thread();
	accept_channel_.enable_read();

	return true;
}

void acceptor::handle_read()
{
	loop_->assert_in_loop_thread();

	std::error_code ec;
	socket new_conn_socket;

	accept(new_conn_socket, peer_endpoint, ec);
	if(!ec && new_conn_socket.is_open())
	{
		if(new_connection_callback_)
		{
			int thread_index = min_connection_thread_callback_();
			new_connection_callback(std::forward<socket>(new_conn_socket), thread_index);
		}
		else
			new_conn_socket.close();
	}
	else
	{
		// Read the section named "The special problem of
		// accept()ing when you can't" in libev's doc.
		// By Marc Lehmann, author of libev.
		if (ec.code() == EMFILE)
		{
			close(idle_fd_);
			idle_fd_ = accept(accept_socket_, NULL, NULL);
			close(idle_fd_);
			idle_fd_ = open("/dev/null", O_RDONLY | O_CLOEXEC);
		}

		LOG_ERROR("Networking", "accept() failed. error %d: %s", ec.value(), ec.message().c_str());
	}
}

int acceptor::open(std::error_code& ec)
{
	if(is_open_)
	{
		ec = std::error_code();
		return;
	}

	tcp::endpoint::protocol_type& protocol = endpoint_.protocol();
	return non_blocking_socket(protocol.family(), protocol.type(), protocol.protocol(), ec);
}

void acceptor::close()
{
	accept_channel_.disable_all();
	accept_channel_.remove();
	close(idle_fd_);

	accept_socket_.close();
	is_open_ = false;
}

void acceptor::bind(std::error_code& ec)
{
	bind(accept_socket_, endpoint_.data(), endpoint_.size(), ec);
}

void acceptor::listen(std::error_code& ec)
{
	listen(accept_socket_, SOMAXCONN, ec);
}

std::error_code acceptor::accept(socket& peer, endpoint& peer_endpoint, std::error_code& ec)
{
	if(peer.is_open())
	{
		ec = std::errc::already_open;
		return ec;
	}

	socklen_t addr_len = peer_endpoint.capacity();
	int connfd = non_blocking_accept(accept_socket_, peer_endpoint.data(), addr_len, ec);
	if(connfd != invalid_socket)
	{
		if(peer_endpoint)
			peer_endpoint->resize(addr_len);
		peer.open(connfd);
	}
	return ec;
}
