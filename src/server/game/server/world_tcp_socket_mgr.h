/*
 * Copyright (C) 2020
 */

#ifndef S_WORLD_TCP_SOCKET_MGR_H
#define S_WORLD_TCP_SOCKET_MGR_H

class world_tcp_socket_mgr
{
public:
	~world_tcp_socket_mgr();

	static world_tcp_socket_mgr& instance();

	bool start_world_network(event_loop* loop, const std::string& bind_ip, uint16_t port, uint16_t instance_port, int netowrk_threads);

	void stop_network() override;

	void on_socket_open(socket& sock) override;

	size_t get_application_send_buffer_size() const { return socket_application_send_buffer_size_; }
protected:
	world_tcp_connection();

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

#define WORLD_TCP_SOCKET_MGR world_tcp_socket_mgr::instance();

#endif
