/*
 * Copyright (C) 2018
 */

#include "tcp_server.h"

namespace net
{
Tcp_server::Tcp_server(Event_loop* loop, const Inet_address& listen_addr,
		const string& nameArg, Option option) : 
	loop_(CHECK_NOTNULL(loop)),
    ip_port_(listen_addr.toIpPort()),
    name_(nameArg),
    acceptor_(new Acceptor(loop, listen_addr, option == kReusePort)),
    thread_pool_(new Event_loop_threadpool(loop, name_)),
    connection_callback_(defaultConnectionCallback),
    message_callback_(defaultMessageCallback),
    next_conn_id_(1),
	stared_(0)
{
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
		it.second.rest();
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
	if (started++ == 0)
	{
		thread_pool_->start(thread_init_callback_);

		assert(!acceptor_->listenning());

		loop_->run_in_loop(std::bind(
					&Acceptor::listen, get_pointer(acceptor_)));
  }
}

void Tcp_server::new_connection(int sockfd, const Inet_address& peer_addr)
{
	loop_->assert_in_loop_thread();
	Event_loop* ioLoop = thread_pool_->get_next_loop();
	char buf[64];
	snprintf(buf, sizeof buf, "-%s#%d", ip_port_.c_str(), next_conn_id_);
	++next_conn_id_;
	string conn_name = name_ + buf;

	LOG_INFO << "Tcp_server::new_connection [" << name_
		<< "] - new connection [" << conn_name
        << "] from " << peer_addr.toIpPort();
	Inet_address localAddr(sockets::getLocalAddr(sockfd));
	// FIXME poll with zero timeout to double confirm the new connection
	// FIXME use make_shared if necessary
	Tcp_connection_ptr conn(new TcpConnection(ioLoop,
                                          conn_name,
                                          sockfd,
                                          localAddr,
                                          peer_addr));
	connections_[conn_name] = conn;
	conn->set_connection_callback(connection_callback_);
	conn->setMessage_callback(message_callback_);
	conn->set_write_complete_callback(write_complete_callback_);
	conn->setCloseCallback(
      std::bind(&Tcp_server::remove_connection, this, _1)); // FIXME: unsafe
  ioLoop->run_in_loop(boost::bind(&TcpConnection::connectEstablished, conn));
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
