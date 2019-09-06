/*
 * Copyright (C) 2019
 */

#include <string>
#include <utility>
#include <vector>
#include <cassert>
#include <cstring>
#include <cctype>
#include <climits>
#include <cstdio>
#include <iostream>
#include <functional>

#include "cmdline.h"
#include "value_semantic.h"
#include "options_description.h"
#include "positional_options.h"
    
std::string invalid_syntax::get_template(kind_t kind)
{
	const char* msg;
    switch(kind)
	{
		case empty_adjacent_parameter:
			msg = "the argument for option '%canonical_option%' should "
				"follow immediately after the equal sign";
			break;
        case missing_parameter:
            msg = "the required argument for "
				"option '%canonical_option%' is missing";
            break;
        case unrecognized_line:
            msg = "the options configuration file contains an invalid "
				"line '%invalid_line%'";
            break;
        // none of the following are currently used:
        case long_not_allowed:
            msg = "the unabbreviated option '%canonical_option%' is not "
				"valid";
            break;
        case long_adjacent_not_allowed:
            msg = "the unabbreviated option '%canonical_option%' does not "
				"take any arguments";
            break;
        case short_adjacent_not_allowed:
            msg = "the abbreviated option '%canonical_option%' does not "
				"take any arguments";
            break;
        case extra_parameter:
            msg = "option '%canonical_option%' does not take any "
				"arguments";
            break;
        default:
            msg = "unknown command line syntax error for '%s'";
    }
    return msg;
}

cmdline::cmdline(int argc, const char*const * argv)
{
    init(std::vector<std::string>(argv+1, argv+argc+!argc));
}

void cmdline::init(const std::vector<std::string>& args)
{
    this->args_ = args;        
    style_ = style_t::default_style;
    desc_ = 0;
    positional_ = 0;
    allow_unregistered_ = false;
}

cmdline& cmdline::style()
{
	check_style(style_t::default_style);
	style_ = (style_t::default_style);
	return *this;
}
    
void cmdline::allow_unregistered()
{
    this->allow_unregistered_ = true;
}

void cmdline::check_style(int style) const
{
    bool allow_some_long = 
        (style & allow_long) /*|| (style & allow_long_disguise)*/;

    const char* error = 0;
    if (allow_some_long && 
        !(style & long_allow_adjacent) && !(style & long_allow_next))
        error = "boost::program_options misconfiguration: "
                    "choose one or other of 'command_line_style::long_allow_next' "
                    "(whitespace separated arguments) or "
                    "'command_line_style::long_allow_adjacent' ('=' separated arguments) for "
                    "long options.";

    if (!error && (style & allow_short) &&
        !(style & short_allow_adjacent) && !(style & short_allow_next))
        error = "boost::program_options misconfiguration: "
                    "choose one or other of 'command_line_style::short_allow_next' "
                    "(whitespace separated arguments) or "
                    "'command_line_style::short_allow_adjacent' ('=' separated arguments) for "
                    "short options.";

    if (!error && (style & allow_short) &&
        !(style & allow_dash_for_short) && !(style & allow_slash_for_short))
        error = "boost::program_options misconfiguration: "
                    "choose one or other of 'command_line_style::allow_slash_for_short' "
                    "(slashes) or 'command_line_style::allow_dash_for_short' (dashes) for "
                    "short options.";

    if (error)
        throw (invalid_command_line_style(error));

        // Need to check that if guessing and long disguise are enabled
        // -f will mean the same as -foo
}
    
bool cmdline::is_style_active(style_t style) const
{
    return ((style_ & style) ? true : false);
}

cmdline& options(const options_description& desc)
{
	desc_ = &desc;
	return *this;
}

cmdline& positional(const positional_options_description& desc)
{
	positional_ = &positional;
	return *this;
}

int cmdline::get_canonical_option_prefix()
{
    if (style_ & allow_long)
        return allow_long;

    if ((style_ & allow_short) && (style_ & allow_dash_for_short))
        return allow_dash_for_short;

    if ((style_ & allow_short) && (style_ & allow_slash_for_short))
        return allow_slash_for_short;

    return 0;
}

parsed_options run()
{
			// save the canonical prefixes which were used by this cmdline parser
			//    eventually inside the parsed results
			//    This will be handy to format recognisable options
			//    for diagnostic messages if everything blows up much later on
parsed_options result(desc_, cmdline::get_canonical_option_prefix());
result.options = cmdline::run();

			// Presense of parsed_options -> wparsed_options conversion
			// does the trick.
	return parsed_options(result);
}

vector<option> cmdline::run()
{
	using namespace std::placeholders;

    assert(desc_);

    vector<style_parser> style_parsers;      

    if (style_ & allow_long)
        style_parsers.push_back(std::bind(&cmdline::parse_long_option, this, _1));

    if ((style_ & allow_short) && (style_ & allow_dash_for_short))
        style_parsers.push_back(std::bind(&cmdline::parse_short_option, this, _1));

    style_parsers.push_back(std::bind(&cmdline::parse_terminator, this, _1));

    vector<option> result;
    vector<string>& args = args_;
    while(!args.empty())
    {
        bool ok = false;
        for(uint32_t i = 0; i < style_parsers.size(); ++i)
        {
            uint32_t current_size = static_cast<uint32_t>(args.size());
            vector<option> next = style_parsers[i](args);

                //检查选项名称是否有效，以及所有值都已到位。
            if (!next.empty())
            {
                 vector<string> e;
                 for(uint32_t k = 0; k < next.size()-1; ++k) 
				{
                    finish_option(next[k], e, style_parsers);
                }
                    ////对于最后一个选项，传递未解析的标记，以便可以将它们添加到next.back（）的值（如果适用）。
                    finish_option(next.back(), args, style_parsers);
                    for (uint32_t j = 0; j < next.size(); ++j)
                        result.push_back(next[j]);                    
                }
                                
                if (args.size() != current_size) {
                    ok = true;
                    break;                
                } 
            }
            
            if (!ok) {
                option opt;
                opt.value.push_back(args[0]);
                opt.original_tokens.push_back(args[0]);
                result.push_back(opt);
                args.erase(args.begin());
            }
        }

        /* 如果一个键选项后跟一个位置选项，可以消耗更多的标记（例如它的多重标记选项），给它们这些标记。 */
        vector<option> result2;
        for (uint32_t i = 0; i < result.size(); ++i)
        {
            result2.push_back(result[i]);
            option& opt = result2.back();

            if (opt.string_key.empty())
                continue;

            const option_description* xd;
            try
            {
                xd = desc_->find_nothrow(opt.string_key);
            } 
            catch(error_with_option_name& e)
            {
                // add context and rethrow
                e.add_context(opt.string_key, opt.original_tokens[0], get_canonical_option_prefix());
                throw;
            }

            if (!xd)
                continue;

            uint32_t min_tokens = xd->semantic()->min_tokens();
            uint32_t max_tokens = xd->semantic()->max_tokens();
            if (min_tokens < max_tokens && opt.value.size() < max_tokens)
            {
                // 此选项可能会获取更多令牌。 我们只允许抓取尚未被识别为关键选项的令牌。

                int can_take_more = max_tokens - static_cast<int>(opt.value.size());
                uint32_t j = i+1;
                for (; can_take_more && j < result.size(); --can_take_more, ++j)
                {
                    option& opt2 = result[j];
                    if (!opt2.string_key.empty())
                        break;

                    if (opt2.position_key == INT_MAX)
                    {
                        // 我们使用INT_MAX标记在' - '终止符后找到的位置选项，因此应该永远保持位置。
                        break;
                    }

                    assert(opt2.value.size() == 1);
                    
                    opt.value.push_back(opt2.value[0]);

                    assert(opt2.original_tokens.size() == 1);

                    opt.original_tokens.push_back(opt2.original_tokens[0]);
                }
                i = j-1;
            }
        }
        result.swap(result2);
        

        // Assign position keys to positional options.
        int position_key = 0;
        for(uint32_t i = 0; i < result.size(); ++i) {
            if (result[i].string_key.empty())
                result[i].position_key = position_key++;
        }

        if (positional_)
        {
            uint32_t position = 0;
            for (uint32_t i = 0; i < result.size(); ++i) {
                option& opt = result[i];
                if (opt.position_key != -1) {
                    if (position >= positional_->max_total_count())
                    {
                        boost::throw_exception(too_many_positional_options_error());
                    }
                    opt.string_key = positional_->name_for_position(position);
                    ++position;
                }
            }
        }
        
        // 设置区分大小写标志
        for (uint32_t i = 0; i < result.size(); ++i) {
            if (result[i].string_key.size() > 2 ||
                        (result[i].string_key.size() > 1 && result[i].string_key[0] != '-'))
            {
                // it is a long option
                result[i].case_insensitive = false;//is_style_active(long_case_insensitive);
            }
            else
            {
                // it is a short option
                result[i].case_insensitive = false;//is_style_active(short_case_insensitive);
            }
        }

        return result;
    }

    void cmdline::finish_option(option& opt,
                           vector<string>& other_tokens,
                           const vector<style_parser>& style_parsers)
    {          
        if (opt.string_key.empty())
            return;

        //防守：如果handle_additional_parser（）创建的选项将没有原始令牌
        std::string original_token_for_exceptions = opt.string_key;
        if (opt.original_tokens.size())
            original_token_for_exceptions = opt.original_tokens[0];

        try
        {
            // 首先检查该选项是否有效，并获取其描述。
            const option_description* xd = desc_->find_nothrow(opt.string_key);

            if (!xd)
            {
                if (allow_unregistered_) {
                    opt.unregistered = true;
                    return;
                } else {
                    boost::throw_exception(unknown_option());
                }                
            }
            const option_description& d = *xd;

            // Canonize the name
            opt.string_key = d.key(opt.string_key);

            //我们检查该选项的最小/最大令牌数是否与我们拥有的令牌数一致。 'adjacent_value'（--foo = 1中的值）计为单独的标记，如果存在则必须使用。 命令行上的以下标记可能未被占用。
            uint32_t min_tokens = d.semantic()->min_tokens();
            uint32_t max_tokens = d.semantic()->max_tokens();
            
            uint32_t present_tokens = static_cast<uint32_t>(opt.value.size() + other_tokens.size());
            
            if (present_tokens >= min_tokens)
            {
                if (!opt.value.empty() && max_tokens == 0) 
                {
                    boost::throw_exception(
                        invalid_command_line_syntax(invalid_command_line_syntax::extra_parameter));
                }
                
                // 从other_tokens中获取min_tokens值，但前提是这些令牌本身不被识别为选项。
                if (opt.value.size() <= min_tokens) 
                {
                    min_tokens -= static_cast<uint32_t>(opt.value.size());
                }
                else
                {
                    min_tokens = 0;
                }

                // 一切都OK，将值移动到结果中。
                for(;!other_tokens.empty() && min_tokens--; ) 
                {
                    //检查额外参数是否看起来像已知选项我们使用样式解析器检查它是否在语法上是一个选项，另外我们检查是否存在option_description
                    vector<option> followed_option;  
                    vector<string> next_token(1, other_tokens[0]);      
                    for (uint32_t i = 0; followed_option.empty() && i < style_parsers.size(); ++i)
                    {
                        followed_option = style_parsers[i](next_token);
                    }
                    if (!followed_option.empty()) 
                    {
                        original_token_for_exceptions = other_tokens[0];
                        const option_description* od = desc_->find_nothrow(other_tokens[0]);
                        if (od) 
                            boost::throw_exception(
                                invalid_command_line_syntax(invalid_command_line_syntax::missing_parameter));
                    }
                    opt.value.push_back(other_tokens[0]);
                    opt.original_tokens.push_back(other_tokens[0]);
                    other_tokens.erase(other_tokens.begin());
                }
            }
            else
            {
                boost::throw_exception(
                            invalid_command_line_syntax(invalid_command_line_syntax::missing_parameter)); 

            }
        } 
        // 仅对unknown_option / ambiguous_option使用原始令牌，因为根据定义它们是无法识别/不可解析的
        catch(error_with_option_name& e)
        {
            // add context and rethrow
            e.add_context(opt.string_key, original_token_for_exceptions, get_canonical_option_prefix());
            throw;
        }

    }

    vector<option> cmdline::parse_long_option(vector<string>& args)
    {
        vector<option> result;
        const string& tok = args[0];
        if (tok.size() >= 3 && tok[0] == '-' && tok[1] == '-')
        {   
            string name, adjacent;

            string::size_type p = tok.find('=');
            if (p != tok.npos)
            {
                name = tok.substr(2, p-2);
                adjacent = tok.substr(p+1);
                if (adjacent.empty())
                    boost::throw_exception( invalid_command_line_syntax(
                                                      invalid_command_line_syntax::empty_adjacent_parameter, 
                                                      name,
                                                      name,
                                                      get_canonical_option_prefix()) );
            }
            else
            {
                name = tok.substr(2);
            }
            option opt;
            opt.string_key = name;
            if (!adjacent.empty())
                opt.value.push_back(adjacent);
            opt.original_tokens.push_back(tok);
            result.push_back(opt);
            args.erase(args.begin());
        }
        return result;
    }


    vector<option> cmdline::parse_short_option(vector<string>& args)
    {
        const string& tok = args[0];
        if (tok.size() >= 2 && tok[0] == '-' && tok[1] != '-')
        {   
            vector<option> result;

            string name = tok.substr(0,2);
            string adjacent = tok.substr(2);

            // 短选项可以“分组”，因此“-d -a”变为“-da”。 循环，一次处理一个选项。 当我们处理了所有令牌时，或者当令牌的剩余部分被认为是值时，我们退出循环，而不是进一步分组选项。
            for(;;) {
                const option_description* d;
                try
                {
                     
                    d = desc_->find_nothrow(name);
                } 
                catch(error_with_option_name& e)
                {
                    // add context and rethrow
                    e.add_context(name, name, get_canonical_option_prefix());
                    throw;
                }


                // FIXME: check for 'allow_sticky'.
                if (d && (style_ & allow_sticky) &&
                    d->semantic()->max_tokens() == 0 && !adjacent.empty()) {
                    // 'adjacent' is in fact further option.
                    option opt;
                    opt.string_key = name;
                    result.push_back(opt);

                    if (adjacent.empty())
                    {
                        args.erase(args.begin());
                        break;
                    }

                    name = string("-") + adjacent[0];
                    adjacent.erase(adjacent.begin());
                } else {
                    
                    option opt;
                    opt.string_key = name;
                    opt.original_tokens.push_back(tok);
                    if (!adjacent.empty())
                        opt.value.push_back(adjacent);
                    result.push_back(opt);
                    args.erase(args.begin());                    
                    break;
                }
            }
            return result;
        }
        return vector<option>();
    }

/*
    vector<option> 
    cmdline::parse_dos_option(vector<string>& args)
    {
        vector<option> result;
        const string& tok = args[0];
        if (tok.size() >= 2 && tok[0] == '/')
        {   
            string name = "-" + tok.substr(1,1);
            string adjacent = tok.substr(2);

            option opt;
            opt.string_key = name;
            if (!adjacent.empty())
                opt.value.push_back(adjacent);
            opt.original_tokens.push_back(tok);
            result.push_back(opt);
            args.erase(args.begin());
        }
        return result;
    }


    vector<option> 
    cmdline::parse_disguised_long_option(vector<string>& args)
    {
        const string& tok = args[0];
        if (tok.size() >= 2 && 
            ((tok[0] == '-' && tok[1] != '-') ||
             ((style_ & allow_slash_for_short) && tok[0] == '/')))            
        {
            try
            {
                if (desc_->find_nothrow(tok.substr(1, tok.find('=')-1), 
                                         is_style_active(allow_guessing),
                                         is_style_active(long_case_insensitive),
                                         is_style_active(short_case_insensitive)))
                {
                    args[0].insert(0, "-");
                    if (args[0][1] == '/')
                        args[0][1] = '-';
                    return parse_long_option(args);
                }
            } 
            catch(error_with_option_name& e)
            {
                // add context and rethrow
                e.add_context(tok, tok, get_canonical_option_prefix());
                throw;
            }
        }
        return vector<option>();
    }
*/

    vector<option> 
    cmdline::parse_terminator(vector<string>& args)
    {
        vector<option> result;
        const string& tok = args[0];
        if (tok == "--")
        {
            for(uint32_t i = 1; i < args.size(); ++i)
            {
                option opt;
                opt.value.push_back(args[i]);
                opt.original_tokens.push_back(args[i]);
                opt.position_key = INT_MAX;
                result.push_back(opt);
            }
            args.clear();
        }
        return result;
    }

/*
    vector<option> 
    cmdline::handle_additional_parser(vector<string>& args)
    {
        vector<option> result;
        pair<string, string> r = additional_parser_(args[0]);
        if (!r.first.empty()) {
            option next;
            next.string_key = r.first;
            if (!r.second.empty())
                next.value.push_back(r.second);
            result.push_back(next);
            args.erase(args.begin());
        }
        return result;
    }


void cmdline::set_additional_parser(additional_parser p)
{
	additional_parser_ = p;
}

void cmdline::extra_style_parser(style_parser s)
{
	style_parser_ = s;
}
*/
