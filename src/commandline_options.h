/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef COMMANDLINE_OPTIONS_H
#define COMMANDLINE_OPTIONS_H

#include <optional>
#include <string>
#include <tuple>
#include <vector>
#include <iostream>
#include "cmdln.h"

class Commandline_options
{
		friend std::ostream& operator<<(std::ostream& os, 
				const Commandline_options& cmdline_opts);
	public:
		Commandline_options(const std::vector<std::string>& args);

                bool parse_commandline( 
									const int flags = 0,
									const int start_pos = 1);

                bool check_parse_status(const Commandline::Parse_status& 
										status);

                void print_usage();
                void print_header();

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
	private:
		std::string args0_;
                std::vector<std::string> args_;

                Commandline cmdline_;
};

#endif
