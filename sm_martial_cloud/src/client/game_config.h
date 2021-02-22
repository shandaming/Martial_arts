/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include <string>
#include <vector>
#include "version.h"

#define VERSION "1.13.10+dev"

namespace game_config
{
	extern const std::string version;

	extern std::string path;

	extern std::string game_program_dir;

	extern const Version_info game_version;

        extern const std::string revision;
}

#endif
