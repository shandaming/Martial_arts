/*
 * Copyright (C) 2020
 */

#ifndef CRY_WORLD_PACKET_CRYPT_H
#define CRY_WORLD_PACKET_CRYPT_H

#include "AES.h"

class world_packet_crypt
{
public:
	world_packet_crypt();

	void init(const uint8_t key);
	bool decrypt_recv(uint8_t* data, size_t len, uint8_t(&tag)[12]);
	bool encrypt_send(uint8_t* data, size_t len, uint8_t(&tag)[12]);

	bool is_initialized() const { return initialized_; }
private:
	uint64_t client_counter_;
	uint64_t server_counter_;
	bool initialized_;
	AES client_descrypt_;
	AES server_encrypt_;
};

#endif
