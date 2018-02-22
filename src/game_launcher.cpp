/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "game_launcher.h"
#include "log.h"

static lg::Log_domain log_config("config");
#define ERR_CONFIG LOG_STREAM(lg::err, log_config)
#define WRN_CONFIG LOG_STREAM(lg::warn, log_config)
#define LOG_CONFIG LOG_STREAM(lg::info, log_config)

#define LOG_GENERAL LOG_STREAM(lg::info, lg::general())
#define WRN_GENERAL LOG_STREAM(lg::warn, lg::general())
#define DBG_GENERAL LOG_STREAM(lg::debug, lg::genernal())

Game_launcher::Game_launcher(const Commandline_options& cmdline_opts, 
		const char* appname) : cmdline_opts_(cmdline_opts), 
		video_(new Video()), font_manager_()
//, prefs_manager_(), image_manager_(), main_event_context_(), hotkey_manager_(),music_thinker_(), music_muter_(), test_scenario_("test"),screenshot_map_(), screenshot_filename_(), state_(), play_replay(false), multiplayer_server_(), jummp_to_multiplayer_(false), jump_to_campaign_(false, -1, "", ""),load_data_()
{
	bool no_music = false;
	bool no_soundl = false;
}
