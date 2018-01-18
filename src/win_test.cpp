/*
 * Copyright (C)
 */
#include "sdl/window.h"

int main()
{
	Window win("SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			800, 600, 0, 0);

	bool quit = false;

	SDL_Event e;
	while(SDL_PollEvent(&e) != 0)
	{
		if(e.type == SDL_QUIT)
			quit = true;
	}
}
