/*
 * Copyright (C) 2020
 */

#include "world_socket_mgr.h"

static void on_socket_accept(tcp::socket&& sock)
{
	WORLD_SOCKET_MGR.on_socket_open(std::forward<tcp::socket>(sock));
}

struct world_socket_thread : public network_thread<world_socket>
{
	void socket_added(std::shared_ptr<world_socket> sock) override
	{
		sock->set_send_buffer_size(WORLD_SOCKET_MGR.get_application_send_buffer_size());
		//sScriptMgr->on_socket_open(sock);
	}

	void socket_removed(std::shared_ptr<world_socket> sock) override
	{
		//sScriptMgr->on_socket_close(sock);
	}
};

world_socket_mgr::world_socket_mgr() : base_socket_mgr(), socket_system_send_buffer_size_(-1), socket_application_send_buffer_size_(65536) {}

world_socket_mgr::~world_socket_mgr()
{
}

world_socket_mgr& world_socket_mgr::instance()
{
	static world_socket_mgr instance;
	return instance;
}

bool world_socket_mgr::start_world_network(event_loop* event_loop, const std::string& bind_ip, uint16_t port, uint16_t instance_port, int thread_count)
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

	if(!base_socket_mgr::start_network(event_loop, bind_ip, port, thread_count))
		return false;

	acceptor_->set_new_connection_callback(std::bind(&on_socket_accept, std::placeholders::_1));

	//sScriptMgr->OnNetworkStart();
}

void world_socket_mgr::on_socket_open(tcp::socket&& sock)
{
	if(socket_system_send_buffer_size_ >= 0)
	{
		if(!sock.set_option(option::send_buffer_size(socket_system_send_buffer_size_)))
			return;
	}

	if(tcp_no_delay_)
	{
		if(!sock.set_option(option::tcp_no_delay()))
			return;
	}

	base_socket_mgr::on_socket_open(std::forward<tcp::socket>(sock));
}

network_thread<world_socket>* world_socket_mgr::create_threads() const
{
    return new world_socket_thread[get_network_thread_count()];
}
