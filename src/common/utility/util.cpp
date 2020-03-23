/*
 * Copyright (C) 2019
 */

#include <sys/syscall.h> 

#include <cstring>

#include "util.h"

tokenizer::tokenizer(const std::string &src, const char sep, uint32_t vectorReserve /*= 0*/, bool keep_empty_strings /*= true*/)
{
    str_ = new char[src.length() + 1];
    memcpy(str_, src.c_str(), src.length() + 1);

    if (vectorReserve)
        storage_.reserve(vectorReserve);

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

int this_thread_id()
{
	static int thread_local thread_id = 0;
	if(thread_id == 0)
		thread_id = syscall(SYS_gettid);
	return thread_id;
}

