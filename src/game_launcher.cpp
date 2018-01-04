/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "game_launcher.h"

static log::Log_domain log_config("config");

Game_launcher::Game_launcher(const Commandline_opts& cmdline_opts, 
		const char* appname) : cmdline_opts_(cmdline_opts), 
		video_(new Video()), font_manager_(), prefs_manager_(), 
		image_manager_(), main_event_context_(), hotkey_manager_(),
		music_thinker_(), music_muter_(), test_scenario_("test"),
		screenshot_map_(), screenshot_filename_(), state_(), 
		play_replay(false), multiplayer_server_(), 
		jummp_to_multiplayer_(false), jump_to_campaign_(false, -1, "", ""),
		load_data_()
{}
