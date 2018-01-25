/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef COMMANDLINE_OPTIONS_H
#define COMMANDLINE_OPTIONS_H

#include <optional>
#include <string>
#include <tuple>
#include <vector>

class Commandline_options
{
		friend std::ostream& operator<<(std::ostream& os, 
				const Commandline_options& cmdline_opts);
	public:
		Commandline_options(const std::vector<std::string>& args);

		// Non-empty if --bunzip2 was given on the command line.
		// Uncompresses a .bz2 file and exits.
		std::optional<std::string> bunzip2;

		// Non-empty if --bzip2 was given on the command line. Compresses
		// a file to .bz2 and exits.
		std::optional<std::string> bzip2;

		// Non-empty if --campaign was given on the command line. ID of the
		// campaign we want to start.
		std::optional<std::string> campaign;

		// Non-empty if --campaign-difficulty was given on the command line.
		// Numerical difficulty of the campaign to be played. Dependent on
		// --campaign.
		std::optional<int> campaign_difficulty;

		// Non-empty if --campaign-scenario was given on the command line.
		// Choose starting scenario in the campaign to be palyed.
		std::optional<std::string> campaign_scenario;

		// Ture if --clock was given on the command line. Enables
		bool clock;

		// Non-empty if --core was given on the command line. Chooses the
		// core to be loaded.
		std::optional<std::string> core_id;

		// True if --data-path was given on the command line. Prints path to
		// data directory and exits.
		bool data_path;

		// Non-empty if --data-dir was given on the command line. Sets the 
		// config dir to the specified one.
		std::optional<std::string> data_dir;

		// True if --debug was given on the command line. Enables debug mode
		bool debug;

		// True if --debug-lua was given in the commandline. Enables some
		// Lua debugging mechanisms.
		bool debug_lua;

#ifdef DEBUG_WINDOW_LAYOUT_GRAPHS
		// Non-empty if --debug-dot-domain was given on the command line.
		std::optional<std::string> debug_dot_domain;

		// Non-empty if --debug-dot-level was given on the command line.
		std::optional<std::string> debug_dot_level;
#endif

		// Non-empty if --editor was given on the command line. Goes 
		// directly into editor. If string is longer than 0. it contains
		// path to the file to edit.
		std::optional<std::string> editor;

		// True if --fps was given on the command line. Shows number of fps.
		bool fps;

		// True if --fullscreen was given on the command line. Starts game
		// in fullscreen mode.
		bool fullscreen;

		// Non-empty if --gunzip was given on the command line. 
		// Uncompresses a .gz file and exits.
		std::optional<std::string> gunzip;

		// Non-empty if --gzip was given on the command line. Compresses a 
		// file to .gz and exits.
		std::optional<std::string> gzip;

		// True if --help was given on the command line. Prints help and 
		// exits.
		bool help;

		// Non-empty if --language was given on the command line. Sets the
		// language for this session.
		std::optional<std::string> language;

		// Contains parsed arguments of --log-* (e.g. --log-debug)
		// Vector of pairs (severity, log_domain)
		std::optional<std::vector<std::pair<int, std::string>>> log;

		// Non-empty if --log-strict was given
		std::optional<int> log_strict_level;

		// Non-empty if --load was given on the command line. Savegame
		// specified to load after start.
		std::optional<std::string> load;

		// Non-empty if --logdomains was given on the command line. Prints
		// possible logdomains filtered by given string and exits.
		std::optional<std::string> logdomains;

		// True if --log-precise was given on the command line. Shows 
		// timestamps in log with more precision.
		bool log_precise_timestamps;

		// True if --multiplayer was given on the command line. Goes 
		// directly into multiplayer mode.
		bool multiplayer;
	private:
		std::string args0_;
};

#endif
