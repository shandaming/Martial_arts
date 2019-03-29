

#ifndef BOOST_PROGRAM_OPTIONS_POSITIONAL_OPTIONS_VP_2004_03_02
#define BOOST_PROGRAM_OPTIONS_POSITIONAL_OPTIONS_VP_2004_03_02

#include <vector>
#include <string>

    class  positional_options_description {
    public:
		positional_options_description() {}

        /** 最多为'max_count'下一个位置选项的物种应该被赋予'name'。 值“-1”表示“无限制”。 
		调用'max_value'等于'-1'后，不能调用'add'。          
        */
        positional_options_description& add(const char* name, int max_count);

        /** 返回可以存在的最大位置选项数。 可以返回（numeric_limits <uint32_t> :: max）（）以指示无限数量。 */
        uint32_t max_total_count() const;

        /** Returns the name that should be associated with positional
            options at 'position'. 
            Precondition: position < max_total_count()
        */
        const std::string& name_for_position(uint32_t position) const;
    private:
        // List of names corresponding to the positions. If the number of
        // positions is unlimited, then the last name is stored in
        // trailing_;
        std::vector<std::string> names_;
        std::string trailing_;
    };


#endif

