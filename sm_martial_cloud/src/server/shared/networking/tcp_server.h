/*
 * Copyright (C) 2018
 */

#ifndef NET_TCP_SERVER_H
#define NET_TCP_SERVER_H

#include <atomic>

#include "net/event_loop_threadpool.h"
#include "net/acceptor.h"

namespace net
{
class Tcp_server
{
public:
	typedef std::function<void(Event_loop*)> Thread_init_callback;

	enum Option
	{
		kNoReusePort,
		kReusePort,
	};

	Tcp_server(Event_loop* loop,
			const Inet_address& listen_addr,
            const std::string& nameArg,
            Option option = kNoReusePort);

	~Tcp_server();  // 强行离线，对于scoped_ptr成员。

	Tcp_server(const Tcp_server&) = delete;
	Tcp_server& operator=(const Tcp_server&) = delete;

	const std::string& ip_port() const { return ip_port_; }
	const std::string& name() const { return name_; }
	Event_loop* get_loop() const { return loop_; }

	/*
	 * 设置处理输入的线程数。
	 *
	 * 始终在循环的线程中接受新连接。必须在@c start之前调用
	 * @param num_threads
	 * 0表示循环线程中的所有I / O，不会创建任何线程。 这是默认值。
	 * 1表示另一个线程中的所有I / O.
	 * N表示具有N个线程的线程池，在循环的基础上分配新连接。
	 */
	void set_thread_num(size_t num_threads);
	void set_thread_init_callback(const Thread_init_callback& cb)
	{ 
		thread_init_callback_ = cb; 
	}

	// 调用start（）后有效
	std::shared_ptr<Event_loop_threadpool> thread_pool()
	{ 
		return thread_pool_; 
	}

	void start();

	// Not thread safe.
	void set_connection_callback(const Connection_callback& cb)
	{
		connection_callback_ = cb; 
	}

	// Not thread safe.
	void setMessage_callback(const Message_callback& cb)
	{
		message_callback_ = cb; 
	}

	// Not thread safe.
	void set_write_complete_callback(const Write_complete_callback& cb)
	{ 
		write_complete_callback_ = cb; 
	}
private:
	// Not thread safe, but in loop
	void new_connection(int sockfd, const Inet_address& peer_addr);
	// Thread safe.
	void remove_connection(const Tcp_connection_ptr& conn);
	// Not thread safe, but in loop
	void remove_connection_in_loop(const Tcp_connection_ptr& conn);

	typedef std::map<std::string, Tcp_connection_ptr> Connection_map;

	Event_loop* loop_;  // the acceptor loop
	const std::string ip_port_;
	const std::string name_;
	Scoped_ptr<Acceptor> acceptor_; // avoid revealing Acceptor
	std::shared_ptr<Event_loop_threadpool> thread_pool_;

	Connection_callback connection_callback_;
	Message_callback message_callback_;
	Write_complete_callback write_complete_callback_;
	Thread_init_callback thread_init_callback_;

	std::atomic<int> started_;
  	// always in loop thread
	int next_conn_id_;
	Connection_map connections_;
};
} // end namespace net

#endif
