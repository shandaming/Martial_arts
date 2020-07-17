/*
 * Copyright (C) 2020
 */

#include "world_socket_mgr.h"

static void on_socket_accept(socket&& socket)
{
	WORLD_SOCKET_MGR.on_socket_open(std::forward<socket>(socket));
}

world_socket_mgr::world_socket_mgr() : base_socket_mgr(), socket_system_send_buffer_size(-1), socket_application_send_buffer_size_(65536) {}

world_socket_mgr::~world_socket_mgr()
{
}

world_socket_mgr& world_socket_mgr::instance()
{
	static world_socket_mgr instance;
	return instance;
}

bool world_socket_mgr::start_world_network(const std::string& bind_ip, uint16_t port, uint16_t instance_port, int thread_count)
{
	const int max_connection = MAX_LISTEN_CONNECTIONS;
	LOG_DEBUG("misc", "Max allowed socket connection %d", max_connection);

	socket_system_send_buffer_size_ = 1024;
	socket_application_send_buffer_size_ = 1024;

	if(socket_application_send_buffer_size_ == 0)
	{
		LOG_ERROR("misc", "Network Out buffer is wrong in your config file.");
		return false;
	}

	if(!base_socket_mgr::start_network(bind_ip, port, thread_count))
		return false;

	acceptor->set_new_connection_callback(std::bind(on_socket_accept, this, _1)
}

void world_socket_mgr::on_socket_open(socket&& sock)
{
	if(socket_system_send_buffer_size_ >= 0)
	{
		if(!sock.set_option(send_buffer_size(socket_system_send_buffer_size_)))
			return;
	}

	if(tcp_no_delay_)
	{
		if(!sock.set_option(tcp_no_delay()))
			return;
	}

	base_socket_mgr::on_socket_open(std::forward<socket>(sock));
}
