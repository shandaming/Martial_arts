/*
 * Copyright (C) 2020
 */

#ifndef H_MAC_HASH_H
#define H_MAC_HASH_H

#include <string>
#include <openssl/hmac.h>
#include <openssl/sha.h>

class big_number;

#define SEED_KEY_SIZE 16

typedef const EVP_MD* (*HashCreateFn)();

template<HashCreateFn hash_creator, uint32_t digest_length>
class hmac_hash
{
public:
	hmac_hash(uint32_t len, const uint8_t* seed);
	~hmac_hash();
	void update_data(const std::string& str);
	void update_data(const uint8_t* data, size_t len);
	void finalize();
	uint8_t* compute_hash(big_number* bn);
	uint8_t* get_digest() const { return digest_; }
	uint32_t get_length() const { return digest_length; }
private:
	HMAC_CTX* ctx_;
	uint8_t digest_[digest_length];
};

typedef hmac_hash<EVP_sha1, SHA_DIGEST_LENGTH> hmac_sha1;
typedef hmac_hash<EVP_sha256, SHA256_DIGEST_LENGTH> hmac_sha256;

#endif
