/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "game.h"

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

	return 0;
}
