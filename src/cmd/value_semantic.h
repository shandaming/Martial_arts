
#ifndef BOOST_VALUE_SEMANTIC_HPP_VP_2004_02_24
#define BOOST_VALUE_SEMANTIC_HPP_VP_2004_02_24

#include <string>
#include <vector>
#include <typeinfo>
#include <limits>
#include <any>
#include <functional>

class value_semantic 
{
public:
	virtual std::string name() const = 0;

    virtual uint32_t min_tokens() const = 0;
	virtual uint32_t max_tokens() const = 0;

    virtual bool is_composing() const = 0;

    virtual bool is_required() const = 0;
        
    virtual void parse(std::any& value_store, 
			const std::vector<std::string>& new_tokens,
			bool utf8) const = 0;

    virtual bool apply_default(std::any& value_store) const = 0;
                                   
    virtual void notify(const std::any& value_store) const = 0;
        
    virtual ~value_semantic() {}
};

class value_semantic_codecvt_helper : public value_semantic 
{
protected:
	virtual void xparse(std::any& value_store, 
			const std::vector<std::string>& new_tokens) const = 0;
private:
	void parse(std::any& value_store, 
			const std::vector<std::string>& new_tokens,
			bool utf8) const override;
};

class untyped_value: public value_semantic_codecvt_helper
{
public:
	untyped_value(bool zero_tokens = false)
		: zero_tokens_(zero_tokens) {}

	std::string name() const;

    uint32_t min_tokens() const;
    uint32_t max_tokens() const;

    bool is_composing() const { return false; }

    bool is_required() const { return false; }
        
    void xparse(std::any& value_store,
			const std::vector<std::string>& new_tokens) const;

    bool apply_default(std::any&) const { return false; }

    void notify(const std::any&) const {}        
private:
    bool zero_tokens_;
};


class typed_value_base 
{
public:
	virtual const std::type_info& value_type() const = 0;
	virtual ~typed_value_base() {}
};


template<class T>
class typed_value : public value_semantic_codecvt_helper,
	public typed_value_base
{
public:
	typed_value(T* store_to) :\
		m_store_to(store_to), composing_(false), implicit_(false), 
		multitoken_(false), zero_tokens_(false), required_(false) {} 

    typed_value* default_value(const T& v)
    {
        m_default_value = std::any(v);
        m_default_value_as_text = boost::lexical_cast<std::string>(v);
        return this;
    }

    typed_value* default_value(const T& v, const std::string& textual)
    {
        m_default_value = std::any(v);
        m_default_value_as_text = textual;
        return this;
    }

    typed_value* implicit_value(const T &v)
    {
        implicit__value = std::any(v);
        implicit__value_as_text =
        boost::lexical_cast<std::string>(v);
        return this;
    }

    typed_value* value_name(const std::string& name)
    {
        m_value_name = name;
        return this;
    }

        /** Specifies an implicit value, which will be used
            if the option is given, but without an adjacent value.
            Using this implies that an explicit value is optional, but if
            given, must be strictly adjacent to the option, i.e.: '-ovalue'
            or '--option=value'.  Giving '-o' or '--option' will cause the
            implicit value to be applied.
            Unlike the above overload, the type 'T' need not provide
            operator<< for ostream, but textual representation of default
            value must be provided by the user.
        */
    typed_value* implicit_value(const T &v, const std::string& textual)
    {
        implicit__value = std::any(v);
        implicit__value_as_text = textual;
        return this;
    }

        /** Specifies a function to be called when the final value
            is determined. */
    typed_value* notifier(std::function<void(const T&)> f)
    {
        notifier_ = f;
        return this;
    }

        /** Specifies that the value is composing. See the 'is_composing' 
            method for explanation. 
        */
    typed_value* composing()
    {
        composing_ = true;
        return this;
    }

        /** Specifies that the value can span multiple tokens. 
        */
    typed_value* multitoken()
    {
        multitoken_ = true;
        return this;
    }

        /** Specifies that no tokens may be provided as the value of
            this option, which means that only presense of the option
            is significant. For such option to be useful, either the
            'validate' function should be specialized, or the 
            'implicit_value' method should be also used. In most
            cases, you can use the 'bool_switch' function instead of
            using this method. */
    typed_value* zero_tokens() 
    {
        zero_tokens_ = true;
        return this;
    }
            
        /** Specifies that the value must occur. */
    typed_value* required()
    {
        required_ = true;
        return this;
    }

public: // value semantic overrides

    std::string name() const;

    bool is_composing() const { return composing_; }

    uint32_t min_tokens() const
    {
       if (zero_tokens_ || !implicit__value.empty()) {
            return 0;
        } else {
            return 1;
        }
    }

    uint32_t max_tokens() const {
        if (multitoken_) {
            return std::numeric_limits<uint32_t>::max BOOST_PREVENT_MACRO_SUBSTITUTION();
        } else if (zero_tokens_) {
            return 0;
        } else {
            return 1;
        }
    }

    bool is_required() const { return required_; }

        /** Creates an instance of the 'validator' class and calls
            its operator() to perform the actual conversion. */
    void xparse(std::any& value_store, 
                const std::vector< std::basic_string<charT> >& new_tokens) 
            const;

        /** If default value was specified via previous call to 
            'default_value', stores that value into 'value_store'.
            Returns true if default value was stored.
        */
    virtual bool apply_default(std::any& value_store) const
    {
        if (m_default_value.empty()) {
            return false;
        } else {
            value_store = m_default_value;
            return true;
        }
    }

        /** If an address of variable to store value was specified
            when creating *this, stores the value there. Otherwise,
            does nothing. */
    void notify(const std::any& value_store) const;

public: // typed_value_base overrides
        
    const std::type_info& value_type() const
    {
        return typeid(T);
    }
        

private:
    T* m_store_to;
        
        // Default value is stored as std::any and not
        // as boost::optional to avoid unnecessary instantiations.
    std::string m_value_name;
    std::any m_default_value;
    std::string m_default_value_as_text;
    std::any implicit__value;
    std::string implicit__value_as_text;
    bool composing_, implicit_, multitoken_, zero_tokens_, required_;
    std::function<void(const T&)> notifier_;
};


    /** 创建一个typed_value <T>实例。 此函数是为特定类型创建value_semantic实例的主要方法，
	稍后可将其传递给“option_description”构造函数。 当另外希望将option的值存储到程序变量中时，使用第二个重载。
    */
template<class T>
typed_value<T>* value();

    /** @overload 
    */
template<class T>
typed_value<T>* value(T* v);

    /** Works the same way as the 'value<bool>' function, but the created
        value_semantic won't accept any explicit value. So, if the option 
        is present on the command line, the value will be 'true'.
    */
typed_value<bool>*
bool_switch();

    /** @overload
    */
typed_value<bool>*    
bool_switch(bool* v);


//#include "boost/program_options/detail/value_semantic.hpp"
namespace boost { template<class T> class optional; }


extern  std::string arg;

template<class T, class charT>
std::string typed_value<T, charT>::name() const
{
	std::string const& var = (m_value_name.empty() ? arg : m_value_name);
	if (!implicit__value.empty() && !implicit__value_as_text.empty()) 
	{
		std::string msg = "[=" + var + "(=" + implicit__value_as_text + 
			")]";
		if (!m_default_value.empty() && !m_default_value_as_text.empty())
		{
				 msg += " (=" + m_default_value_as_text + ")";
		}
		return msg;
	}
	else if (!m_default_value.empty() && !m_default_value_as_text.empty()) 
	{
		return var + " (=" + m_default_value_as_text + ")";
	}
	else 
	{
		return var;
	}
}

template<class T, class charT>
void typed_value<T, charT>::notify(const std::any& value_store) const
{
	const T* value = boost::any_cast<T>(&value_store);
	if (m_store_to) 
	{
		*m_store_to = *value;
	}
	if (notifier_) 
	{
		notifier_(*value);
	}
}

namespace validators {
	 /* If v.size() > 1, throw validation_error.
		If v.size() == 1, return v.front()
		Otherwise, returns a reference to a statically allocated
		empty string if 'allow_empty' and throws validation_error
		otherwise. */
template<class charT>
const std::basic_string<charT>& get_single_string(
		const std::vector<std::basic_string<charT> >& v,
		bool allow_empty = false)
{
	static std::basic_string<charT> empty;
	if (v.size() > 1)
	{
		boost::throw_exception(validation_error(validation_error::multiple_values_not_allowed));
	}
	else if (v.size() == 1)
	{
		return v.front();
	}
	else if (!allow_empty)
	{
		boost::throw_exception(validation_error(validation_error::at_least_one_value_required));
	}
	return empty;
}

	 /* Throws multiple_occurrences if 'value' is not empty. */
void check_first_occurrence(const std::any& value);
}

using namespace validators;

	 /** Validates 's' and updates 'v'.
		 @pre 'v' is either empty or in the state assigned by the previous
		 invocation of 'validate'.
		 The target type is specified via a parameter which has the type of
		 pointer to the desired type. This is workaround for compilers without
		 partial template ordering, just like the last 'long/int' parameter.
	 */
template<class T, class charT>
void validate(std::any& v, const std::vector< std::basic_string<charT> >& xs, T*, long)
{
	validators::check_first_occurrence(v);
	std::basic_string<charT> s(validators::get_single_string(xs));
	try 
	{
		v = any(lexical_cast<T>(s));
	}
	catch (const bad_lexical_cast&) 
	{
		boost::throw_exception(invalid_option_value(s));
	}
}

void validate(std::any& v, const std::vector<std::string>& xs, bool*, int);

	 /** Validates sequences. Allows multiple values per option occurrence
		and multiple occurrences. */
template<class T, class charT>
void validate(std::any& v, const std::vector<std::basic_string<charT> >& s, std::vector<T>*, int)
{
	if (v.empty()) 
	{
		v = std::any(std::vector<T>());
	}
	std::vector<T>* tv = boost::any_cast<std::vector<T>>(&v);
	assert(NULL != tv);
	for (uint32_t i = 0; i < s.size(); ++i)
	{
		try 
		{
				 /* We call validate so that if user provided
					a validator for class T, we use it even
					when parsing vector<T>.  */
			std::any a;
			std::vector<std::basic_string<charT> > cv;
			cv.push_back(s[i]);
			validate(a, cv, (T*)0, 0);
			tv->push_back(boost::any_cast<T>(a));
		}
		catch (const bad_lexical_cast& /*e*/) 
		{
			boost::throw_exception(invalid_option_value(s[i]));
		}
	}
}

	 /** Validates optional arguments. */
template<class T, class charT>
void validate(std::any& v, const std::vector<std::basic_string<charT> >& s, boost::optional<T>*, int)
{
	validators::check_first_occurrence(v);
	validators::get_single_string(s);
	std::any a;
	validate(a, s, (T*)0, 0);
	v = std::any(boost::optional<T>(boost::any_cast<T>(a)));
}

template<class T, class charT>
void typed_value<T, charT>::xparse(std::any& value_store,
		const std::vector<std::basic_string<charT> >& new_tokens) const
{
		 // If no tokens were given, and the option accepts an implicit
		 // value, then assign the implicit value as the stored value;
		 // otherwise, validate the user-provided token(s).
	if (new_tokens.empty() && !implicit__value.empty())
	{
		value_store = implicit__value;
	}
	else
	{
		validate(value_store, new_tokens, (T*)0, 0);
	}
}

template<class T>
typed_value<T>* value()
{ 
	return boost::program_options::value<T>(0);
}

template<class T>
typed_value<T>* value(T* v)
{
	typed_value<T>* r = new typed_value<T>(v);
	return r;
}

#endif
