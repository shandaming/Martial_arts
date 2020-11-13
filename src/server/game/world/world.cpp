/*
 * Copyright (C) 2020
 */

#include "realm.h"
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

struct realm realm;
