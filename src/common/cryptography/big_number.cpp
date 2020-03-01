/*
 * Copyright (C) 2020
 */

#include "big_number.h"

#include <openssl/bn.h>
#include <cstring>
#include <algorithm>
#include <memory>

big_number::big_number()
    : bn_(BN_new())
{ }

big_number::big_number(big_number const& bn)
    : bn_(BN_dup(bn.bn_))
{ }

big_number::big_number(uint32_t val)
    : bn_(BN_new())
{
    BN_set_word(bn_, val);
}

big_number::~big_number()
{
    BN_free(bn_);
}

void big_number::set_dword(uint32_t val)
{
    BN_set_word(bn_, val);
}

void big_number::set_qword(uint64_t val)
{
    BN_set_word(bn_, (uint32_t)(val >> 32));
    BN_lshift(bn_, bn_, 32);
    BN_add_word(bn_, (uint32_t)(val & 0xFFFFFFFF));
}

void big_number::set_binary(uint8_t const* bytes, int32_t len)
{
    uint8_t* array = new uint8_t[len];

    for (int i = 0; i < len; i++)
        array[i] = bytes[len - 1 - i];

    BN_bin2bn(array, len, bn_);

    delete[] array;
}

void big_number::set_hex_str(char const* str)
{
    BN_hex2bn(&bn_, str);
}

void big_number::set_rand(int32_t numbits)
{
    BN_rand(bn_, numbits, 0, 1);
}

big_number& big_number::operator=(big_number const& bn)
{
    if (this == &bn)
        return *this;

    BN_copy(bn_, bn.bn_);
    return *this;
}

big_number big_number::operator+=(big_number const& bn)
{
    BN_add(bn_, bn_, bn.bn_);
    return *this;
}

big_number big_number::operator-=(big_number const& bn)
{
    BN_sub(bn_, bn_, bn.bn_);
    return *this;
}

big_number big_number::operator*=(big_number const& bn)
{
    BN_CTX *bnctx;

    bnctx = BN_CTX_new();
    BN_mul(bn_, bn_, bn.bn_, bnctx);
    BN_CTX_free(bnctx);

    return *this;
}

big_number big_number::operator/=(big_number const& bn)
{
    BN_CTX *bnctx;

    bnctx = BN_CTX_new();
    BN_div(bn_, NULL, bn_, bn.bn_, bnctx);
    BN_CTX_free(bnctx);

    return *this;
}

big_number big_number::operator%=(big_number const& bn)
{
    BN_CTX *bnctx;

    bnctx = BN_CTX_new();
    BN_mod(bn_, bn_, bn.bn_, bnctx);
    BN_CTX_free(bnctx);

    return *this;
}

big_number big_number::exp(big_number const& bn)
{
    big_number ret;
    BN_CTX *bnctx;

    bnctx = BN_CTX_new();
    BN_exp(ret.bn_, bn_, bn.bn_, bnctx);
    BN_CTX_free(bnctx);

    return ret;
}

big_number big_number::mod_exp(big_number const& bn1, big_number const& bn2)
{
    big_number ret;
    BN_CTX *bnctx;

    bnctx = BN_CTX_new();
    BN_mod_exp(ret.bn_, bn_, bn1.bn_, bn2.bn_, bnctx);
    BN_CTX_free(bnctx);

    return ret;
}

int32_t big_number::get_num_bytes(void)
{
    return BN_num_bytes(bn_);
}

uint32_t big_number::as_dword()
{
    return (uint32_t)BN_get_word(bn_);
}

bool big_number::is_zero() const
{
    return BN_is_zero(bn_);
}

bool big_number::is_negative() const
{
    return BN_is_negative(bn_);
}

std::unique_ptr<uint8_t[]> big_number::as_byte_array(int32_t minSize, bool littleEndian)
{
    int numBytes = get_num_bytes();
    int length = (minSize >= numBytes) ? minSize : numBytes;

    uint8_t* array = new uint8_t[length];

    // If we need more bytes than length of big_number set the rest to 0
    if (length > numBytes)
        memset((void*)array, 0, length);

    BN_bn2bin(bn_, (unsigned char *)array);

    // openssl's bn stores data internally in big endian format, reverse if little endian desired
    if (littleEndian)
        std::reverse(array, array + numBytes);

    std::unique_ptr<uint8_t[]> ret(array);
    return ret;
}

std::string big_number::as_hex_str() const
{
    char* ch = BN_bn2hex(bn_);
    std::string ret = ch;
    OPENSSL_free(ch);
    return ret;
}

std::string big_number::as_dec_str() const
{
    char* ch = BN_bn2dec(bn_);
    std::string ret = ch;
    OPENSSL_free(ch);
    return ret;
}
