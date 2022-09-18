/*
 * Copyright (C) 2020
 */

#include "world.h"

world::world() : is_closed_(false)
{}

world::~world()
{}

bool world::is_closed() const { return is_closed_; }
void world::set_closed(bool val)
{
	is_closed_ = val;
}

world* world::instance()
{
	static world instance;
	return &instance;
}
