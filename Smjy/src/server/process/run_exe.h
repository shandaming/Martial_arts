/*
 * Copyright (C) 2019
 */

#ifndef PRO_RUN_EXE_H
#define PRO_RUN_EXE_H

#include <filesystem>

#include "initializer.h"

class run_exe_ : public initializer_base
{
public:
	explicit run_exe_(const std::string& s) : s_(s), cmd_line_(new char*[2])
	{
		cmd_line_[0] = const_cast<char*>(s_.c_str());
		cmd_line_[1] = 0;
	}

	template<typename T>
	void on_exec_setup(T& e) const
	{
		e.exe = s_.c_str();
		if(!e.cmd_line)
			e.cmd_line = cmd_line_.get();
	}
private:
	template<typename T>
	struct array_deleter
	{
		void operator()(T* const p) { delete[] p; }
	};

	std::string s_;
	std::shared_ptr<char*[]> cmd_line_;
};

run_exe_ run_exe(const char* s)
{
	return run_exe_(s);
}

run_exe_ run_exe(const std::string& s)
{
	return run_exe_(s);
}

namespace fs = std::filesystem;

run_exe_ run_exe(const fs::path& p)
{
	return run_exe_(p.string());
}

#endif
