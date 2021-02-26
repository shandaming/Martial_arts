/*
 * Copyrhgt (C) 2020
 */

#ifndef CRY_AES_H
#define CRY_AES_H

#include <openssl/evp.h>

class AES
{
public:
	AES(bool encrypting);
	~AES();

	void init(const uint8_t* key);
	bool process(const uint8_t* iv, uint8_t* data, size_t len, uint8_t(&tag)[12]);
private:
	EVP_CIPHER_CTX* ctx_;
	bool encrypting_;
};

#endif
