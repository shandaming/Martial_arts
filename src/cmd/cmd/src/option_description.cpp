/*
 * Copyright (C) 2019
 */

#include <cassert>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "option_description.h"

option_description::option_description(const std::string& name, const value_semantic* vs) :
	value_semantic_(vs)
{
	set_name(name);
}

option_description::option_description(const std::string& name, const value_semantic* vs,
		const std::string& description) : value_semantic_(vs), description_(description)
{
	set_name(name);
}

option_description& option_description::set_name(const std::string& name)
{
	long_names_.clear();
	std::istringstream is(name);
	std::string option_long_name;
	// 取,分割后的选项名
	while(std::getline(is, option_long_name, ','))
	{
		long_names_.emplace_back(option_long_name); // 默认都是长选项
	}

	assert(!long_names_.empty());

	if(long_names_.size() > 1) // 有2个选项
	{
		const std::string& last_name = *long_names_.rbegin(); // 第2个选项一般都是短选项
		if(last_name.size() == 1) // 短选项默认只有一个字节
		{
			short_name_ = "-" + last_name; // 构建短选项名
			long_names_.pop_back();

			// 还有剩余选项名，但这选项名为空则直接忽略
			if(long_names_.size() == 1 && (*long_names_.begin()).empty())
			{
				long_names_.clear();
			}
		}
	}
	return *this;
}

std::string option_description::format_name() const
{
	if(!short_name_.empty())
	{
		return long_names_.empty() ? short_name_ : 
			//short_name_.append("[ --").append(*long_names_.begin()).append(" ]");
			//(short_name_ + " [--" + *long_names_.begin() + "]");
			(short_name_ + ", --" + *long_names_.begin());
	}
	
	return "    --" + *long_names_.begin();
}

std::string option_description::format_parameter() const
{
	if(value_semantic_->max_tokens() != 0)
	{
		return value_semantic_->name();
	}
	return "";
}

// ------------------------------- optins_description --------------------------------

options_description::options_description(const std::string& caption) : caption_(caption)
{
	assert(min_description_length_ < line_length_ - 1);
}

void options_description::add(const std::shared_ptr<option_description>& description)
{
	options_.push_back(description);
	belong_to_group_.emplace_back(false);
}

uint32_t options_description::get_option_column_width() const
{
	uint32_t width(32); // 默认32个字节大小
	std::stringstream ss;

	for(auto& it : options_)
	{
		ss.str("");
		ss << "  " << it->format_name() << ' ' << it->format_parameter();
		width = std::max(width, static_cast<uint32_t>(ss.str().size())); // 取最大的宽度
	}

	for(auto& it : groups_)
	{
		width = std::max(width, it->get_option_column_width());
	}

	const uint32_t start_of_description_column = line_length_ - min_description_length_;
	width = std::max(width, start_of_description_column);
	return ++width;
}

namespace
{
void format_paragraph(std::ostream& os, std::string par, uint32_t indent, uint32_t line_length)
{
	// line_length是描述的长度，不包括缩进
	assert(indent < line_length);
	line_length -= indent;

	// 如果有tab则是附加的缩进
	std::string::size_type par_indent = par.find('\t');
	if(par_indent == std::string::npos)
	{
		par_indent = 0;
	}
	else
	{
		// 每个段落只允许有一个tab
		if(std::count(par.begin(), par.end(), '\t') > 1)
		{
			throw std::logic_error("Only one tab par paragraph is allowed in the options description");
		}

		// 将tab删了
		par.erase(par_indent, 1);

		assert(par_indent < line_length);

		// tab不在第一行则忽略
		if(par_indent >= line_length)
		{
			par_indent = 0;
		}
	}

	if(par.size() < line_length)
	{
		os << par;
	}
	else
	{
		bool first_line = true;
		for(std::string::const_iterator it = par.begin(); it != par.end();)
		{
			if(!first_line)
			{
				// 以空格开头，不删除
				if(*it == ' ' && (it + 1 < par.end()) && (*(it + 1) != ' '))
				{
					it += 1;
				}
			}

			std::string::const_iterator par_end = par.end();

			uint32_t remaining = std::distance(it, par_end);
			std::string::const_iterator line_end = it + 
				(remaining < line_length ? remaining : line_length); // 一行结尾

			if(*(line_end - 1) != ' ' && line_end < par_end && *line_end != ' ')
			{
				// 在当前段落的后面找到最后一个' '
				std::string::const_iterator last_space = std::find(std::reverse_iterator<std::string::const_iterator>(line_end),
						std::reverse_iterator<std::string::const_iterator>(it), ' ').base();
				if(last_space != it)
				{
					// 是最后一个空格
					if(std::distance(last_space, line_end) < line_length / 2)
					{
						line_end = last_space;
					}
				}
			}

			// 将行写入os
			std::copy(it, line_end, std::ostream_iterator<char>(os));
			if(first_line)
			{
				indent += par_indent;
				line_length -= par_indent;
				first_line = false;
			}

			// 更多行？
			if(line_end != par_end)
			{
				os << std::endl;
				for(uint32_t pad = indent; pad > 0; --pad)
				{
					os.put(' ');
				}
			}
			// 下一行在这之后进行
			it = line_end;
		}
	}
}

void format_description(std::ostream& os, const std::string& description, 
		uint32_t first_column_width, uint32_t line_length)
{
	assert(line_length > 1);

	// 如果控制台有更长的行，则使用少一个字符的长度
	if(line_length > 1)
	{
		--line_length;
	}

	// line_length必须 > first_column_width
	assert(line_length > first_column_width);

	std::vector<std::string> tokens;
	std::stringstream ss(description);
	std::string line;
	
	while(std::getline(ss, line, '\n')) //分割描述
	{
		tokens.emplace_back(line);
	}

	for(auto it = tokens.begin(); it != tokens.end();)
	{
		format_paragraph(os, *it, first_column_width, line_length); // 格式化段落
		++it;

		if(it != tokens.end())
		{
			os << std::endl;
			for(uint32_t pad = first_column_width; pad > 0; --pad)
			{
				os.put(' ');
			}
		}
	}
}

void format_one(std::ostream& os, const option_description& option, uint32_t first_column_width,
		uint32_t line_length)
{
	std::string format_option = "  " + option.format_name();
	format_option += (" " + option.format_parameter());
	os << format_option;

	if(!option.description().empty())
	{
		if(format_option.size() >= first_column_width)
		{
			// 第一列太常，将描述放到新行
			os.put('\n');
			for(uint32_t pad = first_column_width; pad > 0; --pad)
			{
				os.put(' ');
			}
		}
		else
		{
			for(uint32_t pad = first_column_width - format_option.size(); pad > 0; --pad)
			{
				os.put(' ');
			}
		}
		format_description(os, option.description(), first_column_width, line_length);
	}
}
}

void options_description::print(std::ostream& os) const
{
	if(!caption_.empty())
	{
		os << caption_ << ":" << std::endl;
	}

	const uint32_t width = get_option_column_width();

	for(size_t i = 0; i < options_.size(); ++i)
	{
		if(belong_to_group_[i]) // 跳过组
		{
			continue;
		}
		auto opt = *options_[i];
		format_one(os, opt, width, line_length_);
		os << std::endl;
	}

	for(auto& it : groups_)
	{
		os << std::endl;
		it->print(os); // 继续打印下一组选项
	}
}


std::ostream& operator<<(std::ostream& os, const options_description& desc) 
{
	desc.print(os);
	return os;
}
// ------------------------------- options_description_easy_init ----------------------------------

options_description_easy_init& options_description_easy_init::operator()(const std::string& name,
		const std::string& description)
{
	std::shared_ptr<option_description> d(new option_description(name, new untyped_value(true),
				description));
	owner_->add(d);
	return *this;
}

options_description_easy_init& options_description_easy_init::operator()(const std::string& name,
		const value_semantic* vs)
{
	std::shared_ptr<option_description> d(new option_description(name, vs));
	owner_->add(d);
	return *this;
}

options_description_easy_init& options_description_easy_init::operator()(const std::string& name,
		const value_semantic* vs, const std::string& description)
{
	std::shared_ptr<option_description> d(new option_description(name, vs, description));
	owner_->add(d);
	return *this;
}
