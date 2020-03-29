/*
 * Copyright (C) 2020
 */

#ifndef S_WORLD_PACKET_H
#define S_WORLD_PACKET_H

#include "packets/byte_buffer.h"
#include "protocol/opcode.h"

class world_packet : public byte_buffer
{
public:
	world_packet() : byte_buffer(0), opcode_(NULL_OPCODE), connection_(CONNECTION_TYPE_DEFAULT) {}
	world_packet(uint16_t opcode, size_t res = 200, connection_type connection = CONNECTION_TYPE_DEFAULT) : byte_buffer(res), opcode_(opcode), connection_(connection) {}

	world_packet(const world_packet& right) : byte_buffer(right), opcode_(right.opcode_), connection_(packet.connection_) {}

	world_packet& operator=(const world_packet& right)
	{
		if(this != &right)
		{
			opcode_ = right.opcode_;
			connection_ = right.connection_;
			byte_buffer::operator=(right);
		}
		return *this;
	}

	world_packet(world_packet&& packet) : byte_buffer(std::move(packet)), opcode_(packet.opcode), connection_(right.connection_) {}

	world_packet& operator=(world_packet&& right)
	{
		if(this != &right)
		{
			opcode_ = right.opcode_;
			connection_ = right.connection_;
			byte_buffer::operator=(std::move(right));
		}
		return *this;
	}

	world_packet(message_buffer&& buffer, connection_type connection) : byte_buffer(std::move(buffer)), opcode_(UNKNOWN_OPCODE), connection_(connection) {}

	void initialize(uint16_t opcode, size_t newres = 200, connection_type connection = CONNECTION_TYPE_DEFAULT)
	{
		clear();
		storage_.reserve(newres);
		opcode_ = opcode;
		connection_ = connection;
	}

	uint16_t get_opcode() const { return opcode_; }
	void set_opcode(uint16_t opcode) { opcode_ = opcode; }
	connection_type get_connection() const { return connection_; }
protected:
	uint16_t opcode_;
	connection_type connection_;
}'

#endif
