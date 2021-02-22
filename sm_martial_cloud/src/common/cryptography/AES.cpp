/*
 * Copyright (C) 2020
 */

#include "AES.h"

AES::AES(bool encrypting) : ctx_(EVP_CIPHER_CTX_new()), encrypting_(encrypting)
{
	EVP_CIPHER_CTX_init(ctx_);
	EVP_CipherInit_ex(ctx_, EVP_aes_128_gcm(), nullptr, nullptr, nullptr, encrypting_ ? 1 : 0);
}

AES::~AES()
{
	EVP_CIPHER_CTX_free(ctx_);
}

void AES::init(const uint8_t* key)
{
	EVP_CipherInit_ex(ctx_, nullptr, nullptr, key, nullptr, -1);
}

bool AES::process(const uint8_t* iv, uint8_t* data, size_t len, uint8_t(&tag)[12])
{
	if(!EVP_CipherInit_ex(ctx_, nullptr, nullptr, nullptr, iv, -1))
		return false;

	int outlen;
	if(!EVP_CipherUpdate(ctx_, data, &outlen, data, len))
		return false;

	if(!encrypting_ && !EVP_CIPHER_CTX_ctrl(ctx_, EVP_CTRL_GCM_SET_TAG, sizeof(tag), tag))
		return false;
	return true;
}
