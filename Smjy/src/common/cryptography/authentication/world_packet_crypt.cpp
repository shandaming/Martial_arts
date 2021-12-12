/*
 * Copyright (C) 2020
 */

#include <array>
#include <cstring>

#include "world_packet_crypt.h"

world_packet_crypt::world_packet_crypt() : client_decrypt_(false), server_encrypt_(true), client_counter_(0), service_counter_(0), initialized_(false) {}

void world_packet_crypt::init(const uint8_t* key)
{
	client_decrypt_.init(key);
	server_encrypt_.init(key);
	initialized_ = true;
}

struct world_packet_crypt_iv
{
	world_packet_crypt_iv(uint64_t counter, uint32_t magic)
	{
		memcpy(value.data(), &counter, sizeof(uint64));
		memcpy(value.data() + sizeof(uint64_t), &magic, sizeof(uint32_t));
	}

	std::array<uint8_t, 12> value;
};

bool world_packet_crypt::decrypt_recv(uint8_t* data, size_t len, uint8_t(&tag)[12])
{
	if(initialized_)
	{
		world_packet_crypt_iv iv{client_counter_, 0x544E4C43};
		if(!client_decrypt_.process(iv.value.data(), data, len, tag))
			return false;
	}
	else
		memset(tag, 0, sizeof(tag));

	++client_counter_;
	return true;
}

bool world_packet_crypt::encrypt_send(uint8_t* data, size_t len, uint8_t(&tag)[12])
{
	if(initialized_)
	{
		world_packet_crypt_iv iv{server_counter_, 0x52565253};
		if(!server_encrypt_.process(iv.value.data(), data, len, tag))
			return false;
	}
	else
		memset(tag, 0, sizeof(tag));

	++server_counter_;
	return true;
}
