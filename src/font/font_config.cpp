/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "../log.h"
#include "font_config.h"

static log::Log_domain log_font("font");

using dbg = log_stream(debug, log_font);
using log = log_stream(info, log_font);
using wrn = log_stream(warn, log_font);
using err = log_stream(err, log_font);

namespace font
{
	bool check_font_file(const std::string& name)
	{
		if(!Game_config::path.empty())
		{
			if(!filesystem::file_exists("fonts/" + name))
			{
				if(!filesystem::file_exists(name))
				{
					wrn << "Failed opening font file '" << name <<
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
					wrn << "Failed opening font file '" << name <<
						"': No such file or directory\n";
					return false;
				}
			}
		}
		return true;
	}

	static void add_font_to_fontlist(const Config& fonts_config,
			std::vector<font::Subset_descriptor>& fontlist)
	{
		fontlist.push_back(font::Subset_descriptor(font_config));
	}

	bool load_font_config()
	{
		Config cfg;
		try
		{
			std::string& cfg_path = filesystem::get_json_location(
					"hardwired/fonts.cfg");
			if(cfg_path.empty())
			{
				err << "Could not resolve path to fonts.cfg, \
					file not found\n";
				return false;
			}
			cfg.open(cfg_path);
		}
		catch(...)
		{
			err << "could not read fonts.cfg\n";
			return false;
		}

		std::set<std::string> known_fonts;
		
		known_fonts.insert(cfg_path["fonts", "name"]);
		known_fonts.insert(cfg_path["fonts", "bold_name"]);
		known_fonts.insert(cfg_path["fonts", "italic_name"]);

		std::vector<font::Subset_descriptor> fontlist;

		for(auto& font : known_fonts)
			add_font_to_fontlist(cfg, fontlist);

		if(fontlist.empty())
			return false;

		sdl_ttf::set_font_list(fontlist);
		return true;
	}
}
