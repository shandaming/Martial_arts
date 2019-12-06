/*
 * Copyright (C) 2019
 */

#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>

class tokenizer
{
public:
    typedef std::vector<char const*> storage_type;

    typedef storage_type::size_type size_type;

    typedef storage_type::const_iterator const_iterator;
    typedef storage_type::reference reference;
    typedef storage_type::const_reference const_reference;

public:
    tokenizer(const std::string &src, char const sep, uint32_t vectorReserve = 0, bool keep_empty_strings = true);
    ~tokenizer() { delete[] str_; }

    const_iterator begin() const { return storage_.begin(); }
    const_iterator end() const { return storage_.end(); }

    size_type size() const { return storage_.size(); }

    reference operator [] (size_type i) { return storage_[i]; }
    const_reference operator [] (size_type i) const { return storage_[i]; }

private:
    char* str_;
    storage_type storage_;
};

#endif
