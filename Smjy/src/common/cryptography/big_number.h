/*
 * Copyright (C) 2020
 */

#ifndef CRY_BIG_NUMBER_H
#define CRY_BIG_NUMBER_H

#include <memory>
#include <string>

struct bignum_st;

class big_number
{
    public:
        big_number();
        big_number(big_number const& bn);
        big_number(uint32_t);
        ~big_number();

        void set_dword(uint32_t);
        void set_qword(uint64_t);
        void set_binary(uint8_t const* bytes, int32_t len);
        void set_hex_str(char const* str);

        void set_rand(int32_t numbits);

        big_number& operator=(big_number const& bn);

        big_number operator+=(big_number const& bn);
        big_number operator+(big_number const& bn)
        {
            big_number t(*this);
            return t += bn;
        }

        big_number operator-=(big_number const& bn);
        big_number operator-(big_number const& bn)
        {
            big_number t(*this);
            return t -= bn;
        }

        big_number operator*=(big_number const& bn);
        big_number operator*(big_number const& bn)
        {
            big_number t(*this);
            return t *= bn;
        }

        big_number operator/=(big_number const& bn);
        big_number operator/(big_number const& bn)
        {
            big_number t(*this);
            return t /= bn;
        }

        big_number operator%=(big_number const& bn);
        big_number operator%(big_number const& bn)
        {
            big_number t(*this);
            return t %= bn;
        }

        bool is_zero() const;
        bool is_negative() const;

        big_number mod_exp(big_number const& bn1, big_number const& bn2);
        big_number exp(big_number const&);

        int32_t get_num_bytes(void);

        struct bignum_st *bn() { return bn_; }

        uint32_t as_dword();

        std::unique_ptr<uint8_t[]> as_byte_array(int32_t minSize = 0, bool littleEndian = true);

        std::string as_hex_str() const;
        std::string as_dec_str() const;

    private:
        struct bignum_st* bn_;

};

#endif
