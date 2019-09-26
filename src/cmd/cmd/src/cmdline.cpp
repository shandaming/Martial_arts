/*
 * Copyright (C) 2019
 */

#include <climits>
#include <cassert>

#include "cmdline.h"

cmdline::cmdline(int argc, char* argv[])
{
	init(std::vector<std::string>(argv + 1, argv + argc + !argc)); 
}

void cmdline::init(const std::vector<std::string>& args)
{
	this->args = args;
	desc = 0;
	//positional = 0;
}

cmdline& cmdline::options(const options_description& desc_)
{
	desc = &desc_;
	return *this;
}

std::vector<option> cmdline::parse_long_option(std::vector<std::string>& args)
{
	std::vector<option> result;
	if(args.size() > 0)
	{
		std::string name;
		std::string adjacent;
		const std::string tok = args[0];
		if(tok.size() >= 3 && tok[0] == '-' && tok[1] == '-')
		{
			std::string::size_type pos = tok.find('=');
			if(pos != std::string::npos)
			{
				name = tok.substr(2, pos - 2);
				adjacent = tok.substr(pos + 1);
				if(adjacent.empty())
				{
					throw std::logic_error("Invalid command line syntax: empty adjacent parameter"
							" name = "+ name);
				}
			}
			else
			{
				name = tok.substr(2);
			}
			option opt;
			opt.key = name;
			if(!adjacent.empty())
			{
				opt.value.emplace_back(adjacent);
			}
			opt.original_tokens.emplace_back(tok);
			result.emplace_back(opt);
			args.erase(args.begin());
		}
	}
	return result;
}

std::vector<option> cmdline::parse_short_option(std::vector<std::string>& args)
{
	std::vector<option> result;

	if(args.size() > 0)
	{
		const std::string tok = args[0];
		if(tok.size() >= 2 && tok[0] == '-' && tok[1] != '-')
		{
			std::string name = tok.substr(0, 2);
			std::string adjacent = tok.substr(2);

			// 短选项可以分组，“-d -a”可以合并为"-da" 一次处理一个选项
			while(1)
			{
				const option_description* d = desc->find(name);
				if(d && d->semantic()->max_tokens() == 0 && !adjacent.empty())
				{
					option opt;
					opt.key = name;
					result.emplace_back(opt);

					if(adjacent.empty())
					{
						args.erase(args.begin());
						break;
					}
					name = std::string("-") + adjacent[0];
					adjacent.erase(adjacent.begin());
				}
				else
				{
option opt;
					opt.key = name;
					opt.original_tokens.emplace_back(tok);
					if(!adjacent.empty())
					{
						opt.value.emplace_back(adjacent);
					}
					result.emplace_back(opt);
					args.erase(args.begin());
					break;
				}
			}
		}
	}
	return result;
}

std::vector<option> cmdline::parse_terminator(std::vector<std::string>& args)
{
	std::vector<option> result;

	if(args.size() > 0)
	{
		const std::string tok = args[0];
		if(tok == "--")
		{
			for(auto& it : args)
			{
				option opt;
				opt.value.push_back(it);
				opt.original_tokens.push_back(it);
				opt.position_key = INT_MAX;
				result.push_back(opt);
			}
			args.clear();
		}
	}
	return result;
}

std::vector<option> cmdline::run()
{
	assert(desc);

	std::vector<cmdline::style_parser> style_parsers;
	using namespace std::placeholders;
	style_parsers.push_back(std::bind(&cmdline::parse_long_option, this, _1));
	style_parsers.push_back(std::bind(&cmdline::parse_short_option, this, _1));
	style_parsers.push_back(std::bind(&cmdline::parse_terminator, this, _1));

	std::vector<option> result;
	while(!args.empty())
	{
		bool ok = false;

		for(auto& it : style_parsers)
		{
			uint32_t current_size = args.size();
			std::vector<option> next = it(args);

			// 检测选项名是否有效
			if(!next.empty())
			{
				std::vector<std::string> e;
				for(uint32_t i = 0; i < next.size() - 1; ++i)
				{
					finish_option(next[i], e, style_parsers);
				}
				// 对于最后一个选项，传递为解析标志，以便可以将他们添加到next.back()
				finish_option(next.back(), args, style_parsers);
				for(auto& it : next)
				{
					result.push_back(it);
				}
			}
			if(args.size() != current_size)
			{
				ok = true;
				break;
			}
		}
		if(!ok)
		{
			option opt;
			opt.value.push_back(args[0]);
			opt.original_tokens.push_back(args[0]);
			result.push_back(opt);
			args.erase(args.begin());
		}
	}

	std::vector<option> result2;
	for(uint32_t i = 0; i < result.size(); ++i)
	{
		result2.push_back(result[i]);
		option& opt = result2.back();
		if(opt.key.empty())
		{
			continue;
		}

		const option_description* d;
		try
		{
			d = desc->find(opt.key);
		}
		catch(...)
		{
			throw std::logic_error("Find description error.");
		}
		if(!d)
		{
			continue;
		}

		uint32_t min_tokens = d->semantic()->min_tokens();
		uint32_t max_tokens = d->semantic()->max_tokens();
		if(min_tokens < max_tokens && opt.value.size() < max_tokens)
		{
			//
			int can_take_more = max_tokens - opt.value.size();
			uint32_t j = i + 1;
			for(; can_take_more && j < result.size(); --can_take_more, ++j)
			{
				option& opt2 = result[j];
				if(!opt2.key.empty())
				{
					break;
				}
				if(opt2.position_key == INT_MAX)
				{
					//
					break;
				}

				assert(opt2.value.size() == 1);
				
				opt.value.push_back(opt2.value[0]);
			
				assert(opt2.original_tokens.size() == 1);

				opt.original_tokens.push_back(opt2.original_tokens[0]);
			}
			i = j - 1;
		}
	}
	result.swap(result2);

	//
	int position_key = 0;
	for(auto& it : result)
	{
		if(it.key.empty())
		{
			it.position_key = position_key++;
		}
	}

	for(auto& it : result)
	{
		if(it.key.size() > 2 || it.key[0] != '-')
		{
			//it.case_insensitive = false;
		}
		else
		{
			//it.case_insensitive = false;
		}
	}
	return result;
}

void cmdline::finish_option(option& opt, std::vector<std::string>& other_tokens, 
		const std::vector<style_parser>& style_parsers)
{
	if(opt.key.empty())
	{
		return;
	}

	std::string original_token_for_exceptions = opt.key;
	if(opt.original_tokens.size())
	{
		original_token_for_exceptions = opt.original_tokens[0];
	}

	try
	{
		// 检测选项是否有效，获取其描述
		const option_description* d = desc->find(opt.key);
		if(!d)
		{
			throw std::logic_error("Unknown option.");
		}
		
		opt.key = d->key(opt.key);
		// 检测该选项的最小/最大tokens是否和现在拥有的tokens一样多,
		uint32_t min_tokens = d->semantic()->min_tokens();
		uint32_t max_tokens = d->semantic()->max_tokens();

		uint32_t present_tokens = opt.value.size() + other_tokens.size();
		if(present_tokens >= min_tokens)
		{
			if(!opt.value.empty() && max_tokens == 0)
			{
				throw std::logic_error("Invalid command line syntax: extra parameter");
			}
			// 从other_tokens中获取的min_tokens值，但前提是这些tokens不被识别为选项
			if(opt.value.size() <= min_tokens)
			{
				min_tokens -= opt.value.size();
			}
			else
			{
				min_tokens = 0;
			}
			// 将值移动到result
			for(; !other_tokens.empty() && min_tokens--;)
			{
				// 检查额外参数是否看起来像已知选项，使用样式解析器检查它是否在语法上是一个选项，
				// 另外检查是否存在option_descriptin
				std::vector<option> followed_option;
				std::vector<std::string> next_token(1, other_tokens[0]);
				for(uint32_t i = 0; followed_option.empty() && i < style_parsers.size(); ++i)
				{
					followed_option = style_parsers[i](next_token);
				}

				if(!followed_option.empty())
				{
					original_token_for_exceptions = other_tokens[0];
					const option_description* od = desc->find(other_tokens[0]);
					if(od)
					{
						throw std::logic_error("Invalid command line syntax : missing parameter");
					}
				}
				opt.value.push_back(other_tokens[0]);
				opt.original_tokens.push_back(other_tokens[0]);
				other_tokens.erase(other_tokens.begin());
			}
		}
		else
		{
			throw std::logic_error("Invalid command line syntax: missing parameter.");
		}
	}
	catch(...)
	{
		throw std::logic_error("Finish option error.");
	}
}
