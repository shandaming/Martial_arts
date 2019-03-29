
#ifndef BOOST_VARIABLES_MAP_VP_2003_05_19
#define BOOST_VARIABLES_MAP_VP_2003_05_19

#include <string>
#include <map>
#include <set>
#include <any>
#include <memory>
#include <any>


    template<class charT>
    class basic_parsed_options;

    class value_semantic;
    class variables_map;
    
    void store(const basic_parsed_options<char>& options, variables_map& m,  bool utf8 = false);



    /** Runs all 'notify' function for options in 'm'. */
	void notify(variables_map& m) { m.notify(); }

    class  variable_value {
    public:
        variable_value() : defaulted_(false) {}
        variable_value(const std::any& xv, bool xdefaulted)
        : v_(xv), defaulted_(xdefaulted) {}

       template<typename T>
       const T& as() const { return std::any_cast<const T&>(v); }

       template<typename T>
       T& as() { return std::any_cast<T&>(v); }

	   bool empty() const { return v_.empty(); }

	   bool defaulted() const { return defaulted_; }

	   const std::any& value() const { return v_; }
	   std::any& value() { return v_; }
    private:
        std::any v_;
        bool defaulted_;

        std::shared_ptr<const value_semantic> value_semantic_;

        friend void store(const basic_parsed_options<char>& options, variables_map& m, bool);

        friend class variables_map;
    };

    class  abstract_variables_map {
    public:
		abstract_variables_map() : next_(0) {}
        abstract_variables_map(const abstract_variables_map* next) : next_(next) {}

        virtual ~abstract_variables_map() {}

        const variable_value& operator[](const std::string& name) const;

		void next(abstract_variables_map* next) { next_ = next; }
    private:
        /** 返回存储在* this中的变量'name'的值，否则返回空值。 */
        virtual const variable_value& get(const std::string& name) const = 0;

        const abstract_variables_map* next_;
    };

    class  variables_map : public abstract_variables_map,
                               public std::map<std::string, variable_value>
    {
    public:
		variables_map() {}
		variables_map(const abstract_variables_map* next) : abstract_variables_map(next) {}

        const variable_value& operator[](const std::string& name) const
        { return abstract_variables_map::operator[](name); }

		void clear();
        void notify();
    private:
        const variable_value& get(const std::string& name) const;

        std::set<std::string> final_;

        friend void store(const basic_parsed_options<char>& options,
                          variables_map& xm,
                          bool utf8);

        /** 必需选项的名称，由可以访问options_description的解析器填充。 映射值是每个相应选项的“规范”名称。 
		当选项不存在时，这对于创建诊断消息很有用。 */
        std::map<std::string, std::string> required_;
    };


#endif
