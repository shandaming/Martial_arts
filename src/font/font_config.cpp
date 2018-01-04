/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "../log.h"
#include "font_config.h"

static log::Log_domain log_font("font");

#define DBG log_stream(debug, log_font)
#define LOG log_stream(info, log_font)
#define WRN log_stream(warn, log_font)
#define ERR log_stream(err, log_font)

namespace font
{
	bool check_font_file(const std::string& name)
	{
		if()
	}

	static bool add_font_to_fontlist(const Config& fonts_config,
			std::vector<font::Subset_descriptor>& fontlist,
			const std::string& name)
	{
		auto font = fonts_config();

		fontlist.push_back(font::Subset_descriptor(font));
		return true;
	}

	bool load_font_config()
	{
		Config cfg;
		try
		{
			if(!filesystem::file_exit("fonts.cfg"))
				return false;
		}
	}
}
