#ifndef BOOST_CMDLINE_VP_2003_05_19
#define BOOST_CMDLINE_VP_2003_05_19

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
	/** ����������ѡ��ϲ���һ��
		 ������-s -k���ͱ���ˡ�-sk���� ����ѡ��
		 �����Ӧ�ò������κβ����� ���磬���
		 ��-s������һ��������Ȼ��k��������Ϊ
		 ������������һ����ѡ�
		 Dos���Ķ�ѡ���ճ��
	*/
	allow_sticky = short_allow_next << 1,
	/** ���������ȷ��ʶ��ѡ���������дƴдΪ��ѡ� ����²���Ч����û�г�ѡ������Ӧ����������ѡ�����Ƶ�ǰ׺��
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



    /** �����н������ࡣ��ҪҪ���ǣ�
         - ����֧�����г�����;��
         - ����ѧ���á�
         - ��С�Ĵ����С���ⲿ�����ԡ�
         - ����չ�Զ����﷨��

        ����ע������ѡ�֮�������е�Ԫ��
        ʹ��operator ++��ȡ��

        ����ÿ��Ԫ�أ��û��������ҵ�
         - �������һ��ѡ������
         - ѡ�������
         - ��Ȩ��ָ��
         - ��Ȩ��ֵ������У�
        
        ��ʱע���ѡ�����Ʋ�����������
        ���磬��Ϊ֧��������д�����
        ���Ի������ѡ�����ƣ�
         - ע���
         - ���������ҵ����Ǹ�

        �кܶ���ʽѡ������ڵ�������
        �н��������⣬�����԰�װ����Ľ�����
        �⽫�����Զ���ѡ����ʽ��
    */
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
        
        /** Ϊ��argc��argv���Թ��������н������� ʹ��'style'�д��ݵ���ʽѡ���Ӧ����style_tö�ٵĶ����ƻ���ֵ�� ��Ҳ����Ϊ�㣬
		����������½�ʹ�á�Ĭ�ϡ���ʽ�� ���'allow_unregistered'Ϊtrue��������δע���ѡ� ���ǽ�����������1���ٶ����п�ѡ������
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


        /** ���������������� ��Ϊ�����е�ÿ����ǵ��ô˷����� ���pair�еĵ�һ���ַ�����Ϊ�գ���ý�������Ϊ������ƥ�䣬
		���ҵ�һ���ַ���������Ϊѡ�����ƣ������ǳ���̣������ڶ����ַ�������ѡ��Ĳ����������Ϊ�գ��� ��ע�⣬����������ֻ��ƥ��һ�����ơ�
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

