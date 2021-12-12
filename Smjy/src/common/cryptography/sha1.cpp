/*
 * Copyright (C) 2020
 */

#include <cstring>
#include <stdarg.h>

#include "sha1.h"
#include "big_number.h"
#include "util.h"

sha1_hash::sha1_hash()
{
	SHA1_Init(&mc_);
	memset(digest, 0, SHA_DIGEST_LENGTH * sizeof(uint8_t));
}

sha1_hash::~sha1_hash()
{
	SHA1_Init(&mc_);
}

void sha1_hash::update_data(const uint8_t* data, int len)
{
	SHA1_Update(&mc_, data, len);
}

void sha1_hash::update_data(const std::string& str)
{
	update_data((const uint8_t*)str.c_str(), str.size());
}

void sha1_hash::update_big_numbers(big_number* bn0, ...)
{
	va_list va;
	big_number* bn;

	va_start(va, bn0);
	bn = bn0;

	while(bn)
	{
		update_data(bn->as_byte_array().get(), bn->get_num_bytes());
		bn = va_arg(va, big_number*);
	}
	va_end(va);
}

void sha1_hash::initialize()
{
	SHA1_Init(&mc_);
}

void sha1_hash::finalize()
{
	SHA1_Final(digest, &mc_);
}

std::string calculate_sha1_hash(const std::string& content)
{
	unsigned char digest[SHA_DIGEST_LENGTH];
	SHA1((unsigned char*)content.c_str(), content.size(), (unsigned char*)&digest);

	return byte_array_to_hex_str(digest, SHA_DIGEST_LENGTH);
}
