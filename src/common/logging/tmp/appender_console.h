/*
 * Copyright (C) 2018
 */

#ifndef APPENDER_CONSOLE_H
#define APPENDER_SONSOLE_H

enum color_types : uint8
{
	BLACK,
	RED,
	GREEN,
	BROWN,
	BLUE,
	MAGENTA,
	CYAN,
	GREY,
	YELLOW,
	LRED,
	LGREEN,
	LBLUE,
	LMAGENTA,
	LCYAN,
	WHITE
};

constexpr uint8 max_colors = WHITE + 1;

class appender_console : public appender
{
public:
	typedef std::integral_constant<appender_type, APPENDER_CONSOLE>::type type_index;

	appender_console(uint8 id, const std::string& name, log_level level, appender_flags flags, 
			std::vector<const char*> extra_args);
	void init_colors(const std::string& init_str);
	appender_type get_type() const override { return type_index::value; }
private:
	void set_color(bool stdout_stream, color_types color);
	void reset_color(bool stdout_stream);
	void write_stream(const log_message* message) override;

	bool colored_;
	color_types colors_[NUM_ENABLED_LOG_LEVELS];
};

#endif
