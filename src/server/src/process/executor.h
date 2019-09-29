/*
 * Copyright (C) 2019
 */

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <unistd.h>

template<typename F, typename T, typename... Args>
void for_each(F&& f, T&& t, Args&&... args)
{
	f(t);
	for_each(f, args...);
}

template<typename F, typename T>
void for_each(F&& f, T&& t)
{
	f(t);
}

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

	//template<typename T>
	//child operator()(const T& seq)
	template<typename... Args>
	child operator()(Args&&... seq)
	{
		//std::for_each(seq, call_on_fork_setup(*this)); // inherit_env有
for_each(call_on_fork_setup(*this), std::forward<Args>(seq)...);
		pid_t pid = ::fork();
		if(pid == -1)
		{
			//std::for_each(seq, call_on_fork_error(*this)); // 都没有
for_each(call_on_fork_error(*this), std::forward<Args>(seq)...);
		}
		else if(pid == 0)
		{
			//std::for_each(seq, call_on_exec_setup(*this)); // 除inherit_env其他都有
for_each(call_on_exec_setup(*this), std::forward<Args>(seq)...);
			::execve(exe, cmd_line, env);
			//std::for_each(seq, call_on_exec_error(*this)); // 都没有
for_each(call_on_exec_error(*this), std::forward<Args>(seq)...);
			_exit(EXIT_FAILURE);
		}
		//std::for_each(seq, call_on_fork_success(*this)); // 都没有
for_each(call_on_fork_success(*this), std::forward<Args>(seq)...);
		return child(pid);
	}

	const char* exe;
	char** cmd_line;
	char** env;
};

template<typename... Args>
child execute(Args&&... args)
{
	//return executor()(std::make_tuple(std::forword<Args>(args)...));
return executor()(std::forword<Args>(args)...);
}

/*
child execute(const std::string& run_exe, const std::vector<std::string>& args, const int in_fd, const int out_fd, const int err_fd)
{
	const char** env = environ;

	pid_t pid = ::fork();
	if(pid == -1)
	{
		PROCESS_THROW_LAST_SYSTEM_ERROR("fork() failed.")
	}
	else if(pid == 0) // 子进程
	{
		std::unique_ptr<char*[]> cmdline(new char*[args.size() + 1]);
		std::transform(args.begin(), args.end(), cmdline.get(), 
				[](const std::string& s)
				{ return const_cast<char*>(s.c_str()); });
		cmdline[args.size()] = 0;

		if(!run_exe.empty() && args.size() > 1)
		{
			const char* exe = args[0].c_str();
			if(in_fd >= 0)
			{
				::dup2(in_fd, STDIN_FILENO);
				::close(in_fd);
			}
			if(out_fd >= 0)
			{
				::dup2(out_fd, STDOUT_FILENO);
				::close(out_fd);
			}
			if(err_fd >= 0)
			{
				::dup2(err_fd, STDERR_FILENO);
				::close(err_fd);
			}

			::execve(exe, cmdline, env);

			_exit(EXIT_FAILURE);
		}
	}
	return child(pid);
}
*/

#endif
