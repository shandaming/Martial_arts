/*
 * Copyright (C) 2019
 */

#ifndef EXECUTOR_H
#define EXECUTOR_H

struct executor
{
	executor() : exe(0), cmd_line(0), env(0) {}

	struct call_on_fork_setup
	{
		call_on_fork_setup(executor& e) : e(e) {}

		template<typename T>
		void operator()(const T& arg) const
		{
			arg.on_fork_setup(e);
		}

		executor& e;
	};

	struct call_on_fork_error
	{
		call_on_fork_error(executor& e) : e(e) {}

		template<typename T>
		void operator()(T& arg) const
		{
			arg.on_fork_error(e);
		}

		executor& e;
	};

	struct call_on_fork_success
	{
		call_on_fork_success(executor& e) : e(e) {}

		template<typename T>
		void operator()(const T& arg) const
		{
			arg.on_fork_success(e);
		}

		executor& e;
	};

	struct call_on_exec_setup
	{
		call_on_exec_setup(executor& e) : e(e) {}

		template<typename T>
		void operator()(T& arg) const
		{
			arg.on_exec_setup(e);
		}

		executor& e;
	};

	struct call_on_exec_error
	{
		call_on_exec_error(executor& e) : e(e) {}

		template<typename T>
		void operator()(T& arg) const
		{
			arg.on_exec_error(e);
		}

		executor& e;
	};

	template<typename T>
	child operator()(const T& seq)
	{
		std::for_each(seq, call_on_fork_setup(*this));
		pid_t pid = ::fork();
		if(pid == -1)
		{
			std::for_each(seq, call_on_fork_error(*this));
		}
		else if(pid == 0)
		{
			std::for_each(seq, call_on_exec_setup(*this));
			::execve(exe, cmd_line, env);
			std::for_each(seq, call_on_exec_error(*this));
			_exit(EXIT_FAILURE);
		}
		std::for_each(seq, call_on_fork_success(*this));
		return child(pid);
	}

	const char* exe;
	char** cmd_line;
	char** env;
};

template<typename... Args>
child execute(Args&&... args)
{
	return executor()(std::make_tuple(std::forword<Args>(args)...));
}

#endif
