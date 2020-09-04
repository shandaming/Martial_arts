/*
 * Copyright (C) 2020
 */

#ifndef _AUTHENTICATION_PACKET_H
#define _AUTHENTICATION_PACKET_H

#include "packet.h"
#include "opcodes.h"

struct character_template;
struct race_class_availability;

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

struct auth_wait_info
{
	uint32_t wait_count = 0;
	uint32_t wait_time = 0;
	bool has_fcm = false;
};

struct virtual_realm_name_info
{
	virtual_realm_name_info() : is_local(false), is_internal_realm(false) {}
	virtual_realm_name_info(bool is_home_realm, bool is_internal_realm_, const std::string realm_name_actual_, const std::string realm_name_normalized_) : is_local(is_home_realm), is_internal_realm(is_internal_realm_), realm_name_actual(realm_name_actual_), realm_name_normalized(realm_name_normalized_) {}

	bool is_local;
	bool is_internal_realm;
	std::string realm_name_actual;
	std::string realm_name_normalized;
};

struct virtual_realm_info
{
	virtual_realm_info(uint32_t realm_address_, bool is_home_realm, bool is_internal_realm, const std::string& realm_name_actual, const std::string& realm_name_normalized) : realm_address(realm_address_), realm_name_info(is_home_realm, is_internal_realm, realm_name_actual, realm_name_normalized) {}

	uint32_t realm_address;
	virtual_realm_name_info realm_name_info;
};

struct auth_response final : public server_packet
{
	auth_response() : server_packet(SMSG_AUTH_RESPONSE, 132) {}

	const world_packet* write() override;

	struct auth_success_info
	{
		struct billing_info
		{
			uint32_t billing_plan = 0;
			uint32_t time_remain = 0;
			uint32_t unknown735 = 0;
			bool in_game_room = false;
		};

		uint8_t active_expansion_level = 0;
		uint8_t account_expansion_level = 0;
		uint32_t time_rested = 0;

		uint32_t virtual_realm_address = 0;
		uint32_t time_seconds_until_pckick = 0;
		uint32_t currency_id = 0;
		int32_t time = 0;

		billing_info billing;

		std::vector<virtual_realm_info> virtual_realms;
		std::vector<const character_template*> templates;
		const std::vector<race_class_availability> * available_classes = nullptr;

		bool is_expansion_trial = false;
		bool force_character_template = false;
		std::optional<uint16_t> num_players_horde;
		std::optional<uint16_t> num_players_alliance;
		std::optional<int32_t> expansion_trial_expiration;
	};

	std::optional<auth_success_info> success_info;
	std::optional<auth_wait_info> wait_info;
	uint32_t result = 0;
};

struct wait_queue_update final : public server_packet
{
	wait_queue_update() : server_packet(SMSG_WAIT_QUEUE_UPDATE, 4 + 4 - 1) {}

	const world_packet* write() override;

	auth_wait_info wait_info;
};

struct wait_queue_finish final : public server_packet
{
	wait_queue_finish() : server_packet(SMSG_WAIT_QUEUE_FINISH, 0) {}

	const world_packet* write() override { return &world_packet_; }
};

enum class connect_to_serial : uint32_t
{
	none = 0,
	realm = 14,
	world_attempt1 = 17,
	world_attempt2 = 35,
	world_attempt3 = 53,
	world_attempt4 = 71,
	world_attempt5 = 89
};

struct connect_to final : public server_packet
{
	connect_to();

	static bool initialize_encryption();

	enum address_type : uint8_t
	{
		ipv4 = 1,
		ipv6 = 2,
		named_socket = 3
	};

	struct socket_address
	{
		union
		{
			std::array<uint8_t, 4> v4;
			std::array<uint8_t, 16> v6;
			std::array<char, 128> name;
		} address;
		address_type type;
	};

	struct connect_pay_load
	{
		socket_address where;
		uint16_t port;
		std::array<uint8_t, 256> signature;
	};

	const world_packet* write() override;

	uint64_t key = 0;
	connect_to_serial serial = connect_to_serial::none;
	connect_pay_load pal_load;
	uint8_t con = 0;
};

class auth_continued_session final : public early_process_client_packet
{
public:
	static const uint32_t digest_length = 24;

	auth_continued_session(world_packet&& packet) : early_process_client_packet(CMSG_AUTH_CONTINUED_SESSION, std::move(packet))
	{
		local_challenge.fill(0);
		digest.fill(0);
	}

	uint64_t dos_response = 0;
	uint64_t key = 0;
	std::array<uint8_t, 16> local_challenge;
	std::array<uint8_t, digest_length> digest;
private:
	void read() override;
};

struct resume_comms final : public server_packet
{
	resume_comms(connection_type connection) : server_packet(SMSG_RESUME_COMMS, 0, connection) {}

	const world_packet* write() override { return &world_packet_; }
};

class connect_to_failed final : public early_process_client_packet
{
public:
	connect_to_failed(world_packet&& packet) : early_process_client_packet(CMSG_CONNECT_TO_FAILED, std::move(packet)) {}

	connect_to_serial serial = connect_to_serial::none;
	uint8_t con = 0;
private:
	void read() override;
};

struct enable_encryption final : public server_packet
{
	enable_encryption(const uint8_t* encryption_key, bool enabled_) : server_packet(SMSG_ENABLE_ENCRYPTION, 256 + 1), encryption_key(encryption_key), enabled(enabled_) {}

	const world_packet* write() override;

	const uint8_t* encryption_key;
	bool enabled = false;
};
}
}

byte_buffer& operator<<(byte_buffer& data, const world_packets::auth::virtual_realm_name_info& realm_info);

#endif
