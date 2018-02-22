/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "game_config.h"

namespace game_config
{
	const std::string version = VERSION;

	const Version_info game_version(VERSION);

#ifdef MARTIAL_ARTS
	std::string path = MARTIAL_ARTS;
#else
	std::string path = "";
#endif

	std::string game_program_dir;

#ifdef REVISION
        const std::string revision = VERSION " (" REVISION ")";
#elif defined(VCS_SHORT_HASH) && defined(VCS_WC_MODIFIED)
        const std::string revision = std::string(VERSION) + " (" + VCS_SHORT_HASH +     (VCS_WC_MODIFIED ? "-Modified" : "-Clean") + ")";
#else
        const std::string revision = VERSION;
#endif
}
