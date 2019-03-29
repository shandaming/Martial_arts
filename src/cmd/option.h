
#ifndef BOOST_OPTION_HPP_VP_2004_02_25
#define BOOST_OPTION_HPP_VP_2004_02_25

#include <string>
#include <vector>

    //template<class charT>
    //class basic_option {
class option
{
    public:
		option()
        : position_key(-1)
        , unregistered(false) 
        , case_insensitive(false)
        {}
		option(const std::string& xstring_key,
               const std::vector< std::string> &xvalue)
        : string_key(xstring_key)
        , position_key(-1)
        , value(xvalue)
        , unregistered(false)
        , case_insensitive(false)
        {}

		/*��ѡ����ַ������� ���������ģ�������*/
        std::string string_key;
        /** ��ѡ���λ�ü��� û����ʽ���Ƶ�����ѡ���0��ʼ��˳���š����ѡ�������ʽ���ƣ���position_key������-1��
		�����ʽ������ƣ������ָ��position_key��string_key��
         */
        int position_key;
        /** Option's value */
		std::vector<std::string> value;
        /** ԭʼδ���ĵı�Ǵ�ѡ���Ǵ��д�����. */
        std::vector< std::basic_string<char> > original_tokens;
        /** ���δʶ��ѡ���ΪTrue�� ����������£�'string_key'��'value'�Ǵ��﷨����Դ�Ľ���� 
		ԭʼ���ƿ��Դӡ�original_tokens����Ա�лָ���
        */
        bool unregistered;
        /** ������봦��string_key�����ִ�Сд����ΪTrue��
        */
        bool case_insensitive;
    };
    //typedef basic_option<char> option;

#endif
