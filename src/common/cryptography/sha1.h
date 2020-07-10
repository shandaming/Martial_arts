/*
 * Copyright (C) 2020
 */

#ifndef _CRY_SHA1_H
#define _CRY_SHA1_H

#include <type_traits>
#include <string>

#include <openssl/sha.h>

class big_number;

class sha1_hash
{
public:
	typedef std::integral_constant<uint32_t, SHA_DIGEST_LENGTH> digest_length;

	sha1_hash();
	~sha1_hash();

	void update_big_numbers(big_number* bn0, ...);

	void update_data(const uint8_t* data, int len);
	void update_data(const std::string& str);

	void initialize();
	void finalize();

	uint8_t* get_digest() { return digest_; }
	int get_length() const { return SHA_DIGEST_LENGTH; }
private:
	SHA_CTX mc_;
	uint8_t digest_[SHA_DIGEST_LENGTH];
};

std::string calculate_sha1_hash(const std::string& content);

#endif
