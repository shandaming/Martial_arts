/*
 * Copyright (C) 2019
 */

#ifndef CMD_VALUE_SEMANTIC_H
#define CMD_VALUE_SEMANTIC_H

#include <string>
#include <vector>
#include <typeinfo>
#include <limits>
#include <any>
#include <functional>

// 值语义
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
	typed_value(T* store_to) :
		store_to_(store_to), composing_(false), implicit_(false), 
		multitoken_(false), zero_tokens_(false), required_(false) {} 

    typed_value* default_value(const T& v)
    {
        default_value_ = std::any(v);
        default_value_as_text_ = static_cast<std::string>(v);
        return this;
    }

    typed_value* default_value(const T& v, const std::string& textual)
    {
        default_value_ = std::any(v);
        default_value_as_text_ = textual;
        return this;
    }

    typed_value* implicit_value(const T &v)
    {
        implicit__value_ = std::any(v);
        implicit__value_as_text_ =
        static_cast<std::string>(v);
        return this;
    }

    typed_value* value_name(const std::string& name)
    {
        value_name_ = name;
        return this;
    }

        /** 指定隐式值，如果给出了选项，则将使用该值，但没有相邻值。 使用这意味着显式值是可选的，但如果给定，则必须严格地与选项相邻，即：' -  font'或'--option = value'。 给'-o'或'--option'将导致隐含值被应用。 与上述重载不同，类型'T'不需要为ostream提供operator <<，但是必须由用户提供默认值的文本表示。
        */
    typed_value* implicit_value(const T &v, const std::string& textual)
    {
        implicit__value_ = std::any(v);
        implicit__value_as_text_ = textual;
        return this;
    }

        /** 指定在确定最终值时要调用的函数。 */
    typed_value* notifier(std::function<void(const T&)> f)
    {
        notifier_ = f;
        return this;
    }

        /** 指定值正在合成。 有关说明，请参阅'is_composing'方法。
        */
    typed_value* composing()
    {
        composing_ = true;
        return this;
    }

        /** 指定该值可以跨越多个标记。 
        */
    typed_value* multitoken()
    {
        multitoken_ = true;
        return this;
    }

        /** 指定不能提供任何标记作为此选项的值，这意味着只有该选项的presense是重要的。 要使此选项有用，要么'validate'功能应该是专用的，要么也应该使用'implicit_value'方法。 在大多数情况下，您可以使用'bool_switch'函数而不是使用此方法。 */
    typed_value* zero_tokens() 
    {
        zero_tokens_ = true;
        return this;
    }
            
        /** 指定必须出现该值。 */
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
       if (zero_tokens_ || implicit__value_.has_value()) {
            return 0;
        } else {
            return 1;
        }
    }

    uint32_t max_tokens() const {
        if (multitoken_) {
            return std::numeric_limits<uint32_t>::max;
        } else if (zero_tokens_) {
            return 0;
        } else {
            return 1;
        }
    }

    bool is_required() const { return required_; }

        /** 创建'validator'类的实例并调用其operator（）来执行实际转换。 */
    void xparse(std::any& value_store, 
                const std::vector<std::string>& new_tokens) 
            const;

        /** 如果通过先前调用'default_value'指定了默认值，则将该值存储到'value_store'中。 如果存储了默认值，则返回true。
        */
    virtual bool apply_default(std::any& value_store) const
    {
        if (!default_value_.has_value()) {
            return false;
        } else {
            value_store = default_value_;
            return true;
        }
    }

        /** 如果在创建* this时指定了存储值变量的地址，则将值存储在那里。 否则，什么都不做。 */
    void notify(const std::any& value_store) const;

public: // typed_value_base overrides
        
    const std::type_info& value_type() const
    {
        return typeid(T);
    }
        

private:
    T* store_to_;
        
        // 默认值存储为std :: any而不是boost :: optional以避免不必要的实例化。
    std::string value_name_;
    std::any default_value_;
    std::string default_value_as_text_;
    std::any implicit__value_;
    std::string implicit__value_as_text_;
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

    /** 与'value <bool>'函数的工作方式相同，但创建的value_semantic不接受任何显式值。 因此，如果命令行中存在该选项，则该值将为“true”。
    */
typed_value<bool>*
bool_switch();

    /** @overload
    */
typed_value<bool>*    
bool_switch(bool* v);


extern  std::string arg;

template<class T, class charT>
std::string typed_value<T, charT>::name() const
{
	std::string const& var = (value_name_.has_value() ? value_name_ : arg);
	if (!implicit__value_.empty() && !implicit__value_as_text_.empty()) 
	{
		std::string msg = "[=" + var + "(=" + implicit__value_as_text_ + 
			")]";
		if (default_value_.has_value() && default_value_as_text_.has_value())
		{
				 msg += " (=" + default_value_as_text_ + ")";
		}
		return msg;
	}
	else if (default_value_.has_value() && default_value_as_text_.has_value()) 
	{
		return var + " (=" + default_value_as_text_ + ")";
	}
	else 
	{
		return var;
	}
}

template<class T, class charT>
void typed_value<T, charT>::notify(const std::any& value_store) const
{
	const T* value = std::any_cast<T>(&value_store);
	if (store_to_) 
	{
		*store_to_ = *value;
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
	if (new_tokens.empty() && !implicit__value_.empty())
	{
		value_store = implicit__value_;
	}
	else
	{
		validate(value_store, new_tokens, (T*)0, 0);
	}
}

template<class T>
typed_value<T>* value()
{ 
	return value<T>(0);
}

template<class T>
typed_value<T>* value(T* v)
{
	typed_value<T>* r = new typed_value<T>(v);
	return r;
}

#endif
