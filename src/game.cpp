/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "game.h"

static int do_gameloop(std::vector<std::string>& args)
{
	srand(time(nullptr));

	//Commandline_options cmdline_opts = Commandline_options(args);
	game_config::game_program_dir = filesystem::directory_name(args[0]);

	// int finished = process_command_args(cmdine_opts);
	// if(finished == -1)
	//	return finished;
	

}

static void game_terminate_handler(int) { exit(0); }

int main(int argc, char* argv[])
{
	std::locale locale("zh_CN.UTF-8");
	std::locale::global(locale);

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
		std::out << "JY " << game_config::revison << "\n";
		time_t t = time(nullptr);
		std::cout << std::put_time(localtime(&t), %c %Z) << "\n";

		auto exe_dir = filesystem::get_exe_dir();
		if(!exe_dir.empty())
		{
			std::string auto_dir;
			if(filesystem::file_exists(exe_dir + "/data/main.cfg"))
				auto_dir = exe_dir;

			//
		}

		int res = do_gameloop(args);
		safe_exit(res);
	}catch()
	catch(...)
	{}

	return 0;
}
