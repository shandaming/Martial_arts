/*
 * Copyright (C) 2018
 */

#ifndef APPENDER_CONSOLE_H
#define APPENDER_SONSOLE_H

namespace lg
{
	enum Color_types : uint8
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

	class Appender_console : public Appender
	{
		public:
			typedef std::integral_constant<Appender_type, APPENDER_CONSOLE>::type Type_index;

			Appender_console(uint8 id, const std::string& name, Log_level level, Appender_flags flags, std::vector<const char*> extra_args);
			void init_colors(const std::string& init_str);
			Appender_type get_type() const override { return Type_index::value; }
		private:
			void set_color(bool stdout_stream, Color_types color);
			void reset_color(bool stdout_stream);
			void write_stream(const Log_message* message) override;

			bool colored_;
			Color_types colors_[NUM_ENABLED_LOG_LEVELS];
	};
}

#endif
