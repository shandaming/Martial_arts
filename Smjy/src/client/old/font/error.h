/*
 * Copyright (C) 2018 by Shan Daming
 */

#ifndef FONT_ERROR_H
#define FONT_ERROR_H

#include "../exceptions.h"

namespace font
{
	struct Font_error : public Error
	{
		Font_error(const std::string& str = "Font initialization failed") :
			Error(str) {}
	};
}

#endif
