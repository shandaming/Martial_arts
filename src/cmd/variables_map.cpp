#define BOOST_PROGRAM_OPTIONS_SOURCE

#include <cassert>

#include "parsers.h"
#include "options_description.h"
#include "value_semantic.h"
#include "variables_map.h"

    using namespace std;
    
    void store(const parsed_options& options, variables_map& xm,
               bool utf8)
    {
        //TODO����������ڲ�ͬ�ĵ���'store'�ж���ͬ��ѡ�������в�ͬ�Ķ��壬����ô�죿
        assert(options.description);
        const options_description& desc = *options.description;

        // ������Ҫ����map��operator []�������Ǹ��ǰ汾variables_map�� ��..���ҡ�
        std::map<std::string, variable_value>& m = xm;

        std::set<std::string> new_final;

        // Declared once, to please Intel in VC++ mode;
        uint32_t i;

        // Declared here so can be used to provide context for exceptions
        string option_name;
        string original_token;

#ifndef BOOST_NO_EXCEPTIONS
        try
#endif
        {

            //���ȣ�ת��/�洢���и�����ѡ��
            for (i = 0; i < options.options.size(); ++i) {

                option_name = options.options[i].string_key;
                // ����û�����ֵ�λ��ѡ��
                if (option_name.empty())
                    continue;

                // ����δע���ѡ� �����û���ȷҪ������δע���ѡ��ʱ����δע�ᡱ�ֶβſ���Ϊtrue�� 
				//���ǲ��ܽ����Ǵ洢������ӳ�䣨ȱ�ٹ�����Ե��κ���Ϣ��������ֻ��������ǡ�
                if (options.options[i].unregistered)
                    continue;

                //���ѡ���������ֵ���������˷���
                if (xm.final_.count(option_name))
                    continue;

                original_token = options.options[i].original_tokens.size() ?
                                        options.options[i].original_tokens[0]     : "";
                const option_description& d = desc.find(option_name, false,
                                                        false, false);

                variable_value& v = m[option_name];
                if (v.defaulted()) {
                    // �������ʽ��ֵ��ɾ��Ĭ��ֵ
                    v = variable_value();
                }

                d.semantic()->parse(v.value(), options.options[i].value, utf8);

                v.value_semantic_ = d.semantic();

                // ��ѡ��Ǻϳɣ�������ʽ�ṩ�˸�ֵ�� ���Դ�ѡ���ֵ�Ա�������á�store���� 
				//���ǽ���洢����ʱ���У��Ա�������* this *'store'�����ڽ��ж�η��䡣
                if (!d.semantic()->is_composing())
                    new_final.insert(option_name);
            }
        }
#ifndef BOOST_NO_EXCEPTIONS
        catch(error_with_option_name& e)
        {
            // add context and rethrow
            e.add_context(option_name, original_token, options.m_options_prefix);
            throw;
        }
#endif
        xm.final_.insert(new_final.begin(), new_final.end());



        // ��Σ�Ӧ��Ĭ��ֵ���洢����ѡ�
        const vector<std::shared_ptr<option_description> >& all = desc.options();
        for(i = 0; i < all.size(); ++i)
        {
            const option_description& d = *all[i];
            string key = d.key("");
            // FIXME������߼�������option_description�ڲ���֪ʶ�� ���ѡ��˵������'*'����'key'Ϊ�ա� ����������£�Ĭ��ֵ����û�����塣
            if (key.empty())
            {
                continue;
            }
            if (m.count(key) == 0) {

                std::any def;
                if (d.semantic()->apply_default(def)) {
                    m[key] = variable_value(def, true);
                    m[key].value_semantic_ = d.semantic();
                }
            }

            // ������Ǳ���ѡ�����ӿ�ֵ
            if (d.semantic()->is_required()) {

                //�����Զ��ַ�ʽָ����ѡ�����ƣ����� �������У������ļ����ϣ��������ȹ������ã��� - ��>���� - ����/����>����ͨ�����ε���length�������Է�����������ȼ�
                string canonical_name = d.canonical_display_name(options.m_options_prefix);
                if (canonical_name.length() > xm.required_[key].length())
                    xm.required_[key] = canonical_name;
            }
        }
    }

    
    void store(const wparsed_options& options, variables_map& m)
    {
        store(options.utf8_encoded_options, m, true);
    }

    const variable_value&
    abstract_variables_map::operator[](const std::string& name) const
    {
        const variable_value& v = get(name);
        if (v.empty() && next_)
            return (*next_)[name];
        else if (v.defaulted() && next_) {
            const variable_value& v2 = (*next_)[name];
            if (!v2.empty() && !v2.defaulted())
                return v2;
            else return v;
        } else {
            return v;
        }
    }

    void variables_map::clear()
    {
        std::map<std::string, variable_value>::clear();
        final_.clear();
        required_.clear();
    }

    const variable_value& variables_map::get(const std::string& name) const
    {
        static variable_value empty;
        const_iterator i = this->find(name);
        if (i == this->end())
            return empty;
        else
            return i->second;
    }

    void variables_map::notify()
    {
        // This checks if all required options occur
        for (map<string, string>::const_iterator r = required_.begin();
             r != required_.end();
             ++r)
        {
            const string& opt = r->first;
            const string& display_opt = r->second;
            map<string, variable_value>::const_iterator iter = find(opt);
            if (iter == end() || iter->second.empty())
            {
                boost::throw_exception(required_option(display_opt));

            }
        }

        // Lastly, run notify actions.
        for (map<string, variable_value>::iterator k = begin();
             k != end();
             ++k)
        {
            /* Users might wish to use variables_map to store their own values
               that are not parsed, and therefore will not have value_semantics
               defined. Do not crash on such values. In multi-module programs,
               one module might add custom values, and the 'notify' function
               will be called after that, so we check that value_sematics is
               not NULL. See:
                   https://svn.boost.org/trac/boost/ticket/2782
            */
            if (k->second.value_semantic_)
                k->second.value_semantic_->notify(k->second.value());
        }
    }
