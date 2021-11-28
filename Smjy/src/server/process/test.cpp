#include <algorithm>
#include <vector>
#include <tuple>
#include <utility>
#include <tuple>

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

struct child
{
	explicit child(int p) : pid(p) {}

	int pid;
};

struct initializer_base
{
	template<typename T>
	void on_fork_setup(T&) const {}

	template<typename T>
	void on_fork_error(T&) const {}

	template<typename T>
	void on_fork_success(T&) const {}

	template<typename T>
	void on_exec_setup(T&) const {}

	template<typename T>
	void on_exec_error(T&) const {}
};

class bind_stdin : public initializer_base
{
public:
	explicit bind_stdin(const int fd) : fd_(fd) {}

	template<typename T>
	void on_exec_setup(T&) const
	{
		//::dup2(fd_, STDIN_FILENO);
	}
private:
	const int fd_;
};

class bind_stdout : public initializer_base
{
public:
	explicit bind_stdout(const int fd) : fd_(fd) {}

	template<typename T>
	void on_exec_setup(T&) const
	{
		//::dunp2(fd_, STDOUT_FILENO);
	}
private:
	int fd_;
};

class bind_stderr : public initializer_base
{
public:
	explicit bind_stderr(const int fd) : fd_(fd) {}

	template<typename T>
	void on_exec_setup(T&) const
	{
		//::dup2(fd_, STDERR_FILENO);
	}
private:
	const int fd_;
};

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

	template<typename... Args>
	child operator()(Args&&... seq)
	{
		for_each(call_on_fork_setup(*this), std::forward<Args>(seq)...); // inherit_env有
		pid_t pid = 1;// = ::fork();
#if 0
		if(pid == -1)
		{
			for_each(seq..., call_on_fork_error(*this)); // 都没有
		}
		else if(pid == 0)
		{
			for_each(seq..., call_on_exec_setup(*this)); // 除inherit_env其他都有
			//::execve(exe, cmd_line, env);
			for_each(seq..., call_on_exec_error(*this)); // 都没有
			//_exit(EXIT_FAILURE);
		}
		for_each(seq..., call_on_fork_success(*this)); // 都没有
#endif
		return child(pid);
	}

	const char* exe;
	char** cmd_line;
	char** env;
};

template<typename... Args>
child execute(Args&&... args)
{
	return executor()(std::forward<Args>(args)...);
}


int main()
{
	child c = execute(bind_stdin(1), bind_stdout(2), bind_stderr(3));
	//child c = executor()(bind_stdin(1), bind_stdout(2), bind_stderr(3));
	return 0;
}
