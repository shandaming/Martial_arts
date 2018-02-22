/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef GAME_LAUNCHER_H
#define GAME_LAUNCHER_H

#include "commandline_options.h"
#include "video.h"
#include "config.h"
#include "font/font_config.h"
#include "events.h"

class Game_launcher
{
	public:
		Game_launcher(const Commandline_options& cmdline_opts, 
				const char* appname);

		Game_launcher(const Game_launcher&) = delete;
		Game_launcher& operator=(const Game_launcher&) = delete;

		~Game_launcher();

		Video& video() { return *video_; }

		enum class Selection { CONNECT, HOST, LOCAL };

		bool init_video();
		bool init_language();
		bool init_joystick();
		bool init_lua_script();

		bool play_test();
		bool play_screenshot_mode();
		bool play_render_image_mode();
		int unit_test();

		bool is_loading() const;
		void clear_loaded_game();
		bool load_game();
		void set_tutorial();
		void set_test(const std::string& id);

		std::string jump_to_campaign_id() const;
		bool new_campaign();
		bool goto_campaign();
		bool goto_multiplayer();
		bool goto_editor();

		bool jump_to_editor() const;

		void select_mp_server(const std::string& server) 
		{
			multiplayer_server_ = server; 
		}

		//bool play_multiplayer(Select& res);
		bool play_multiplayer_commandline();
		bool change_language();

		void show_preferences();

		enum class Reload_game_data { RELOAD_DATA, NO_RELOAD_DATA };

		//void launch_game(Reload_game_data& reload = 
				//Reload_game_data::Reload_data);
		void play_replay();

		void start_game();

		const Commandline_options& opts() const { return cmdline_opts_; }
	private:
		void mark_completed_campaigns(std::vector<Config>& campaigns);

		const Commandline_options& cmdline_opts_;

		const std::unique_ptr<Video> video_;

		font::Manager font_manager_;

		//const preferences::Manager prefs_manager_;
		//const image::Manager image_manager_;
		const Event_context main_event_context_;
		//const hotkey::Manager hotkey_manager_;
		//sound::Music_thinker music_thinker_;
		//sound::Music_muter music_muter_;

		std::string test_scenario_;

		std::string screenshot_map_, screenshot_filename_;

		//Saved_game state_;
		bool play_replay_;

		std::string multiplayer_server_;
		bool jump_to_multiplayer_;
		//Jump_to_campaign_info jump_to_campaign_;

		//std::unique_ptr<savegame::Load_game_metadata> load_data_;
};

#endif
