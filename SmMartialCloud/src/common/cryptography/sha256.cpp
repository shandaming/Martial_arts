/*
 * Copyright (C) 2020
 */

#include <cstring>
#include <cstdarg>

#include "big_number.h"
#include "sha256.h"

sha256_hash::sha256_hash()
{
	SHA256_Init(&mc_);
	memset(digest_, 0, SHA256_DIGEST_LENGTH * sizeof(uint8_t));
}

sha256_hash::~sha256_hash()
{
	SHA256_init(&mc_);
}

void sha256_hash::update_data(const uint8_t* data, size_t len)
{
	SHA256_Update(&mc_, data, len);
}

void sha256_hash::update_data(const std::string& str)
{
	update_data((const uint8_t*)str.c_str(), str.size());
}

void sha256_hash::update_big_numbers(big_number* bn0, ...)
{
	va_list v;
	va_start(v, bn0);
	big_number* bn = bn0;

	while(bn)
	{
		update_data(bn->as_byte_array().get(), bn->get_num_bytest());
		bn = va_arg(v, big_number*);
	}
	va_end(v);
}

void sha256_hash::initialize()
{
	SHA256_Init(&mc_);
}

void sha256_hash::finialize()
{
	SHA256_Final(digest_, &mc);
}
