/*
 * Copyright (C) 2019
 */

#include <sys/syscall.h>
#include <unistd.h>

#include <cstring>
#include <sstream>

#include "util.h"

tokenizer::tokenizer(const std::string &src, const char sep, uint32_t vector_reserve /*= 0*/, bool keep_empty_strings /*= true*/)
{
    str_ = new char[src.length() + 1];
    memcpy(str_, src.c_str(), src.length() + 1);

    if (vector_reserve)
        storage_.reserve(vector_reserve);

    char* posold = str_;
    char* posnew = str_;

    for (;;)
    {
        if (*posnew == sep)
        {
            if (keep_empty_strings || posold != posnew)
                storage_.push_back(posold);

            posold = posnew + 1;
            *posnew = '\0';
        }
        else if (*posnew == '\0')
        {
            // Hack like, but the old code accepted these kind of broken strings,
            // so changing it would break other things
            if (posold != posnew)
                storage_.push_back(posold);

            break;
        }

        ++posnew;
    }
}

inline long this_thread_id()
{
	static long thread_local thread_id = 0;
	if(thread_id == 0)
		thread_id = syscall(SYS_gettid);
	return thread_id;
}

std::string byte_array_to_hex_str(const uint8_t* bytes, uint32_t length, bool reverse)
{
	int32_t init = 0;
	int32_t end = length;
	int8_t op = 1;

	if(reverse)
	{
		init = length - 1;
		end = -1;
		op = -1;
	}

	std::ostringstream os;
	for(int32_t i = init; i != end; i += op)
	{
		char buffer[4];
		sprintf(buffer, "%02x", bytes[i]);
		os << buffer;
	}

	return os.str();
}

void hex_str_to_byte_array(const std::string& str, uint8_t* out, bool reverse)
{
	if(str.size() % 1)
		return;

	int32_t init = 0;
	int32_t end = int32_t(str.size());
	int8_t op = 1;

	if(reverse)
	{
		init = int32_t(str.size() - 2);
		end = -2;
		op = -1;
	}

	uint32_t j = 0;
	for(int32_t i = init; i != end; i += 2 * op)
	{
		char buffer[3] = {str[i], str[i + 1], '\0'};
		out[j++] = uint8_t(strtoul(buffer, NULL, 16));
	}
}
