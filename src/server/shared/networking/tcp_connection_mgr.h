/*
 * Copyright (C) 2020
 */

#ifndef TCP_CONNECTION_MGR_H
#define TCP_CONNECTION_MGR_H

#include "acceptor.h"
#include "logging/log.h"

template<typename ConnectionType>
class tcp_connection_mgr
{
public:
	virtual ~tcp_connection_mgr()
	{
		ASSERT(!threads_ && !acceptor_ && !thread_count_, "StopNetwork must be called prior to tcp_connection_mgr destruction.");
	}

	virtual bool start_network(event_loop* event_loop, const std::string& bind_ip, uint16_t port, int thread_count)
	{
		ASSERT(thread_count_ > 0);

		acceptor* acceptor = nullptr;
		acceptor = new acceptor(loop, bind_ip, port);
		if(!acceptor_)
		{
			LOG_ERROR("Network", "Exception caught in tcp_connection_mgr. start_network (%s:%d)", bind_ip.c_str(), port);
			return false;
		}
		if(!acceptor->bind())
		{
			LOG_ERROR("Network", "start_network failed to bind socket acceptor.");
			return false;
		}

		acceptor_ = acceptor;
		thread_count_ = thread_count;
		threads_ = create_threads();

		ASSERT(threads_);

		for(int i = 0; i < thread_count_; ++i)
			threads[i].start();

		return true;
	}

	virtual void stop_network()
	{
		acceptor_->close();

		if(thread_count_ != 0)
			for(int i = 0; i < thread_count_; ++i)
				threads[i].stop();

		wait();

		delete acceptor_;
		acceptor_ = nullptr;
		delete[] threads_;
		threads_ = nullptr;
		thread_count_ = 0;
	}

	void wait()
	{
		if(thread_count_ != 0)
			for(int i = 0; i < thread_count_; ++i)
				threads[i].wait();
	}

	virtual void on_connection_open(socket&& socket, uint32_t thread_index)
	{
		std::shared_ptr<ConnectionType> new_connection = std::make_shared<ConnectionType>(std::move(sock));
		new_connection->start();

std::shared_ptr<SocketType> newSocket = std::make_shared<SocketType>(std::move(sock));

uint32_t thread_index = select_thread_with_min_connection();
_threads[threadIndex].AddSocket(newSocket);

	Tcp_connection_ptr conn(new Tcp_connection(io_loop, std::move(socket)));
	connections_[conn_name] = conn;
	conn->set_connection_callback(connection_callback_);
	conn->set_message_callback(message_callback_);
	conn->set_write_complete_callback(write_complete_callback_);
	conn->set_close_callback(
      std::bind(&Tcp_server::remove_connection, this, std::placeholders::_1)); // FIXME: unsafe
  io_loop->run_in_loop(std::bind(&Tcp_connection::connect_established, conn));



		threads_[thread_index].add_connection(new_connection);
	}

	int32_t get_network_thread_count() const { return thread_count_; }

	uint32_t select_thread_with_min_connection() const
	{
		uint32_t min = 0;
		for(int i = 1; i < thread_count_; ++i)
			if(threads_[i].get_connection_count() < threads_[min].get_connection_count())
				min = i;
		return min;
	}

	std::pair<socket*, uint32_t> get_socket_for_accept()
	{
		uint32_t thread_index = select_thread_with_min_connection();
		return std::pai(threads_[thread_index], get_socket_for_acceptor(), thread_index);
	}
private:
	tcp_connection_mgr() : acceptor_(nullptr), threads_(nullptr), thread_count_(0) {}

	acceptor* acceptor_;
	network_thread<ConnectionType> threads_;
	int32_t thread_count_;
};

#endif
