/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "font_config.h"
#include "../log.h"

static lg::Log_domain log_font("font");

#define DBG_FT LOG_STREAM(lg::debug, log_font)
#define LOG_FT LOG_STREAM(lg::info, log_font)
#define WRN_FT LOG_STREAM(lg::warn, log_font)
#define ERR_FT LOG_STREAM(lg::err, log_font)

namespace font
{
	bool check_font_file(const std::string& name)
	{
		if(!game_config::path.empty())
		{
			if(!filesystem::file_exists("fonts/" + name))
			{
				if(!filesystem::file_exists(name))
				{
					WRN << "Failed opening font file '" << name <<
						"': No such file or directory\n";
					return false;
				}
			}
		}
		else
		{
			if(!filesystem::file_exists("fonts/" + name))
			{
				if(!filesystem::file_exists(name))
				{
					WRN << "Failed opening font file '" << name <<
						"': No such file or directory\n";
					return false;
				}
			}
		}
		return true;
	}

	std::string family_order_sans;
	std::string family_order_mono;
	std::string family_order_light;
	std::string family_order_script;

	bool load_font_config()
	{
		Config cfg;
		try
		{
			std::string cfg_path = filesystem::get_json_location(
					"hardwired/fonts.cfg");
			if(cfg_path.empty())
			{
				ERR_FT << "Could not resolve path to fonts.cfg, \
					file not found\n";
				return false;
			}
			cfg.open(cfg_path);
		}
		catch(...)
		{
			ERR_FT << "could not read fonts.cfg\n";
			return false;
		}

		std::set<std::string> known_fonts;
		
		family_order_sans = cfg["family_order"];
		family_order_mono = cfg["family_order_monospace"];
		family_order_light = cfg["family_order_light"];
		family_order_script = cfg["family_order_script"];

		if(family_order_mono.empty())
		{
			ERR_FT << "No monospace font family order defined, falling back\
				to sans serif order\n";
			family_order_mono = family_order_sans;
		}

		if(family_order_light.empty())
		{
			ERR_FT << "No light font family order defined, falling back\
				to sans serif order\n";
			family_order_light = family_order_sans;
		}

		if(family_order_script.empty())
		{
			ERR_FT << "No script font family order defined, falling back\
				to sans serif order\n";
			family_order_script = family_order_sans;
		}

		return true;
	}

	const std::string& get_font_families(const Family_class& fc)
	{
		switch(fc)
		{
			default:
				return Family_class::FANG_SONG;
		}
	}
}
