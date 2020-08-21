/*
 * Copyright (C) 2020
 */

#ifndef _AUTHENTICATION_PACKET_H
#define _AUTHENTICATION_PACKET_H

#include "packet.h"

namespace world_packets
{
namespace auth
{
struct early_process_client_packet : public client_packet
{
	early_process_client_packet(opcode_client opcode, world_packet&& packet) :
		client_packet(opcode, std::move(packet)) {}

	bool read_no_throw();
};

class ping final : public early_process_client_packet
{
public:
	ping(world_packet&& packet) : early_process_client_packet(CMSG_PING, std::move(packet)) {}

	uint32_t serial = 0;
	uint32_t latency  = 0;
private:
	void read();
};

struct pong final : public server_packet
{
	pong(uint32_t serial_) : server_packet(SMSG_PONG, 4), serial(serial_) {}

	const world_packet* write() override;

	uint32_t serial = 0;
};

struct auth_challenge final : public server_packet
{
	auth_challenge() : server_packet(SMSG_AUTH_CHALLENGE, 16 + 4 * 8 + 1) {}

	const world_packet* write() override;

	std::array<uint8_t, 16> challenge = {};
	std::array<uint32_t, 8> dos_challenge = {};
	uint8_t dos_zero_bits = 0;
};

class auth_session final : public early_process_client_packet
{
public:
	static const uint32_t digest_length = 24;

	auth_session(world_packet&& packet) : early_process_client_packet(CMSG_AUTH_SESSION, std::move(packet))
	{
		local_challenge.fill(0);
		digest.fill(0);
	}

	uint32_t region_id = 0;
	uint32_t battlegroup_id = 0;
	uint32_t realm_id = 0;
	std::array<uint8_t, 16> local_challenge;
	std::array<uint8_t, digest_length> digest;
	uint64_t dos_response = 0;
	std::string realm_join_ticket;
	bool use_ipv6 = false;
private:
	void read() override;
};
}
}

#endif
