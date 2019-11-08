/*
 * Copyright (C) 2018
 */

#include <sstream>
#include <cstdio>

#include "appender_console.h"

appender_console::appender_console(uint8 id, const std::string& name, log_level level, 
		appender_flags flags, std::vector<const char*> extra_args) :
	appender(id, name, level, flags), colored_(false)
{
	for (uint8 i = 0; i < NUM_ENABLED_LOG_LEVELS; ++i)
		colors_[i] = color_types(max_colors);

	if (!extra_args.empty())
		init_colors(extra_args[0]);
}

void appender_console::init_colors(const std::string& str)
{
	if (str.empty())
	{
		colored_ = false;
		return;
	}

	int color[NUM_ENABLED_LOG_LEVELS];

	std::istringstream ss(str);

	for (uint8 i = 0; i < NUM_ENABLED_LOG_LEVELS; ++i)
	{
		ss >> color[i];

		if (!ss)
			return;

		if (color[i] < 0 || color[i] >= max_colors)
			return;
	}

	for (uint8 i = 0; i < NUM_ENABLED_LOG_LEVELS; ++i)
		colors_[i] = Color_types(color[i]);

	colored_ = true;
}

void appender_console::set_color(bool stdout_stream, color_types color)
{
	enum ANSI_text_attr
	{
		TA_NORMAL                                = 0,
		TA_BOLD                                  = 1,
		TA_BLINK                                 = 5,
		TA_REVERSE                               = 7
	};

	enum ANSI_fg_text_attr
	{
		FG_BLACK                                 = 30,
		FG_RED,
		FG_GREEN,
		FG_BROWN,
		FG_BLUE,
		FG_MAGENTA,
		FG_CYAN,
		FG_WHITE,
		FG_YELLOW
	};

	enum ANSI_bg_text_attr
	{
		BG_BLACK                                 = 40,
		BG_RED,
		BG_GREEN,
		BG_BROWN,
		BG_BLUE,
		BG_MAGENTA,
		BG_CYAN,
		BG_WHITE
	};

	static uint8 unix_colors[max_colors] =
	{
		FG_BLACK,                                          // BLACK
		FG_RED,                                            // RED
		FG_GREEN,                                          // GREEN
		FG_BROWN,                                          // BROWN
		FG_BLUE,                                           // BLUE
		FG_MAGENTA,                                        // MAGENTA
		FG_CYAN,                                           // CYAN
		FG_WHITE,                                          // WHITE
		FG_YELLOW,                                         // YELLOW
		FG_RED,                                            // LRED
		FG_GREEN,                                          // LGREEN
		FG_BLUE,                                           // LBLUE
		FG_MAGENTA,                                        // LMAGENTA
		FG_CYAN,                                           // LCYAN
		FG_WHITE                                           // LWHITE
	};

	fprintf((stdout_stream ? stdout : stderr), "\x1b[%d%sm", Unix_colors[color], 
			(color >= YELLOW && color < max_colors ? ";1" : ""));
}

void appender_console::reset_color(bool stdout_stream)
{
	fprintf((stdout_stream ? stdout : stderr), "\x1b[0m");
}

void appender_console::write_stream(const log_message* message)
{
	bool stdout_stream = !(message->level == LOG_LEVEL_ERROR || message->level == LOG_LEVEL_FATAL);

	if (colored_)
	{
		uint8 index;
		switch (message->level)
		{
			case LOG_LEVEL_TRACE:
			   index = 5;
			   break;
			case LOG_LEVEL_DEBUG:
			   index = 4;
			   break;
			case LOG_LEVEL_INFO:
			   index = 3;
			   break;
			case LOG_LEVEL_WARN:
			   index = 2;
			   break;
			case LOG_LEVEL_FATAL:
			   index = 0;
			   break;
			case LOG_LEVEL_ERROR: // No break on purpose
			default:
			   index = 1;
			   break;
		}

		set_color(stdout_stream, _colors[index]);
		fprintf(stdout_stream ? stdout : stderr, "%s%s\n", message->prefix.c_str(), 
				message->text.c_str());
		reset_color(stdout_stream);
	}
	else
		fprintf(stdout_stream ? stdout : stderr, "%s%s\n", message->prefix.c_str(), 
				message->text.c_str());
}
