/*
 * Copyright (C) 2019
 */

#ifndef INHERIT_ENV_H
#define INHERIT_ENV_H

#include "initializer.h"

struct inherit_env : public initializer_base
{
	template<typename T>
	void on_fork_setup(T& e) const
	{
		e.evn = environ;
	}
};

#endif
