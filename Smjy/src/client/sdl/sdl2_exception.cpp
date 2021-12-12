/*
 * Copyright (C) by Shan Daming <shandaming@hotmail.com>
 */

#include <SDL2/SDL_error.h>

#include "sdl2_exception.h"
#include "string_format.h"

const char* sdl2_exception::what() noexcept
{
	return string_format("%s Error >> %s\n", message.c_str(), SDL_GetError());
}
