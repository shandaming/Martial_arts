/*
 * Copyright (C) 2020
 */

#include "world.h"

world::world()
{}

world::~world()
{}

world* world::instance()
{
	static world instance;
	return &instance;
}
