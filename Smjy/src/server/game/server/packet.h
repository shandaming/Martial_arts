/*
 * Copyright (C) 2020
 */

#ifndef S_PACKET_H
#define S_PACKET_H

#include "world_packet.h"

namespace world_packets
{
class packet
{
public:
	packet(world_packet&& world_packet);
	virtual ~packet() = default;

	packet(const packet& right) = delete;
	packet& operator=(const packet& right) = delete;

	virtual const world_packet* write() = 0;
	virtual void read() = 0;

	const world_packet* get_raw_packet() const { return &world_packet_; }
	size_t get_size() const { return world_packet_.size(); }
	connection_type get_connection() const { return world_packet_.get_connection(); }
protected:
	world_packet world_packet_;
};

struct server_packet : public packet
{
	server_packet(opcode_server opcode, size_t initial_size = 200, connection_type connection = CONNECTION_TYPE_DEFAULT);

	void read() override final;
	void clear() { world_packet_.clear(); }
	world_packet&& move() { return std::move(world_packet_); }

	opcode_server get_opcode() const { return opcode_server(world_packet_.get_opcode()); }
};

struct client_packet : public packet
{
	client_packet(world_packet&& packet);
	client_packet(opcode_client expected_opcode, world_packet&& packet);

	const world_packet* write() override final;
	opcode_client get_opcode() const { return opcode_client(world_packet_.get_opcode()); }
};
}

#endif
