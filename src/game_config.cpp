/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "game_config.h"

namespace game_config
{
	constexpr std::string version = VERSION;

#ifdef MARTIAL_ARTS
	std::string path = MARTIAL_ARTS;
#else
	std::string path = "";
#endif

	std::string game_program_dir;
}
