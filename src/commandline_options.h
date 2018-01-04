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
};

#endif
