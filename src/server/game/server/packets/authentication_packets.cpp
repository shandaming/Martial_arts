/*
 * Copyright (C) 2020
 */

#include "authentication_packets.h"

byte_buffer& operator<<(byte_buffer& data, const world_packets::auth::virtual_realm_name_info& virtual_realm_info)
{
	data.write_bit(virtual_realm_info.is_local);
	data.write_bit(virtual_realm_info.is_internal_realm);
	data.write_bits(virtual_realm_info.realm_name_actual.length(), 8);
	data.write_bits(virtual_realm_info.realm_name_normalized.length(), 8);
	data.flush_bits();

	data.write_string(virtual_realm_info.realm_name_actual);
	data.write_string(virtual_realm_info.realm_name_normalized);

	return data;
}

byte_buffer& operator<<(byte_buffer& data, const world_packets::auth::virtual_realm_info& virtual_realm_info)
{
	data << uint32_t(virtual_realm_info.realm_address);
	data << virtual_realm_info.realm_name_info;

	return data;
}

bool world_packets::auth::early_process_client_packet::read_no_throw()
{
	try
	{
		read();
		return true;
	}
	catch(const byte_buffer_position_exception&)
	{}

	return false;
}

void world_packets::auth::ping::read()
{
	world_packet_ >> serial;
	world_packet_ >> latency;
}

const world_packet* world_packets::auth_pong::write()
{
	world_packet_ << uint32_t(serial);
	return &world_packet_;
}

const world_packet* world_packets::auth::auth_challenge::write()
{
	world_packet_.append(dos_challenge.data(), dos_challenge.size());
	world_packet_.append(challenge.data(), challenge.size());
	world_packet_ << uint8_t(dos_zero_bits);

	return &world_packet_;
}

void world_packets::auth::auth_session::read()
{
	world_packet_ >> dos_response;
	world_packet_ >> regin_id;
	world_packet_ >> battlegroup_id;
	world_packet_ >> realm_id;
	world_packet_.read(local_challenge.data(), local_challenge.size());
	world_packet_.read(digest.data(), digest.size());
	use_ipv6 = world_packet_.read_bit();

	uint32_t realm_join_ticket_size;
	world_packet_ >> realm_join_ticket_size;

	if(realm_join_ticket_size)
	{
		realm_join_ticket.resize(std::min(realm_join_ticket_size, uint32_t(world_packet_.size() - world_packet_.rpos())));
	}
}

byte_buffer& operator<<(bute_buffer& data, const world_packets::auth::auth_wait_info& wait_info)
{
	data << uint32_t(wait_info.wait_count);
	data << uint32_t(wait_info.wait_time);
	data.write_bit(wait_info.has_fcm);
	data.flush_bits();

	return data;
}

const world_packet* world_packets::auth::auth_response::write()
{
	world_packet_ << uint32_t(result);
	world_packet_.write_bit(success_info.is_initialized());
	world_packet_.write_bit(wait_info.is_initialized());
	world_packet_.flush_bits();

	if(success_info)
	{
		world_packet_ << uint32_t(success_info->virtual_realm_address);
		world_packet_ << uint32_t(success_info->virtual_realms.size());
		world_packet_ << uint32_t(success_info->time_rested);
		world_packet_ << uint32_t(success_info->active_expansion_level);
		world_packet_ << uint32_t(success_info->account_expansion_level);
		world_packet_ << uint32_t(success_info->time_seconds_until_pc_kick);
		world_packet_ << uint32_t(success_info->available_classes->size());
		world_packet_ << uint32_t(success_info->templates.size());
		world_packet_ << uint32_t(success_info->currency_id);
		world_packet_ << uint32_t(success_info->time);

		for(const race_class_availability& race_class_availability : *success_info->available_classes)
		{
			world_packet_ << uint8_t(race_class_availability.race_id);
			world_packet_ << uint32_t(race_class_availability.classes.size());

			for(const class_availability& class_availability : race_class_availability.classes)
			{
				world_packet_ << uint8_t(class_availability.class_id);
				world_packet_ << uint8_t(class_availability.active_expansion_level);
				world_packet_ << uint8_t(class_availability.account_expansion_level);
			}
		}

		world_packet_.write_bit(success_info->is_expansion_trial);
		world_packet_.write_bit(success_info->force_character_template);
		world_packet_.write_bit(success_info->num_players_horde.is_initialized());
		world_packet_.write_bit(success_info->num_players_alliance.is_initialized());
		world_packet_.write_bit(success_info->expansion_trial_expiration.is_initialized());
		world_packet_.flush_bits();

		{
			world_packet_ << uint32_t(success_info->billing.billing_plan);
			world_packet_ << uint32_t(success_info->billing.time_remain);
			world_packet_ << uint32_t(success_info->billing.unknown735);
			world_packet_.write_bit(success_info->billing.in_game_room);
			world_packet_.write_bit(success_info->billing.in_game_room);
			world_packet_.write_bit(success_info->billing.in_game_room);
			world_packet_.flush_bits();
		}

		if(success_info->num_players_horde)
			world_packet_ << uint16_t(*success_info->num_players_horde);
		if(success_info->num_players_alliance)
			world_packet_ << uint16_t(*success_info->num_players_alliance);
		if(success_info->expansion_trial_expiration)
			world_packet_ << int32_t(*success_info->expansion_trial_expiration);

		for(const virtual_realm_info& virtual_realm : success_info->virtual_realms)
			world_packet_ << virtual_realm;

		for(const character_template* templat : success_info->templates)
		{
			world_packet_ << uint32_t(templat->template_set_id);
			world_packet_ << uint32_t(templat->classes.size());

			for(const character_template_class& template_class : templat->classes)
			{
				world_packet_ << uint8_t(template_class.class_id);
				world_packet_ << uint8_t(template_class.faction_group);
			}

			world_packet_.write_bits(templat->name.length(), 7);
			world_packet_.write_bits(templat->description.length(), 10);
			world_packet_.flush_bits();

			world_packet_.write_string(templat->name);
			world_packet_.write_string(templat->description)
		}
	}

	if(wait_info)
		world_packet_ << *wait_info;

	return &world_packet_;
}

const world_packet* world_packets::auth::wait_queue_update::write()
{
	world_packet_ << wait_info;
	return &world_packet_;
}

namespace
{
const std::string rsa_private_key = R"(-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEA7rPc1NPDtFRRzmZbyzK48PeSU8YZ8gyFL4omqXpFn2DE683q
f41Z2FeyYHsJTJtouMft7x6ADeZrN1tTkOsYEw1/Q2SD2pjmrMIwooKlxsvH+4af
n6kCagNJxTj7wMhVzMDOJZG+hc/R0TfOzIPS6jCAB3uAn51EVCIpvoba20jFqfkT
NpUjdvEO3IQNlAISqJfzOxTuqm+YBSdOH6Ngpana2BffM8viE1SLGLDKubuIZAbf
dabXYQC7sFoOetR3CE0V4hCDsASqnot3qQaJXQhdD7gua8HLZM9uXNtPWGUIUfsN
SBpvtj0fC93+Gx3wv7Ana/WOvMdAAf+nC4DWXwIDAQABAoIBACKa5q/gB2Y0Nyvi
APrDXrZoXclRVd+WWxSaRaKaPE+vuryovI9DUbwgcpa0H5QAj70CFwdsd4oMVozO
6519x56zfTiq8MaXFhIDkQNuR1Q7pMFdMfT2jogJ8/7olO7M3EtzxC8EIwfJKhTX
r15M2h3jbBwplmsNZKOB1GVvrXjOm1KtOZ4CTTM0WrPaLVDT9ax8pykjmFw16vGP
j/R5Dky9VpabtfZOu/AEW259XDEiQgTrB4Eg+S4GJjHqAzPZBmMy/xhlDK4oMXef
qXScfD4w0RxuuCFr6lxLPZz0S35BK1kIWmIkuv+9eQuI4Hr1CyVwch4fkfvrp84x
8tvAFnkCgYEA87NZaG9a8/Mob6GgY4BVLHJVOSzzFdNyMA+4LfSbtzgON2RSZyeD
0JpDowwXssw5XOyUUctj2cLLdlMCpDfdzk4F/PEakloDJWpason3lmur0/5Oq3T9
3+fnNUl4d3UOs1jcJ1yGQ/BfrTyRTcEoZx8Mu9mJ4ituVkKuLeG5vX0CgYEA+r/w
QBJS6kDyQPj1k/SMClUhWhyADwDod03hHTQHc9BleJyjXmVy+/pWhN7aELhjgLbf
o/Gm3aKJjCxS4qBmqUKwAvGoSVux1Bo2ZjcfF7sX9BXBOlFTG+bPVCZUoaksTyXN
g7GsA1frKkWWkgQuOeK3o/p9IZoBl93vEgcTGgsCgYEAv5ucCIjFMllUybCCsrkM
Ps4GQ9YbqmV9ulwhq8BPTlc8lkDCqWhgM3uXAnNXjrUTxQQd+dG4yFZoMrhBs2xZ
cQPXoXDQO5GaN6jPduETUamGiD/DCvwJQCrNlxAVL5dR36FWN3x/9JriHwsoE8Jz
SeEX2frIdpM/RYNX/6sipuECgYEA+rwFRDxOdvm8hGWuQ2WMxyQ7Nn07PEV/LxVM
HkSRkyh23vVakyDEqty3uSOSUJfgv6ud07TnU8ac3fLQatdT8LrDgB4fVkN/fYU8
kldaGwO1vxgl4OfDQCo7dXzisciViwtVBvQZ+jnm6J0vJBFUHAPt9+WZTIlQQIjm
71LtseMCgYBSAhs6lshtz+ujR3fmc4QqJVGqeXvEBPAVm6yYoKYRLwVs/rFv3WLN
LOwwBQ6lz7P9RqYYB5wVlaRvEhb9+lCve/xVcxMeZ5GkOBPxVygYV9l/wNdE25Nz
OHYtKG3GK3GEcFDwZU2LPHq21EroUAdtRfbrJ4KW2yc8igtXKxTBYw==
-----END RSA PRIVATE KEY-----
)";

std::unique_ptr<crypto::rsa> connect_to_rsa;
}

bool world_packets::auth::connect_to::initialize_encryption()
{
	std::unique_ptr<crypto::rsa> rsa = make_unique<crypto::rsa>();
	if(!rsa->load_from_string(rsa_private_key, crypto::rsa::private_key{}))
		return false;

	connect_to_rsa = std::move(rsa);
	return true;
}

world_packets::auth::connect_to() : server_packet(SMSG_CONNECT_TO, 256 + 1 + 16 + 2 + 4 + 1 + 8) {}

const world_packet* world_packets::auth::connect_to::write()
{
	byte_buffer where_buffer;
	where_buffer << uint8_t(payload.where.type);
	switch(payload.where.type)
	{
		case ipv4:
			where_buffer.append(payload.where.address.v4.data(), payload.where.address.v4.size());
			break;
		case ipv6:
			where_buffer.append(payload.where.address.v6.data(), payload.where.address.v6.size());
			break;
		case named_socket:
			where_buffer << payload.where.address.name.data();
			break;
		default:
			break;
	}

	uint32_t type = payload.where.type;
	sha256_hash hash;
	hash.update_data(where_buffer.contents(), where_buffer.size());
	hash.update_data(reinterpret_cast<const uint8_t*>(&type), 4);
	hash.update_data(reinterpret_cast<const uint8_t*>(&payload.port), 2);
	hash.finalize();

	connect_to_rsa->sign(hash.get_digest(), hash.get_length(), payload.signature.data(), crypto::rsa::sha256{});

	world_packet_.append(payload.signature.data(), payload.signature.size());
	world_packet_.append(where_buffer);
	world_packet_ << uint16_t(payload.port);
	world_packet_ << uint32_t(serial);
	world_packet_ << uint8_t(con);
	world_packet_ << uint64_t(key);

	return &world_packet_;
}

void world_packets::auth::auth_continued_session::read()
{
	world_packet_ >> dos_response;
	world_packet_ >> key;
	world_packet_.read(local_challenge.data(), local_challenge.size());
	world_packet_.read(digest.data(), digest.size());
}

void world_packets::auth::connect_to_failed::read()
{
	serial = world_packet_.read<connect_to_serial>();
	world_packet_ >> con;
}

constexpr uint8_t enable_encryption_seed[16] = {0x90, 0x9C, 0xD0, 0x50, 0x5A, 0x2C, 0x14, 0xDD, 0x5C, 0x2C, 0xC0, 0x64, 0x14, 0xF3, 0xFE, 0xC9};

const world_packet* world_packets::auth::enable_encryption::write()
{
	hmac_sha256 hash(16, encryption_key);
	hash.update_data(reinterpret_cast<const uint8_t*>(&enabled), 1);
	hash.update_data(enable_encryption_seed, 16);
	hash.finalize();

	world_packet_.resize(world_packet_.size() + connect_to_rsa->get_output_size());

	connect_to_rsa->sign(hash.get_digest(), hash.get_length(), world_packet_.contents(), crypto::rsa::sha256{});

	world_packet_.write_bit(enabled);
	world_packet_.flush_bits();

	return &world_packet_;
}
