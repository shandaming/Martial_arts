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
	typedef sink_tag category;

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
auto make_log_sink(T&& callback)->log_sink<typename std::decay<T>::type>
{
	return { std::forward<T>(callback) };
}

template<typename T>
int create_child_process(T waiter, const std::string& executable, const std::vector<std::string>& args, const std::string& logger, const std::string& input, bool secure)
{
	auto out_pipe = create_pipe();
	auto err_pipe = create_pipe();

	std::optional<file_descriptor> input_source;

	if(!secure)
		LOG_TRAGE(logger, "Starting process \"%s\" with arguments: \"%s\".",
				executable.c_str(), join(args, " ").c_str());

	child c = [&]
		{
			if(!input.empty())
			{
				intput_source = file_descriptor(input);
				return execute(run_exe(fs::absolute(executable)), 
						set_args(args), 
						inherit_env(), 
						bind_stdin(*input_source), 
						bind_stdout(file_se))
			}
			else
			{
				return execute(run_exe(fs::absolute(executable)),
						set_args(args),
						inherit_env(),
						bind_stdout(file_descripto));
			}
		}

	file_descriptor out_fd(out_pipe.source, close_handle);
	file_descriptor err_fd(err_pipe.source, close_handle);

	auto out_info = make_log_sink([&](std::string msg)
			{
				LOG_INFO(logger, "%s", msg.c_str());
			});

	auto out_error = make_log_sink([&](std::string msg)
			{
				LOG_ERROR(logger, "%s", msg.c_str());
			});

	copy(cout_fd, out_info);
	copy(err_fd, out_error);

	const int result = waiter(c);

	if(!secure)
		LOG_TRACE(logger, ">> Process \"%s\" finished with return value %d.", executable.c_str(), result);

	if(input_source)
		input_source->close();

	return result;
}
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
