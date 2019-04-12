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
        //TODO：如果我们在不同的调用'store'中对相同的选项名称有不同的定义，该怎么办？
        assert(options.description);
        const options_description& desc = *options.description;

        // 我们需要访问map的operator []，而不是覆盖版本variables_map。 嗯..凌乱。
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

            //首先，转换/存储所有给定的选项
            for (i = 0; i < options.options.size(); ++i) {

                option_name = options.options[i].string_key;
                // 跳过没有名字的位置选项
                if (option_name.empty())
                    continue;

                // 忽略未注册的选项。 仅当用户明确要求允许未注册的选项时，“未注册”字段才可以为true。 
				//我们不能将它们存储到变量映射（缺少关于配对的任何信息），所以只需忽略它们。
                if (options.options[i].unregistered)
                    continue;

                //如果选项具有最终值，请跳过此分配
                if (xm.final_.count(option_name))
                    continue;

                original_token = options.options[i].original_tokens.size() ?
                                        options.options[i].original_tokens[0]     : "";
                const option_description& d = desc.find(option_name, false,
                                                        false, false);

                variable_value& v = m[option_name];
                if (v.defaulted()) {
                    // 这里的显式赋值会删除默认值
                    v = variable_value();
                }

                d.semantic()->parse(v.value(), options.options[i].value, utf8);

                v.value_semantic_ = d.semantic();

                // 该选项不是合成，并且显式提供了该值。 忽略此选项的值以便后续调用“store”。 
				//我们将其存储到临时集中，以便允许在* this *'store'调用内进行多次分配。
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



        // 其次，应用默认值并存储所需选项。
        const vector<std::shared_ptr<option_description> >& all = desc.options();
        for(i = 0; i < all.size(); ++i)
        {
            const option_description& d = *all[i];
            string key = d.key("");
            // FIXME：这个逻辑依赖于option_description内部的知识。 如果选项说明包含'*'，则'key'为空。 在这种情况下，默认值根本没有意义。
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

            // 如果这是必需选项，请添加空值
            if (d.semantic()->is_required()) {

                //对于以多种方式指定的选项名称，例如 在命令行，配置文件等上，以下优先规则适用：“ - ”>（“ - ”或“/”）>“”通过单次调用length（）可以方便地设置优先级
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
