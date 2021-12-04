/*
 * Copyright (C) 2018
 */

#include "tcp_server.h"
#include "net/net_utils.h"
#include "net/tcp_connection.h"
#include "log/logging.h"

namespace net
{
Tcp_server::Tcp_server(Event_loop* loop, const Inet_address& listen_addr,
		const std::string& nameArg, Option option) : 
	loop_(loop),
    ip_port_(listen_addr.to_ip_port()),
    name_(nameArg),
    acceptor_(new Acceptor(loop, listen_addr, option == kReusePort)),
    thread_pool_(new Event_loop_threadpool(loop)),
    connection_callback_(default_connection_callback),
    message_callback_(default_message_callback),
    next_conn_id_(1),
	started_(0)
{
	using namespace std::placeholders;
	acceptor_->set_new_connection_callback(
			std::bind(&Tcp_server::new_connection, this, _1, _2));
}

Tcp_server::~Tcp_server()
{
	loop_->assert_in_loop_thread();

	LOG_TRACE << "Tcp_server::~Tcp_server [" << name_ << "] destructing";

	for(auto& it : connections_)
	{
		Tcp_connection_ptr conn(it.second);
		it.second.reset();
		conn->get_loop()->run_in_loop(
				std::bind(&Tcp_connection::connect_destroyed, conn));
	}
}

void Tcp_server::set_thread_num(size_t num_threads)
{
	//static_assert(0 <= num_threads, "线程数不能为负值");
	
	thread_pool_->set_thread_num(num_threads);
}

void Tcp_server::start()
{
	if (started_++ == 0)
	{
		thread_pool_->start(thread_init_callback_);

		assert(!acceptor_->listenning());

		loop_->run_in_loop(std::bind(
					&Acceptor::listen, acceptor_.get()));
  }
}

void Tcp_server::new_connection(int sockfd, const Inet_address& peer_addr)
{
	loop_->assert_in_loop_thread();
	Event_loop* io_loop = thread_pool_->get_next_loop();
	char buf[64];
	snprintf(buf, sizeof buf, "-%s#%d", ip_port_.c_str(), next_conn_id_);
	++next_conn_id_;
	std::string conn_name = name_ + buf;

	LOG_INFO << "Tcp_server::new_connection [" << name_
		<< "] - new connection [" << conn_name
        << "] from " << peer_addr.to_ip_port();
	Inet_address localAddr(get_local_addr(sockfd));
	// FIXME poll with zero timeout to double confirm the new connection
	// FIXME use make_shared if necessary
	Tcp_connection_ptr conn(new Tcp_connection(io_loop,
                                          conn_name,
                                          sockfd,
                                          localAddr,
                                          peer_addr));
	connections_[conn_name] = conn;
	conn->set_connection_callback(connection_callback_);
	conn->set_message_callback(message_callback_);
	conn->set_write_complete_callback(write_complete_callback_);
	conn->set_close_callback(
      std::bind(&Tcp_server::remove_connection, this, std::placeholders::_1)); // FIXME: unsafe
  io_loop->run_in_loop(std::bind(&Tcp_connection::connect_established, conn));
}

void Tcp_server::remove_connection(const Tcp_connection_ptr& conn)
{
	// FIXME: unsafe
	loop_->run_in_loop(std::bind(&Tcp_server::remove_connection_in_loop, this, conn));
}

void Tcp_server::remove_connection_in_loop(const Tcp_connection_ptr& conn)
{
	loop_->assert_in_loop_thread();
	LOG_INFO << "Tcp_server::remove_connection_in_loop [" << name_
           << "] - connection " << conn->name();
	size_t n = connections_.erase(conn->name());
	(void)n;
	assert(n == 1);
	Event_loop* io_loop = conn->get_loop();
	io_loop->queue_in_loop(
      std::bind(&Tcp_connection::connect_destroyed, conn));
}
} // end namespace net
