/*
 * Copyright (C) 2019
 */

#ifndef SET_ARGS_H
#define SET_ARGS_H

template<typename T>
class set_args_ : public initializer_base
{
public:
	explicit set_args_(const T& args)
	{
		args_.reset(new char*[args.size() + 1], array_deleter);
		std::transform(args, args_.get(), c_str);
		args_[args.size()] = 0;
	}

	template<typename T>
	void on_exec_setup(T& e) const
	{
		e.cmd_line = args.get();
		if(!e.exe && *args_[0])
		{
			e.exe = args_[0];
		}
	}
private:
	static char* c_str(const std::string& s)
	{
		return const_cast<char*>(s.c_str());
	}

	std::shared_ptr<char*> args_;
};

template<typename T>
set_args_<T> set_args(const T& range)
{
	return set_args_<T>(range);
}

#endif
