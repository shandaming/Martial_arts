/*
 * Copyright (C) 2019
 */

#ifndef PRO_SET_ARGS_H
#define PRO_SET_ARGS_H

#include <algorithm>

#include "initializer.h"

template<typename Range>
class set_args_ : public initializer_base
{
public:
	explicit set_args_(const Range& args)
	{
		args_.reset(new char*[args.size() + 1]);
		std::transform(args.begin(), args.end(), args_.get(), c_str);
		args_[args.size()] = 0;
	}

	template<typename PosixExecutor>
	void on_exec_setup(PosixExecutor& e) const
	{
		e.cmd_line = args_.get();
		if(!e.exe && *args_[0])
			e.exe = args_[0];
	}
private:
	static char* c_str(const std::string& s)
	{
		return const_cast<char*>(s.c_str());
	}

	template<typename T>
	struct array_deleter
	{
		void operator()(T* const p) { delete[] p; }
	};

	std::unique_ptr<char*[]> args_;
};

template<typename Range>
set_args_<Range> set_args(const Range& range)
{
	return set_args_<Range>(range);
}

#endif
