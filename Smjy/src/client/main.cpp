/*
 * Copyright (C) 2017 - 2018 by Shan Daming
 */

//#include <iostream>
#include <signal.h>
#include "log.h"
#include "game_config.h"
#include "game_launcher.h"
#include "filesystem.h"

static lg::Log_domain log_config("config");

#define LOG_CONFIG LOG_STREAM(lg::info, log_config)
#define LOG_GENERAL LOG_STREAM(lg::info, lg::general())

static void safe_exit(int res)
{
	LOG_GENERAL << "exiting with code " << res << "\n";
	exit(res);
}

static void init_locale()
{
	std::locale locale("zh_CN.UTF-8");
	std::locale::global(locale);
}

static int do_gameloop(std::vector<std::string>& args)
{
	srand(time(nullptr));

	Commandline_options cmdline_opts = Commandline_options(args);
        cmdline_opts.parse_commandline();
	game_config::game_program_dir = filesystem::directory_name(args[0]);

	// int finished = process_command_args(cmdine_opts);
	// if(finished == -1)
	//	return finished;
	
	std::unique_ptr<Game_launcher> game(new Game_launcher(cmdline_opts, 
				args[0].c_str()));
	int start_ticks = SDL_GetTicks();

	init_locale();

	bool res;

	// Do initialize fonts before reading the game config, to have game 
	// config erro messages displayed. fonts will be re-initialized later
	// when the language is read from the game config.
	//res = font::load_font_config();
	if(!res)
	{
		std::cerr << "could not initialize fonts\n";
	}
}

static void game_terminate_handler(int) { exit(0); }

int main(int argc, char* argv[])
{
	std::vector<std::string> args;

	for(int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	assert(!args.empty());

	if(SDL_Init(SDL_INIT_TIMER) < 0)
	{
		fprintf(stderr, "Counldn't initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	struct sigaction terminate_handler;
	terminate_handler.sa_handler = game_terminate_handler;
	terminate_handler.sa_flags = 0;
	sigemptyset(&terminate_handler.sa_mask);
	sigaction(SIGTERM, &terminate_handler, nullptr);
	sigaction(SIGINT, &terminate_handler, nullptr);

	// Explicitly ignore SIGCHLD. This allows us to launch child processes
	// without waiting for them to exit.
	terminate_handler.sa_handler = SIG_IGN;
	sigaction(SIGCHLD, &terminate_handler, nullptr);

	// declare this here so that it will always be at the front of the event
	// queue.
	Event_context global_context;

	SDL_StartTextInput();

	try
	{
		std::cout << "JY " << game_config::revision << "\n";
		time_t t = time(nullptr);
		std::cout << std::put_time(localtime(&t), "%c %Z") << "\n";

		auto exe_dir = filesystem::get_exe_dir();
		if(!exe_dir.empty())
		{
			// Try to autodetect the location of the game data dir. Note 
			// that the root of the source tree currently doubles as the 
			// data dir.
			std::string auto_dir;

			// scons leaves the resulting binaries at the root of the source
			// tree by default.
			if(filesystem::file_exists(exe_dir + "/data/main.cfg"))
				auto_dir = exe_dir;

			// cmake encourages creating a subdir at the root of the source
			// tree for the build, and the resulting binaries are found in
			// it.
			else if(filesystem::file_exists(exe_dir + "/../data/main.cfg"))
				auto_dir = filesystem::normalize_path(exe_dir + "/..");

			if(!auto_dir.empty())
			{
				std::cerr << "Automatically found a possible data \
					directory at " << auto_dir << "\n";
				game_config::path = auto_dir;
			}
		}

		int res = do_gameloop(args);
		safe_exit(res);
	}
	catch(...)
	{}

	return 0;
}
