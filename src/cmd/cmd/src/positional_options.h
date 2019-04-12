

#ifndef BOOST_PROGRAM_OPTIONS_POSITIONAL_OPTIONS_VP_2004_03_02
#define BOOST_PROGRAM_OPTIONS_POSITIONAL_OPTIONS_VP_2004_03_02

#include <vector>
#include <string>

    class  positional_options_description {
    public:
		positional_options_description() {}

        /** ���Ϊ'max_count'��һ��λ��ѡ�������Ӧ�ñ�����'name'�� ֵ��-1����ʾ�������ơ��� 
		����'max_value'����'-1'�󣬲��ܵ���'add'��          
        */
        positional_options_description& add(const char* name, int max_count);

        /** ���ؿ��Դ��ڵ����λ��ѡ������ ���Է��أ�numeric_limits <uint32_t> :: max��������ָʾ���������� */
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

