/*
 * Copyright (C) 2020
 */

#include <cstring>

#include "big_number.h"
#include "errors.h"
#include "hmac_hash.h"

#if defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER < 0x10100000L
HMAC_CTX* HMAC_CTX_new()
{
	HMAC_CTX* ctx = new HMAC_CTX();
	HMAC_CTX_init(ctx);
	return ctx;
}

void HMAC_CTX_free(HMAC_CTX* ctx)
{
	HMAC_CTX_cleanum(ctx);
	delete ctx;
}
#endif

template<HashCreateFn hash_creator, uint32_t digest_length>
hmac_hash<HashCreator, digest_length>::hmac_hash(uint32_t len, const uint8_t* seed): ctx_(HMAC_CTX_new())
{
	HMAC_Init_ex(ctx_, seed, len, hash_creator(), nullptr);
	memset(digest_, 0, digest_length);
}

template<HashCreateFn hash_creator, uint32_t digest_length>
hmac_hash<HashCreator, digest_length>::~hmac_hash()
{
	HMAC_CTX_free(ctx_);
}

template<HashCreateFn hash_creator, uint32_t digest_length>
void hmac_hash<HashCreator, digest_length>::update_data(const std::string& str)
{
	HMAC_Update(ctx_, reinterpret_cast<const uint8_t*>(str.c_str()), str.size());
}


template<HashCreateFn hash_creator, uint32_t digest_length>
void hmac_hash<HashCreator, digest_length>::update_data(const uint8_t* data, size_t len)
{
	HMAC_Update(ctx_, data, len);
}

template<HashCreateFn hash_creator, uint32_t digest_length>
void hmac_hash<HashCreator, digest_length>::finalize()
{
	uint32_t length = 0;
	HMAC_Final(ctx_, digest_, &length);
	ASSERT(length == digest_length);
}

template<HashCreateFn hash_creator, uint32_t digest_length>
uint8_t* hmac_hash<HashCreator, digest_length>::compute_hash(big_number* bn)
{
	HMAC_Update(ctx_, bn->as_byte_array().get(), bn->get_num_bytes());
	finalize();
	return digest_;
}

template class hmac_hash<EVP_sha1, SHA_DIGEST_LENGTH>;
template class hmac_hash<EVP_sha256, SHA256_DIGEST_LENGTH>;
