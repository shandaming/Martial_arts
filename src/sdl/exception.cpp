/*
 * Copyright (C) by Shan Daming <shandaming@hotmail.com>
 */

#include <SDL2/SDL.h>
#include "exception.h"

static std::string create_error(const std::string& operation, bool use_sdl_error)
{
	if(use_sdl_error)
		return operation + "Error >> " + SDL_GetError() + " <<\n";
	else
		return operation;
}

Exception::Exception(const std::string& operation, bool use_sdl_error) :
	 Error(create_error(operation, use_sdl_error)) {}
