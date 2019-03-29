
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

		/*此选项的字符串键。 故意独立于模板参数。*/
        std::string string_key;
        /** 此选项的位置键。 没有显式名称的所有选项从0开始按顺序编号。如果选项具有显式名称，则“position_key”等于-1。
		如果隐式添加名称，则可以指定position_key和string_key。
         */
        int position_key;
        /** Option's value */
		std::vector<std::string> value;
        /** 原始未更改的标记此选项是从中创建的. */
        std::vector< std::basic_string<char> > original_tokens;
        /** 如果未识别选项，则为True。 在这种情况下，'string_key'和'value'是纯语法解析源的结果。 
		原始令牌可以从“original_tokens”成员中恢复。
        */
        bool unregistered;
        /** 如果必须处理string_key不区分大小写，则为True。
        */
        bool case_insensitive;
    };
    //typedef basic_option<char> option;

#endif
