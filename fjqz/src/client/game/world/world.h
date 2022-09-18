/*
 * Copyright (C) 2020
 */

#ifndef _WORLD_H
#define _WORLD_H

#include <cstdint>
#include <atomic>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>
#include <memory>

class world
{
public:
	static world* instance();


	bool is_closed() const;
	void set_closed(bool val);
private:
	world();
	~world();

	bool is_closed_;
};

#define WORLD world::instance()

#endif
