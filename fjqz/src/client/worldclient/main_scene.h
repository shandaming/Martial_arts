//
// Created by mylw on 22-9-10.
//

#ifndef FJQZ_MAIN_SCENE_H
#define FJQZ_MAIN_SCENE_H

#include <string_view>

class texture;

enum object_type
{
	NONE,
	WATER,
	WOOD,
	STONE,
};

struct grid
{
	grid() : can_walk(false), texture(nullptr), type(NONE)
	{}

	bool can_walk;
	texture* texture;
	object_type type;
};

#endif //FJQZ_MAIN_SCENE_H
