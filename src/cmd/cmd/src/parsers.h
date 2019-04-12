


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


    /** ��������Դ�Ľ���� �������Ҫ��;�ǽ���Ϣ�ӽ�����������ݵ�ֵ�洢����� ����α���û�ж�����塣
    */
    template<class charT>
    class basic_parsed_options {
    public:
        explicit basic_parsed_options(const options_description* xdescription, int options_prefix = 0)
        : description(xdescription), m_options_prefix(options_prefix) {}
        /** Options found in the source. */
        std::vector< basic_option<charT> > options;
        /** ���ڽ�����ѡ�������� ������Ӧ����ָ�򴫵ݸ����ǵ�option_descriptionʵ����ָ�룬�����������������ɵ����߾�����
		����ΪNULL��
         */
        const options_description* description;

        /** ��Ҫ�����쳣�е������Ϣ��
          *������Щ����Ľ������Ĺ淶ѡ��ǰ׺��
          *ȡ����basic_command_line_parser :: style�����������
          * cmdline :: style������ ����command_line_styleö�ٵ�����˳�����У�
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
	/*�����н�������

		��������ָ�������ͽ��������������������Ϣ�� ��Ҫ��Ҫģ�������ĺ�������\ - ����5�������ĳ��溯��������ʹ�ã�
		����ʹ�ý��������Ĳ����������ؽ�����������

		������������������parse_command_line�Ǹ��õ�ѡ��

		����charT == char��charT == wchar_t�����������typedefs \ - command_line_parser��wcommand_line_parser��
    */
    template<class charT>
    class basic_command_line_parser : private cmdline {
    public:
        /**Ϊָ���Ĳ����б��������н������� 'args'������Ӧ�����������ơ�
        */
        basic_command_line_parser(const std::vector<
                                  std::basic_string<charT> >& args);
        /** Ϊָ���Ĳ����б��������н������� ����Ӧ�봫�ݸ�'main'�Ĳ�����ͬ��
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

        /** ����ѡ����ؽ�������� �׳�����
        */
        basic_parsed_options<charT> run();

        /** ָ������δע���ѡ�����Ӧ��ͨ�����ǡ� ���ڿ�������ѡ���������ʶ�����Ƶ�����֮���ÿ�����
		���ڽ�������basic_option <charT>��ʵ��������'unrecognized'�ֶ�����Ϊ'true'��
		����ʹ��'collect_unrecognized'�����ռ������޷�ʶ���ѡ�
        */
        basic_command_line_parser& allow_unregistered();

        using cmdline::style_parser;

        basic_command_line_parser& extra_style_parser(style_parser s);

    private:
        const options_description* desc_;
    };

    typedef basic_command_line_parser<char> command_line_parser;

    /** ����'command_line_parser'��ʵ�������������ݸ����������ص���'run'�����Ľ����
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
