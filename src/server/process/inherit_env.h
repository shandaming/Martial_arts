/*
 * Copyright (C) 2019
 */

#ifndef PRO_INHERIT_ENV_H
#define PRO_INHERIT_ENV_H

#include <unistd.h>

#include "initializer.h"

struct inherit_env : public initializer_base
{
	template<typename T>
	void on_fork_setup(T& e) const
	{
		e.env = environ;
	}
};

#endif
