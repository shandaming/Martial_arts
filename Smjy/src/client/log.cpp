/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "log.h"

namespace
{
	class Null_streambuf : public std::streambuf
	{
		public:
			Null_streambuf() {}
		private:
			virtual int overflow(int c) 
			{
				return std::char_traits<char>::not_eof(c);
			}
	};
}

static std::ostream null_ostream(new Null_streambuf);
static int indent = 0;
static bool timestamp = true;
static bool precise_timestamp = false;

static std::ostream* output_stream = nullptr;

static std::ostream& output()
{
	if(output_stream)
		return *output_stream;
	return std::cerr;
}

namespace lg
{
	/* Redirect_output_setter */

	Redirect_output_setter::Redirect_output_setter(std::ostream& stream)
		: old_stream_(output_stream)
	{
		output_stream = &stream;
	}

	Redirect_output_setter::~Redirect_output_setter()
	{
		output_stream = old_stream_;
	}

	static std::map<std::string, int>* domains;
	static int strict_level = -1;
	void timestamps(bool t) { timestamp = t; }
	void precise_timestamps(bool pt) { precise_timestamp = pt; }

	Logger& err()
	{
		static Logger lg("error", 0);
		return lg;
	}

	Logger& warn()
	{
		static Logger lg("warning", 1);
		return lg;
	}

	Logger& info()
	{
		static Logger lg("info", 2);
		return lg;
	}

	Logger& debug()
	{
		static Logger lg("debug", 3);
		return lg;
	}

	static Log_domain dom("general");

	Log_domain& general()
	{
		return dom;
	}

	/* Log_domain */

	Log_domain::Log_domain(const char* name) : domain_(nullptr)
	{
		// Indirection to prevent initialization depending on link order.
		if(!domains)
			domains = new std::map<std::string, int>;
		domain_ = &*domains->insert(std::make_pair(name, 1)).first;
	}

	bool set_log_domain_severity(const std::string& name, int severity)
	{
		auto s = name.size();
		if(name == "all")
		{
			for(auto& l : *domains)
				l.second = severity;
		}
		else if(s > 2 && name.compare(s - 2, 2, "/*") == 0)
		{
			for(auto& l : *domains)
			{
				if(l.first.compare(0, s - 1, name, 0, s - 1) == 0)
					l.second = severity;
			}
		}
		else
		{
			auto it = domains->find(name);
			if(it == domains->end())
				return false;
			it->second = severity;
		}
		return true;
	}

	bool set_log_domain_severity(const std::string& name, const Logger& lg)
	{
		return set_log_domain_severity(name, lg.get_severity());
	}

	bool get_log_domain_severity(const std::string& name, int& severity)
	{
		auto it = domains->find(name);
		if(it == domains->end())
			return false;
		severity = it->second;
		return true;
	}

	std::string list_logdomains(const std::string& filter)
	{
		std::ostringstream res;
		for(auto& l : *domains)
		{
			if(l.first.find(filter) != std::string::npos)
				res << l.first << "\n";
		}

		return res.str();
	}

	void set_strict_severity(int severity)
	{
		strict_level = severity;
	}

	void set_strict_severity(const Logger& lg)
	{
		set_strict_severity(lg.get_severity());
	}

	static bool strict_threw = false;

	bool broke_strict()
	{
		return strict_threw;
	}

	std::string get_timestamp(const time_t& t, const std::string& format)
	{
		std::ostringstream ss;

		ss << std::put_time(localtime(&t), format.c_str());
		return ss.str();
	}

	std::string get_timespan(const time_t& t)
	{
		std::ostringstream out;

		time_t minutes = t / 60;
		time_t days = minutes / 60 / 24;
		if(t <= 0)
			out << "expired";
		else if(minutes == 0)
			out << t << " seconds";
		else if(days == 0)
			out << minutes / 60 << " hours, " << minutes % 60 << " minutes";
		else
			out << days << " days, " << (minutes / 60) % 24 << " hours, " <<
				minutes % 60 << " minutes";
		return out.str();
	}

	static void print_precise_timestamp(std::ostream& out)
	{
		out << get_timestamp(time(nullptr));
	}

	std::ostream& Logger::operator()(Log_domain const& domain, 
			bool show_names, bool do_indent) const
	{
		if(severity_ > domain.domain_->second)
			return null_ostream;
		else
		{
			if(!strict_threw && (severity_ <= strict_level))
			{
				std::stringstream ss;
				ss << "Error (strict mode, strict_level = " << 
					strict_level << "): JY reported on channel " << name_
					<< " " << domain.domain_->first;
				std::cerr << ss.str() << "\n";
				strict_threw = true;
			}
			std::ostream& stream = output();
			if(do_indent)
			{
				for(int i = 0; i != indent; ++i)
					stream << "  ";
			}
			if(timestamp)
			{
				if(precise_timestamp)
					print_precise_timestamp(stream);
				else
					stream << get_timestamp(time(nullptr));
			}
			if(show_names)
				stream << name_ << ' ' << domain.domain_->first << ": ";
			return stream;
		}

	}

	void Scope_logger::do_log_entry(const Log_domain & domain, 
			const std::string& str)
	{
		output_ = &debug()(domain, false, true);
		str_ = str;
	
		ticks_ = std::chrono::system_clock::now();
	
		(*output_) << "{ BEGIN: " << str_ << "\n";
		++indent;
	}

	void Scope_logger::do_log_exit()
	{
		auto ticks = std::chrono::system_clock::now() - ticks_;
		--indent;
		do_indent();

		if(timestamp)
			(*output_) << get_timestamp(time(nullptr));
		(*output_) << "} END: " << str_ << " (took " << ticks.count() << 
			"ms)\n";
	}

	void Scope_logger::do_indent() const
	{
		for(int i = 0; i != indent; ++i)
			(*output_) << " ";
	}

	std::stringstream& vml_error()
	{
		static std::stringstream lg;
		return lg;
	}
}
