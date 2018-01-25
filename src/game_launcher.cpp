/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "game_launcher.h"
#include "log.h"

static log::Log_domain log_config("config");
using err_config = log_stream(err, log_config);
using wrn_config = log_stream(warn, log_config);
using log_config = log_stream(info, log_config);

using log_general = log_stream(info, log::general());
using wrn_general = log_stream(warn, log::general());
using dbg_general = log_stream(debug, log::genernal());

Game_launcher::Game_launcher(const Commandline_opts& cmdline_opts, 
		const char* appname) : cmdline_opts_(cmdline_opts), 
		video_(new Video()), font_manager_(), prefs_manager_(), 
		image_manager_(), main_event_context_(), hotkey_manager_(),
		music_thinker_(), music_muter_(), test_scenario_("test"),
		screenshot_map_(), screenshot_filename_(), state_(), 
		play_replay(false), multiplayer_server_(), 
		jummp_to_multiplayer_(false), jump_to_campaign_(false, -1, "", ""),
		load_data_()
{
	bool no_music = false;
	bool no_soundl = false;
}
