/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <string>
#include <sstream>
#include <utility>
#include <chrono>

namespace lg
{
	/*
	 * Helper class to redirect the output of the logger in a certain scope
	 *
	 * The main usage of the redirection is for the unit tests to validate 
	 * the output on the logger width the expected output
	 */
	class Redirect_output_setter
	{
		public:
			explicit Redirect_output_setter(std::ostream& stream);
			~Redirect_output_setter();
		private:
			// The previously set redirection.
			//
			// This value is stored here to be restored in this destructor
			std::ostream* old_stream_;
	};

	class Logger;

	class Log_domain
	{
		public:
			friend class Logger;

			Log_domain(const char* name);
		private:
			std::pair<const std::string, int>* domain_;
	};

	bool set_log_domain_severity(const std::string& name, int severity);
	bool set_log_domain_severity(const std::string& name, const Logger& lg);
	bool set_log_domain_severity(const std::string& name, int& severity);
	std::string list_logdomains(const std::string& filter);

	void set_strict_severity(int severity);
	void set_strict_severity(const Logger& lg);
	bool broke_strict();

	class Logger
	{
		public:
			Logger(const char* name, int severity) : name_(name), 
				severity_(severity) {}

			std::ostream& operator()(const Log_domain& domain, 
					bool show_names = true, bool do_indent = false) const;

			bool dont_log(const Log_domain& domain) const
			{
				return severity_ > domain.domain_->second;
			}

			int get_severity() const
			{
				return severity_;
			}

			std::string get_name() const
			{
				return name_;
			}
		private:
			const char* name_;
			int severity_;
	};

	void timestamps(bool);
	void precise_timestamps(bool);
	std::string get_timestamp(const time_t& t, 
			const std::string& format = "%Y%m%d %H:%M:%S ");
	std::string get_timespan(const time_t& t);

	Logger& err(), &warn(), &info(), &debug();
	Log_domain& general();

	class Scope_logger
	{
		public:
			Scope_logger(const Log_domain& domain, const char* str) :
				output_(nullptr)
			{
				if(!debug().dont_log(domain))
					do_log_entry(domain, str);
			}

			Scope_logger(const Log_domain& domain, const std::string& str)
				: output_(nullptr)
			{
				if(!debug().dont_log(domain))
					do_log_entry(domain, str);
			}

			~Scope_logger()
			{
				if(output_) do_log_exit();
			}

			void do_indent() const;
		private:
			void do_log_entry(const Log_domain& domain, 
					const std::string& str);
			void do_log_exit();

			std::chrono::time_point<std::chrono::system_clock> ticks_;

			std::ostream* output_;
			std::string str_;
	};
}

template<typename Callabled>
inline std::ostream log_stream(Callabled& c, const lg::Log_domain& domain)
{
	if(std::is_function<decltype(c)>::value)
	{
		if(c().dont_log(domain))
			;
		else
			c()(domain);
	}
}

#endif
