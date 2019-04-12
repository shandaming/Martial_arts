


#ifndef BOOST_PARSERS_VP_2003_05_19
#define BOOST_PARSERS_VP_2003_05_19

#include <functional>
#include <iosfwd>
#include <vector>
#include <utility>

#include "option.h"
#include "cmdline.h"


    class options_description;
    class positional_options_description;


    /** 解析输入源的结果。 此类的主要用途是将信息从解析器组件传递到值存储组件。 这个课本身没有多大意义。
    */
    template<class charT>
    class basic_parsed_options {
    public:
        explicit basic_parsed_options(const options_description* xdescription, int options_prefix = 0)
        : description(xdescription), m_options_prefix(options_prefix) {}
        /** Options found in the source. */
        std::vector< basic_option<charT> > options;
        /** 用于解析的选项描述。 解析器应返回指向传递给它们的option_description实例的指针，并且生命周期问题由调用者决定。
		可以为NULL。
         */
        const options_description* description;

        /** 主要用于异常中的诊断消息。
          *生成这些结果的解析器的规范选项前缀，
          *取决于basic_command_line_parser :: style（）或的设置
          * cmdline :: style（）。 按照command_line_style枚举的优先顺序排列：
          * allow_long
          * allow_long_disguise
          * allow_dash_for_short
          * allow_slash_for_short
        */
        int m_options_prefix;
    };


    typedef basic_parsed_options<char> parsed_options;

    /** Augments basic_parsed_options<wchar_t> with conversion from
        'parsed_options' */


    typedef function1<std::pair<std::string, std::string>, const std::string&> ext_parser;
	/*命令行解析器。

		该类允许指定解析和解析命令行所需的所有信息。 主要需要模拟命名的函数参数\ - 具有5个参数的常规函数将难以使用，
		并且使用较少数量的参数创建重载将会令人困惑。

		对于最常见的情况，函数parse_command_line是更好的选择。

		对于charT == char和charT == wchar_t情况，有两个typedefs \ - command_line_parser和wcommand_line_parser。
    */
    template<class charT>
    class basic_command_line_parser : private cmdline {
    public:
        /**为指定的参数列表创建命令行解析器。 'args'参数不应包含程序名称。
        */
        basic_command_line_parser(const std::vector<
                                  std::basic_string<charT> >& args);
        /** 为指定的参数列表创建命令行解析器。 参数应与传递给'main'的参数相同。
        */
        basic_command_line_parser(int argc, const charT* const argv[]);

        /** Sets options descriptions to use. */
        basic_command_line_parser& options(const options_description& desc);
        /** Sets positional options description to use. */
        basic_command_line_parser& positional(
            const positional_options_description& desc);

        /** Sets the command line style. */
        basic_command_line_parser& style(int);
        /** Sets the extra parsers. */
        basic_command_line_parser& extra_parser(ext_parser);

        /** 解析选项并返回解析结果。 抛出错误。
        */
        basic_parsed_options<charT> run();

        /** 指定允许未注册的选项，并且应该通过它们。 对于看起来像选项但不包含已识别名称的令牌之类的每个命令，
		将在结果中添加basic_option <charT>的实例，并将'unrecognized'字段设置为'true'。
		可以使用'collect_unrecognized'功能收集所有无法识别的选项。
        */
        basic_command_line_parser& allow_unregistered();

        using cmdline::style_parser;

        basic_command_line_parser& extra_style_parser(style_parser s);

    private:
        const options_description* desc_;
    };

    typedef basic_command_line_parser<char> command_line_parser;

    /** 创建'command_line_parser'的实例，将参数传递给它，并返回调用'run'方法的结果。
     */
    template<class charT>
    basic_parsed_options<charT>
    parse_command_line(int argc, const charT* const argv[],
                       const options_description&,
                       int style = 0,
                       function1<std::pair<std::string, std::string>,
                                 const std::string&> ext
                       = ext_parser());


#undef DECL

#include <iterator>

	 template<class charT>
	 basic_command_line_parser<charT>::
		 basic_command_line_parser(const std::vector<
			 std::basic_string<charT> >& xargs)
		 : cmdline(xargs)
	 {}


	 template<class charT>
	 basic_command_line_parser<charT>::
		 basic_command_line_parser(int argc, const charT* const argv[])
		 : cmdline(std::vector<std::basic_string<charT> >(argv + 1, argv + argc)),
		 desc_()
	 {}


	 template<class charT>
	 basic_command_line_parser<charT>&
		 basic_command_line_parser<charT>::options(const options_description& desc)
	 {
		 cmdline::set_options_description(desc);
		 desc_ = &desc;
		 return *this;
	 }

	 template<class charT>
	 basic_command_line_parser<charT>&
		 basic_command_line_parser<charT>::positional(
			 const positional_options_description& desc)
	 {
		 cmdline::set_positional_options(desc);
		 return *this;
	 }

	 template<class charT>
	 basic_command_line_parser<charT>&
		 basic_command_line_parser<charT>::style(int xstyle)
	 {
		 cmdline::style(xstyle);
		 return *this;
	 }

	 template<class charT>
	 basic_command_line_parser<charT>&
		 basic_command_line_parser<charT>::extra_parser(ext_parser ext)
	 {
		 cmdline::set_additional_parser(ext);
		 return *this;
	 }

	 template<class charT>
	 basic_command_line_parser<charT>&
		 basic_command_line_parser<charT>::allow_unregistered()
	 {
		 cmdline::allow_unregistered();
		 return *this;
	 }

	 template<class charT>
	 basic_command_line_parser<charT>&
		 basic_command_line_parser<charT>::extra_style_parser(style_parser s)
	 {
		 cmdline::extra_style_parser(s);
		 return *this;
	 }



	 template<class charT>
	 basic_parsed_options<charT>
		 basic_command_line_parser<charT>::run()
	 {
		 // save the canonical prefixes which were used by this cmdline parser
		 //    eventually inside the parsed results
		 //    This will be handy to format recognisable options
		 //    for diagnostic messages if everything blows up much later on
		 parsed_options result(desc_, cmdline::get_canonical_option_prefix());
		 result.options = cmdline::run();

		 // Presense of parsed_options -> wparsed_options conversion
		 // does the trick.
		 return basic_parsed_options<charT>(result);
	 }


	 template<class charT>
	 basic_parsed_options<charT>
		 parse_command_line(int argc, const charT* const argv[],
			 const options_description& desc,
			 int style,
			 function1<std::pair<std::string, std::string>,
			 const std::string&> ext)
	 {
		 return basic_command_line_parser<charT>(argc, argv).options(desc).
			 style(style).extra_parser(ext).run();
	 }

#endif
