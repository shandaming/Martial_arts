#ifndef UTIL_CMDLINE_H
#define UTIL_CMDLINE_H

#include "options_description.h"
#include "positional_options.h"
#include "option.h"

enum class style_t : uint8_t
{
	/// Allow "--long_name" style
	allow_long = 1,
	/// Allow "-<single character" style
	allow_short = allow_long << 1,
	/// Allow "-" in short options
	allow_dash_for_short = allow_short << 1,
	/// Allow "/" in short options
	allow_slash_for_short = allow_dash_for_short << 1,
	/** Allow option parameter in the same token
		for long option, like in
		@verbatim
		--foo=10
		@endverbatim
	*/
	long_allow_adjacent = allow_slash_for_short << 1,
	/** Allow option parameter in the next token for
		long options. */
	long_allow_next = long_allow_adjacent << 1,
	/** Allow option parameter in the same token for
		short options. */
	short_allow_adjacent = long_allow_next << 1,
	/** Allow option parameter in the next token for
		short options. */
	short_allow_next = short_allow_adjacent << 1,
	/** 允许将几个短选项合并在一起，
		 这样“-s -k”就变成了“-sk”。 所有选项
		 但最后应该不接受任何参数。 例如，如果
		 “-s”接受一个参数，然后“k”将被视为
		 参数，不是另一个短选项。
		 Dos风格的短选项不能粘。
	*/
	allow_sticky = short_allow_next << 1,
	/** 如果他们明确地识别长选项，则允许缩写拼写为长选项。 如果猜测生效，则没有长选项名称应该是其他长选项名称的前缀。
	*/
	allow_guessing = allow_sticky << 1,

	////__________________________
	///** Ignore the difference in case for long options.
	//*/
	//long_case_insensitive = allow_guessing << 1,
	///** Ignore the difference in case for short options.
	//*/
	//short_case_insensitive = long_case_insensitive << 1,
	///** Ignore the difference in case for all options.
	//*/
	//case_insensitive = (long_case_insensitive | short_case_insensitive),
	///** Allow long options with single option starting character,
	//	e.g <tt>-foo=10</tt>
	//*/
	//allow_long_disguise = short_case_insensitive << 1,

	/** The more-or-less traditional unix style. */
	unix_style = (allow_short | short_allow_adjacent | short_allow_next
	| allow_long | long_allow_adjacent | long_allow_next
		| allow_sticky | allow_guessing
		| allow_dash_for_short),
	/** The default style. */
	default_style = unix_style
};

class cmdline 
{
public:
		//typedef function1<std::pair<std::string, std::string>, 
          //                const std::string&> 
            //additional_parser;
	typedef std::function<std::pair<std::string, std::string>(
				const std::string&)> additional_parser;

        //typedef function1<std::vector<option>, std::vector<std::string>&>
          //  style_parser;
	typedef std::function<std::vector<option>(
				std::vector<std::string&>)> style_parser;
        
        /** 为（argc，argv）对构造命令行解析器。 使用'style'中传递的样式选项，它应该是style_t枚举的二进制或者值。 它也可以为零，
		在这种情况下将使用“默认”样式。 如果'allow_unregistered'为true，则允许未注册的选项。 它们将被分配索引1并假定具有可选参数。
        */
	cmdline(const std::vector<std::string>& args) { init(args); }

    cmdline(int argc, const char*const * argv);

    void style(int style = 0);

        /** returns the canonical option prefix associated with the command_line_style
         *  In order of precedence:
         *      allow_long           : allow_long
         *      allow_long_disguise  : allow_long_disguise
         *      allow_dash_for_short : allow_short | allow_dash_for_short
         *      allow_slash_for_short: allow_short | allow_slash_for_short
         *  
         *      This is mainly used for the diagnostic messages in exceptions
        */ 
    int get_canonical_option_prefix();

    void allow_unregistered();

    void set_options_description(const options_description& desc);
    void set_positional_options(
    const positional_options_description& positional_);

    std::vector<option> run();

    std::vector<option> parse_long_option(std::vector<std::string>& args);
    std::vector<option> parse_short_option(std::vector<std::string>& args);
    std::vector<option> parse_dos_option(std::vector<std::string>& args);
    std::vector<option> parse_disguised_long_option(
			std::vector<std::string>& args);
    std::vector<option> parse_terminator(std::vector<std::string>& args);
    std::vector<option> handle_additional_parser(
			std::vector<std::string>& args);


        /** 设置其他解析器。 将为命令行的每个标记调用此方法。 如果pair中的第一个字符串不为空，则该解析器认为该令牌匹配，
		并且第一个字符串将被视为选项名称（可以是长或短），而第二个字符串将是选项的参数（如果不为空）。 请注意，其他解析器只能匹配一个令牌。
        */
    void set_additional_parser(additional_parser p);

    void extra_style_parser(style_parser s);

    void check_style(int style) const;
        
    bool is_style_active(style_t style) const;

    void init(const std::vector<std::string>& args);

    void finish_option(option& opt,
			std::vector<std::string>& other_tokens,
			const std::vector<style_parser>& style_parsers);

        // Copies of input.
    std::vector<std::string> args_;
    style_t style_;
    bool allow_unregistered_;

    const options_description* desc_;
    const positional_options_description* positional_;

    additional_parser additional_parser_;
    style_parser style_parser_;
};
    
#endif

