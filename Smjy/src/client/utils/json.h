/*
 * Copyright (C) 2019
 */

#ifndef UTILS_JSON_H
#define UTILS_JSON_H

enum class value_t : std::uint8_t
{
	null,           
	object,          
	array,           
	string, 
	boolean,         
	number_integer,  
	number_unsigned,  
	number_float,   
	discarded       
};

inline bool operator<(const value_t& l, const value_t& r)
{
	static constexpr std::array<std::uint8_t, 8> order =  {{
		0 /* null */, 3 /* object */, 4 /* array */, 5 /* string */,
        1 /* boolean */, 2 /* integer */, 2 /* unsigned */, 2 /* float */
        }};
	const auto l_index = static_cast<size_t>(l);
	const auto r_index = static_cast<size_t>(r);
	return l_index < order.size() and r_index < order.size() and
		order[l_index] < order[r_index];
}

using basic_json_tpl_declaration = 
template<template<typename U, typename V, typename... Args> 
	class Object_type = std::map,
	template<typename U, typename... Args> 
	class Array_type = std::vector,
    class String_type = std::string, 
	class Boolean_type = bool,
    class Number_integer_type = std::int64_t,
    class Number_unsigned_type = std::uint64_t,
    class Number_float_type = double,
    template<typename U> 
	class Allocator_type = std::allocator,
    template<typename T, typename SFINAE = void> 
	class JSONSerializer = adl_serializer>;

basic_json_tpl_declaration
class basic_json
{
    template<detail::value_t> friend struct detail::external_constructor;
    friend ::nlohmann::json_pointer<basic_json>;
    friend ::nlohmann::detail::parser<basic_json>;
    friend ::nlohmann::detail::serializer<basic_json>;
    template<typename BasicJsonType>
    friend class ::nlohmann::detail::iter_impl;
    template<typename BasicJsonType, typename CharType>
    friend class ::nlohmann::detail::binary_writer;
    template<typename BasicJsonType, typename SAX>
    friend class ::nlohmann::detail::binary_reader;
    template<typename BasicJsonType>
    friend class ::nlohmann::detail::json_sax_dom_parser;
    template<typename BasicJsonType>
    friend class ::nlohmann::detail::json_sax_dom_callback_parser;

    /// workaround type for MSVC
    using basic_json_t = NLOHMANN_BASIC_JSON_TPL;

    // convenience aliases for types residing in namespace detail;
    using lexer = ::nlohmann::detail::lexer<basic_json>;
    using parser = ::nlohmann::detail::parser<basic_json>;

    using primitive_iterator_t = ::nlohmann::detail::primitive_iterator_t;
    template<typename BasicJsonType>
    using internal_iterator = ::nlohmann::detail::internal_iterator<BasicJsonType>;
    template<typename BasicJsonType>
    using iter_impl = ::nlohmann::detail::iter_impl<BasicJsonType>;
    template<typename Iterator>
    using iteration_proxy = ::nlohmann::detail::iteration_proxy<Iterator>;
    template<typename Base> using json_reverse_iterator = ::nlohmann::detail::json_reverse_iterator<Base>;

    template<typename CharType>
    using output_adapter_t = ::nlohmann::detail::output_adapter_t<CharType>;

    using binary_reader = ::nlohmann::detail::binary_reader<basic_json>;
    template<typename CharType> using binary_writer = ::nlohmann::detail::binary_writer<basic_json, CharType>;

    using serializer = ::nlohmann::detail::serializer<basic_json>;

public:
	/// JSON Pointer, see @ref nlohmann::json_pointer
    using json_pointer = ::nlohmann::json_pointer<basic_json>;
    template<typename T, typename SFINAE>
    using json_serializer = JSONSerializer<T, SFINAE>;
    /// how to treat decoding errors
    using error_handler_t = detail::error_handler_t;
    /// helper type for initializer lists of basic_json values
    using initializer_list_t = std::initializer_list<detail::json_ref<basic_json>>;

    using input_format_t = detail::input_format_t;
    /// SAX interface type, see @ref nlohmann::json_sax
    using json_sax_t = json_sax<basic_json>;

    using exception = detail::exception;
    /// @copydoc detail::parse_error
    using parse_error = detail::parse_error;
    /// @copydoc detail::invalid_iterator
    using invalid_iterator = detail::invalid_iterator;
    /// @copydoc detail::type_error
    using type_error = detail::type_error;
    /// @copydoc detail::out_of_range
    using out_of_range = detail::out_of_range;
    /// @copydoc detail::other_error
    using other_error = detail::other_error;


    using value_type = basic_json;

    /// the type of an element reference
    using reference = value_type&;
    /// the type of an element const reference
    using const_reference = const value_type&;

    /// a type to represent differences between iterators
    using difference_type = std::ptrdiff_t;
    /// a type to represent container sizes
    using size_type = std::size_t;

    /// the allocator type
    using allocator_type = Allocator_type<basic_json>;

    /// the type of an element pointer
    using pointer = typename std::allocator_traits<allocator_type>::pointer;
    /// the type of an element const pointer
    using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

    /// an iterator for a basic_json container
    using iterator = iter_impl<basic_json>;
    /// a const iterator for a basic_json container
    using const_iterator = iter_impl<const basic_json>;
    /// a reverse iterator for a basic_json container
    using reverse_iterator = json_reverse_iterator<typename basic_json::iterator>;
    /// a const reverse iterator for a basic_json container
    using const_reverse_iterator = json_reverse_iterator<typename basic_json::const_iterator>;

    using object_comparator_t = std::less<>;

  
    using object_t = Object_type<String_type,
          basic_json,
          object_comparator_t,
          Allocator_type<std::pair<const String_type,
          basic_json>>>;
    using array_t = Array_type<basic_json, Allocator_type<basic_json>>;
    using string_t = String_type;
    using boolean_t = Boolean_type;
    using number_integer_t = Number_integer_type;
    using number_unsigned_t = Number_unsigned_type;
    using number_float_t = Number_float_type;

    static allocator_type get_allocator()
    {
        return allocator_type();
    }

    using parse_event_t = typename parser::parse_event_t; 
    using parser_callback_t = typename parser::parser_callback_t;

    basic_json(const value_t v) : type_(v), value_(v)
    {
        assert_invariant();
    }

    basic_json(std::nullptr_t = nullptr) : basic_json(value_t::null)
    {
        assert_invariant();
    }

    template <typename CompatibleType,
			 typename U = detail::uncvref_t<CompatibleType>,
			 detail::enable_if_t<
				 not detail::is_basic_json<U>::value and 
				 detail::is_compatible_type<basic_json_t, U>::value, int> = 0>
    basic_json(CompatibleType && val) ((
                JSONSerializer<U>::to_json(std::declval<basic_json_t&>(),
                                           std::forward<CompatibleType>(val))))
    {
        JSONSerializer<U>::to_json(*this, std::forward<CompatibleType>(val));
        assert_invariant();
    }

    template <typename BasicJsonType,
              detail::enable_if_t<
                  detail::is_basic_json<BasicJsonType>::value and not std::is_same<basic_json, BasicJsonType>::value, int> = 0>
    basic_json(const BasicJsonType& val)
    {
        using other_boolean_t = typename BasicJsonType::boolean_t;
        using other_number_float_t = typename BasicJsonType::number_float_t;
        using other_number_integer_t = typename BasicJsonType::number_integer_t;
        using other_number_unsigned_t = typename BasicJsonType::number_unsigned_t;
        using other_string_t = typename BasicJsonType::string_t;
        using other_object_t = typename BasicJsonType::object_t;
        using other_array_t = typename BasicJsonType::array_t;

        switch (val.type())
        {
            case value_t::boolean:
                JSONSerializer<other_boolean_t>::to_json(*this, val.template get<other_boolean_t>());
                break;
            case value_t::number_float:
                JSONSerializer<other_number_float_t>::to_json(*this, val.template get<other_number_float_t>());
                break;
            case value_t::number_integer:
                JSONSerializer<other_number_integer_t>::to_json(*this, val.template get<other_number_integer_t>());
                break;
            case value_t::number_unsigned:
                JSONSerializer<other_number_unsigned_t>::to_json(*this, val.template get<other_number_unsigned_t>());
                break;
            case value_t::string:
                JSONSerializer<other_string_t>::to_json(*this, val.template get_ref<const other_string_t&>());
                break;
            case value_t::object:
                JSONSerializer<other_object_t>::to_json(*this, val.template get_ref<const other_object_t&>());
                break;
            case value_t::array:
                JSONSerializer<other_array_t>::to_json(*this, val.template get_ref<const other_array_t&>());
                break;
            case value_t::null:
                *this = nullptr;
                break;
            case value_t::discarded:
                type_ = value_t::discarded;
                break;
        }
        assert_invariant();
    }

    basic_json(initializer_list_t init,
               bool type_deduction = true,
               value_t manual_type = value_t::array)
    {
        // check if each element is an array with two elements whose first
        // element is a string
        bool is_an_object = std::all_of(init.begin(), init.end(),
                                        [](const detail::json_ref<basic_json>& element_ref)
        {
            return (element_ref->is_array() and element_ref->size() == 2 and (*element_ref)[0].is_string());
        });

        // adjust type if type deduction is not wanted
        if (not type_deduction)
        {
            // if array is wanted, do not create an object though possible
            if (manual_type == value_t::array)
            {
                is_an_object = false;
            }

            // if object is wanted but impossible, throw an exception
            if (JSON_UNLIKELY(manual_type == value_t::object and not is_an_object))
            {
                JSON_THROW(type_error::create(301, "cannot create object from initializer list"));
            }
        }

        if (is_an_object)
        {
            // the initializer list is a list of pairs -> create object
            type_ = value_t::object;
            value_ = value_t::object;

            std::for_each(init.begin(), init.end(), [this](const detail::json_ref<basic_json>& element_ref)
            {
                auto element = element_ref.moved_or_copied();
                value_.object->emplace(
                    std::move(*((*element.value_.array)[0].value_.string)),
                    std::move((*element.value_.array)[1]));
            });
        }
        else
        {
            // the initializer list describes an array -> create array
            type_ = value_t::array;
            value_.array = create<array_t>(init.begin(), init.end());
        }

        assert_invariant();
    }

    static basic_json array(initializer_list_t init = {})
    {
        return basic_json(init, false, value_t::array);
    }

    basic_json(size_type cnt, const basic_json& val) : 
		type_(value_t::array)
    {
        value_.array = create<array_t>(cnt, val);
        assert_invariant();
    }


    template<class InputIT, typename std::enable_if<
                 std::is_same<InputIT, typename basic_json_t::iterator>::value or
                 std::is_same<InputIT, typename basic_json_t::const_iterator>::value, int>::type = 0>
    basic_json(InputIT first, InputIT last)
    {
        assert(first.m_object != nullptr);
        assert(last.m_object != nullptr);

        // make sure iterator fits the current value
        if (JSON_UNLIKELY(first.m_object != last.m_object))
        {
            JSON_THROW(invalid_iterator::create(201, "iterators are not compatible"));
        }

        // copy type from first iterator
        type_ = first.m_object->type_;

        // check if iterator range is complete for primitive values
        switch (type_)
        {
            case value_t::boolean:
            case value_t::number_float:
            case value_t::number_integer:
            case value_t::number_unsigned:
            case value_t::string:
            {
                if (JSON_UNLIKELY(not first.m_it.primitive_iterator.is_begin()
                                  or not last.m_it.primitive_iterator.is_end()))
                {
                    JSON_THROW(invalid_iterator::create(204, "iterators out of range"));
                }
                break;
            }

            default:
                break;
        }

        switch (type_)
        {
            case value_t::number_integer:
            {
                value_.number_integer = first.m_object->value_.number_integer;
                break;
            }

            case value_t::number_unsigned:
            {
                value_.number_unsigned = first.m_object->value_.number_unsigned;
                break;
            }

            case value_t::number_float:
            {
                value_.number_float = first.m_object->value_.number_float;
                break;
            }

            case value_t::boolean:
            {
                value_.boolean = first.m_object->value_.boolean;
                break;
            }

            case value_t::string:
            {
                value_ = *first.m_object->value_.string;
                break;
            }

            case value_t::object:
            {
                value_.object = create<object_t>(first.m_it.object_iterator,
                                                  last.m_it.object_iterator);
                break;
            }

            case value_t::array:
            {
                value_.array = create<array_t>(first.m_it.array_iterator,
                                                last.m_it.array_iterator);
                break;
            }

            default:
                JSON_THROW(invalid_iterator::create(206, "cannot construct with iterators from " +
                                                    std::string(first.m_object->type_name())));
        }

        assert_invariant();
    }

    basic_json(const detail::json_ref<basic_json>& ref) :
		basic_json(ref.moved_or_copied()) {}

    basic_json(const basic_json& other) : type_(other.type_)
    {
        // check of passed value is valid
        other.assert_invariant();

        switch (type_)
        {
			case value_t::object:
				value_ = *other.value_.object;
				break;
            case value_t::array:
                value_ = *other.value_.array;
                break;
            case value_t::string:
                value_ = *other.value_.string;
                break;
            case value_t::boolean:
                value_ = other.value_.boolean;
                break;
            case value_t::number_integer:
                value_ = other.value_.number_integer;
                break;
            case value_t::number_unsigned:
                value_ = other.value_.number_unsigned;
                break;
            case value_t::number_float:
                value_ = other.value_.number_float;
                break;
            default:
                break;
        }
        assert_invariant();
    }

 
    basic_json(basic_json&& other) : type_(std::move(other.type_)),
          value_(std::move(other.value_))
    {
        // check that passed value is valid
        other.assert_invariant();

        // invalidate payload
        other.type_ = value_t::null;
        other.value_ = {};

        assert_invariant();
    }


    basic_json& operator=(basic_json other)  (
        std::is_nothrow_move_constructible<value_t>::value and
        std::is_nothrow_move_assignable<value_t>::value and
        std::is_nothrow_move_constructible<json_value>::value and
        std::is_nothrow_move_assignable<json_value>::value
    )
    {
        // check that passed value is valid
        other.assert_invariant();

        using std::swap;
        swap(type_, other.type_);
        swap(value_, other.value_);

        assert_invariant();
        return *this;
    }


    ~basic_json() 
    {
        assert_invariant();
        value_.destroy(type_);
    }

    string_t dump(const int indent = -1,
                  const char indent_char = ' ',
                  const bool ensure_ascii = false,
                  const error_handler_t error_handler = error_handler_t::strict) const
    {
        string_t result;
        serializer s(detail::output_adapter<char, string_t>(result), indent_char, error_handler);

        if (indent >= 0)
        {
            s.dump(*this, true, ensure_ascii, static_cast<unsigned int>(indent));
        }
        else
        {
            s.dump(*this, false, ensure_ascii, 0);
        }

        return result;
    }


    constexpr value_t type() const { return type_; }

	// bool

    constexpr bool is_primitive() const 
    {
        return is_null() or is_string() or is_boolean() or is_number();
    }

    constexpr bool is_structured() const 
	{
		return is_array() or is_object();
    }

    constexpr bool is_null() const { return (type_ == value_t::null); }
 
    constexpr bool is_boolean() const \
	{
		return (type_ == value_t::boolean);
    }

    constexpr bool is_number() const 
    {
        return is_number_integer() or is_number_float();
    }

    constexpr bool is_number_integer() const 
    {
        return (type_ == value_t::number_integer or 
				type_ == value_t::number_unsigned);
    }

    constexpr bool is_number_unsigned() const 
    {
        return (type_ == value_t::number_unsigned);
    }

    constexpr bool is_number_float() const 
    {
        return (type_ == value_t::number_float);
    }

    constexpr bool is_object() const 
    {
        return (type_ == value_t::object);
    }

    constexpr bool is_array() const 
    {
        return (type_ == value_t::array);
    }

    constexpr bool is_string() const 
    {
        return (type_ == value_t::string);
    }
  
    constexpr bool is_discarded() const 
    {
        return (type_ == value_t::discarded);
    }
 
    constexpr operator value_t() const { return type_; }

    template<typename BasicJsonType, detail::enable_if_t<
                 std::is_same<typename std::remove_const<BasicJsonType>::type, basic_json_t>::value,
                 int> = 0>
    basic_json get() const
    {
        return *this;
    }


    template<typename BasicJsonType, detail::enable_if_t<
                 not std::is_same<BasicJsonType, basic_json>::value and
                 detail::is_basic_json<BasicJsonType>::value, int> = 0>
    BasicJsonType get() const
    {
        return *this;
    }


    template<typename ValueTypeCV, typename ValueType = detail::uncvref_t<ValueTypeCV>,
             detail::enable_if_t <
                 not detail::is_basic_json<ValueType>::value and
                 detail::has_from_json<basic_json_t, ValueType>::value and
                 not detail::has_non_default_from_json<basic_json_t, ValueType>::value,
                 int> = 0>
    ValueType get() const ((
                                       JSONSerializer<ValueType>::from_json(std::declval<const basic_json_t&>(), std::declval<ValueType&>())))
    {
        // we cannot static_assert on ValueTypeCV being non-const, because
        // there is support for get<const basic_json_t>(), which is why we
        // still need the uncvref
        static_assert(not std::is_reference<ValueTypeCV>::value,
                      "get() cannot be used with reference types, you might want to use get_ref()");
        static_assert(std::is_default_constructible<ValueType>::value,
                      "types must be DefaultConstructible when used with get()");

        ValueType ret;
        JSONSerializer<ValueType>::from_json(*this, ret);
        return ret;
    }


    template<typename ValueTypeCV, typename ValueType = detail::uncvref_t<ValueTypeCV>,
             detail::enable_if_t<not std::is_same<basic_json_t, ValueType>::value and
                                 detail::has_non_default_from_json<basic_json_t, ValueType>::value,
                                 int> = 0>
    ValueType get() const ((
                                       JSONSerializer<ValueTypeCV>::from_json(std::declval<const basic_json_t&>())))
    {
        static_assert(not std::is_reference<ValueTypeCV>::value,
                      "get() cannot be used with reference types, you might want to use get_ref()");
        return JSONSerializer<ValueTypeCV>::from_json(*this);
    }


    template<typename ValueType,
             detail::enable_if_t <
                 not detail::is_basic_json<ValueType>::value and
                 detail::has_from_json<basic_json_t, ValueType>::value,
                 int> = 0>
    ValueType & get_to(ValueType& v) const ((
                JSONSerializer<ValueType>::from_json(std::declval<const basic_json_t&>(), v)))
    {
        JSONSerializer<ValueType>::from_json(*this, v);
        return v;
    }


    template<typename PointerType, typename std::enable_if<
                 std::is_pointer<PointerType>::value, int>::type = 0>
    auto get_ptr()  -> decltype(std::declval<basic_json_t&>().get_impl_ptr(std::declval<PointerType>()))
    {
        // delegate the call to get_impl_ptr<>()
        return get_impl_ptr(static_cast<PointerType>(nullptr));
    }

    /*!
    @brief get a pointer value (implicit)
    @copydoc get_ptr()
    */
    template<typename PointerType, typename std::enable_if<
                 std::is_pointer<PointerType>::value and
                 std::is_const<typename std::remove_pointer<PointerType>::type>::value, int>::type = 0>
    constexpr auto get_ptr() const  -> decltype(std::declval<const basic_json_t&>().get_impl_ptr(std::declval<PointerType>()))
    {
        // delegate the call to get_impl_ptr<>() const
        return get_impl_ptr(static_cast<PointerType>(nullptr));
    }


    template<typename PointerType, typename std::enable_if<
                 std::is_pointer<PointerType>::value, int>::type = 0>
    auto get()  -> decltype(std::declval<basic_json_t&>().template get_ptr<PointerType>())
    {
        // delegate the call to get_ptr
        return get_ptr<PointerType>();
    }

    /*!
    @brief get a pointer value (explicit)
    @copydoc get()
    */
    template<typename PointerType, typename std::enable_if<
                 std::is_pointer<PointerType>::value, int>::type = 0>
    constexpr auto get() const  -> decltype(std::declval<const basic_json_t&>().template get_ptr<PointerType>())
    {
        // delegate the call to get_ptr
        return get_ptr<PointerType>();
    }


    template<typename ReferenceType, typename std::enable_if<
                 std::is_reference<ReferenceType>::value, int>::type = 0>
    ReferenceType get_ref()
    {
        // delegate call to get_ref_impl
        return get_ref_impl<ReferenceType>(*this);
    }

    /*!
    @brief get a reference value (implicit)
    @copydoc get_ref()
    */
    template<typename ReferenceType, typename std::enable_if<
                 std::is_reference<ReferenceType>::value and
                 std::is_const<typename std::remove_reference<ReferenceType>::type>::value, int>::type = 0>
    ReferenceType get_ref() const
    {
        // delegate call to get_ref_impl
        return get_ref_impl<ReferenceType>(*this);
    }

    template < typename ValueType, typename std::enable_if <
                   not std::is_pointer<ValueType>::value and
                   not std::is_same<ValueType, detail::json_ref<basic_json>>::value and
                   not std::is_same<ValueType, typename string_t::value_type>::value and
                   not detail::is_basic_json<ValueType>::value

#ifndef _MSC_VER  // fix for issue #167 operator<< ambiguity under VS2015
                   and not std::is_same<ValueType, std::initializer_list<typename string_t::value_type>>::value
#if defined(JSON_HAS_CPP_17) && defined(_MSC_VER) and _MSC_VER <= 1914
                   and not std::is_same<ValueType, typename std::string_view>::value
#endif
#endif
                   and detail::is_detected<detail::get_template_function, const basic_json_t&, ValueType>::value
                   , int >::type = 0 >
    operator ValueType() const
    {
        // delegate the call to get<>() const
        return get<ValueType>();
    }


    reference at(size_type idx)
    {
        // at only works for arrays
        if (JSON_LIKELY(is_array()))
        {
            JSON_TRY
            {
                return value_.array->at(idx);
            }
            JSON_CATCH (std::out_of_range&)
            {
                // create better exception explanation
                JSON_THROW(out_of_range::create(401, "array index " + std::to_string(idx) + " is out of range"));
            }
        }
        else
        {
            JSON_THROW(type_error::create(304, "cannot use at() with " + std::string(type_name())));
        }
    }


    const_reference at(size_type idx) const
    {
        // at only works for arrays
        if (JSON_LIKELY(is_array()))
        {
            JSON_TRY
            {
                return value_.array->at(idx);
            }
            JSON_CATCH (std::out_of_range&)
            {
                // create better exception explanation
                JSON_THROW(out_of_range::create(401, "array index " + std::to_string(idx) + " is out of range"));
            }
        }
        else
        {
            JSON_THROW(type_error::create(304, "cannot use at() with " + std::string(type_name())));
        }
    }

 
    reference at(const typename object_t::key_type& key)
    {
        // at only works for objects
        if (JSON_LIKELY(is_object()))
        {
            JSON_TRY
            {
                return value_.object->at(key);
            }
            JSON_CATCH (std::out_of_range&)
            {
                // create better exception explanation
                JSON_THROW(out_of_range::create(403, "key '" + key + "' not found"));
            }
        }
        else
        {
            JSON_THROW(type_error::create(304, "cannot use at() with " + std::string(type_name())));
        }
    }

  
    const_reference at(const typename object_t::key_type& key) const
    {
        // at only works for objects
        if (JSON_LIKELY(is_object()))
        {
            JSON_TRY
            {
                return value_.object->at(key);
            }
            JSON_CATCH (std::out_of_range&)
            {
                // create better exception explanation
                JSON_THROW(out_of_range::create(403, "key '" + key + "' not found"));
            }
        }
        else
        {
            JSON_THROW(type_error::create(304, "cannot use at() with " + std::string(type_name())));
        }
    }


reference operator[](size_type idx)
    {
        // implicitly convert null value to an empty array
        if (is_null())
        {
            type_ = value_t::array;
            value_.array = create<array_t>();
            assert_invariant();
        }

        // operator[] only works for arrays
        if (JSON_LIKELY(is_array()))
        {
            // fill up array with null values if given idx is outside range
            if (idx >= value_.array->size())
            {
                value_.array->insert(value_.array->end(),
                                      idx - value_.array->size() + 1,
                                      basic_json());
            }

            return value_.array->operator[](idx);
        }

        JSON_THROW(type_error::create(305, "cannot use operator[] with a numeric argument with " + std::string(type_name())));
    }

 
    const_reference operator[](size_type idx) const
    {
        // const operator[] only works for arrays
        if (JSON_LIKELY(is_array()))
        {
            return value_.array->operator[](idx);
        }

        JSON_THROW(type_error::create(305, "cannot use operator[] with a numeric argument with " + std::string(type_name())));
    }

  
    reference operator[](const typename object_t::key_type& key)
    {
        // implicitly convert null value to an empty object
        if (is_null())
        {
            type_ = value_t::object;
            value_.object = create<object_t>();
            assert_invariant();
        }

        // operator[] only works for objects
        if (JSON_LIKELY(is_object()))
        {
            return value_.object->operator[](key);
        }

        JSON_THROW(type_error::create(305, "cannot use operator[] with a string argument with " + std::string(type_name())));
    }


    const_reference operator[](const typename object_t::key_type& key) const
    {
        // const operator[] only works for objects
        if (JSON_LIKELY(is_object()))
        {
            assert(value_.object->find(key) != value_.object->end());
            return value_.object->find(key)->second;
        }

        JSON_THROW(type_error::create(305, "cannot use operator[] with a string argument with " + std::string(type_name())));
    }

 
    template<typename T>
    reference operator[](T* key)
    {
        // implicitly convert null to object
        if (is_null())
        {
            type_ = value_t::object;
            value_ = value_t::object;
            assert_invariant();
        }

        // at only works for objects
        if (JSON_LIKELY(is_object()))
        {
            return value_.object->operator[](key);
        }

        JSON_THROW(type_error::create(305, "cannot use operator[] with a string argument with " + std::string(type_name())));
    }

  
    template<typename T>
    const_reference operator[](T* key) const
    {
        // at only works for objects
        if (JSON_LIKELY(is_object()))
        {
            assert(value_.object->find(key) != value_.object->end());
            return value_.object->find(key)->second;
        }

        JSON_THROW(type_error::create(305, "cannot use operator[] with a string argument with " + std::string(type_name())));
    }

 
    template<class ValueType, typename std::enable_if<
                 std::is_convertible<basic_json_t, ValueType>::value, int>::type = 0>
    ValueType value(const typename object_t::key_type& key, const ValueType& default_value) const
    {
        // at only works for objects
        if (JSON_LIKELY(is_object()))
        {
            // if key is found, return value and given default value otherwise
            const auto it = find(key);
            if (it != end())
            {
                return *it;
            }

            return default_value;
        }

        JSON_THROW(type_error::create(306, "cannot use value() with " + std::string(type_name())));
    }

    /*!
    @brief overload for a default value of type const char*
    @copydoc basic_json::value(const typename object_t::key_type&, const ValueType&) const
    */
    string_t value(const typename object_t::key_type& key, const char* default_value) const
    {
        return value(key, string_t(default_value));
    }

 
    template<class ValueType, typename std::enable_if<
                 std::is_convertible<basic_json_t, ValueType>::value, int>::type = 0>
    ValueType value(const json_pointer& ptr, const ValueType& default_value) const
    {
        // at only works for objects
        if (JSON_LIKELY(is_object()))
        {
            // if pointer resolves a value, return it or use default value
            JSON_TRY
            {
                return ptr.get_checked(this);
            }
            JSON_INTERNAL_CATCH (out_of_range&)
            {
                return default_value;
            }
        }

        JSON_THROW(type_error::create(306, "cannot use value() with " + std::string(type_name())));
    }

    /*!
    @brief overload for a default value of type const char*
    @copydoc basic_json::value(const json_pointer&, ValueType) const
    */
    string_t value(const json_pointer& ptr, const char* default_value) const
    {
        return value(ptr, string_t(default_value));
    }

 
    reference front() { return *begin(); }

    const_reference front() const { return *cbegin(); }

    reference back()
    {
        auto tmp = end();
        --tmp;
        return *tmp;
    }

    const_reference back() const
    {
        auto tmp = cend();
        --tmp;
        return *tmp;
    }


    template<class IteratorType, typename std::enable_if<
                 std::is_same<IteratorType, typename basic_json_t::iterator>::value or
                 std::is_same<IteratorType, typename basic_json_t::const_iterator>::value, int>::type
             = 0>
    IteratorType erase(IteratorType pos)
    {
        // make sure iterator fits the current value
        if (JSON_UNLIKELY(this != pos.m_object))
        {
            JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
        }

        IteratorType result = end();

        switch (type_)
        {
            case value_t::boolean:
            case value_t::number_float:
            case value_t::number_integer:
            case value_t::number_unsigned:
            case value_t::string:
            {
                if (JSON_UNLIKELY(not pos.m_it.primitive_iterator.is_begin()))
                {
                    JSON_THROW(invalid_iterator::create(205, "iterator out of range"));
                }

                if (is_string())
                {
                    Allocator_type<string_t> alloc;
                    std::allocator_traits<decltype(alloc)>::destroy(alloc, value_.string);
                    std::allocator_traits<decltype(alloc)>::deallocate(alloc, value_.string, 1);
                    value_.string = nullptr;
                }

                type_ = value_t::null;
                assert_invariant();
                break;
            }

            case value_t::object:
            {
                result.m_it.object_iterator = value_.object->erase(pos.m_it.object_iterator);
                break;
            }

            case value_t::array:
            {
                result.m_it.array_iterator = value_.array->erase(pos.m_it.array_iterator);
                break;
            }

            default:
                JSON_THROW(type_error::create(307, "cannot use erase() with " + std::string(type_name())));
        }

        return result;
    }

 
    template<class IteratorType, typename std::enable_if<
                 std::is_same<IteratorType, typename basic_json_t::iterator>::value or
                 std::is_same<IteratorType, typename basic_json_t::const_iterator>::value, int>::type
             = 0>
    IteratorType erase(IteratorType first, IteratorType last)
    {
        // make sure iterator fits the current value
        if (JSON_UNLIKELY(this != first.m_object or this != last.m_object))
        {
            JSON_THROW(invalid_iterator::create(203, "iterators do not fit current value"));
        }

        IteratorType result = end();

        switch (type_)
        {
            case value_t::boolean:
            case value_t::number_float:
            case value_t::number_integer:
            case value_t::number_unsigned:
            case value_t::string:
            {
                if (JSON_LIKELY(not first.m_it.primitive_iterator.is_begin()
                                or not last.m_it.primitive_iterator.is_end()))
                {
                    JSON_THROW(invalid_iterator::create(204, "iterators out of range"));
                }

                if (is_string())
                {
                    Allocator_type<string_t> alloc;
                    std::allocator_traits<decltype(alloc)>::destroy(alloc, value_.string);
                    std::allocator_traits<decltype(alloc)>::deallocate(alloc, value_.string, 1);
                    value_.string = nullptr;
                }

                type_ = value_t::null;
                assert_invariant();
                break;
            }

            case value_t::object:
            {
                result.m_it.object_iterator = value_.object->erase(first.m_it.object_iterator,
                                              last.m_it.object_iterator);
                break;
            }

            case value_t::array:
            {
                result.m_it.array_iterator = value_.array->erase(first.m_it.array_iterator,
                                             last.m_it.array_iterator);
                break;
            }

            default:
                JSON_THROW(type_error::create(307, "cannot use erase() with " + std::string(type_name())));
        }

        return result;
    }

 
    size_type erase(const typename object_t::key_type& key)
    {
        // this erase only works for objects
        if (JSON_LIKELY(is_object()))
        {
            return value_.object->erase(key);
        }

        JSON_THROW(type_error::create(307, "cannot use erase() with " + std::string(type_name())));
    }

 
    void erase(const size_type idx)
    {
        // this erase only works for arrays
        if (JSON_LIKELY(is_array()))
        {
            if (JSON_UNLIKELY(idx >= size()))
            {
                JSON_THROW(out_of_range::create(401, "array index " + std::to_string(idx) + " is out of range"));
            }

            value_.array->erase(value_.array->begin() + static_cast<difference_type>(idx));
        }
        else
        {
            JSON_THROW(type_error::create(307, "cannot use erase() with " + std::string(type_name())));
        }
    }

  
    template<typename KeyT>
    iterator find(KeyT&& key)
    {
        auto result = end();

        if (is_object())
        {
            result.m_it.object_iterator = value_.object->find(std::forward<KeyT>(key));
        }

        return result;
    }

    template<typename KeyT>
    const_iterator find(KeyT&& key) const
    {
        auto result = cend();

        if (is_object())
        {
            result.m_it.object_iterator = value_.object->find(std::forward<KeyT>(key));
        }

        return result;
    }

  
    template<typename KeyT>
    size_type count(KeyT&& key) const
    {
        // return 0 for all nonobject types
        return is_object() ? value_.object->count(std::forward<KeyT>(key)) : 0;
    }

 
    iterator begin() 
    {
        iterator result(this);
        result.set_begin();
        return result;
    }

    const_iterator begin() const { return cbegin(); }

    const_iterator cbegin() const 
    {
        const_iterator result(this);
        result.set_begin();
        return result;
    }
 
    iterator end() 
    {
        iterator result(this);
        result.set_end();
        return result;
    }

    const_iterator end() const { return cend(); }
  
    const_iterator cend() const 
    {
        const_iterator result(this);
        result.set_end();
        return result;
    }
  
    reverse_iterator rbegin() { return reverse_iterator(end()); }

    const_reverse_iterator rbegin() const { return crbegin(); }
  
    reverse_iterator rend() { return reverse_iterator(begin()); }

    const_reverse_iterator rend() const { return crend(); }
 
    const_reverse_iterator crbegin() const 
    {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const 
    {
        return const_reverse_iterator(cbegin());
    }

    JSON_DEPRECATED
    static iteration_proxy<iterator> iterator_wrapper(reference ref) 
    {
        return ref.items();
    }

    /*!
    @copydoc iterator_wrapper(reference)
    */
    JSON_DEPRECATED
    static iteration_proxy<const_iterator> iterator_wrapper(const_reference ref) 
    {
        return ref.items();
    }

  
    iteration_proxy<iterator> items() 
    {
        return iteration_proxy<iterator>(*this);
    }

    /*!
    @copydoc items()
    */
    iteration_proxy<const_iterator> items() const 
    {
        return iteration_proxy<const_iterator>(*this);
    }

 
    bool empty() const 
    {
        switch (type_)
        {
            case value_t::null:
                return true;
            case value_t::array:
                return value_.array->empty();
            case value_t::object:
                return value_.object->empty();
            default:
                return false;
        }
    }

    size_type size() const 
    {
        switch (type_)
        {
            case value_t::null:
                return 0;
            case value_t::array:
                return value_.array->size();
            case value_t::object:
                return value_.object->size();
            default:
                return 1;
        }
    }

    size_type max_size() const 
    {
        switch (type_)
        {
            case value_t::array:
                return value_.array->max_size();
            case value_t::object:
                return value_.object->max_size();
            default:
                return size();
        }
    }
  
    void clear() 
    {
        switch (type_)
        {
            case value_t::number_integer:
                value_.number_integer = 0;
                break;
            case value_t::number_unsigned:
                value_.number_unsigned = 0;
                break;
            case value_t::number_float:
                value_.number_float = 0.0;
                break;
            case value_t::boolean:
                value_.boolean = false;
                break;
            case value_t::string:
                value_.string->clear();
                break;
            case value_t::array:
                value_.array->clear();
                break;
            case value_t::object:
                value_.object->clear();
                break;
            default:
                break;
        }
    }

    void push_back(basic_json&& val)
    {
        // push_back only works for null objects or arrays
        if (JSON_UNLIKELY(not(is_null() or is_array())))
        {
            JSON_THROW(type_error::create(308, "cannot use push_back() with " + std::string(type_name())));
        }

        // transform null object into an array
        if (is_null())
        {
            type_ = value_t::array;
            value_ = value_t::array;
            assert_invariant();
        }

        // add element to array (move semantics)
        value_.array->push_back(std::move(val));
        // invalidate object
        val.type_ = value_t::null;
    }

    /*!
    @brief add an object to an array
    @copydoc push_back(basic_json&&)
    */
    reference operator+=(basic_json&& val)
    {
        push_back(std::move(val));
        return *this;
    }

    /*!
    @brief add an object to an array
    @copydoc push_back(basic_json&&)
    */
    void push_back(const basic_json& val)
    {
        // push_back only works for null objects or arrays
        if (JSON_UNLIKELY(not(is_null() or is_array())))
        {
            JSON_THROW(type_error::create(308, "cannot use push_back() with " + std::string(type_name())));
        }

        // transform null object into an array
        if (is_null())
        {
            type_ = value_t::array;
            value_ = value_t::array;
            assert_invariant();
        }

        // add element to array
        value_.array->push_back(val);
    }

    /*!
    @brief add an object to an array
    @copydoc push_back(basic_json&&)
    */
    reference operator+=(const basic_json& val)
    {
        push_back(val);
        return *this;
    }

  
    void push_back(const typename object_t::value_type& val)
    {
        // push_back only works for null objects or objects
        if (JSON_UNLIKELY(not(is_null() or is_object())))
        {
            JSON_THROW(type_error::create(308, "cannot use push_back() with " + std::string(type_name())));
        }

        // transform null object into an object
        if (is_null())
        {
            type_ = value_t::object;
            value_ = value_t::object;
            assert_invariant();
        }

        // add element to array
        value_.object->insert(val);
    }

    /*!
    @brief add an object to an object
    @copydoc push_back(const typename object_t::value_type&)
    */
    reference operator+=(const typename object_t::value_type& val)
    {
        push_back(val);
        return *this;
    }


    void push_back(initializer_list_t init)
    {
        if (is_object() and init.size() == 2 and (*init.begin())->is_string())
        {
            basic_json&& key = init.begin()->moved_or_copied();
            push_back(typename object_t::value_type(
                          std::move(key.get_ref<string_t&>()), (init.begin() + 1)->moved_or_copied()));
        }
        else
        {
            push_back(basic_json(init));
        }
    }

    /*!
    @brief add an object to an object
    @copydoc push_back(initializer_list_t)
    */
    reference operator+=(initializer_list_t init)
    {
        push_back(init);
        return *this;
    }


    template<class... Args>
    void emplace_back(Args&& ... args)
    {
        // emplace_back only works for null objects or arrays
        if (JSON_UNLIKELY(not(is_null() or is_array())))
        {
            JSON_THROW(type_error::create(311, "cannot use emplace_back() with " + std::string(type_name())));
        }

        // transform null object into an array
        if (is_null())
        {
            type_ = value_t::array;
            value_ = value_t::array;
            assert_invariant();
        }

        // add element to array (perfect forwarding)
        value_.array->emplace_back(std::forward<Args>(args)...);
    }

 
    template<class... Args>
    std::pair<iterator, bool> emplace(Args&& ... args)
    {
        // emplace only works for null objects or arrays
        if (JSON_UNLIKELY(not(is_null() or is_object())))
        {
            JSON_THROW(type_error::create(311, "cannot use emplace() with " + std::string(type_name())));
        }

        // transform null object into an object
        if (is_null())
        {
            type_ = value_t::object;
            value_ = value_t::object;
            assert_invariant();
        }

        // add element to array (perfect forwarding)
        auto res = value_.object->emplace(std::forward<Args>(args)...);
        // create result iterator and set iterator to the result of emplace
        auto it = begin();
        it.m_it.object_iterator = res.first;

        // return pair of iterator and boolean
        return {it, res.second};
    }

    /// Helper for insertion of an iterator
    /// @note: This uses std::distance to support GCC 4.8,
    ///        see https://github.com/nlohmann/json/pull/1257
    template<typename... Args>
    iterator insert_iterator(const_iterator pos, Args&& ... args)
    {
        iterator result(this);
        assert(value_.array != nullptr);

        auto insert_pos = std::distance(value_.array->begin(), pos.m_it.array_iterator);
        value_.array->insert(pos.m_it.array_iterator, std::forward<Args>(args)...);
        result.m_it.array_iterator = value_.array->begin() + insert_pos;

        // This could have been written as:
        // result.m_it.array_iterator = value_.array->insert(pos.m_it.array_iterator, cnt, val);
        // but the return value of insert is missing in GCC 4.8, so it is written this way instead.

        return result;
    }

 
    iterator insert(const_iterator pos, const basic_json& val)
    {
        // insert only works for arrays
        if (JSON_LIKELY(is_array()))
        {
            // check if iterator pos fits to this JSON value
            if (JSON_UNLIKELY(pos.m_object != this))
            {
                JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
            }

            // insert to array and return iterator
            return insert_iterator(pos, val);
        }

        JSON_THROW(type_error::create(309, "cannot use insert() with " + std::string(type_name())));
    }

    iterator insert(const_iterator pos, basic_json&& val)
    {
        return insert(pos, val);
    }


    iterator insert(const_iterator pos, size_type cnt, const basic_json& val)
    {
        // insert only works for arrays
        if (JSON_LIKELY(is_array()))
        {
            // check if iterator pos fits to this JSON value
            if (JSON_UNLIKELY(pos.m_object != this))
            {
                JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
            }

            // insert to array and return iterator
            return insert_iterator(pos, cnt, val);
        }

        JSON_THROW(type_error::create(309, "cannot use insert() with " + std::string(type_name())));
    }


    iterator insert(const_iterator pos, const_iterator first, const_iterator last)
    {
        // insert only works for arrays
        if (JSON_UNLIKELY(not is_array()))
        {
            JSON_THROW(type_error::create(309, "cannot use insert() with " + std::string(type_name())));
        }

        // check if iterator pos fits to this JSON value
        if (JSON_UNLIKELY(pos.m_object != this))
        {
            JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
        }

        // check if range iterators belong to the same JSON object
        if (JSON_UNLIKELY(first.m_object != last.m_object))
        {
            JSON_THROW(invalid_iterator::create(210, "iterators do not fit"));
        }

        if (JSON_UNLIKELY(first.m_object == this))
        {
            JSON_THROW(invalid_iterator::create(211, "passed iterators may not belong to container"));
        }

        // insert to array and return iterator
        return insert_iterator(pos, first.m_it.array_iterator, last.m_it.array_iterator);
    }


    iterator insert(const_iterator pos, initializer_list_t ilist)
    {
        // insert only works for arrays
        if (JSON_UNLIKELY(not is_array()))
        {
            JSON_THROW(type_error::create(309, "cannot use insert() with " + std::string(type_name())));
        }

        // check if iterator pos fits to this JSON value
        if (JSON_UNLIKELY(pos.m_object != this))
        {
            JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
        }

        // insert to array and return iterator
        return insert_iterator(pos, ilist.begin(), ilist.end());
    }

 
    void insert(const_iterator first, const_iterator last)
    {
        // insert only works for objects
        if (JSON_UNLIKELY(not is_object()))
        {
            JSON_THROW(type_error::create(309, "cannot use insert() with " + std::string(type_name())));
        }

        // check if range iterators belong to the same JSON object
        if (JSON_UNLIKELY(first.m_object != last.m_object))
        {
            JSON_THROW(invalid_iterator::create(210, "iterators do not fit"));
        }

        // passed iterators must belong to objects
        if (JSON_UNLIKELY(not first.m_object->is_object()))
        {
            JSON_THROW(invalid_iterator::create(202, "iterators first and last must point to objects"));
        }

        value_.object->insert(first.m_it.object_iterator, last.m_it.object_iterator);
    }


    void update(const_reference j)
    {
        // implicitly convert null value to an empty object
        if (is_null())
        {
            type_ = value_t::object;
            value_.object = create<object_t>();
            assert_invariant();
        }

        if (JSON_UNLIKELY(not is_object()))
        {
            JSON_THROW(type_error::create(312, "cannot use update() with " + std::string(type_name())));
        }
        if (JSON_UNLIKELY(not j.is_object()))
        {
            JSON_THROW(type_error::create(312, "cannot use update() with " + std::string(j.type_name())));
        }

        for (auto it = j.cbegin(); it != j.cend(); ++it)
        {
            value_.object->operator[](it.key()) = it.value();
        }
    }


    void update(const_iterator first, const_iterator last)
    {
        // implicitly convert null value to an empty object
        if (is_null())
        {
            type_ = value_t::object;
            value_.object = create<object_t>();
            assert_invariant();
        }

        if (JSON_UNLIKELY(not is_object()))
        {
            JSON_THROW(type_error::create(312, "cannot use update() with " + std::string(type_name())));
        }

        // check if range iterators belong to the same JSON object
        if (JSON_UNLIKELY(first.m_object != last.m_object))
        {
            JSON_THROW(invalid_iterator::create(210, "iterators do not fit"));
        }

        // passed iterators must belong to objects
        if (JSON_UNLIKELY(not first.m_object->is_object()
                          or not last.m_object->is_object()))
        {
            JSON_THROW(invalid_iterator::create(202, "iterators first and last must point to objects"));
        }

        for (auto it = first; it != last; ++it)
        {
            value_.object->operator[](it.key()) = it.value();
        }
    }


    void swap(reference other)  (
        std::is_nothrow_move_constructible<value_t>::value and
        std::is_nothrow_move_assignable<value_t>::value and
        std::is_nothrow_move_constructible<json_value>::value and
        std::is_nothrow_move_assignable<json_value>::value
    )
    {
        std::swap(type_, other.type_);
        std::swap(value_, other.value_);
        assert_invariant();
    }

    void swap(array_t& other)
    {
        // swap only works for arrays
        if (JSON_LIKELY(is_array()))
        {
            std::swap(*(value_.array), other);
        }
        else
        {
            JSON_THROW(type_error::create(310, "cannot use swap() with " + std::string(type_name())));
        }
    }


    void swap(object_t& other)
    {
        // swap only works for objects
        if (JSON_LIKELY(is_object()))
        {
            std::swap(*(value_.object), other);
        }
        else
        {
            JSON_THROW(type_error::create(310, "cannot use swap() with " + std::string(type_name())));
        }
    }

 
    void swap(string_t& other)
    {
        // swap only works for strings
        if (JSON_LIKELY(is_string()))
        {
            std::swap(*(value_.string), other);
        }
        else
        {
            JSON_THROW(type_error::create(310, "cannot use swap() with " + std::string(type_name())));
        }
    }

    friend bool operator==(const_reference lhs, const_reference rhs) 
    {
        const auto lhs_type = lhs.type();
        const auto rhs_type = rhs.type();

        if (lhs_type == rhs_type)
        {
            switch (lhs_type)
            {
                case value_t::array:
                    return (*lhs.value_.array == *rhs.value_.array);
                case value_t::object:
                    return (*lhs.value_.object == *rhs.value_.object);
                case value_t::null:
                    return true;
                case value_t::string:
                    return (*lhs.value_.string == *rhs.value_.string);
                case value_t::boolean:
                    return (lhs.value_.boolean == rhs.value_.boolean);
                case value_t::number_integer:
                    return (lhs.value_.number_integer == rhs.value_.number_integer);
                case value_t::number_unsigned:
                    return (lhs.value_.number_unsigned == rhs.value_.number_unsigned);
                case value_t::number_float:
                    return (lhs.value_.number_float == rhs.value_.number_float);
                default:
                    return false;
            }
        }
        else if (lhs_type == value_t::number_integer and rhs_type == value_t::number_float)
        {
            return (static_cast<number_float_t>(lhs.value_.number_integer) == rhs.value_.number_float);
        }
        else if (lhs_type == value_t::number_float and rhs_type == value_t::number_integer)
        {
            return (lhs.value_.number_float == static_cast<number_float_t>(rhs.value_.number_integer));
        }
        else if (lhs_type == value_t::number_unsigned and rhs_type == value_t::number_float)
        {
            return (static_cast<number_float_t>(lhs.value_.number_unsigned) == rhs.value_.number_float);
        }
        else if (lhs_type == value_t::number_float and rhs_type == value_t::number_unsigned)
        {
            return (lhs.value_.number_float == static_cast<number_float_t>(rhs.value_.number_unsigned));
        }
        else if (lhs_type == value_t::number_unsigned and rhs_type == value_t::number_integer)
        {
            return (static_cast<number_integer_t>(lhs.value_.number_unsigned) == rhs.value_.number_integer);
        }
        else if (lhs_type == value_t::number_integer and rhs_type == value_t::number_unsigned)
        {
            return (lhs.value_.number_integer == static_cast<number_integer_t>(rhs.value_.number_unsigned));
        }

        return false;
    }

    /*!
    @brief comparison: equal
    @copydoc operator==(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator==(const_reference lhs, const ScalarType rhs) 
    {
        return (lhs == basic_json(rhs));
    }

    /*!
    @brief comparison: equal
    @copydoc operator==(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator==(const ScalarType lhs, const_reference rhs) 
    {
        return (basic_json(lhs) == rhs);
    }


    friend bool operator!=(const_reference lhs, const_reference rhs) 
    {
        return not (lhs == rhs);
    }

    /*!
    @brief comparison: not equal
    @copydoc operator!=(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator!=(const_reference lhs, const ScalarType rhs) 
    {
        return (lhs != basic_json(rhs));
    }

    /*!
    @brief comparison: not equal
    @copydoc operator!=(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator!=(const ScalarType lhs, const_reference rhs) 
    {
        return (basic_json(lhs) != rhs);
    }


    friend bool operator<(const_reference lhs, const_reference rhs) 
    {
        const auto lhs_type = lhs.type();
        const auto rhs_type = rhs.type();

        if (lhs_type == rhs_type)
        {
            switch (lhs_type)
            {
                case value_t::array:
                    return (*lhs.value_.array) < (*rhs.value_.array);
                case value_t::object:
                    return *lhs.value_.object < *rhs.value_.object;
                case value_t::null:
                    return false;
                case value_t::string:
                    return *lhs.value_.string < *rhs.value_.string;
                case value_t::boolean:
                    return lhs.value_.boolean < rhs.value_.boolean;
                case value_t::number_integer:
                    return lhs.value_.number_integer < rhs.value_.number_integer;
                case value_t::number_unsigned:
                    return lhs.value_.number_unsigned < rhs.value_.number_unsigned;
                case value_t::number_float:
                    return lhs.value_.number_float < rhs.value_.number_float;
                default:
                    return false;
            }
        }
        else if (lhs_type == value_t::number_integer and rhs_type == value_t::number_float)
        {
            return static_cast<number_float_t>(lhs.value_.number_integer) < rhs.value_.number_float;
        }
        else if (lhs_type == value_t::number_float and rhs_type == value_t::number_integer)
        {
            return lhs.value_.number_float < static_cast<number_float_t>(rhs.value_.number_integer);
        }
        else if (lhs_type == value_t::number_unsigned and rhs_type == value_t::number_float)
        {
            return static_cast<number_float_t>(lhs.value_.number_unsigned) < rhs.value_.number_float;
        }
        else if (lhs_type == value_t::number_float and rhs_type == value_t::number_unsigned)
        {
            return lhs.value_.number_float < static_cast<number_float_t>(rhs.value_.number_unsigned);
        }
        else if (lhs_type == value_t::number_integer and rhs_type == value_t::number_unsigned)
        {
            return lhs.value_.number_integer < static_cast<number_integer_t>(rhs.value_.number_unsigned);
        }
        else if (lhs_type == value_t::number_unsigned and rhs_type == value_t::number_integer)
        {
            return static_cast<number_integer_t>(lhs.value_.number_unsigned) < rhs.value_.number_integer;
        }

        // We only reach this line if we cannot compare values. In that case,
        // we compare types. Note we have to call the operator explicitly,
        // because MSVC has problems otherwise.
        return operator<(lhs_type, rhs_type);
    }

    /*!
    @brief comparison: less than
    @copydoc operator<(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator<(const_reference lhs, const ScalarType rhs) 
    {
        return (lhs < basic_json(rhs));
    }

    /*!
    @brief comparison: less than
    @copydoc operator<(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator<(const ScalarType lhs, const_reference rhs) 
    {
        return (basic_json(lhs) < rhs);
    }


    friend bool operator<=(const_reference lhs, const_reference rhs) 
    {
        return not (rhs < lhs);
    }

    /*!
    @brief comparison: less than or equal
    @copydoc operator<=(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator<=(const_reference lhs, const ScalarType rhs) 
    {
        return (lhs <= basic_json(rhs));
    }

    /*!
    @brief comparison: less than or equal
    @copydoc operator<=(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator<=(const ScalarType lhs, const_reference rhs) 
    {
        return (basic_json(lhs) <= rhs);
    }

 
    friend bool operator>(const_reference lhs, const_reference rhs) 
    {
        return not (lhs <= rhs);
    }

    /*!
    @brief comparison: greater than
    @copydoc operator>(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator>(const_reference lhs, const ScalarType rhs) 
    {
        return (lhs > basic_json(rhs));
    }

    /*!
    @brief comparison: greater than
    @copydoc operator>(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator>(const ScalarType lhs, const_reference rhs) 
    {
        return (basic_json(lhs) > rhs);
    }


    friend bool operator>=(const_reference lhs, const_reference rhs) 
    {
        return not (lhs < rhs);
    }

    /*!
    @brief comparison: greater than or equal
    @copydoc operator>=(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator>=(const_reference lhs, const ScalarType rhs) 
    {
        return (lhs >= basic_json(rhs));
    }

    /*!
    @brief comparison: greater than or equal
    @copydoc operator>=(const_reference, const_reference)
    */
    template<typename ScalarType, typename std::enable_if<
                 std::is_scalar<ScalarType>::value, int>::type = 0>
    friend bool operator>=(const ScalarType lhs, const_reference rhs) 
    {
        return (basic_json(lhs) >= rhs);
    }


    friend std::ostream& operator<<(std::ostream& o, const basic_json& j)
    {
        // read width member and use it as indentation parameter if nonzero
        const bool pretty_print = (o.width() > 0);
        const auto indentation = (pretty_print ? o.width() : 0);

        // reset width to 0 for subsequent calls to this stream
        o.width(0);

        // do the actual serialization
        serializer s(detail::output_adapter<char>(o), o.fill());
        s.dump(j, pretty_print, false, static_cast<unsigned int>(indentation));
        return o;
    }


    JSON_DEPRECATED
    friend std::ostream& operator>>(const basic_json& j, std::ostream& o)
    {
        return o << j;
    }


    static basic_json parse(detail::input_adapter&& i,
                            const parser_callback_t cb = nullptr,
                            const bool allow_exceptions = true)
    {
        basic_json result;
        parser(i, cb, allow_exceptions).parse(true, result);
        return result;
    }

    static bool accept(detail::input_adapter&& i)
    {
        return parser(i).accept(true);
    }


    template <typename SAX>
    static bool sax_parse(detail::input_adapter&& i, SAX* sax,
                          input_format_t format = input_format_t::json,
                          const bool strict = true)
    {
        assert(sax);
        switch (format)
        {
            case input_format_t::json:
                return parser(std::move(i)).sax_parse(sax, strict);
            default:
                return detail::binary_reader<basic_json, SAX>(std::move(i)).sax_parse(format, sax, strict);
        }
    }


    template<class IteratorType, typename std::enable_if<
                 std::is_base_of<
                     std::random_access_iterator_tag,
                     typename std::iterator_traits<IteratorType>::iterator_category>::value, int>::type = 0>
    static basic_json parse(IteratorType first, IteratorType last,
                            const parser_callback_t cb = nullptr,
                            const bool allow_exceptions = true)
    {
        basic_json result;
        parser(detail::input_adapter(first, last), cb, allow_exceptions).parse(true, result);
        return result;
    }

    template<class IteratorType, typename std::enable_if<
                 std::is_base_of<
                     std::random_access_iterator_tag,
                     typename std::iterator_traits<IteratorType>::iterator_category>::value, int>::type = 0>
    static bool accept(IteratorType first, IteratorType last)
    {
        return parser(detail::input_adapter(first, last)).accept(true);
    }

    template<class IteratorType, class SAX, typename std::enable_if<
                 std::is_base_of<
                     std::random_access_iterator_tag,
                     typename std::iterator_traits<IteratorType>::iterator_category>::value, int>::type = 0>
    static bool sax_parse(IteratorType first, IteratorType last, SAX* sax)
    {
        return parser(detail::input_adapter(first, last)).sax_parse(sax);
    }


    JSON_DEPRECATED
    friend std::istream& operator<<(basic_json& j, std::istream& i)
    {
        return operator>>(i, j);
    }

 
    friend std::istream& operator>>(std::istream& i, basic_json& j)
    {
        parser(detail::input_adapter(i)).parse(false, j);
        return i;
    }


    const char* type_name() const 
    {
        {
            switch (type_)
            {
                case value_t::null:
                    return "null";
                case value_t::object:
                    return "object";
                case value_t::array:
                    return "array";
                case value_t::string:
                    return "string";
                case value_t::boolean:
                    return "boolean";
                case value_t::discarded:
                    return "discarded";
                default:
                    return "number";
            }
        }
    }


    static std::vector<uint8_t> to_cbor(const basic_json& j)
    {
        std::vector<uint8_t> result;
        to_cbor(j, result);
        return result;
    }

    static void to_cbor(const basic_json& j, detail::output_adapter<uint8_t> o)
    {
        binary_writer<uint8_t>(o).write_cbor(j);
    }

    static void to_cbor(const basic_json& j, detail::output_adapter<char> o)
    {
        binary_writer<char>(o).write_cbor(j);
    }

    static std::vector<uint8_t> to_msgpack(const basic_json& j)
    {
        std::vector<uint8_t> result;
        to_msgpack(j, result);
        return result;
    }

    static void to_msgpack(const basic_json& j, detail::output_adapter<uint8_t> o)
    {
        binary_writer<uint8_t>(o).write_msgpack(j);
    }

    static void to_msgpack(const basic_json& j, detail::output_adapter<char> o)
    {
        binary_writer<char>(o).write_msgpack(j);
    }


    static std::vector<uint8_t> to_ubjson(const basic_json& j,
                                          const bool use_size = false,
                                          const bool use_type = false)
    {
        std::vector<uint8_t> result;
        to_ubjson(j, result, use_size, use_type);
        return result;
    }

    static void to_ubjson(const basic_json& j, detail::output_adapter<uint8_t> o,
                          const bool use_size = false, const bool use_type = false)
    {
        binary_writer<uint8_t>(o).write_ubjson(j, use_size, use_type);
    }

    static void to_ubjson(const basic_json& j, detail::output_adapter<char> o,
                          const bool use_size = false, const bool use_type = false)
    {
        binary_writer<char>(o).write_ubjson(j, use_size, use_type);
    }


    static std::vector<uint8_t> to_bson(const basic_json& j)
    {
        std::vector<uint8_t> result;
        to_bson(j, result);
        return result;
    }


    static void to_bson(const basic_json& j, detail::output_adapter<uint8_t> o)
    {
        binary_writer<uint8_t>(o).write_bson(j);
    }

    /*!
    @copydoc to_bson(const basic_json&, detail::output_adapter<uint8_t>)
    */
    static void to_bson(const basic_json& j, detail::output_adapter<char> o)
    {
        binary_writer<char>(o).write_bson(j);
    }



    static basic_json from_cbor(detail::input_adapter&& i,
                                const bool strict = true,
                                const bool allow_exceptions = true)
    {
        basic_json result;
        detail::json_sax_dom_parser<basic_json> sdp(result, allow_exceptions);
        const bool res = binary_reader(detail::input_adapter(i)).sax_parse(input_format_t::cbor, &sdp, strict);
        return res ? result : basic_json(value_t::discarded);
    }

    /*!
    @copydoc from_cbor(detail::input_adapter&&, const bool, const bool)
    */
    template<typename A1, typename A2,
             detail::enable_if_t<std::is_constructible<detail::input_adapter, A1, A2>::value, int> = 0>
    static basic_json from_cbor(A1 && a1, A2 && a2,
                                const bool strict = true,
                                const bool allow_exceptions = true)
    {
        basic_json result;
        detail::json_sax_dom_parser<basic_json> sdp(result, allow_exceptions);
        const bool res = binary_reader(detail::input_adapter(std::forward<A1>(a1), std::forward<A2>(a2))).sax_parse(input_format_t::cbor, &sdp, strict);
        return res ? result : basic_json(value_t::discarded);
    }


    static basic_json from_msgpack(detail::input_adapter&& i,
                                   const bool strict = true,
                                   const bool allow_exceptions = true)
    {
        basic_json result;
        detail::json_sax_dom_parser<basic_json> sdp(result, allow_exceptions);
        const bool res = binary_reader(detail::input_adapter(i)).sax_parse(input_format_t::msgpack, &sdp, strict);
        return res ? result : basic_json(value_t::discarded);
    }

    /*!
    @copydoc from_msgpack(detail::input_adapter&&, const bool, const bool)
    */
    template<typename A1, typename A2,
             detail::enable_if_t<std::is_constructible<detail::input_adapter, A1, A2>::value, int> = 0>
    static basic_json from_msgpack(A1 && a1, A2 && a2,
                                   const bool strict = true,
                                   const bool allow_exceptions = true)
    {
        basic_json result;
        detail::json_sax_dom_parser<basic_json> sdp(result, allow_exceptions);
        const bool res = binary_reader(detail::input_adapter(std::forward<A1>(a1), std::forward<A2>(a2))).sax_parse(input_format_t::msgpack, &sdp, strict);
        return res ? result : basic_json(value_t::discarded);
    }


    static basic_json from_ubjson(detail::input_adapter&& i,
                                  const bool strict = true,
                                  const bool allow_exceptions = true)
    {
        basic_json result;
        detail::json_sax_dom_parser<basic_json> sdp(result, allow_exceptions);
        const bool res = binary_reader(detail::input_adapter(i)).sax_parse(input_format_t::ubjson, &sdp, strict);
        return res ? result : basic_json(value_t::discarded);
    }

    /*!
    @copydoc from_ubjson(detail::input_adapter&&, const bool, const bool)
    */
    template<typename A1, typename A2,
             detail::enable_if_t<std::is_constructible<detail::input_adapter, A1, A2>::value, int> = 0>
    static basic_json from_ubjson(A1 && a1, A2 && a2,
                                  const bool strict = true,
                                  const bool allow_exceptions = true)
    {
        basic_json result;
        detail::json_sax_dom_parser<basic_json> sdp(result, allow_exceptions);
        const bool res = binary_reader(detail::input_adapter(std::forward<A1>(a1), std::forward<A2>(a2))).sax_parse(input_format_t::ubjson, &sdp, strict);
        return res ? result : basic_json(value_t::discarded);
    }


    static basic_json from_bson(detail::input_adapter&& i,
                                const bool strict = true,
                                const bool allow_exceptions = true)
    {
        basic_json result;
        detail::json_sax_dom_parser<basic_json> sdp(result, allow_exceptions);
        const bool res = binary_reader(detail::input_adapter(i)).sax_parse(input_format_t::bson, &sdp, strict);
        return res ? result : basic_json(value_t::discarded);
    }

    /*!
    @copydoc from_bson(detail::input_adapter&&, const bool, const bool)
    */
    template<typename A1, typename A2,
             detail::enable_if_t<std::is_constructible<detail::input_adapter, A1, A2>::value, int> = 0>
    static basic_json from_bson(A1 && a1, A2 && a2,
                                const bool strict = true,
                                const bool allow_exceptions = true)
    {
        basic_json result;
        detail::json_sax_dom_parser<basic_json> sdp(result, allow_exceptions);
        const bool res = binary_reader(detail::input_adapter(std::forward<A1>(a1), std::forward<A2>(a2))).sax_parse(input_format_t::bson, &sdp, strict);
        return res ? result : basic_json(value_t::discarded);
    }



    reference operator[](const json_pointer& ptr)
    {
        return ptr.get_unchecked(this);
    }

    const_reference operator[](const json_pointer& ptr) const
    {
        return ptr.get_unchecked(this);
    }

    reference at(const json_pointer& ptr)
    {
        return ptr.get_checked(this);
    }

    const_reference at(const json_pointer& ptr) const
    {
        return ptr.get_checked(this);
    }

    basic_json flatten() const
    {
        basic_json result(value_t::object);
        json_pointer::flatten("", *this, result);
        return result;
    }

    basic_json unflatten() const
    {
        return json_pointer::unflatten(*this);
    }

    basic_json patch(const basic_json& json_patch) const
    {
        // make a working copy to apply the patch to
        basic_json result = *this;

        // the valid JSON Patch operations
        enum class patch_operations {add, remove, replace, move, copy, test, invalid};

        const auto get_op = [](const std::string & op)
        {
            if (op == "add")
            {
                return patch_operations::add;
            }
            if (op == "remove")
            {
                return patch_operations::remove;
            }
            if (op == "replace")
            {
                return patch_operations::replace;
            }
            if (op == "move")
            {
                return patch_operations::move;
            }
            if (op == "copy")
            {
                return patch_operations::copy;
            }
            if (op == "test")
            {
                return patch_operations::test;
            }

            return patch_operations::invalid;
        };

        // wrapper for "add" operation; add value at ptr
        const auto operation_add = [&result](json_pointer & ptr, basic_json val)
        {
            // adding to the root of the target document means replacing it
            if (ptr.is_root())
            {
                result = val;
            }
            else
            {
                // make sure the top element of the pointer exists
                json_pointer top_pointer = ptr.top();
                if (top_pointer != ptr)
                {
                    result.at(top_pointer);
                }

                // get reference to parent of JSON pointer ptr
                const auto last_path = ptr.pop_back();
                basic_json& parent = result[ptr];

                switch (parent.type_)
                {
                    case value_t::null:
                    case value_t::object:
                    {
                        // use operator[] to add value
                        parent[last_path] = val;
                        break;
                    }

                    case value_t::array:
                    {
                        if (last_path == "-")
                        {
                            // special case: append to back
                            parent.push_back(val);
                        }
                        else
                        {
                            const auto idx = json_pointer::array_index(last_path);
                            if (JSON_UNLIKELY(static_cast<size_type>(idx) > parent.size()))
                            {
                                // avoid undefined behavior
                                JSON_THROW(out_of_range::create(401, "array index " + std::to_string(idx) + " is out of range"));
                            }

                            // default case: insert add offset
                            parent.insert(parent.begin() + static_cast<difference_type>(idx), val);
                        }
                        break;
                    }

                    // LCOV_EXCL_START
                    default:
                    {
                        // if there exists a parent it cannot be primitive
                        assert(false);
                    }
                        // LCOV_EXCL_STOP
                }
            }
        };

        // wrapper for "remove" operation; remove value at ptr
        const auto operation_remove = [&result](json_pointer & ptr)
        {
            // get reference to parent of JSON pointer ptr
            const auto last_path = ptr.pop_back();
            basic_json& parent = result.at(ptr);

            // remove child
            if (parent.is_object())
            {
                // perform range check
                auto it = parent.find(last_path);
                if (JSON_LIKELY(it != parent.end()))
                {
                    parent.erase(it);
                }
                else
                {
                    JSON_THROW(out_of_range::create(403, "key '" + last_path + "' not found"));
                }
            }
            else if (parent.is_array())
            {
                // note erase performs range check
                parent.erase(static_cast<size_type>(json_pointer::array_index(last_path)));
            }
        };

        // type check: top level value must be an array
        if (JSON_UNLIKELY(not json_patch.is_array()))
        {
            JSON_THROW(parse_error::create(104, 0, "JSON patch must be an array of objects"));
        }

        // iterate and apply the operations
        for (const auto& val : json_patch)
        {
            // wrapper to get a value for an operation
            const auto get_value = [&val](const std::string & op,
                                          const std::string & member,
                                          bool string_type) -> basic_json &
            {
                // find value
                auto it = val.value_.object->find(member);

                // context-sensitive error message
                const auto error_msg = (op == "op") ? "operation" : "operation '" + op + "'";

                // check if desired value is present
                if (JSON_UNLIKELY(it == val.value_.object->end()))
                {
                    JSON_THROW(parse_error::create(105, 0, error_msg + " must have member '" + member + "'"));
                }

                // check if result is of type string
                if (JSON_UNLIKELY(string_type and not it->second.is_string()))
                {
                    JSON_THROW(parse_error::create(105, 0, error_msg + " must have string member '" + member + "'"));
                }

                // no error: return value
                return it->second;
            };

            // type check: every element of the array must be an object
            if (JSON_UNLIKELY(not val.is_object()))
            {
                JSON_THROW(parse_error::create(104, 0, "JSON patch must be an array of objects"));
            }

            // collect mandatory members
            const std::string op = get_value("op", "op", true);
            const std::string path = get_value(op, "path", true);
            json_pointer ptr(path);

            switch (get_op(op))
            {
                case patch_operations::add:
                {
                    operation_add(ptr, get_value("add", "value", false));
                    break;
                }

                case patch_operations::remove:
                {
                    operation_remove(ptr);
                    break;
                }

                case patch_operations::replace:
                {
                    // the "path" location must exist - use at()
                    result.at(ptr) = get_value("replace", "value", false);
                    break;
                }

                case patch_operations::move:
                {
                    const std::string from_path = get_value("move", "from", true);
                    json_pointer from_ptr(from_path);

                    // the "from" location must exist - use at()
                    basic_json v = result.at(from_ptr);

                    // The move operation is functionally identical to a
                    // "remove" operation on the "from" location, followed
                    // immediately by an "add" operation at the target
                    // location with the value that was just removed.
                    operation_remove(from_ptr);
                    operation_add(ptr, v);
                    break;
                }

                case patch_operations::copy:
                {
                    const std::string from_path = get_value("copy", "from", true);
                    const json_pointer from_ptr(from_path);

                    // the "from" location must exist - use at()
                    basic_json v = result.at(from_ptr);

                    // The copy is functionally identical to an "add"
                    // operation at the target location using the value
                    // specified in the "from" member.
                    operation_add(ptr, v);
                    break;
                }

                case patch_operations::test:
                {
                    bool success = false;
                    JSON_TRY
                    {
                        // check if "value" matches the one at "path"
                        // the "path" location must exist - use at()
                        success = (result.at(ptr) == get_value("test", "value", false));
                    }
                    JSON_INTERNAL_CATCH (out_of_range&)
                    {
                        // ignore out of range errors: success remains false
                    }

                    // throw an exception if test fails
                    if (JSON_UNLIKELY(not success))
                    {
                        JSON_THROW(other_error::create(501, "unsuccessful: " + val.dump()));
                    }

                    break;
                }

                case patch_operations::invalid:
                {
                    // op must be "add", "remove", "replace", "move", "copy", or
                    // "test"
                    JSON_THROW(parse_error::create(105, 0, "operation value '" + op + "' is invalid"));
                }
            }
        }

        return result;
    }


    static basic_json diff(const basic_json& source, const basic_json& target,
                           const std::string& path = "")
    {
        // the patch
        basic_json result(value_t::array);

        // if the values are the same, return empty patch
        if (source == target)
        {
            return result;
        }

        if (source.type() != target.type())
        {
            // different types: replace value
            result.push_back(
            {
                {"op", "replace"}, {"path", path}, {"value", target}
            });
        }
        else
        {
            switch (source.type())
            {
                case value_t::array:
                {
                    // first pass: traverse common elements
                    std::size_t i = 0;
                    while (i < source.size() and i < target.size())
                    {
                        // recursive call to compare array values at index i
                        auto temp_diff = diff(source[i], target[i], path + "/" + std::to_string(i));
                        result.insert(result.end(), temp_diff.begin(), temp_diff.end());
                        ++i;
                    }

                    // i now reached the end of at least one array
                    // in a second pass, traverse the remaining elements

                    // remove my remaining elements
                    const auto end_index = static_cast<difference_type>(result.size());
                    while (i < source.size())
                    {
                        // add operations in reverse order to avoid invalid
                        // indices
                        result.insert(result.begin() + end_index, object(
                        {
                            {"op", "remove"},
                            {"path", path + "/" + std::to_string(i)}
                        }));
                        ++i;
                    }

                    // add other remaining elements
                    while (i < target.size())
                    {
                        result.push_back(
                        {
                            {"op", "add"},
                            {"path", path + "/" + std::to_string(i)},
                            {"value", target[i]}
                        });
                        ++i;
                    }

                    break;
                }

                case value_t::object:
                {
                    // first pass: traverse this object's elements
                    for (auto it = source.cbegin(); it != source.cend(); ++it)
                    {
                        // escape the key name to be used in a JSON patch
                        const auto key = json_pointer::escape(it.key());

                        if (target.find(it.key()) != target.end())
                        {
                            // recursive call to compare object values at key it
                            auto temp_diff = diff(it.value(), target[it.key()], path + "/" + key);
                            result.insert(result.end(), temp_diff.begin(), temp_diff.end());
                        }
                        else
                        {
                            // found a key that is not in o -> remove it
                            result.push_back(object(
                            {
                                {"op", "remove"}, {"path", path + "/" + key}
                            }));
                        }
                    }

                    // second pass: traverse other object's elements
                    for (auto it = target.cbegin(); it != target.cend(); ++it)
                    {
                        if (source.find(it.key()) == source.end())
                        {
                            // found a key that is not in this -> add it
                            const auto key = json_pointer::escape(it.key());
                            result.push_back(
                            {
                                {"op", "add"}, {"path", path + "/" + key},
                                {"value", it.value()}
                            });
                        }
                    }

                    break;
                }

                default:
                {
                    // both primitive type: replace value
                    result.push_back(
                    {
                        {"op", "replace"}, {"path", path}, {"value", target}
                    });
                    break;
                }
            }
        }

        return result;
    }


    void merge_patch(const basic_json& apply_patch)
    {
        if (apply_patch.is_object())
        {
            if (not is_object())
            {
                *this = object();
            }
            for (auto it = apply_patch.begin(); it != apply_patch.end(); ++it)
            {
                if (it.value().is_null())
                {
                    erase(it.key());
                }
                else
                {
                    operator[](it.key()).merge_patch(it.value());
                }
            }
        }
        else
        {
            *this = apply_patch;
        }
    }

private:
	template<typename T, typename... Args>
	static T* create(Args&& ... args)
	{
		Allocator_type<T> alloc;
		using AllocatorTraits = std::allocator_traits<Allocator_type<T>>;

        auto deleter = [&](T * object)
        {
            AllocatorTraits::deallocate(alloc, object, 1);
        };
        std::unique_ptr<T, decltype(deleter)> object(AllocatorTraits::allocate(alloc, 1), deleter);
        AllocatorTraits::construct(alloc, object.get(), std::forward<Args>(args)...);
        assert(object != nullptr);
        return object.release();
    }

  
    union json_value
    {
        object_t* object;
        array_t* array;
        string_t* string;
        boolean_t boolean;
        number_integer_t number_integer;
        number_unsigned_t number_unsigned;
        number_float_t number_float;

        json_value() = default;
        json_value(boolean_t v)  : boolean(v) {}
        json_value(number_integer_t v)  : number_integer(v) {}
        json_value(number_unsigned_t v)  : number_unsigned(v) {}
        json_value(number_float_t v)  : number_float(v) {}
        json_value(value_t t)
        {
            switch (t)
            {
                case value_t::object:
                    object = create<object_t>();
                    break;
                case value_t::array:
                    array = create<array_t>();
                    break;
                case value_t::string:
                    string = create<string_t>("");
                    break;
                case value_t::boolean:
                    boolean = boolean_t(false);
                    break;
                case value_t::number_integer:
                    number_integer = number_integer_t(0);
                    break;
                case value_t::number_unsigned:
                    number_unsigned = number_unsigned_t(0);
                    break;
                case value_t::number_float:
                    number_float = number_float_t(0.0);
                    break;
                case value_t::null:
                    object = nullptr; 
                    break;
                default:
                    break;
            }
        }

        json_value(const string_t& value)
        {
            string = create<string_t>(value);
        }

        json_value(string_t&& value)
        {
            string = create<string_t>(std::move(value));
        }

        json_value(const object_t& value)
        {
            object = create<object_t>(value);
        }

        json_value(object_t&& value)
        {
            object = create<object_t>(std::move(value));
        }

        json_value(const array_t& value)
        {
            array = create<array_t>(value);
        }

        json_value(array_t&& value)
        {
            array = create<array_t>(std::move(value));
        }

        void destroy(value_t t) 
        {
            switch (t)
            {
                case value_t::object:
                {
                    Allocator_type<object_t> alloc;
                    std::allocator_traits<decltype(alloc)>::destroy(alloc, object);
                    std::allocator_traits<decltype(alloc)>::deallocate(alloc, object, 1);
                    break;
                }
                case value_t::array:
                {
                    Allocator_type<array_t> alloc;
                    std::allocator_traits<decltype(alloc)>::destroy(alloc, array);
                    std::allocator_traits<decltype(alloc)>::deallocate(alloc, array, 1);
                    break;
                }
                case value_t::string:
                {
                    Allocator_type<string_t> alloc;
                    std::allocator_traits<decltype(alloc)>::destroy(alloc, string);
                    std::allocator_traits<decltype(alloc)>::deallocate(alloc, string, 1);
                    break;
                }
                default:
                    break;
            }
        }
    };


    void assert_invariant() const 
    {
        assert(type_ != value_t::object or value_.object != nullptr);
        assert(type_ != value_t::array or value_.array != nullptr);
        assert(type_ != value_t::string or value_.string != nullptr);
    }

    boolean_t get_impl(boolean_t* /*unused*/) const
    {
        if (JSON_LIKELY(is_boolean()))
        {
            return value_.boolean;
        }

        JSON_THROW(type_error::create(302, "type must be boolean, but is " + std::string(type_name())));
    }

    /// get a pointer to the value (object)
    object_t* get_impl_ptr(object_t* /*unused*/) 
    {
        return is_object() ? value_.object : nullptr;
    }

    /// get a pointer to the value (object)
    constexpr const object_t* get_impl_ptr(const object_t* /*unused*/) const 
    {
        return is_object() ? value_.object : nullptr;
    }

    /// get a pointer to the value (array)
    array_t* get_impl_ptr(array_t* /*unused*/) 
    {
        return is_array() ? value_.array : nullptr;
    }

    /// get a pointer to the value (array)
    constexpr const array_t* get_impl_ptr(const array_t* /*unused*/) const 
    {
        return is_array() ? value_.array : nullptr;
    }

    /// get a pointer to the value (string)
    string_t* get_impl_ptr(string_t* /*unused*/) 
    {
        return is_string() ? value_.string : nullptr;
    }

    /// get a pointer to the value (string)
    constexpr const string_t* get_impl_ptr(const string_t* /*unused*/) const 
    {
        return is_string() ? value_.string : nullptr;
    }

    /// get a pointer to the value (boolean)
    boolean_t* get_impl_ptr(boolean_t* /*unused*/) 
    {
        return is_boolean() ? &value_.boolean : nullptr;
    }

    /// get a pointer to the value (boolean)
    constexpr const boolean_t* get_impl_ptr(const boolean_t* /*unused*/) const 
    {
        return is_boolean() ? &value_.boolean : nullptr;
    }

    /// get a pointer to the value (integer number)
    number_integer_t* get_impl_ptr(number_integer_t* /*unused*/) 
    {
        return is_number_integer() ? &value_.number_integer : nullptr;
    }

    /// get a pointer to the value (integer number)
    constexpr const number_integer_t* get_impl_ptr(const number_integer_t* /*unused*/) const 
    {
        return is_number_integer() ? &value_.number_integer : nullptr;
    }

    /// get a pointer to the value (unsigned number)
    number_unsigned_t* get_impl_ptr(number_unsigned_t* /*unused*/) 
    {
        return is_number_unsigned() ? &value_.number_unsigned : nullptr;
    }

    /// get a pointer to the value (unsigned number)
    constexpr const number_unsigned_t* get_impl_ptr(const number_unsigned_t* /*unused*/) const 
    {
        return is_number_unsigned() ? &value_.number_unsigned : nullptr;
    }

    /// get a pointer to the value (floating-point number)
    number_float_t* get_impl_ptr(number_float_t* /*unused*/) 
    {
        return is_number_float() ? &value_.number_float : nullptr;
    }

    /// get a pointer to the value (floating-point number)
    constexpr const number_float_t* get_impl_ptr(const number_float_t* /*unused*/) const 
    {
        return is_number_float() ? &value_.number_float : nullptr;
    }


    template<typename ReferenceType, typename ThisType>
    static ReferenceType get_ref_impl(ThisType& obj)
    {
        // delegate the call to get_ptr<>()
        auto ptr = obj.template get_ptr<typename std::add_pointer<ReferenceType>::type>();

        if (JSON_LIKELY(ptr != nullptr))
        {
            return *ptr;
        }

        JSON_THROW(type_error::create(303, "incompatible ReferenceType for get_ref, actual type is " + std::string(obj.type_name())));
    }



    value_t type_ = value_t::null;
    json_value value_ = {};
};

#endif
