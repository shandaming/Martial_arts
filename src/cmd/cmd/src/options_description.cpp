/*
 * Copyright (C) 2019
 */

#include <cassert>
#include <algorithm>
#include <sstream>
#include <iterator>
#if 0
#include <climits>
#include <cstring>
#include <cstdarg>



#endif

#include "options_description.h"
#include "cmdline.h"

namespace 
{
template<typename T>
std::basic_string<T>  tolower(const std::basic_string<T>& str)
{
	std::basic_string<T> result;
    for (typename std::basic_string<T>::size_type i = 0; i < str.size(); ++i)
    {
		result.append(1, static_cast<T>(std::tolower(str[i])));
    }   
    return result;
}
}

option_description::option_description(const char* names, const value_semantic* s) :
	value_semantic_(s)
{
	this->set_names(names);
}
                                           
option_description::option_description(const char* names, const value_semantic* s,
		const char* description) : description_(description), value_semantic_(s)
{
	this->set_names(names);
}

option_description::match_result option_description::match(const std::string& option) const
{
	match_result result = no_match; 
    std::string local_option =  option;
       
	std::string local_long_name;
	for(auto it : long_names_)
    {
		local_long_name =  it;
        if (!local_long_name.empty()) 
		{
			if ((result == no_match) && (*local_long_name.rbegin() == '*'))
            {
				if (local_option.find(local_long_name.substr(0, local_long_name.length()-1)) 
						== 0)
				{
					result = approximate_match; // 靠靠
				}
            }
			if (local_long_name == local_option)
            {
				result = full_match;
                break;
            }
        }
    }

    if (result != full_match)
    {
		std::string local_short_name(short_name_);
        if (local_short_name == local_option)
        {
			result = full_match;
        }
    }
    return result;        
}

const std::string& option_description::key(const std::string& option) const
{
	if (!long_names_.empty()) 
	{
		const std::string& first_long_name = *long_names_.begin();
        if (first_long_name.find('*') != std::string::npos)
		{
			return option;
		}
        else
		{
			return first_long_name;
		}
    }
    else
	{
		return short_name_;
	}
}

std::string option_description::canonical_display_name(int prefix_style) const
{
    if (!long_names_.empty())
    {
        if (prefix_style == static_cast<int>(style_t::allow_long))
		{
            return "--" + *long_names_.begin();
		}
    }
        // 完整性检查：short_name_ [0]应该是' - '或'/'
    if (short_name_.length() == 2)
    {
        if (prefix_style == static_cast<int>(style_t::allow_slash_for_short))
		{
            return std::string("/") + short_name_[1];
		}
        if (prefix_style == static_cast<int>(style_t::allow_dash_for_short))
		{
            return std::string("-") + short_name_[1];
		}
    }
    if (!long_names_.empty())
	{
        return *long_names_.begin();
	}
    else
	{
        return short_name_;
	}
}

const std::string& option_description::long_name() const
{
    static std::string empty_string("");
    return long_names_.empty() ? empty_string : *long_names_.begin();
}

const std::pair<const std::string*, std::size_t> option_description::long_names() const
{
    // reinterpret_cast是为了取悦msvc 10。
    return (long_names_.empty())
        ? std::pair<const std::string*, size_t>(reinterpret_cast<const std::string*>(0), 0 )
        : std::pair<const std::string*, size_t>( &(*long_names_.begin()), long_names_.size());
}

option_description& option_description::set_names(const char* _names)
{
    long_names_.clear();
    std::istringstream iss(_names);
    std::string name;

    while(std::getline(iss, name, ',')) 
	{
        long_names_.push_back(name);
    }
    assert(!long_names_.empty() && "No option names were specified");

    bool try_interpreting_last_name_as_a_switch = long_names_.size() > 1;
    if (try_interpreting_last_name_as_a_switch) 
	{
        const std::string& last_name = *long_names_.rbegin();
        if (last_name.length() == 1) 
		{	
            short_name_ = '-' + last_name;
            long_names_.pop_back();

            if (long_names_.size() == 1 && (*long_names_.begin()).empty()) 
			{
                long_names_.clear();
            }
        }
    }
    return *this;
}

std::string option_description::format_name() const
{
    if (!short_name_.empty())
    {
        return long_names_.empty() ? 
			short_name_ : 
			short_name_.append(" [ --").append(*long_names_.begin()).append(" ]");
    }
    return std::string("--").append(*long_names_.begin());
}

std::string option_description::format_parameter() const
{
    if (value_semantic_->max_tokens() != 0)
	{
        return value_semantic_->name();
	}
    else
	{
        return "";
	}
}


/****************************** optins_desciption_easy_init  *****************************/

options_description_easy_init& options_description_easy_init:: operator()(const char* name,
		const char* description)
{
    std::shared_ptr<option_description> d(
			new option_description(name, new untyped_value(true), description));

    owner_->add(d);
    return *this;
}

options_description_easy_init& options_description_easy_init::operator()(const char* name,
		const value_semantic* s)
{
	std::shared_ptr<option_description> d(new option_description(name, s));
    owner_->add(d);
    return *this;
}

options_description_easy_init& options_description_easy_init::operator()(const char* name,
		const value_semantic* s, const char* description)
{
    std::shared_ptr<option_description> d(new option_description(name, s, description));

    owner_->add(d);
    return *this;
}

/***************************** options_description ****************************/

options_description::options_description(const std::string& caption) : 
			caption_(caption)
{
        // 我们需要在选项和描述部分之间留一个空格，所以加1。
    assert(min_description_length_ < line_length_ - 1);
}
    
void options_description::add(std::shared_ptr<option_description> desc)
{
	options_.push_back(desc);
    belong_to_group_.push_back(false);
}

options_description& options_description::add(const options_description& desc)
{
	std::shared_ptr<options_description> d(new options_description(desc));
    groups_.push_back(d);

    for (size_t i = 0; i < desc.options_.size(); ++i) 
	{
        add(desc.options_[i]);
        belong_to_group_.back() = true;
    }
    return *this;
}

const option_description& options_description::find(const std::string& name) const
{
	const option_description* d = find_nothrow(name);
    if (!d)
	{
        throw (unknown_option());
	}
    return *d;
}

const option_description* options_description::find_nothrow(const std::string& name) const
{
    std::shared_ptr<option_description> found;
    bool had_full_match = false;
    std::vector<std::string> approximate_matches;
    std::vector<std::string> full_matches;
        
    for(uint32_t i = 0; i < options_.size(); ++i)
    {
        option_description::match_result r = 
            options_[i]->match(name);

        if (r == option_description::no_match)
            continue;

        if (r == option_description::full_match)
        {                
            full_matches.push_back(options_[i]->key(name));
            found = options_[i];
            had_full_match = true;
        } 
        else 
        {                        
                // FIXME：这里使用'key'可能不行
                 //是最好的方法
            approximate_matches.push_back(options_[i]->key(name));
            if (!had_full_match)
                found = options_[i];
        }
    }
    if (full_matches.size() > 1) 
        throw (ambiguous_option(full_matches));
        
        // 如果我们有完整的匹配和近似匹配，
         //忽略近似匹配而不是报告错误。
         //说，如果我们有“全部”和“所有chroots”选项，那么
         //命令行上的“--all”应该选择第一个，
         //没有歧义
    if (full_matches.empty() && approximate_matches.size() > 1)
        throw (ambiguous_option(approximate_matches));

    return found.get();
}

    
std::ostream& operator<<(std::ostream& os, const options_description& desc)
{
    desc.print(os);
    return os;
}

namespace 
{
void format_paragraph(std::ostream& os, std::string par, uint32_t indent, uint32_t line_length)
{                    
            // 通过提醒这个功能，'line_length'会
             //是字符可用的长度，不包括
             //缩进
	assert(indent < line_length);
    line_length -= indent;

            // tab的索引（如果存在）用作附加的缩进相对
             //如果paragrapth跨越多个，则返回first_column_width
             //如果tab不在第一行，则忽略它
    std::string::size_type par_indent = par.find('\t');

    if (par_indent == std::string::npos)
    {
        par_indent = 0;
    }
    else
    {
                // only one tab per paragraph allowed
		if (std::count(par.begin(), par.end(), '\t') > 1)
        {
           throw std::logic_error("Only one tab per paragraph is allowed in the options description");
        }
          
                // erase tab from string
        par.erase(par_indent, 1);

                // 此断言可能由于用户错误或失败而失败
                 //环境条件！
        assert(par_indent < line_length);

                // ignore tab if not on first line
        if (par_indent >= line_length)
        {
            par_indent = 0;
        }            
    }
          
    if (par.size() < line_length)
    {
        os << par;
    }
    else
    {
        std::string::const_iterator line_begin = par.begin();
        const std::string::const_iterator par_end = par.end();

        bool first_line = true; // of current paragraph!        
            
        while (line_begin < par_end)  // paragraph lines
        {
            if (!first_line)
            {
                        // 如果行以空格开头，则为第二个字符
                         //不是空格，删除前导空格。
                         //我们不删除双空格，因为那些
                         //可能是intentianal。
                if ((*line_begin == ' ') &&
                    ((line_begin + 1 < par_end) &&
                     (*(line_begin + 1) != ' ')))
                {
                    line_begin += 1;  // line_begin != line_end
                }
            }

                    // 注意永远不要增加过去的迭代器
                     //结束，因为MSVC 8.0（破碎），假设
                     //这样做，即使没有访问，也是一个错误。
            uint32_t remaining = static_cast<uint32_t>(std::distance(line_begin, par_end));
            std::string::const_iterator line_end = line_begin + 
                ((remaining < line_length) ? remaining : line_length);
            
                    // 防止切碎的话
                     //两个非空格字符之间是否有line_end？
            if ((*(line_end - 1) != ' ') &&
                ((line_end < par_end) && (*line_end != ' ')))
            {
                        // find last ' ' in the second half of the current paragraph line
                std::string::const_iterator last_space =
                    std::find(std::reverse_iterator<std::string::const_iterator>(line_end),
                         std::reverse_iterator<std::string::const_iterator>(line_begin),
                         ' ')
                    .base();
                
                if (last_space != line_begin)
                {                 
                            // is last_space within the second half ot the 
                            // current line
                    if (static_cast<uint32_t>(std::distance(last_space, line_end)) < 
                        (line_length / 2))
                    {
                       line_end = last_space;
                    }
                }                                                
            } // prevent chopped words
             
                    // write line to stream
            std::copy(line_begin, line_end, std::ostream_iterator<char>(os));
              
            if (first_line)
            {
                indent += static_cast<uint32_t>(par_indent);
                line_length -= static_cast<uint32_t>(par_indent); // there's less to work with now
                first_line = false;
            }

            // more lines to follow?
            if (line_end != par_end)
            {
                os << '\n';
                
                for(uint32_t pad = indent; pad > 0; --pad)
                {
                    os.put(' ');
                }                                                        
            }
              
            // next line starts after of this line
            line_begin = line_end;              
        } // paragraph lines
    }          
}                              
        
void format_description(std::ostream& os, const std::string& desc, 
		uint32_t first_column_width, uint32_t line_length)
{
            // we need to use one char less per line to work correctly if actual
            // console has longer lines
	assert(line_length > 1);
    if (line_length > 1)
    {
        --line_length;
    }

            // line_length must be larger than first_column_width
            // this assert may fail due to user error or environment conditions!
    assert(line_length > first_column_width);

	std::vector<std::string> tokens;
	std::string::size_type pos;		
	std::string str = desc;
	while((pos = str.find('\n')) != std::string::npos)
	{
		tokens.emplace_back(desc.substr(0, pos));
		str = desc.substr(pos + 1);
	}
	tokens.emplace_back(str);

	for (auto it = tokens.begin(); it != tokens.end();)
	{
		format_paragraph(os, *it, first_column_width, line_length);
		++it;

				// prepair next line if any
		if (it != tokens.end())
		{
			os << '\n';

			for (uint32_t pad = first_column_width; pad > 0; --pad)
			{
				os.put(' ');
			}
		}
	}
}
    
void format_one(std::ostream& os, const option_description& opt, 
		uint32_t first_column_width, uint32_t line_length)
{
	std::stringstream ss;
    ss << "  " << opt.format_name() << ' ' << opt.format_parameter();
            
            // Don't use ss.rdbuf() since g++ 2.96 is buggy on it.
    os << ss.str();

    if (!opt.description().empty())
    {
		if (ss.str().size() >= first_column_width)
        {
			os.put('\n'); // first column is too long, lets put description in new line
            for (uint32_t pad = first_column_width; pad > 0; --pad)
            {
				os.put(' ');
            }
        }
		else 
		{
			for(uint32_t pad = first_column_width - static_cast<uint32_t>(ss.str().size()); 
					pad > 0; --pad)
			{
				os.put(' ');
            }
        }
            
        format_description(os, opt.description(), first_column_width, line_length);
        }
    }
}

uint32_t options_description::get_option_column_width() const                                
{
	std::stringstream ss;
    uint32_t width(23);
    for (size_t i = 0; i < options_.size(); ++i)
    {
        const option_description& opt = *options_[i];
		ss.str("");
        ss << "  " << opt.format_name() << ' ' << opt.format_parameter();
        width = std::max(width, static_cast<uint32_t>(ss.str().size()));            
    }
	for (size_t i = 0; i < groups_.size(); ++i)
	{
		width = std::max(width, groups_[i]->get_option_column_width());
	}

    const uint32_t start_of_description_column = line_length_ - min_description_length_;
	width = std::min(width, start_of_description_column - 1);
        
    ++width;
    return width;                                                    
}

void options_description::print(std::ostream& os, uint32_t width) const
{
	if (!caption_.empty())
	{
		os << caption_ << ":\n";
	}
	if (!width)
	{
		width = get_option_column_width();
	}

    for (uint32_t i = 0; i < options_.size(); ++i)
    {
		if (belong_to_group_[i])
		{
			continue;
		}

        const option_description& opt = *options_[i];
        format_one(os, opt, width, line_length_);
        os << "\n";
    }

    for (uint32_t j = 0; j < groups_.size(); ++j) 
	{            
        os << "\n";
        groups_[j]->print(os, width);
    }
}
