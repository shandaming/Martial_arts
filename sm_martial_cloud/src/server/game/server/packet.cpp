/*
 * Copyright (C) 2020
 */

#include "packet.h"
#include "debugging/errors.h"

namespace world_packet
{
packet::packet(world_packet&& world_packet) : world_packet_(std::move(world_packet)) {}

server_packet::server_packet(opcode_server opcode, size_t initial_size, connection_type connection) : 
	packet(world_packet(opcode, initial_size, connection)) {}

void server_packet::read()
{
	ASSERT("Read not implemented for server packets.");
}

client_packet::client_packet(opcode_client expected_opcode, world_packet&& packet) :
	packet(std::move(packet))
{
	ASSERT(get_opcode() == expected_opcode);
}

client_packet::client_packet(world_packet&& packet) : packet(std::move(packet)) {}

const world_packet* client_packet::write()
{
	ASSERT("Write not allowed for client packets.");
	return nullptr;
}
}
