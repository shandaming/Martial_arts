/*
 * Copyright (C) 2018
 */

#ifndef APPENDER_FILE_H
#define APPENDER_FILE_H

namespace lg
{
	class Appender_file : public Appender
	{
		public:
			typedef std::integral_constant<Appender_type, APPENDER_FILE>::type Type_index;

			Appender_file(uint8 id, const std::string& name, Log_level level, Appender_flags flags, std::vector<const char*> extra_args);
			~Appender_file() { close_file(); }
			FILE* open_file(const std::string& filename, const std::string& mode, bool backup);
			Appender_type get_type() const override { return Type_index::value; }
		private:
			void close_file();
			void write_stream(const Log_message* message) override;

			FILE* logfile_;
			std::string filename_;
			std::string log_dir_;
			bool dynamic_name_;
			bool backup_;
			uint64 max_file_size_;
			std::atomic<uint64> file_size_;
	};
}

#endif
