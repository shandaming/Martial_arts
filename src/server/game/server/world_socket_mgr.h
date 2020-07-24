/*
 * Copyright (C) 2020
 */

#ifndef S_WORLD_TCP_SOCKET_MGR_H
#define S_WORLD_TCP_SOCKET_MGR_H

#include <string>
#include <cstdint>

#include "network_thread.h"
#include "world_socket.h"

class event_loop;
class acceptor;

class world_socket_mgr
{
public:
	~world_socket_mgr();

	static world_socket_mgr& instance();

	bool start_world_network(event_loop* loop, const std::string& bind_ip, uint16_t port, uint16_t instance_port, int netowrk_threads);

	void stop_network() override;

	void on_socket_open(tcp::socket& sock) override;

	size_t get_application_send_buffer_size() const { return socket_application_send_buffer_size_; }
protected:
	world_socket_mgr();

	network_thread<world_socket>* create_thread() const override;
private:
	bool start_network(const std::string& bind_ip, uint16_t port, int thread_count) override
	{
		return base_socket_mgr::start_network(bind_ip, port, thread_count);
	}

	acceptor* acceptor_;
	int32_t socket_system_send_buffer_size_;
	int32_t socket_application_send_buffer_size_;
	bool tcp_no_delay_;
};

#define WORLD_SOCKET_MGR world_socket_mgr::instance();

#endif
