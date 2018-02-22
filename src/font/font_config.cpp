/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "font_config.h"
#include "../log.h"

static lg::Log_domain log_font("font");

#define DBG LOG_STREAM(lg::debug, log_font)
#define LOG LOG_STREAM(lg::info, log_font)
#define WRN LOG_STREAM(lg::warn, log_font)
#define ERR LOG_STREAM(lg::err, log_font)

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

	static void add_font_to_fontlist(Config& fonts_config,
			std::vector<Subset_descriptor>& fontlist)
	{
		fontlist.push_back(Subset_descriptor(fonts_config));
	}

	bool load_font_config()
	{
		Config cfg;
		try
		{
			std::string cfg_path = filesystem::get_json_location(
					"hardwired/fonts.cfg");
			if(cfg_path.empty())
			{
				ERR << "Could not resolve path to fonts.cfg, \
					file not found\n";
				return false;
			}
			cfg.open(cfg_path);
		}
		catch(...)
		{
			ERR << "could not read fonts.cfg\n";
			return false;
		}

		std::set<std::string> known_fonts;
		
		known_fonts.insert(cfg["fonts", "name"]);
		known_fonts.insert(cfg["fonts", "bold_name"]);
		known_fonts.insert(cfg["fonts", "italic_name"]);

		std::vector<Subset_descriptor> fontlist;

		for(auto& font : known_fonts)
			add_font_to_fontlist(cfg, fontlist);

		if(fontlist.empty())
			return false;

		SDL_ttf::set_font_list(fontlist);
		return true;
	}
}
