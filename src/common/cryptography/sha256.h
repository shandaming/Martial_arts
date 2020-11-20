/*
 * Copyright (C) 2020
 */

#ifndef _SHA256_H
#define _SHA256_H

#include <string>
#include <type_traits>
#include <openssl/sha.h>

class big_number;

class sha256_hash
{
public:
	typedef std::integral_constant<uint32_t, SHA256_DIGEST_LENGTH> digest_length;

	sha256_hash();
	~sha256_hash();

	void update_big_numberrs(big_number* bn0, ...);

	void update_data(const uint8_t* data, size_t len);
	void update_data(const std::string& str);

	void initialize();
	void finalize();

	uint8_t* get_digest() { return digest_; }
	uint32_t get_length() const { return SHA256_DIGEST_LENGTH; }
private:
	SHA256_CTX mc_;
	uint8_t digest_[SHA256_DIGEST_LENGTH];
};

#endif
