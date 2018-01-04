/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "sdl_ttf.h"

Sdl_ttf::Sdl_ttf()
{
	int res = TTF_Init();
	if(res == -1)
	{
	}
}

Sdl_ttf::
