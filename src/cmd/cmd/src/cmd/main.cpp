#include "options_description.h"


int main(int argc, char* argv[])
{
	const std::string l1;
	const std::string l2;
	const std::string l3;
	const std::string l4;
	const std::string l5;
	const std::string l6;
	options_description general_opts("General options");
	general_opts.add_options()
		("all-translations", "Show all translations, even incomplete ones.")
		 ("bunzip2",  "decompresses a file (<arg>.bz2) in bzip2 format and stores it without the .bz2 suffix. <arg>.bz2 will be removed.")
		("bzip2",  "compresses a file (<arg>) in bzip2 format, stores it as <arg>.bz2 and removes <arg>.")
		("clock", "Adds the option to show a clock for testing the drawing timer.")
		("config-dir",  "sets the path of the userdata directory to $HOME/<arg> or My Documents\\My Games\\<arg> for Windows. You can specify also an absolute path outside the $HOME or My Documents\\My Games directory. DEPRECATED: use userdata-dir instead.")
		("config-path", "prints the path of the userdata directory and exits. DEPRECATED: use userdata-path instead.")
		("core",  "overrides the loaded core with the one whose id is specified.")
		("data-dir",  "overrides the data directory with the one specified.")
		("data-path", "prints the path of the data directory and exits.")
		("debug,d", "enables additional command mode options in-game.")
		("debug-lua", "enables some Lua debugging mechanisms");
#if 0
	general_opts.add_options()
		("all-translations", "Show all translations, even incomplete ones."
		 ("bunzip2", value<std::string>(), "decompresses a file (<arg>.bz2) in bzip2 format and stores it without the .bz2 suffix. <arg>.bz2 will be removed.")
		("bzip2", value<std::string>(), "compresses a file (<arg>) in bzip2 format, stores it as <arg>.bz2 and removes <arg>.")
		("clock", "Adds the option to show a clock for testing the drawing timer.")
		("config-dir", value<std::string>(), "sets the path of the userdata directory to $HOME/<arg> or My Documents\\My Games\\<arg> for Windows. You can specify also an absolute path outside the $HOME or My Documents\\My Games directory. DEPRECATED: use userdata-dir instead.")
		("config-path", "prints the path of the userdata directory and exits. DEPRECATED: use userdata-path instead.")
		("core", value<std::string>(), "overrides the loaded core with the one whose id is specified.")
		("data-dir", value<std::string>(), "overrides the data directory with the one specified.")
		("data-path", "prints the path of the data directory and exits.")
		("debug,d", "enables additional command mode options in-game.")
		("debug-lua", "enables some Lua debugging mechanisms");

	variables_map vm;
	store(command_line_parser(args_).options(all_).positional(positional).style().run(), vm);

	if (vm.count("ai-config"))
		std::string multiplayer_ai_config = (vm["ai-config"].as<std::vector<std::string>>());
	if (vm.count("algorithm"))
		std::vector<std::string> multiplayer_algorithm = (vm["algorithm"].as<std::vector<std::string>>());
	if (vm.count("bunzip2"))
		std::string bunzip2 = vm["bunzip2"].as<std::string>();
	if (vm.count("bzip2"))
		std::string bzip2 = vm["bzip2"].as<std::string>();
	if (vm.count("campaign"))
		std::string campaign = vm["campaign"].as<std::string>();
	if (vm.count("campaign-difficulty"))
		int campaign_difficulty = vm["campaign-difficulty"].as<int>();
	if (vm.count("campaign-scenario"))
		std::string campaign_scenario = vm["campaign-scenario"].as<std::string>();
	if (vm.count("campaign-skip-story"))
		bool campaign_skip_story = true;
	if (vm.count("clock"))
		bool clock = true;
#endif
}
