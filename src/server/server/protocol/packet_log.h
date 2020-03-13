/*
 * Copyright (C) 2020
 */

#ifndef PACKET_LOG_H
#define PACKET_LOG_H

enum direction
{
	CLIENT_TO_SERVER,
	SERVER_TO_CLIENT
};

class world_packet;
class address;
enum connection_type : int8_t;

class packet_log
{
public:
	static packet_log* instance();
	void initialize();
	bool can_log_packet() const { return file_ != NULL; }
	void log_packet(const world_packet& packet, direction direction, const address& addr, uint16_t port, connection_type connection_type);
private:
	packet_log();
	~packet_log();

	std::mutex log_packet_lock_;
	std::once_flag_initialize_flag_;
	FILE* file_;
};

#endif
