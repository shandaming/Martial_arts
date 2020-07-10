/*
 * Copyright (C) 2020
 */

#include <unistd.h>

#include <filesystem>
#include <vector>
#include <cstdlib>

#include "start_process.h"
#include "util.h"
#include "string_utils.h"
#include "file_descriptor.h"
#include "executor.h"
#include "set_args.h"
#include "socket_operations.h"
#include "message_buffer.h"
#include "log.h"
#include "pipe.h"
#include "run_exe.h"
#include "inherit_env.h"
#include "bind_stdin.h"
#include "bind_stdout.h"
#include "bind_stderr.h"
#include "wait_for_exit.h"

namespace fs = std::filesystem;

namespace
{
std::vector<fs::path> path()
{
	const std::string path_env = getenv("PATH");
	tokenizer token(path_env, ';');

	std::vector<fs::path> res;

	for(auto& it : token)
	{
		res.push_back(std::move(fs::path(it)));
	}

	return res;
}

template<typename T>
class log_sink
{
public:
	typedef char char_type;
	//typedef sink_tag category;

	log_sink(T callback) : callback_(std::move(callback)) {}

	std::streamsize write(const char* str, std::streamsize size)
	{
		callback_(std::string(str, size));
		return size;
	}
private:
	T callback_;
};

template<typename T>
auto make_log_sink1(T&& callback)->log_sink<typename std::decay<T>::type>
{
	return { std::forward<T>(callback) };
}

std::string make_log_sink(int fd)
{
	std::error_code ec;
	set_user_non_blocking(fd, ec);
	if(ec)
	{
		LOG_ERROR("proc", "set_user_non_blocking failed. fd = %d", fd);
		return "";
	}


	message_buffer read_buffer;
	size_t remaining_space = 0;
	size_t transferred_bytes = 0;
	size_t bytes_read = 0;

	while(1)
	{
		read_buffer.normalize();
		read_buffer.ensure_free_space();
		remaining_space = read_buffer.get_remaining_space();

		iovec vec;
		vec.iov_base = read_buffer.get_write_pointer();
		vec.iov_len = remaining_space;

		if(non_blocking_read(fd, &vec, remaining_space, ec, bytes_read))
			transferred_bytes += bytes_read;
		if(ec)
		{
			LOG_ERROR("proc", "file description [%d] read filed. error %d:%s", fd, ec.value(), ec.message().c_str());
			break;
		}
		else if(bytes_read == 0)
			break;
		else if(bytes_read < remaining_space)
		{
			read_buffer.write_completed(bytes_read);
			break;
		}
		else
			read_buffer.write_completed(bytes_read);
	}
	char* str_base = reinterpret_cast<char*>(read_buffer.get_base_pointer());
	const std::string result(str_base, read_buffer.get_buffer_size());

	return result;
}

template<typename T>
int create_child_process(T waiter, const std::string& executable, const std::vector<std::string>& args, const std::string& logger, const std::string& input, bool secure)
{
	auto out_pipe = create_pipe();
	auto err_pipe = create_pipe();

	std::optional<file_descriptor> input_source;

	if(!secure)
		LOG_TRACE(logger, "Starting process \"%s\" with arguments: \"%s\".",
				executable.c_str(), join(args, " ").c_str());
#if 1
	child c(1);
			if(!input.empty())
			{
				input_source = file_descriptor(input);
				c = execute(run_exe(fs::absolute(executable)), 
						set_args(args), 
						inherit_env(), 
						bind_stdin(*input_source), 
						bind_stdout(file_descriptor(out_pipe.sink)),
						bind_stderr(file_descriptor(err_pipe.sink)));
			}
			else
			{
				c = execute(run_exe(fs::absolute(executable)),
						set_args(args),
						inherit_env(),
						bind_stdout(file_descriptor(out_pipe.sink)),
						bind_stderr(file_descriptor(err_pipe.sink)));
			}
#else
	child c = [&]()->child
		{
			if(!input.empty())
			{
				input_source = file_descriptor(input);
				return execute(run_exe(fs::absolute(executable)), 
						set_args(args), 
						inherit_env(), 
						bind_stdin(*input_source), 
						bind_stdout(file_descriptor(out_pipe.sink)),
						bind_stderr(file_descriptor(err_pipe.sink)));
			}
			else
			{
				return execute(run_exe(fs::absolute(executable)),
						set_args(args),
						inherit_env(),
						bind_stdout(file_descriptor(out_pipe.sink)),
						bind_stderr(file_descriptor(err_pipe.sink)));
			}
		};
#endif

	file_descriptor out_fd(out_pipe.source);
	file_descriptor err_fd(err_pipe.source);

	/*
	auto out_info = make_log_sink([&](std::string msg)
			{
				LOG_INFO(logger, "%s", msg.c_str());
			});

	auto out_error = make_log_sink([&](std::string msg)
			{
				LOG_ERROR(logger, "%s", msg.c_str());
			});
		*/

	/*
	copy(cout_fd, out_info);
	copy(err_fd, out_error);
	*/
	const std::string log_info = make_log_sink(out_fd);
	const std::string log_error = make_log_sink(err_fd);
	LOG_INFO(logger, "%s", log_info.c_str());
	LOG_ERROR(logger, "%s", log_error.c_str());

	const int result = waiter(c);

	if(!secure)
		LOG_TRACE(logger, ">> Process \"%s\" finished with return value %d.", executable.c_str(), result);

	if(input_source)
		input_source->close();

	return result;
}
}

int start_process(std::string const& executable, std::vector<std::string> const& args,
                 std::string const& logger, std::string input_file, bool secure)
{
    return create_child_process([](child& c) -> int
    {
        try
        {
            return wait_for_exit(c);
        }
        catch (...)
        {
            return EXIT_FAILURE;
        }
    }, executable, args, logger, input_file, secure);
}

inline fs::path search_path(const fs::path& filename, const std::vector<fs::path>& path = path())
{
	for(const fs::path& pp : path)
	{
		auto p = pp / filename;
		std::error_code ec;
		bool file = fs::is_regular_file(p, ec);
		if(!ec && file && ::access(p.c_str(), X_OK) == 0)
			return p;
	}
	return "";
}

std::string search_executable_in_path(const std::string& filename)
{
	return search_path(filename);
}
