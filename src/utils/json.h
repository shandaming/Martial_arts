/*
 * Copyright (C) 2019
 */

#ifndef SCC_JSON_H
#define SCC_JSON_H

#include <algorithm> // all_of, find, for_each
#include <cassert> // assert
#include <ciso646> // and, not, or
#include <cstddef> // nullptr_t, ptrdiff_t, size_t
#include <functional> // hash, less
#include <initializer_list> // initializer_list
#include <iosfwd> // istream, ostream
#include <iterator> // random_access_iterator_tag
#include <numeric> // accumulate
#include <string> // string, stoi, to_string
#include <utility> // declval, forward, move, pair, swap

#include <cstdint> // int64_t, uint64_t
#include <map> // map
#include <memory> // allocator
#include <string> // string
#include <vector> // vector

/*!
@brief namespace for Niels Lohmann
@see https://github.com/nlohmann
@since version 1.0.0
*/
namespace nlohmann
{
template<typename T = void, typename SFINAE = void>
struct adl_serializer;

template<template<typename U, typename V, typename... Args> class Object =
         std::map,
         template<typename U, typename... Args> class Array = std::vector,
         class String = std::string, class Boolean = bool,
         class Num_integer = std::int64_t,
         class Num_unsigned = std::uint64_t,
         class Num_float = double,
         template<typename U> class Allocator = std::allocator,
         template<typename T, typename SFINAE = void> class Serializer =
         adl_serializer>
class basic_json;

/*!
@brief JSON Pointer

A JSON pointer defines a string syntax for identifying a specific value
within a JSON document. It can be used with functions `at` and
`operator[]`. Furthermore, JSON pointers are the base for JSON patches.

@sa [RFC 6901](https://tools.ietf.org/html/rfc6901)

@since version 2.0.0
*/
template<typename Basic_json>
class Json_ptr;

/*!
@brief default JSON class

This type is the default specialization of the @ref basic_json class which
uses the standard template types.

@since version 1.0.0
*/
using json = basic_json<>;
}  // namespace nlohmann

#endif

// #include <nlohmann/detail/macro_scope.hpp>


// This file contains all internal macro definitions
// You MUST include macro_unscope.hpp at the end of json.hpp to undef all of them

// exclude unsupported compilers
#if !defined(JSON_SKIP_UNSUPPORTED_COMPILER_CHECK)
    #if defined(__clang__)
        #if (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__) < 30400
            #error "unsupported Clang version - see https://github.com/nlohmann/json#supported-compilers"
        #endif
    #elif defined(__GNUC__) && !(defined(__ICC) || defined(__INTEL_COMPILER))
        #if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) < 40800
            #error "unsupported GCC version - see https://github.com/nlohmann/json#supported-compilers"
        #endif
    #endif
#endif

// disable float-equal warnings on GCC/clang
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

// disable documentation warnings on clang
#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdocumentation"
#endif

// allow for portable deprecation warnings
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    #define JSON_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
    #define JSON_DEPRECATED __declspec(deprecated)
#else
    #define JSON_DEPRECATED
#endif

// allow to disable exceptions
#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)) && !defined(JSON_NOEXCEPTION)
    #define JSON_THROW(exception) throw exception
    #define JSON_TRY try
    #define JSON_CATCH(exception) catch(exception)
    #define JSON_INTERNAL_CATCH(exception) catch(exception)
#else
    #define JSON_THROW(exception) std::abort()
    #define JSON_TRY if(true)
    #define JSON_CATCH(exception) if(false)
    #define JSON_INTERNAL_CATCH(exception) if(false)
#endif

// override exception macros
#if defined(JSON_THROW_USER)
    #undef JSON_THROW
    #define JSON_THROW JSON_THROW_USER
#endif
#if defined(JSON_TRY_USER)
    #undef JSON_TRY
    #define JSON_TRY JSON_TRY_USER
#endif
#if defined(JSON_CATCH_USER)
    #undef JSON_CATCH
    #define JSON_CATCH JSON_CATCH_USER
    #undef JSON_INTERNAL_CATCH
    #define JSON_INTERNAL_CATCH JSON_CATCH_USER
#endif
#if defined(JSON_INTERNAL_CATCH_USER)
    #undef JSON_INTERNAL_CATCH
    #define JSON_INTERNAL_CATCH JSON_INTERNAL_CATCH_USER
#endif


/*!
@brief macro to briefly define a mapping between an enum and JSON
@def NLOHMANN_JSON_SERIALIZE_ENUM
@since version 3.4.0
*/
#define NLOHMANN_JSON_SERIALIZE_ENUM(ENUM_TYPE, ...)                                           \
    template<typename Basic_json>                                                           \
    inline void to_json(Basic_json& j, const ENUM_TYPE& e)                                  \
    {                                                                                          \
        static_assert(std::is_enum<ENUM_TYPE>::value, #ENUM_TYPE " must be an enum!");         \
        static const std::pair<ENUM_TYPE, Basic_json> m[] = __VA_ARGS__;                    \
        auto it = std::find_if(std::begin(m), std::end(m),                                     \
                               [e](const std::pair<ENUM_TYPE, Basic_json>& ej_pair) -> bool \
        {                                                                                      \
            return ej_pair.first == e;                                                         \
        });                                                                                    \
        j = ((it != std::end(m)) ? it : std::begin(m))->second;                                \
    }                                                                                          \
    template<typename Basic_json>                                                           \
    inline void from_json(const Basic_json& j, ENUM_TYPE& e)                                \
    {                                                                                          \
        static_assert(std::is_enum<ENUM_TYPE>::value, #ENUM_TYPE " must be an enum!");         \
        static const std::pair<ENUM_TYPE, Basic_json> m[] = __VA_ARGS__;                    \
        auto it = std::find_if(std::begin(m), std::end(m),                                     \
                               [j](const std::pair<ENUM_TYPE, Basic_json>& ej_pair) -> bool \
        {                                                                                      \
            return ej_pair.second == j;                                                        \
        });                                                                                    \
        e = ((it != std::end(m)) ? it : std::begin(m))->first;                                 \
    }

// Ugly macros to avoid uglier copy-paste when specializing basic_json. They
// may be removed in the future once the class is split.

#define NLOHMANN_BASIC_JSON_TPL_DECLARATION                                \
    template<template<typename, typename, typename...> class Object,   \
             template<typename, typename...> class Array,              \
             class String, class Boolean, class Num_integer, \
             class Num_unsigned, class Num_float,              \
             template<typename> class Allocator,                       \
             template<typename, typename = void> class Serializer>

#define NLOHMANN_BASIC_JSON_TPL                                            \
    basic_json<Object, Array, String, Boolean,             \
    Num_integer, Num_unsigned, Num_float,                \
    Allocator, Serializer>

// #include <nlohmann/detail/meta/cpp_future.hpp>


#include <ciso646> // not
#include <cstddef> // size_t
#include <type_traits> // conditional, enable_if, false_type, integral_constant, is_constructible, is_integral, is_same, remove_cv, remove_reference, true_type

namespace nlohmann
{
namespace detail
{
// alias templates to reduce boilerplate
template<bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template<typename T>
using uncvref_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

// implementation of C++14 index_sequence and affiliates
// source: https://stackoverflow.com/a/32223343
template<std::size_t... Ints>
struct index_sequence
{
    using type = index_sequence;
    using value_type = std::size_t;
    static constexpr std::size_t size() 
    {
        return sizeof...(Ints);
    }
};

template<class Sequence1, class Sequence2>
struct merge_and_renumber;

template<std::size_t... I1, std::size_t... I2>
struct merge_and_renumber<index_sequence<I1...>, index_sequence<I2...>>
        : index_sequence < I1..., (sizeof...(I1) + I2)... > {};

template<std::size_t N>
struct make_index_sequence
    : merge_and_renumber < typename make_index_sequence < N / 2 >::type,
      typename make_index_sequence < N - N / 2 >::type > {};

template<> struct make_index_sequence<0> : index_sequence<> {};
template<> struct make_index_sequence<1> : index_sequence<0> {};

template<typename... Ts>
using index_sequence_for = make_index_sequence<sizeof...(Ts)>;

// dispatch utility (taken from ranges-v3)
template<unsigned N> struct priority_tag : priority_tag < N - 1 > {};
template<> struct priority_tag<0> {};

// taken from ranges-v3
template<typename T>
struct static_const
{
    static constexpr T value{};
};

template<typename T>
constexpr T static_const<T>::value;
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/meta/type_traits.hpp>


#include <ciso646> // not
#include <limits> // numeric_limits
#include <type_traits> // false_type, is_constructible, is_integral, is_same, true_type
#include <utility> // declval

// #include <nlohmann/json_fwd.hpp>

// #include <nlohmann/detail/iterators/iterator_traits.hpp>


#include <iterator> // random_access_iterator_tag

// #include <nlohmann/detail/meta/void_t.hpp>


namespace nlohmann
{
namespace detail
{
template <typename ...Ts> struct make_void
{
    using type = void;
};
template <typename ...Ts> using void_t = typename make_void<Ts...>::type;
} // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/meta/cpp_future.hpp>


namespace nlohmann
{
namespace detail
{
template <typename It, typename = void>
struct iterator_types {};

template <typename It>
struct iterator_types <
    It,
    void_t<typename It::difference_type, typename It::value_type, typename It::pointer,
    typename It::reference, typename It::iterator_category >>
{
    using difference_type = typename It::difference_type;
    using value_type = typename It::value_type;
    using pointer = typename It::pointer;
    using reference = typename It::reference;
    using iterator_category = typename It::iterator_category;
};

// This is required as some compilers implement std::iterator_traits in a way that
// doesn't work with SFINAE. See https://github.com/nlohmann/json/issues/1341.
template <typename T, typename = void>
struct iterator_traits
{
};

template <typename T>
struct iterator_traits < T, enable_if_t < !std::is_pointer<T>::value >>
            : iterator_types<T>
{
};

template <typename T>
struct iterator_traits<T*, enable_if_t<std::is_object<T>::value>>
{
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using reference = T&;
};
}
}

// #include <nlohmann/detail/meta/cpp_future.hpp>

// #include <nlohmann/detail/meta/detected.hpp>


#include <type_traits>

// #include <nlohmann/detail/meta/void_t.hpp>


// http://en.cppreference.com/w/cpp/experimental/is_detected
namespace nlohmann
{
namespace detail
{
struct nonesuch
{
    nonesuch() = delete;
    ~nonesuch() = delete;
    nonesuch(nonesuch const&) = delete;
    void operator=(nonesuch const&) = delete;
};

template <class Default,
          class AlwaysVoid,
          template <class...> class Op,
          class... Args>
struct detector
{
    using Value = std::false_type;
    using type = Default;
};

template <class Default, template <class...> class Op, class... Args>
struct detector<Default, void_t<Op<Args...>>, Op, Args...>
{
    using Value = std::true_type;
    using type = Op<Args...>;
};

template <template <class...> class Op, class... Args>
using is_detected = typename detector<nonesuch, void, Op, Args...>::Value;

template <template <class...> class Op, class... Args>
using detected_t = typename detector<nonesuch, void, Op, Args...>::type;

template <class Default, template <class...> class Op, class... Args>
using detected_or = detector<Default, void, Op, Args...>;

template <class Default, template <class...> class Op, class... Args>
using detected_or_t = typename detected_or<Default, Op, Args...>::type;

template <class Expected, template <class...> class Op, class... Args>
using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

template <class To, template <class...> class Op, class... Args>
using is_detected_convertible =
    std::is_convertible<detected_t<Op, Args...>, To>;
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/macro_scope.hpp>


namespace nlohmann
{
/*!
@brief detail namespace with internal helper functions

This namespace collects functions that should not be exposed,
implementations of some @ref basic_json methods, and meta-programming helpers.

@since version 2.1.0
*/
namespace detail
{
// Is_basic_json
template<typename> 
struct Is_basic_json : std::false_type {};

NLOHMANN_BASIC_JSON_TPL_DECLARATION
struct Is_basic_json<NLOHMANN_BASIC_JSON_TPL> : std::true_type {};

//////////////////////////
// aliases for detected //
//////////////////////////

template <typename T>
using mapped_type_t = typename T::mapped_type;

template <typename T>
using key_type_t = typename T::key_type;

template <typename T>
using value_type_t = typename T::value_type;

template <typename T>
using difference_type_t = typename T::difference_type;

template <typename T>
using pointer_t = typename T::pointer;

template <typename T>
using reference_t = typename T::reference;

template <typename T>
using iterator_category_t = typename T::iterator_category;

template <typename T>
using iterator_t = typename T::iterator;

template <typename T, typename... Args>
using to_json_function = decltype(T::to_json(std::declval<Args>()...));

template <typename T, typename... Args>
using from_json_function = decltype(T::from_json(std::declval<Args>()...));

template <typename T, typename U>
using get_template_function = decltype(std::declval<T>().template get<U>());

// trait checking if Serializer<T>::from_json(json const&, udt&) exists
template <typename Basic_json, typename T, typename = void>
struct has_from_json : std::false_type {};

template <typename Basic_json, typename T>
struct has_from_json<Basic_json, T,
           enable_if_t<not Is_basic_json<T>::value>>
{
    using serializer = typename Basic_json::template json_serializer<T, void>;

    static constexpr bool value =
        is_detected_exact<void, from_json_function, serializer,
        const Basic_json&, T&>::value;
};

// This trait checks if Serializer<T>::from_json(json const&) exists
// this overload is used for non-default-constructible user-defined-types
template <typename Basic_json, typename T, typename = void>
struct has_non_default_from_json : std::false_type {};

template<typename Basic_json, typename T>
struct has_non_default_from_json<Basic_json, T, enable_if_t<not Is_basic_json<T>::value>>
{
    using serializer = typename Basic_json::template json_serializer<T, void>;

    static constexpr bool value =
        is_detected_exact<T, from_json_function, serializer,
        const Basic_json&>::value;
};

// This trait checks if Basic_json::json_serializer<T>::to_json exists
// Do not evaluate the trait when T is a basic_json type, to avoid template instantiation infinite recursion.
template <typename Basic_json, typename T, typename = void>
struct has_to_json : std::false_type {};

template <typename Basic_json, typename T>
struct has_to_json<Basic_json, T, enable_if_t<not Is_basic_json<T>::value>>
{
    using serializer = typename Basic_json::template json_serializer<T, void>;

    static constexpr bool value =
        is_detected_exact<void, to_json_function, serializer, Basic_json&,
        T>::value;
};


///////////////////
// is_ functions //
///////////////////

template <typename T, typename = void>
struct is_iterator_traits : std::false_type {};

template <typename T>
struct is_iterator_traits<iterator_traits<T>>
{
  private:
    using traits = iterator_traits<T>;

  public:
    static constexpr auto value =
        is_detected<value_type_t, traits>::value &&
        is_detected<difference_type_t, traits>::value &&
        is_detected<pointer_t, traits>::value &&
        is_detected<iterator_category_t, traits>::value &&
        is_detected<reference_t, traits>::value;
};

// Is_complete_type
template <typename T, typename = void>
struct Is_complete_type : std::false_type {};

template <typename T>
struct Is_complete_type<T, decltype(void(sizeof(T)))> : std::true_type {};


template <typename Basic_json, typename CompatibleObjectType,
          typename = void>
struct is_compatible_object_type_impl : std::false_type {};

template <typename Basic_json, typename CompatibleObjectType>
struct is_compatible_object_type_impl <
    Basic_json, CompatibleObjectType,
    enable_if_t<is_detected<mapped_type_t, CompatibleObjectType>::value and
    is_detected<key_type_t, CompatibleObjectType>::value >>
{

    using Object = typename Basic_json::Object;

    // macOS's is_constructible does not play well with nonesuch...
    static constexpr bool value =
        std::is_constructible<typename Object::key_type,
        typename CompatibleObjectType::key_type>::value and
        std::is_constructible<typename Object::mapped_type,
        typename CompatibleObjectType::mapped_type>::value;
};

template <typename Basic_json, typename CompatibleObjectType>
struct is_compatible_object_type
    : is_compatible_object_type_impl<Basic_json, CompatibleObjectType> {};

template <typename Basic_json, typename ConstructibleObjectType,
          typename = void>
struct is_constructible_object_type_impl : std::false_type {};

template <typename Basic_json, typename ConstructibleObjectType>
struct is_constructible_object_type_impl <
    Basic_json, ConstructibleObjectType,
    enable_if_t<is_detected<mapped_type_t, ConstructibleObjectType>::value and
    is_detected<key_type_t, ConstructibleObjectType>::value >>
{
    using Object = typename Basic_json::Object;

    static constexpr bool value =
        (std::is_constructible<typename ConstructibleObjectType::key_type, typename Object::key_type>::value and
         std::is_same<typename Object::mapped_type, typename ConstructibleObjectType::mapped_type>::value) or
        (has_from_json<Basic_json, typename ConstructibleObjectType::mapped_type>::value or
         has_non_default_from_json<Basic_json, typename ConstructibleObjectType::mapped_type >::value);
};

template <typename Basic_json, typename ConstructibleObjectType>
struct is_constructible_object_type
    : is_constructible_object_type_impl<Basic_json,
      ConstructibleObjectType> {};

template <typename Basic_json, typename CompatibleStringType,
          typename = void>
struct is_compatible_string_type_impl : std::false_type {};

template <typename Basic_json, typename CompatibleStringType>
struct is_compatible_string_type_impl <
    Basic_json, CompatibleStringType,
    enable_if_t<is_detected_exact<typename Basic_json::String::value_type,
    value_type_t, CompatibleStringType>::value >>
{
    static constexpr auto value =
        std::is_constructible<typename Basic_json::String, CompatibleStringType>::value;
};

template <typename Basic_json, typename ConstructibleStringType>
struct is_compatible_string_type
    : is_compatible_string_type_impl<Basic_json, ConstructibleStringType> {};

template <typename Basic_json, typename ConstructibleStringType,
          typename = void>
struct is_constructible_string_type_impl : std::false_type {};

template <typename Basic_json, typename ConstructibleStringType>
struct is_constructible_string_type_impl <
    Basic_json, ConstructibleStringType,
    enable_if_t<is_detected_exact<typename Basic_json::String::value_type,
    value_type_t, ConstructibleStringType>::value >>
{
    static constexpr auto value =
        std::is_constructible<ConstructibleStringType,
        typename Basic_json::String>::value;
};

template <typename Basic_json, typename ConstructibleStringType>
struct is_constructible_string_type
    : is_constructible_string_type_impl<Basic_json, ConstructibleStringType> {};

template <typename Basic_json, typename CompatibleArrayType, typename = void>
struct is_compatible_array_type_impl : std::false_type {};

template <typename Basic_json, typename CompatibleArrayType>
struct is_compatible_array_type_impl <
    Basic_json, CompatibleArrayType,
    enable_if_t<is_detected<value_type_t, CompatibleArrayType>::value and
    is_detected<iterator_t, CompatibleArrayType>::value and
// This is needed because Json_reverse_iterator has a ::iterator type...
// Therefore it is detected as a CompatibleArrayType.
// The real fix would be to have an Iterable concept.
    not is_iterator_traits<
    iterator_traits<CompatibleArrayType>>::value >>
{
    static constexpr bool value =
        std::is_constructible<Basic_json,
        typename CompatibleArrayType::value_type>::value;
};

template <typename Basic_json, typename CompatibleArrayType>
struct is_compatible_array_type
    : is_compatible_array_type_impl<Basic_json, CompatibleArrayType> {};

template <typename Basic_json, typename ConstructibleArrayType, typename = void>
struct is_constructible_array_type_impl : std::false_type {};

template <typename Basic_json, typename ConstructibleArrayType>
struct is_constructible_array_type_impl <
    Basic_json, ConstructibleArrayType,
    enable_if_t<std::is_same<ConstructibleArrayType,
    typename Basic_json::value_type>::value >>
            : std::true_type {};

template <typename Basic_json, typename ConstructibleArrayType>
struct is_constructible_array_type_impl <
    Basic_json, ConstructibleArrayType,
    enable_if_t<not std::is_same<ConstructibleArrayType,
    typename Basic_json::value_type>::value and
    is_detected<value_type_t, ConstructibleArrayType>::value and
    is_detected<iterator_t, ConstructibleArrayType>::value and
    Is_complete_type<
    detected_t<value_type_t, ConstructibleArrayType>>::value >>
{
    static constexpr bool value =
        // This is needed because Json_reverse_iterator has a ::iterator type,
        // furthermore, std::back_insert_iterator (and other iterators) have a base class `iterator`...
        // Therefore it is detected as a ConstructibleArrayType.
        // The real fix would be to have an Iterable concept.
        not is_iterator_traits <
        iterator_traits<ConstructibleArrayType >>::value and

        (std::is_same<typename ConstructibleArrayType::value_type, typename Basic_json::Array::value_type>::value or
         has_from_json<Basic_json,
         typename ConstructibleArrayType::value_type>::value or
         has_non_default_from_json <
         Basic_json, typename ConstructibleArrayType::value_type >::value);
};

template <typename Basic_json, typename ConstructibleArrayType>
struct is_constructible_array_type
    : is_constructible_array_type_impl<Basic_json, ConstructibleArrayType> {};

template <typename RealIntegerType, typename CompatibleNumberIntegerType,
          typename = void>
struct is_compatible_integer_type_impl : std::false_type {};

template <typename RealIntegerType, typename CompatibleNumberIntegerType>
struct is_compatible_integer_type_impl <
    RealIntegerType, CompatibleNumberIntegerType,
    enable_if_t<std::is_integral<RealIntegerType>::value and
    std::is_integral<CompatibleNumberIntegerType>::value and
    not std::is_same<bool, CompatibleNumberIntegerType>::value >>
{
    // is there an assert somewhere on overflows?
    using RealLimits = std::numeric_limits<RealIntegerType>;
    using CompatibleLimits = std::numeric_limits<CompatibleNumberIntegerType>;

    static constexpr auto value =
        std::is_constructible<RealIntegerType,
        CompatibleNumberIntegerType>::value and
        CompatibleLimits::is_integer and
        RealLimits::is_signed == CompatibleLimits::is_signed;
};

template <typename RealIntegerType, typename CompatibleNumberIntegerType>
struct is_compatible_integer_type
    : is_compatible_integer_type_impl<RealIntegerType,
      CompatibleNumberIntegerType> {};

// Is_compatible_type_impl
template <typename Basic_json, typename Compatible, typename = void>
struct Is_compatible_type_impl: std::false_type {};

template <typename Basic_json, typename Compatible>
struct Is_compatible_type_impl <
    Basic_json, Compatible,
    enable_if_t<Is_complete_type<Compatible>::value >>
{
    static constexpr bool value =
        has_to_json<Basic_json, Compatible>::value;
};

template <typename Basic_json, typename Compatible>
struct Is_compatible_type
    : Is_compatible_type_impl<Basic_json, Compatible> {};
}  // namespace detail
}  // namespace nlohmann


#include <exception> // exception
#include <stdexcept> // runtime_error
#include <string> // to_string

// #include <nlohmann/detail/input/position_t.hpp>


#include <cstddef> // size_t

namespace nlohmann
{
namespace detail
{
/// struct to capture the start position of the current token
struct position_t
{
    /// the total number of characters read
    std::size_t chars_read_total = 0;
    /// the number of characters read in the current line
    std::size_t chars_read_current_line = 0;
    /// the number of lines read
    std::size_t lines_read = 0;

    /// conversion to size_t to preserve SAX interface
    constexpr operator size_t() const
    {
        return chars_read_total;
    }
};

}
}


namespace nlohmann
{
namespace detail
{

class exception : public std::exception
{
  public:
    /// returns the explanatory string
    const char* what() const  override
    {
        return m.what();
    }

    /// the id of the exception
    const int id;

  protected:
    exception(int id_, const char* what_arg) : id(id_), m(what_arg) {}

    static std::string name(const std::string& ename, int id_)
    {
        return "[json.exception." + ename + "." + std::to_string(id_) + "] ";
    }

  private:
    /// an exception object as storage for error messages
    std::runtime_error m;
};


class parse_error : public exception
{
  public:
    /*!
    @brief create a parse error exception
    @param[in] id_       the id of the exception
    @param[in] position  the position where the error occurred (or with
                         chars_read_total=0 if the position cannot be
                         determined)
    @param[in] what_arg  the explanatory string
    @return parse_error object
    */
    static parse_error create(int id_, const position_t& pos, const std::string& what_arg)
    {
        std::string w = exception::name("parse_error", id_) + "parse error" +
                        position_string(pos) + ": " + what_arg;
        return parse_error(id_, pos.chars_read_total, w.c_str());
    }

    static parse_error create(int id_, std::size_t byte_, const std::string& what_arg)
    {
        std::string w = exception::name("parse_error", id_) + "parse error" +
                        (byte_ != 0 ? (" at byte " + std::to_string(byte_)) : "") +
                        ": " + what_arg;
        return parse_error(id_, byte_, w.c_str());
    }

    /*!
    @brief byte index of the parse error

    The byte index of the last read character in the input file.

    @note For an input with n bytes, 1 is the index of the first character and
          n+1 is the index of the terminating null byte or the end of file.
          This also holds true when reading a byte vector (CBOR or MessagePack).
    */
    const std::size_t byte;

  private:
    parse_error(int id_, std::size_t byte_, const char* what_arg)
        : exception(id_, what_arg), byte(byte_) {}

    static std::string position_string(const position_t& pos)
    {
        return " at line " + std::to_string(pos.lines_read + 1) +
               ", column " + std::to_string(pos.chars_read_current_line);
    }
};


class invalid_iterator : public exception
{
  public:
    static invalid_iterator create(int id_, const std::string& what_arg)
    {
        std::string w = exception::name("invalid_iterator", id_) + what_arg;
        return invalid_iterator(id_, w.c_str());
    }

  private:
    invalid_iterator(int id_, const char* what_arg)
        : exception(id_, what_arg) {}
};


class type_error : public exception
{
  public:
    static type_error create(int id_, const std::string& what_arg)
    {
        std::string w = exception::name("type_error", id_) + what_arg;
        return type_error(id_, w.c_str());
    }

  private:
    type_error(int id_, const char* what_arg) : exception(id_, what_arg) {}
};


class out_of_range : public exception
{
  public:
    static out_of_range create(int id_, const std::string& what_arg)
    {
        std::string w = exception::name("out_of_range", id_) + what_arg;
        return out_of_range(id_, w.c_str());
    }

  private:
    out_of_range(int id_, const char* what_arg) : exception(id_, what_arg) {}
};


class other_error : public exception
{
  public:
    static other_error create(int id_, const std::string& what_arg)
    {
        std::string w = exception::name("other_error", id_) + what_arg;
        return other_error(id_, w.c_str());
    }

  private:
    other_error(int id_, const char* what_arg) : exception(id_, what_arg) {}
};
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/Value.hpp>


#include <array> // array
#include <ciso646> // and
#include <cstddef> // size_t
#include <cstdint> // uint8_t

namespace nlohmann
{
namespace detail
{

enum class Value : std::uint8_t
{
    null,             ///< null value
    object,           ///< object (unordered set of name/value pairs)
    array,            ///< array (ordered collection of values)
    string,           ///< string value
    boolean,          ///< boolean value
    num_integer,   ///< number value (signed integer)
    num_unsigned,  ///< number value (unsigned integer)
    num_float,     ///< number value (floating-point)
    discarded         ///< discarded by the the Parser callback function
};

/*!
@brief comparison operator for JSON types

Returns an ordering that is similar to Python:
- order: null < boolean < number < object < array < string
- furthermore, each type is not smaller than itself
- discarded values are not comparable

@since version 1.0.0
*/
inline bool operator<(const Value lhs, const Value rhs) 
{
    static constexpr std::array<std::uint8_t, 8> order = {{
            0 /* null */, 3 /* object */, 4 /* array */, 5 /* string */,
            1 /* boolean */, 2 /* integer */, 2 /* unsigned */, 2 /* float */
        }
    };

    const auto l_index = static_cast<std::size_t>(lhs);
    const auto r_index = static_cast<std::size_t>(rhs);
    return l_index < order.size() and r_index < order.size() and order[l_index] < order[r_index];
}
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/conversions/from_json.hpp>


#include <algorithm> // transform
#include <array> // array
#include <ciso646> // and, not
#include <forward_list> // forward_list
#include <iterator> // inserter, front_inserter, end
#include <map> // map
#include <string> // string
#include <tuple> // tuple, make_tuple
#include <type_traits> // is_arithmetic, is_same, is_enum, underlying_type, is_convertible
#include <unordered_map> // unordered_map
#include <utility> // pair, declval
#include <valarray> // valarray

// #include <nlohmann/detail/exceptions.hpp>

// #include <nlohmann/detail/macro_scope.hpp>

// #include <nlohmann/detail/meta/cpp_future.hpp>

// #include <nlohmann/detail/meta/type_traits.hpp>

// #include <nlohmann/detail/Value.hpp>


namespace nlohmann
{
namespace detail
{
template<typename Basic_json>
void from_json(const Basic_json& j, typename std::nullptr_t& n)
{
    if ((not j.is_null()))
    {
        JSON_THROW(type_error::create(302, "type must be null, but is " + std::string(j.type_name())));
    }
    n = nullptr;
}

// overloads for basic_json template parameters
template<typename Basic_json, typename ArithmeticType,
         enable_if_t<std::is_arithmetic<ArithmeticType>::value and
                     not std::is_same<ArithmeticType, typename Basic_json::Boolean>::value,
                     int> = 0>
void get_arithmetic_value(const Basic_json& j, ArithmeticType& val)
{
    switch (static_cast<Value>(j))
    {
        case Value::num_unsigned:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const typename Basic_json::Num_unsigned*>());
            break;
        }
        case Value::num_integer:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const typename Basic_json::Num_integer*>());
            break;
        }
        case Value::num_float:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const typename Basic_json::Num_float*>());
            break;
        }

        default:
            JSON_THROW(type_error::create(302, "type must be number, but is " + std::string(j.type_name())));
    }
}

template<typename Basic_json>
void from_json(const Basic_json& j, typename Basic_json::Boolean& b)
{
    if ((not j.is_boolean()))
    {
        JSON_THROW(type_error::create(302, "type must be boolean, but is " + std::string(j.type_name())));
    }
    b = *j.template get_ptr<const typename Basic_json::Boolean*>();
}

template<typename Basic_json>
void from_json(const Basic_json& j, typename Basic_json::String& s)
{
    if ((not j.is_string()))
    {
        JSON_THROW(type_error::create(302, "type must be string, but is " + std::string(j.type_name())));
    }
    s = *j.template get_ptr<const typename Basic_json::String*>();
}

template <
    typename Basic_json, typename ConstructibleStringType,
    enable_if_t <
        is_constructible_string_type<Basic_json, ConstructibleStringType>::value and
        not std::is_same<typename Basic_json::String,
                         ConstructibleStringType>::value,
        int > = 0 >
void from_json(const Basic_json& j, ConstructibleStringType& s)
{
    if ((not j.is_string()))
    {
        JSON_THROW(type_error::create(302, "type must be string, but is " + std::string(j.type_name())));
    }

    s = *j.template get_ptr<const typename Basic_json::String*>();
}

template<typename Basic_json>
void from_json(const Basic_json& j, typename Basic_json::Num_float& val)
{
    get_arithmetic_value(j, val);
}

template<typename Basic_json>
void from_json(const Basic_json& j, typename Basic_json::Num_unsigned& val)
{
    get_arithmetic_value(j, val);
}

template<typename Basic_json>
void from_json(const Basic_json& j, typename Basic_json::Num_integer& val)
{
    get_arithmetic_value(j, val);
}

template<typename Basic_json, typename EnumType,
         enable_if_t<std::is_enum<EnumType>::value, int> = 0>
void from_json(const Basic_json& j, EnumType& e)
{
    typename std::underlying_type<EnumType>::type val;
    get_arithmetic_value(j, val);
    e = static_cast<EnumType>(val);
}

// forward_list doesn't have an insert method
template<typename Basic_json, typename T, typename Allocator,
         enable_if_t<std::is_convertible<Basic_json, T>::value, int> = 0>
void from_json(const Basic_json& j, std::forward_list<T, Allocator>& l)
{
    if ((not j.is_array()))
    {
        JSON_THROW(type_error::create(302, "type must be array, but is " + std::string(j.type_name())));
    }
    std::transform(j.rbegin(), j.rend(),
                   std::front_inserter(l), [](const Basic_json & i)
    {
        return i.template get<T>();
    });
}

// valarray doesn't have an insert method
template<typename Basic_json, typename T,
         enable_if_t<std::is_convertible<Basic_json, T>::value, int> = 0>
void from_json(const Basic_json& j, std::valarray<T>& l)
{
    if ((not j.is_array()))
    {
        JSON_THROW(type_error::create(302, "type must be array, but is " + std::string(j.type_name())));
    }
    l.resize(j.size());
    std::copy(j.value_.array->begin(), j.value_.array->end(), std::begin(l));
}

template<typename Basic_json>
void from_json_array_impl(const Basic_json& j, typename Basic_json::Array& arr, priority_tag<3> /*unused*/)
{
    arr = *j.template get_ptr<const typename Basic_json::Array*>();
}

template <typename Basic_json, typename T, std::size_t N>
auto from_json_array_impl(const Basic_json& j, std::array<T, N>& arr,
                          priority_tag<2> /*unused*/)
-> decltype(j.template get<T>(), void())
{
    for (std::size_t i = 0; i < N; ++i)
    {
        arr[i] = j.at(i).template get<T>();
    }
}

template<typename Basic_json, typename ConstructibleArrayType>
auto from_json_array_impl(const Basic_json& j, ConstructibleArrayType& arr, priority_tag<1> /*unused*/)
-> decltype(
    arr.reserve(std::declval<typename ConstructibleArrayType::size_type>()),
    j.template get<typename ConstructibleArrayType::value_type>(),
    void())
{
    using std::end;

    arr.reserve(j.size());
    std::transform(j.begin(), j.end(),
                   std::inserter(arr, end(arr)), [](const Basic_json & i)
    {
        // get<Basic_json>() returns *this, this won't call a from_json
        // method when value_type is Basic_json
        return i.template get<typename ConstructibleArrayType::value_type>();
    });
}

template <typename Basic_json, typename ConstructibleArrayType>
void from_json_array_impl(const Basic_json& j, ConstructibleArrayType& arr,
                          priority_tag<0> /*unused*/)
{
    using std::end;

    std::transform(
        j.begin(), j.end(), std::inserter(arr, end(arr)),
        [](const Basic_json & i)
    {
        // get<Basic_json>() returns *this, this won't call a from_json
        // method when value_type is Basic_json
        return i.template get<typename ConstructibleArrayType::value_type>();
    });
}

template <typename Basic_json, typename ConstructibleArrayType,
          enable_if_t <
              is_constructible_array_type<Basic_json, ConstructibleArrayType>::value and
              not is_constructible_object_type<Basic_json, ConstructibleArrayType>::value and
              not is_constructible_string_type<Basic_json, ConstructibleArrayType>::value and
              not Is_basic_json<ConstructibleArrayType>::value,
              int > = 0 >

auto from_json(const Basic_json& j, ConstructibleArrayType& arr)
-> decltype(from_json_array_impl(j, arr, priority_tag<3> {}),
j.template get<typename ConstructibleArrayType::value_type>(),
void())
{
    if ((not j.is_array()))
    {
        JSON_THROW(type_error::create(302, "type must be array, but is " +
                                      std::string(j.type_name())));
    }

    from_json_array_impl(j, arr, priority_tag<3> {});
}

template<typename Basic_json, typename ConstructibleObjectType,
         enable_if_t<is_constructible_object_type<Basic_json, ConstructibleObjectType>::value, int> = 0>
void from_json(const Basic_json& j, ConstructibleObjectType& obj)
{
    if ((not j.is_object()))
    {
        JSON_THROW(type_error::create(302, "type must be object, but is " + std::string(j.type_name())));
    }

    auto inner_object = j.template get_ptr<const typename Basic_json::Object*>();
    using value_type = typename ConstructibleObjectType::value_type;
    std::transform(
        inner_object->begin(), inner_object->end(),
        std::inserter(obj, obj.begin()),
        [](typename Basic_json::Object::value_type const & p)
    {
        return value_type(p.first, p.second.template get<typename ConstructibleObjectType::mapped_type>());
    });
}

// overload for arithmetic types, not chosen for basic_json template arguments
// (Boolean, etc..); note: Is it really necessary to provide explicit
// overloads for Boolean etc. in case of a custom Boolean which is not
// an arithmetic type?
template<typename Basic_json, typename ArithmeticType,
         enable_if_t <
             std::is_arithmetic<ArithmeticType>::value and
             not std::is_same<ArithmeticType, typename Basic_json::Num_unsigned>::value and
             not std::is_same<ArithmeticType, typename Basic_json::Num_integer>::value and
             not std::is_same<ArithmeticType, typename Basic_json::Num_float>::value and
             not std::is_same<ArithmeticType, typename Basic_json::Boolean>::value,
             int> = 0>
void from_json(const Basic_json& j, ArithmeticType& val)
{
    switch (static_cast<Value>(j))
    {
        case Value::num_unsigned:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const typename Basic_json::Num_unsigned*>());
            break;
        }
        case Value::num_integer:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const typename Basic_json::Num_integer*>());
            break;
        }
        case Value::num_float:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const typename Basic_json::Num_float*>());
            break;
        }
        case Value::boolean:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const typename Basic_json::Boolean*>());
            break;
        }

        default:
            JSON_THROW(type_error::create(302, "type must be number, but is " + std::string(j.type_name())));
    }
}

template<typename Basic_json, typename A1, typename A2>
void from_json(const Basic_json& j, std::pair<A1, A2>& p)
{
    p = {j.at(0).template get<A1>(), j.at(1).template get<A2>()};
}

template<typename Basic_json, typename Tuple, std::size_t... Idx>
void from_json_tuple_impl(const Basic_json& j, Tuple& t, index_sequence<Idx...> /*unused*/)
{
    t = std::make_tuple(j.at(Idx).template get<typename std::tuple_element<Idx, Tuple>::type>()...);
}

template<typename Basic_json, typename... Args>
void from_json(const Basic_json& j, std::tuple<Args...>& t)
{
    from_json_tuple_impl(j, t, index_sequence_for<Args...> {});
}

template <typename Basic_json, typename Key, typename Value, typename Compare, typename Allocator,
          typename = enable_if_t<not std::is_constructible<
                                     typename Basic_json::String, Key>::value>>
void from_json(const Basic_json& j, std::map<Key, Value, Compare, Allocator>& m)
{
    if ((not j.is_array()))
    {
        JSON_THROW(type_error::create(302, "type must be array, but is " + std::string(j.type_name())));
    }
    for (const auto& p : j)
    {
        if ((not p.is_array()))
        {
            JSON_THROW(type_error::create(302, "type must be array, but is " + std::string(p.type_name())));
        }
        m.emplace(p.at(0).template get<Key>(), p.at(1).template get<Value>());
    }
}

template <typename Basic_json, typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator,
          typename = enable_if_t<not std::is_constructible<
                                     typename Basic_json::String, Key>::value>>
void from_json(const Basic_json& j, std::unordered_map<Key, Value, Hash, KeyEqual, Allocator>& m)
{
    if ((not j.is_array()))
    {
        JSON_THROW(type_error::create(302, "type must be array, but is " + std::string(j.type_name())));
    }
    for (const auto& p : j)
    {
        if ((not p.is_array()))
        {
            JSON_THROW(type_error::create(302, "type must be array, but is " + std::string(p.type_name())));
        }
        m.emplace(p.at(0).template get<Key>(), p.at(1).template get<Value>());
    }
}

struct from_json_fn
{
    template<typename Basic_json, typename T>
    auto operator()(const Basic_json& j, T& val) const
    ((from_json(j, val)))
    -> decltype(from_json(j, val), void())
    {
        return from_json(j, val);
    }
};
}  // namespace detail

/// namespace to hold default `from_json` function
/// to see why this is required:
/// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html
namespace
{
constexpr const auto& from_json = detail::static_const<detail::from_json_fn>::value;
} // namespace
}  // namespace nlohmann

// #include <nlohmann/detail/conversions/to_json.hpp>


#include <ciso646> // or, and, not
#include <iterator> // begin, end
#include <tuple> // tuple, get
#include <type_traits> // is_same, is_constructible, is_floating_point, is_enum, underlying_type
#include <utility> // move, forward, declval, pair
#include <valarray> // valarray
#include <vector> // vector

// #include <nlohmann/detail/meta/cpp_future.hpp>

// #include <nlohmann/detail/meta/type_traits.hpp>

// #include <nlohmann/detail/Value.hpp>

// #include <nlohmann/detail/iterators/iteration_proxy.hpp>


#include <cstddef> // size_t
#include <string> // string, to_string
#include <iterator> // input_iterator_tag
#include <tuple> // tuple_size, get, tuple_element

// #include <nlohmann/detail/Value.hpp>

// #include <nlohmann/detail/meta/type_traits.hpp>


namespace nlohmann
{
namespace detail
{
template <typename IteratorType> class iteration_proxy_value
{
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = iteration_proxy_value;
    using pointer = value_type * ;
    using reference = value_type & ;
    using iterator_category = std::input_iterator_tag;

  private:
    /// the iterator
    IteratorType anchor;
    /// an index for arrays (used to create key names)
    std::size_t array_index = 0;
    /// last stringified array index
    mutable std::size_t array_index_last = 0;
    /// a string representation of the array index
    mutable std::string array_index_str = "0";
    /// an empty string (to return a reference for primitive values)
    const std::string empty_str = "";

  public:
    explicit iteration_proxy_value(IteratorType it)  : anchor(it) {}

    /// dereference operator (needed for range-based for)
    iteration_proxy_value& operator*()
    {
        return *this;
    }

    /// increment operator (needed for range-based for)
    iteration_proxy_value& operator++()
    {
        ++anchor;
        ++array_index;

        return *this;
    }

    /// equality operator (needed for InputIterator)
    bool operator==(const iteration_proxy_value& o) const 
    {
        return anchor == o.anchor;
    }

    /// inequality operator (needed for range-based for)
    bool operator!=(const iteration_proxy_value& o) const 
    {
        return anchor != o.anchor;
    }

    /// return key of the iterator
    const std::string& key() const
    {
        assert(anchor.m_object != nullptr);

        switch (anchor.m_object->type())
        {
            // use integer array index as key
            case Value::array:
            {
                if (array_index != array_index_last)
                {
                    array_index_str = std::to_string(array_index);
                    array_index_last = array_index;
                }
                return array_index_str;
            }

            // use key from the object
            case Value::object:
                return anchor.key();

            // use an empty key for all primitive types
            default:
                return empty_str;
        }
    }

    /// return value of the iterator
    typename IteratorType::reference value() const
    {
        return anchor.value();
    }
};

/// proxy class for the items() function
template<typename IteratorType> class iteration_proxy
{
  private:
    /// the container to iterate
    typename IteratorType::reference container;

  public:
    /// construct iteration proxy from a container
    explicit iteration_proxy(typename IteratorType::reference cont) 
        : container(cont) {}

    /// return iterator begin (needed for range-based for)
    iteration_proxy_value<IteratorType> begin() 
    {
        return iteration_proxy_value<IteratorType>(container.begin());
    }

    /// return iterator end (needed for range-based for)
    iteration_proxy_value<IteratorType> end() 
    {
        return iteration_proxy_value<IteratorType>(container.end());
    }
};
// Structured Bindings Support
// For further reference see https://blog.tartanllama.xyz/structured-bindings/
// And see https://github.com/nlohmann/json/pull/1391
template <std::size_t N, typename IteratorType, enable_if_t<N == 0, int> = 0>
auto get(const nlohmann::detail::iteration_proxy_value<IteratorType>& i) -> decltype(i.key())
{
    return i.key();
}
// Structured Bindings Support
// For further reference see https://blog.tartanllama.xyz/structured-bindings/
// And see https://github.com/nlohmann/json/pull/1391
template <std::size_t N, typename IteratorType, enable_if_t<N == 1, int> = 0>
auto get(const nlohmann::detail::iteration_proxy_value<IteratorType>& i) -> decltype(i.value())
{
    return i.value();
}
}  // namespace detail
}  // namespace nlohmann

// The Addition to the STD Namespace is required to add
// Structured Bindings Support to the iteration_proxy_value class
// For further reference see https://blog.tartanllama.xyz/structured-bindings/
// And see https://github.com/nlohmann/json/pull/1391
namespace std
{
template <typename IteratorType>
class tuple_size<::nlohmann::detail::iteration_proxy_value<IteratorType>>
            : public std::integral_constant<std::size_t, 2> {};

template <std::size_t N, typename IteratorType>
class tuple_element<N, ::nlohmann::detail::iteration_proxy_value<IteratorType >>
{
  public:
    using type = decltype(
                     get<N>(std::declval <
                            ::nlohmann::detail::iteration_proxy_value<IteratorType >> ()));
};
}

namespace nlohmann
{
namespace detail
{
//////////////////
// constructors //
//////////////////

template<Value> struct external_constructor;

template<>
struct external_constructor<Value::boolean>
{
    template<typename Basic_json>
    static void construct(Basic_json& j, typename Basic_json::Boolean b) 
    {
        j.type_ = Value::boolean;
        j.value_ = b;
        j.assert_invariant();
    }
};

template<>
struct external_constructor<Value::string>
{
    template<typename Basic_json>
    static void construct(Basic_json& j, const typename Basic_json::String& s)
    {
        j.type_ = Value::string;
        j.value_ = s;
        j.assert_invariant();
    }

    template<typename Basic_json>
    static void construct(Basic_json& j, typename Basic_json::String&& s)
    {
        j.type_ = Value::string;
        j.value_ = std::move(s);
        j.assert_invariant();
    }

    template<typename Basic_json, typename CompatibleStringType,
             enable_if_t<not std::is_same<CompatibleStringType, typename Basic_json::String>::value,
                         int> = 0>
    static void construct(Basic_json& j, const CompatibleStringType& str)
    {
        j.type_ = Value::string;
        j.value_.string = j.template create<typename Basic_json::String>(str);
        j.assert_invariant();
    }
};

template<>
struct external_constructor<Value::num_float>
{
    template<typename Basic_json>
    static void construct(Basic_json& j, typename Basic_json::Num_float val) 
    {
        j.type_ = Value::num_float;
        j.value_ = val;
        j.assert_invariant();
    }
};

template<>
struct external_constructor<Value::num_unsigned>
{
    template<typename Basic_json>
    static void construct(Basic_json& j, typename Basic_json::Num_unsigned val) 
    {
        j.type_ = Value::num_unsigned;
        j.value_ = val;
        j.assert_invariant();
    }
};

template<>
struct external_constructor<Value::num_integer>
{
    template<typename Basic_json>
    static void construct(Basic_json& j, typename Basic_json::Num_integer val) 
    {
        j.type_ = Value::num_integer;
        j.value_ = val;
        j.assert_invariant();
    }
};

template<>
struct external_constructor<Value::array>
{
    template<typename Basic_json>
    static void construct(Basic_json& j, const typename Basic_json::Array& arr)
    {
        j.type_ = Value::array;
        j.value_ = arr;
        j.assert_invariant();
    }

    template<typename Basic_json>
    static void construct(Basic_json& j, typename Basic_json::Array&& arr)
    {
        j.type_ = Value::array;
        j.value_ = std::move(arr);
        j.assert_invariant();
    }

    template<typename Basic_json, typename CompatibleArrayType,
             enable_if_t<not std::is_same<CompatibleArrayType, typename Basic_json::Array>::value,
                         int> = 0>
    static void construct(Basic_json& j, const CompatibleArrayType& arr)
    {
        using std::begin;
        using std::end;
        j.type_ = Value::array;
        j.value_.array = j.template create<typename Basic_json::Array>(begin(arr), end(arr));
        j.assert_invariant();
    }

    template<typename Basic_json>
    static void construct(Basic_json& j, const std::vector<bool>& arr)
    {
        j.type_ = Value::array;
        j.value_ = Value::array;
        j.value_.array->reserve(arr.size());
        for (const bool x : arr)
        {
            j.value_.array->push_back(x);
        }
        j.assert_invariant();
    }

    template<typename Basic_json, typename T,
             enable_if_t<std::is_convertible<T, Basic_json>::value, int> = 0>
    static void construct(Basic_json& j, const std::valarray<T>& arr)
    {
        j.type_ = Value::array;
        j.value_ = Value::array;
        j.value_.array->resize(arr.size());
        std::copy(std::begin(arr), std::end(arr), j.value_.array->begin());
        j.assert_invariant();
    }
};

template<>
struct external_constructor<Value::object>
{
    template<typename Basic_json>
    static void construct(Basic_json& j, const typename Basic_json::Object& obj)
    {
        j.type_ = Value::object;
        j.value_ = obj;
        j.assert_invariant();
    }

    template<typename Basic_json>
    static void construct(Basic_json& j, typename Basic_json::Object&& obj)
    {
        j.type_ = Value::object;
        j.value_ = std::move(obj);
        j.assert_invariant();
    }

    template<typename Basic_json, typename CompatibleObjectType,
             enable_if_t<not std::is_same<CompatibleObjectType, typename Basic_json::Object>::value, int> = 0>
    static void construct(Basic_json& j, const CompatibleObjectType& obj)
    {
        using std::begin;
        using std::end;

        j.type_ = Value::object;
        j.value_.object = j.template create<typename Basic_json::Object>(begin(obj), end(obj));
        j.assert_invariant();
    }
};

/////////////
// to_json //
/////////////

template<typename Basic_json, typename T,
         enable_if_t<std::is_same<T, typename Basic_json::Boolean>::value, int> = 0>
void to_json(Basic_json& j, T b) 
{
    external_constructor<Value::boolean>::construct(j, b);
}

template<typename Basic_json, typename CompatibleString,
         enable_if_t<std::is_constructible<typename Basic_json::String, CompatibleString>::value, int> = 0>
void to_json(Basic_json& j, const CompatibleString& s)
{
    external_constructor<Value::string>::construct(j, s);
}

template<typename Basic_json>
void to_json(Basic_json& j, typename Basic_json::String&& s)
{
    external_constructor<Value::string>::construct(j, std::move(s));
}

template<typename Basic_json, typename FloatType,
         enable_if_t<std::is_floating_point<FloatType>::value, int> = 0>
void to_json(Basic_json& j, FloatType val) 
{
    external_constructor<Value::num_float>::construct(j, static_cast<typename Basic_json::Num_float>(val));
}

template<typename Basic_json, typename CompatibleNumberUnsignedType,
         enable_if_t<is_compatible_integer_type<typename Basic_json::Num_unsigned, CompatibleNumberUnsignedType>::value, int> = 0>
void to_json(Basic_json& j, CompatibleNumberUnsignedType val) 
{
    external_constructor<Value::num_unsigned>::construct(j, static_cast<typename Basic_json::Num_unsigned>(val));
}

template<typename Basic_json, typename CompatibleNumberIntegerType,
         enable_if_t<is_compatible_integer_type<typename Basic_json::Num_integer, CompatibleNumberIntegerType>::value, int> = 0>
void to_json(Basic_json& j, CompatibleNumberIntegerType val) 
{
    external_constructor<Value::num_integer>::construct(j, static_cast<typename Basic_json::Num_integer>(val));
}

template<typename Basic_json, typename EnumType,
         enable_if_t<std::is_enum<EnumType>::value, int> = 0>
void to_json(Basic_json& j, EnumType e) 
{
    using underlying_type = typename std::underlying_type<EnumType>::type;
    external_constructor<Value::num_integer>::construct(j, static_cast<underlying_type>(e));
}

template<typename Basic_json>
void to_json(Basic_json& j, const std::vector<bool>& e)
{
    external_constructor<Value::array>::construct(j, e);
}

template <typename Basic_json, typename CompatibleArrayType,
          enable_if_t<is_compatible_array_type<Basic_json,
                      CompatibleArrayType>::value and
                      not is_compatible_object_type<
                          Basic_json, CompatibleArrayType>::value and
                      not is_compatible_string_type<Basic_json, CompatibleArrayType>::value and
                      not Is_basic_json<CompatibleArrayType>::value,
                      int> = 0>
void to_json(Basic_json& j, const CompatibleArrayType& arr)
{
    external_constructor<Value::array>::construct(j, arr);
}

template<typename Basic_json, typename T,
         enable_if_t<std::is_convertible<T, Basic_json>::value, int> = 0>
void to_json(Basic_json& j, const std::valarray<T>& arr)
{
    external_constructor<Value::array>::construct(j, std::move(arr));
}

template<typename Basic_json>
void to_json(Basic_json& j, typename Basic_json::Array&& arr)
{
    external_constructor<Value::array>::construct(j, std::move(arr));
}

template<typename Basic_json, typename CompatibleObjectType,
         enable_if_t<is_compatible_object_type<Basic_json, CompatibleObjectType>::value and not Is_basic_json<CompatibleObjectType>::value, int> = 0>
void to_json(Basic_json& j, const CompatibleObjectType& obj)
{
    external_constructor<Value::object>::construct(j, obj);
}

template<typename Basic_json>
void to_json(Basic_json& j, typename Basic_json::Object&& obj)
{
    external_constructor<Value::object>::construct(j, std::move(obj));
}

template <
    typename Basic_json, typename T, std::size_t N,
    enable_if_t<not std::is_constructible<typename Basic_json::String,
                const T(&)[N]>::value,
                int> = 0 >
void to_json(Basic_json& j, const T(&arr)[N])
{
    external_constructor<Value::array>::construct(j, arr);
}

template<typename Basic_json, typename... Args>
void to_json(Basic_json& j, const std::pair<Args...>& p)
{
    j = { p.first, p.second };
}

// for https://github.com/nlohmann/json/pull/1134
template < typename Basic_json, typename T,
           enable_if_t<std::is_same<T, iteration_proxy_value<typename Basic_json::iterator>>::value, int> = 0>
void to_json(Basic_json& j, const T& b)
{
    j = { {b.key(), b.value()} };
}

template<typename Basic_json, typename Tuple, std::size_t... Idx>
void to_json_tuple_impl(Basic_json& j, const Tuple& t, index_sequence<Idx...> /*unused*/)
{
    j = { std::get<Idx>(t)... };
}

template<typename Basic_json, typename... Args>
void to_json(Basic_json& j, const std::tuple<Args...>& t)
{
    to_json_tuple_impl(j, t, index_sequence_for<Args...> {});
}

struct to_json_fn
{
    template<typename Basic_json, typename T>
    auto operator()(Basic_json& j, T&& val) const ((to_json(j, std::forward<T>(val))))
    -> decltype(to_json(j, std::forward<T>(val)), void())
    {
        return to_json(j, std::forward<T>(val));
    }
};
}  // namespace detail

/// namespace to hold default `to_json` function
namespace
{
constexpr const auto& to_json = detail::static_const<detail::to_json_fn>::value;
} // namespace
}  // namespace nlohmann

// #include <nlohmann/detail/input/input_adapters.hpp>


#include <cassert> // assert
#include <cstddef> // size_t
#include <cstring> // strlen
#include <istream> // istream
#include <iterator> // begin, end, iterator_traits, random_access_iterator_tag, distance, next
#include <memory> // shared_ptr, make_shared, addressof
#include <numeric> // accumulate
#include <string> // string, char_traits
#include <type_traits> // enable_if, is_base_of, is_pointer, is_integral, remove_pointer
#include <utility> // pair, declval
#include <cstdio> //FILE *

// #include <nlohmann/detail/macro_scope.hpp>


namespace nlohmann
{
namespace detail
{
/// the supported input formats
enum class input_format_t { json, cbor, msgpack, ubjson, bson };

////////////////////
// input adapters //
////////////////////

/*!
@brief abstract input adapter interface

Produces a stream of std::char_traits<char>::int_type characters from a
std::istream, a buffer, or some other input type. Accepts the return of
exactly one non-EOF character for future input. The int_type characters
returned consist of all valid char values as positive values (typically
unsigned char), plus an EOF value outside that range, specified by the value
of the function std::char_traits<char>::eof(). This value is typically -1, but
could be any arbitrary value which is not a valid char value.
*/
struct input_adapter_protocol
{
    /// get a character [0,255] or std::char_traits<char>::eof().
    virtual std::char_traits<char>::int_type get_character() = 0;
    virtual ~input_adapter_protocol() = default;
};

/// a type to simplify interfaces
using input_adapter_t = std::shared_ptr<input_adapter_protocol>;

/*!
Input adapter for stdio file access. This adapter read only 1 byte and do not use any
 buffer. This adapter is a very low level adapter.
*/
class file_input_adapter : public input_adapter_protocol
{
  public:
    explicit file_input_adapter(std::FILE* f)  
        : m_file(f)
    {}

    std::char_traits<char>::int_type get_character()  override
    {
        return std::fgetc(m_file);
    }
  private:
    /// the file pointer to read from
    std::FILE* m_file;
};


/*!
Input adapter for a (caching) istream. Ignores a UFT Byte Order Mark at
beginning of input. Does not support changing the underlying std::streambuf
in mid-input. Maintains underlying std::istream and std::streambuf to support
subsequent use of standard std::istream operations to process any input
characters following those used in parsing the JSON input.  Clears the
std::istream flags; any input errors (e.g., EOF) will be detected by the first
subsequent call for input from the std::istream.
*/
class input_stream_adapter : public input_adapter_protocol
{
  public:
    ~input_stream_adapter() override
    {
        // clear stream flags; we use underlying streambuf I/O, do not
        // maintain ifstream flags, except eof
        is.clear(is.rdstate() & std::ios::eofbit);
    }

    explicit input_stream_adapter(std::istream& i)
        : is(i), sb(*i.rdbuf())
    {}

    // delete because of pointer members
    input_stream_adapter(const input_stream_adapter&) = delete;
    input_stream_adapter& operator=(input_stream_adapter&) = delete;
    input_stream_adapter(input_stream_adapter&&) = delete;
    input_stream_adapter& operator=(input_stream_adapter&&) = delete;

    // std::istream/std::streambuf use std::char_traits<char>::to_int_type, to
    // ensure that std::char_traits<char>::eof() and the character 0xFF do not
    // end up as the same value, eg. 0xFFFFFFFF.
    std::char_traits<char>::int_type get_character() override
    {
        auto res = sb.sbumpc();
        // set eof manually, as we don't use the istream interface.
        if (res == EOF)
        {
            is.clear(is.rdstate() | std::ios::eofbit);
        }
        return res;
    }

  private:
    /// the associated input stream
    std::istream& is;
    std::streambuf& sb;
};

/// input adapter for buffer input
class input_buffer_adapter : public input_adapter_protocol
{
  public:
    input_buffer_adapter(const char* b, const std::size_t l) 
        : cursor(b), limit(b + l)
    {}

    // delete because of pointer members
    input_buffer_adapter(const input_buffer_adapter&) = delete;
    input_buffer_adapter& operator=(input_buffer_adapter&) = delete;
    input_buffer_adapter(input_buffer_adapter&&) = delete;
    input_buffer_adapter& operator=(input_buffer_adapter&&) = delete;
    ~input_buffer_adapter() override = default;

    std::char_traits<char>::int_type get_character()  override
    {
        if ((cursor < limit))
        {
            return std::char_traits<char>::to_int_type(*(cursor++));
        }

        return std::char_traits<char>::eof();
    }

  private:
    /// pointer to the current character
    const char* cursor;
    /// pointer past the last character
    const char* const limit;
};

template<typename WideStringType, size_t T>
struct wide_string_input_helper
{
    // UTF-32
    static void fill_buffer(const WideStringType& str, size_t& current_wchar, std::array<std::char_traits<char>::int_type, 4>& utf8_bytes, size_t& utf8_bytes_index, size_t& utf8_bytes_filled)
    {
        utf8_bytes_index = 0;

        if (current_wchar == str.size())
        {
            utf8_bytes[0] = std::char_traits<char>::eof();
            utf8_bytes_filled = 1;
        }
        else
        {
            // get the current character
            const auto wc = static_cast<int>(str[current_wchar++]);

            // UTF-32 to UTF-8 encoding
            if (wc < 0x80)
            {
                utf8_bytes[0] = wc;
                utf8_bytes_filled = 1;
            }
            else if (wc <= 0x7FF)
            {
                utf8_bytes[0] = 0xC0 | ((wc >> 6) & 0x1F);
                utf8_bytes[1] = 0x80 | (wc & 0x3F);
                utf8_bytes_filled = 2;
            }
            else if (wc <= 0xFFFF)
            {
                utf8_bytes[0] = 0xE0 | ((wc >> 12) & 0x0F);
                utf8_bytes[1] = 0x80 | ((wc >> 6) & 0x3F);
                utf8_bytes[2] = 0x80 | (wc & 0x3F);
                utf8_bytes_filled = 3;
            }
            else if (wc <= 0x10FFFF)
            {
                utf8_bytes[0] = 0xF0 | ((wc >> 18) & 0x07);
                utf8_bytes[1] = 0x80 | ((wc >> 12) & 0x3F);
                utf8_bytes[2] = 0x80 | ((wc >> 6) & 0x3F);
                utf8_bytes[3] = 0x80 | (wc & 0x3F);
                utf8_bytes_filled = 4;
            }
            else
            {
                // unknown character
                utf8_bytes[0] = wc;
                utf8_bytes_filled = 1;
            }
        }
    }
};

template<typename WideStringType>
struct wide_string_input_helper<WideStringType, 2>
{
    // UTF-16
    static void fill_buffer(const WideStringType& str, size_t& current_wchar, std::array<std::char_traits<char>::int_type, 4>& utf8_bytes, size_t& utf8_bytes_index, size_t& utf8_bytes_filled)
    {
        utf8_bytes_index = 0;

        if (current_wchar == str.size())
        {
            utf8_bytes[0] = std::char_traits<char>::eof();
            utf8_bytes_filled = 1;
        }
        else
        {
            // get the current character
            const auto wc = static_cast<int>(str[current_wchar++]);

            // UTF-16 to UTF-8 encoding
            if (wc < 0x80)
            {
                utf8_bytes[0] = wc;
                utf8_bytes_filled = 1;
            }
            else if (wc <= 0x7FF)
            {
                utf8_bytes[0] = 0xC0 | ((wc >> 6));
                utf8_bytes[1] = 0x80 | (wc & 0x3F);
                utf8_bytes_filled = 2;
            }
            else if (0xD800 > wc or wc >= 0xE000)
            {
                utf8_bytes[0] = 0xE0 | ((wc >> 12));
                utf8_bytes[1] = 0x80 | ((wc >> 6) & 0x3F);
                utf8_bytes[2] = 0x80 | (wc & 0x3F);
                utf8_bytes_filled = 3;
            }
            else
            {
                if (current_wchar < str.size())
                {
                    const auto wc2 = static_cast<int>(str[current_wchar++]);
                    const int charcode = 0x10000 + (((wc & 0x3FF) << 10) | (wc2 & 0x3FF));
                    utf8_bytes[0] = 0xf0 | (charcode >> 18);
                    utf8_bytes[1] = 0x80 | ((charcode >> 12) & 0x3F);
                    utf8_bytes[2] = 0x80 | ((charcode >> 6) & 0x3F);
                    utf8_bytes[3] = 0x80 | (charcode & 0x3F);
                    utf8_bytes_filled = 4;
                }
                else
                {
                    // unknown character
                    ++current_wchar;
                    utf8_bytes[0] = wc;
                    utf8_bytes_filled = 1;
                }
            }
        }
    }
};

template<typename WideStringType>
class wide_string_input_adapter : public input_adapter_protocol
{
  public:
    explicit wide_string_input_adapter(const WideStringType& w)  
        : str(w)
    {}

    std::char_traits<char>::int_type get_character()  override
    {
        // check if buffer needs to be filled
        if (utf8_bytes_index == utf8_bytes_filled)
        {
            fill_buffer<sizeof(typename WideStringType::value_type)>();

            assert(utf8_bytes_filled > 0);
            assert(utf8_bytes_index == 0);
        }

        // use buffer
        assert(utf8_bytes_filled > 0);
        assert(utf8_bytes_index < utf8_bytes_filled);
        return utf8_bytes[utf8_bytes_index++];
    }

  private:
    template<size_t T>
    void fill_buffer()
    {
        wide_string_input_helper<WideStringType, T>::fill_buffer(str, current_wchar, utf8_bytes, utf8_bytes_index, utf8_bytes_filled);
    }

    /// the wstring to process
    const WideStringType& str;

    /// index of the current wchar in str
    std::size_t current_wchar = 0;

    /// a buffer for UTF-8 bytes
    std::array<std::char_traits<char>::int_type, 4> utf8_bytes = {{0, 0, 0, 0}};

    /// index to the utf8_codes array for the next valid byte
    std::size_t utf8_bytes_index = 0;
    /// number of valid bytes in the utf8_codes array
    std::size_t utf8_bytes_filled = 0;
};

class input_adapter
{
  public:
    // native support
    input_adapter(std::FILE* file)
        : ia(std::make_shared<file_input_adapter>(file)) {}
    /// input adapter for input stream
    input_adapter(std::istream& i)
        : ia(std::make_shared<input_stream_adapter>(i)) {}

    /// input adapter for input stream
    input_adapter(std::istream&& i)
        : ia(std::make_shared<input_stream_adapter>(i)) {}

    input_adapter(const std::wstring& ws)
        : ia(std::make_shared<wide_string_input_adapter<std::wstring>>(ws)) {}

    input_adapter(const std::u16string& ws)
        : ia(std::make_shared<wide_string_input_adapter<std::u16string>>(ws)) {}

    input_adapter(const std::u32string& ws)
        : ia(std::make_shared<wide_string_input_adapter<std::u32string>>(ws)) {}

    /// input adapter for buffer
    template<typename CharT,
             typename std::enable_if<
                 std::is_pointer<CharT>::value and
                 std::is_integral<typename std::remove_pointer<CharT>::type>::value and
                 sizeof(typename std::remove_pointer<CharT>::type) == 1,
                 int>::type = 0>
    input_adapter(CharT b, std::size_t l)
        : ia(std::make_shared<input_buffer_adapter>(reinterpret_cast<const char*>(b), l)) {}

    // derived support

    /// input adapter for string literal
    template<typename CharT,
             typename std::enable_if<
                 std::is_pointer<CharT>::value and
                 std::is_integral<typename std::remove_pointer<CharT>::type>::value and
                 sizeof(typename std::remove_pointer<CharT>::type) == 1,
                 int>::type = 0>
    input_adapter(CharT b)
        : input_adapter(reinterpret_cast<const char*>(b),
                        std::strlen(reinterpret_cast<const char*>(b))) {}

    /// input adapter for iterator range with contiguous storage
    template<class IteratorType,
             typename std::enable_if<
                 std::is_same<typename iterator_traits<IteratorType>::iterator_category, std::random_access_iterator_tag>::value,
                 int>::type = 0>
    input_adapter(IteratorType first, IteratorType last)
    {
#ifndef NDEBUG
        // assertion to check that the iterator range is indeed contiguous,
        // see http://stackoverflow.com/a/35008842/266378 for more discussion
        const auto is_contiguous = std::accumulate(
                                       first, last, std::pair<bool, int>(true, 0),
                                       [&first](std::pair<bool, int> res, decltype(*first) val)
        {
            res.first &= (val == *(std::next(std::addressof(*first), res.second++)));
            return res;
        }).first;
        assert(is_contiguous);
#endif

        // assertion to check that each element is 1 byte long
        static_assert(
            sizeof(typename iterator_traits<IteratorType>::value_type) == 1,
            "each element in the iterator range must have the size of 1 byte");

        const auto len = static_cast<size_t>(std::distance(first, last));
        if ((len > 0))
        {
            // there is at least one element: use the address of first
            ia = std::make_shared<input_buffer_adapter>(reinterpret_cast<const char*>(&(*first)), len);
        }
        else
        {
            // the address of first cannot be used: use nullptr
            ia = std::make_shared<input_buffer_adapter>(nullptr, len);
        }
    }

    /// input adapter for array
    template<class T, std::size_t N>
    input_adapter(T (&array)[N])
        : input_adapter(std::begin(array), std::end(array)) {}

    /// input adapter for contiguous container
    template<class ContiguousContainer, typename
             std::enable_if<not std::is_pointer<ContiguousContainer>::value and
                            std::is_base_of<std::random_access_iterator_tag, typename iterator_traits<decltype(std::begin(std::declval<ContiguousContainer const>()))>::iterator_category>::value,
                            int>::type = 0>
    input_adapter(const ContiguousContainer& c)
        : input_adapter(std::begin(c), std::end(c)) {}

    operator input_adapter_t()
    {
        return ia;
    }

  private:
    /// the actual adapter
    input_adapter_t ia = nullptr;
};
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/input/Lexer.hpp>


#include <clocale> // localeconv
#include <cstddef> // size_t
#include <cstdlib> // strtof, strtod, strtold, strtoll, strtoull
#include <cstdio> // snprintf
#include <initializer_list> // initializer_list
#include <string> // char_traits, string
#include <vector> // vector

// #include <nlohmann/detail/macro_scope.hpp>

// #include <nlohmann/detail/input/input_adapters.hpp>

// #include <nlohmann/detail/input/position_t.hpp>


namespace nlohmann
{
namespace detail
{
///////////
// Lexer //
///////////

/*!
@brief lexical analysis

This class organizes the lexical analysis during JSON deserialization.
*/
template<typename Basic_json>
class Lexer
{
    using Num_integer = typename Basic_json::Num_integer;
    using Num_unsigned = typename Basic_json::Num_unsigned;
    using Num_float = typename Basic_json::Num_float;
    using String = typename Basic_json::String;

  public:
    /// token types for the Parser
    enum class token_type
    {
        uninitialized,    ///< indicating the scanner is uninitialized
        literal_true,     ///< the `true` literal
        literal_false,    ///< the `false` literal
        literal_null,     ///< the `null` literal
        value_string,     ///< a string -- use get_string() for actual value
        value_unsigned,   ///< an unsigned integer -- use get_number_unsigned() for actual value
        value_integer,    ///< a signed integer -- use get_number_integer() for actual value
        value_float,      ///< an floating point number -- use get_number_float() for actual value
        begin_array,      ///< the character for array begin `[`
        begin_object,     ///< the character for object begin `{`
        end_array,        ///< the character for array end `]`
        end_object,       ///< the character for object end `}`
        name_separator,   ///< the name separator `:`
        value_separator,  ///< the value separator `,`
        parse_error,      ///< indicating a parse error
        end_of_input,     ///< indicating the end of the input buffer
        literal_or_value  ///< a literal or the begin of a value (only for diagnostics)
    };

    /// return name of values of type token_type (only used for errors)
    static const char* token_type_name(const token_type t) 
    {
        switch (t)
        {
            case token_type::uninitialized:
                return "<uninitialized>";
            case token_type::literal_true:
                return "true literal";
            case token_type::literal_false:
                return "false literal";
            case token_type::literal_null:
                return "null literal";
            case token_type::value_string:
                return "string literal";
            case Lexer::token_type::value_unsigned:
            case Lexer::token_type::value_integer:
            case Lexer::token_type::value_float:
                return "number literal";
            case token_type::begin_array:
                return "'['";
            case token_type::begin_object:
                return "'{'";
            case token_type::end_array:
                return "']'";
            case token_type::end_object:
                return "'}'";
            case token_type::name_separator:
                return "':'";
            case token_type::value_separator:
                return "','";
            case token_type::parse_error:
                return "<parse error>";
            case token_type::end_of_input:
                return "end of input";
            case token_type::literal_or_value:
                return "'[', '{', or a literal";
            // LCOV_EXCL_START
            default: // catch non-enum values
                return "unknown token";
                // LCOV_EXCL_STOP
        }
    }

    explicit Lexer(detail::input_adapter_t&& adapter)
        : ia(std::move(adapter)), decimal_point_char(get_decimal_point()) {}

    // delete because of pointer members
    Lexer(const Lexer&) = delete;
    Lexer(Lexer&&) = delete;
    Lexer& operator=(Lexer&) = delete;
    Lexer& operator=(Lexer&&) = delete;
    ~Lexer() = default;

  private:
    /////////////////////
    // locales
    /////////////////////

    /// return the locale-dependent decimal point
    static char get_decimal_point() 
    {
        const auto loc = localeconv();
        assert(loc != nullptr);
        return (loc->decimal_point == nullptr) ? '.' : *(loc->decimal_point);
    }

    /////////////////////
    // scan functions
    /////////////////////

    /*!
    @brief get codepoint from 4 hex characters following `\u`

    For input "\u c1 c2 c3 c4" the codepoint is:
      (c1 * 0x1000) + (c2 * 0x0100) + (c3 * 0x0010) + c4
    = (c1 << 12) + (c2 << 8) + (c3 << 4) + (c4 << 0)

    Furthermore, the possible characters '0'..'9', 'A'..'F', and 'a'..'f'
    must be converted to the integers 0x0..0x9, 0xA..0xF, 0xA..0xF, resp. The
    conversion is done by subtracting the offset (0x30, 0x37, and 0x57)
    between the ASCII value of the character and the desired integer value.

    @return codepoint (0x0000..0xFFFF) or -1 in case of an error (e.g. EOF or
            non-hex character)
    */
    int get_codepoint()
    {
        // this function only makes sense after reading `\u`
        assert(current == 'u');
        int codepoint = 0;

        const auto factors = { 12, 8, 4, 0 };
        for (const auto factor : factors)
        {
            get();

            if (current >= '0' and current <= '9')
            {
                codepoint += ((current - 0x30) << factor);
            }
            else if (current >= 'A' and current <= 'F')
            {
                codepoint += ((current - 0x37) << factor);
            }
            else if (current >= 'a' and current <= 'f')
            {
                codepoint += ((current - 0x57) << factor);
            }
            else
            {
                return -1;
            }
        }

        assert(0x0000 <= codepoint and codepoint <= 0xFFFF);
        return codepoint;
    }

    /*!
    @brief check if the next byte(s) are inside a given range

    Adds the current byte and, for each passed range, reads a new byte and
    checks if it is inside the range. If a violation was detected, set up an
    error message and return false. Otherwise, return true.

    @param[in] ranges  list of integers; interpreted as list of pairs of
                       inclusive lower and upper bound, respectively

    @pre The passed list @a ranges must have 2, 4, or 6 elements; that is,
         1, 2, or 3 pairs. This precondition is enforced by an assertion.

    @return true if and only if no range violation was detected
    */
    bool next_byte_in_range(std::initializer_list<int> ranges)
    {
        assert(ranges.size() == 2 or ranges.size() == 4 or ranges.size() == 6);
        add(current);

        for (auto range = ranges.begin(); range != ranges.end(); ++range)
        {
            get();
            if ((*range <= current and current <= *(++range)))
            {
                add(current);
            }
            else
            {
                error_message = "invalid string: ill-formed UTF-8 byte";
                return false;
            }
        }

        return true;
    }

    /*!
    @brief scan a string literal

    This function scans a string according to Sect. 7 of RFC 7159. While
    scanning, bytes are escaped and copied into buffer token_buffer. Then the
    function returns successfully, token_buffer is *not* null-terminated (as it
    may contain \0 bytes), and token_buffer.size() is the number of bytes in the
    string.

    @return token_type::value_string if string could be successfully scanned,
            token_type::parse_error otherwise

    @note In case of errors, variable error_message contains a textual
          description.
    */
    token_type scan_string()
    {
        // reset token_buffer (ignore opening quote)
        reset();

        // we entered the function by reading an open quote
        assert(current == '\"');

        while (true)
        {
            // get next character
            switch (get())
            {
                // end of file while parsing string
                case std::char_traits<char>::eof():
                {
                    error_message = "invalid string: missing closing quote";
                    return token_type::parse_error;
                }

                // closing quote
                case '\"':
                {
                    return token_type::value_string;
                }

                // escapes
                case '\\':
                {
                    switch (get())
                    {
                        // quotation mark
                        case '\"':
                            add('\"');
                            break;
                        // reverse solidus
                        case '\\':
                            add('\\');
                            break;
                        // solidus
                        case '/':
                            add('/');
                            break;
                        // backspace
                        case 'b':
                            add('\b');
                            break;
                        // form feed
                        case 'f':
                            add('\f');
                            break;
                        // line feed
                        case 'n':
                            add('\n');
                            break;
                        // carriage return
                        case 'r':
                            add('\r');
                            break;
                        // tab
                        case 't':
                            add('\t');
                            break;

                        // unicode escapes
                        case 'u':
                        {
                            const int codepoint1 = get_codepoint();
                            int codepoint = codepoint1; // start with codepoint1

                            if ((codepoint1 == -1))
                            {
                                error_message = "invalid string: '\\u' must be followed by 4 hex digits";
                                return token_type::parse_error;
                            }

                            // check if code point is a high surrogate
                            if (0xD800 <= codepoint1 and codepoint1 <= 0xDBFF)
                            {
                                // expect next \uxxxx entry
                                if ((get() == '\\' and get() == 'u'))
                                {
                                    const int codepoint2 = get_codepoint();

                                    if ((codepoint2 == -1))
                                    {
                                        error_message = "invalid string: '\\u' must be followed by 4 hex digits";
                                        return token_type::parse_error;
                                    }

                                    // check if codepoint2 is a low surrogate
                                    if ((0xDC00 <= codepoint2 and codepoint2 <= 0xDFFF))
                                    {
                                        // overwrite codepoint
                                        codepoint =
                                            // high surrogate occupies the most significant 22 bits
                                            (codepoint1 << 10)
                                            // low surrogate occupies the least significant 15 bits
                                            + codepoint2
                                            // there is still the 0xD800, 0xDC00 and 0x10000 noise
                                            // in the result so we have to subtract with:
                                            // (0xD800 << 10) + DC00 - 0x10000 = 0x35FDC00
                                            - 0x35FDC00;
                                    }
                                    else
                                    {
                                        error_message = "invalid string: surrogate U+DC00..U+DFFF must be followed by U+DC00..U+DFFF";
                                        return token_type::parse_error;
                                    }
                                }
                                else
                                {
                                    error_message = "invalid string: surrogate U+DC00..U+DFFF must be followed by U+DC00..U+DFFF";
                                    return token_type::parse_error;
                                }
                            }
                            else
                            {
                                if ((0xDC00 <= codepoint1 and codepoint1 <= 0xDFFF))
                                {
                                    error_message = "invalid string: surrogate U+DC00..U+DFFF must follow U+D800..U+DBFF";
                                    return token_type::parse_error;
                                }
                            }

                            // result of the above calculation yields a proper codepoint
                            assert(0x00 <= codepoint and codepoint <= 0x10FFFF);

                            // translate codepoint into bytes
                            if (codepoint < 0x80)
                            {
                                // 1-byte characters: 0xxxxxxx (ASCII)
                                add(codepoint);
                            }
                            else if (codepoint <= 0x7FF)
                            {
                                // 2-byte characters: 110xxxxx 10xxxxxx
                                add(0xC0 | (codepoint >> 6));
                                add(0x80 | (codepoint & 0x3F));
                            }
                            else if (codepoint <= 0xFFFF)
                            {
                                // 3-byte characters: 1110xxxx 10xxxxxx 10xxxxxx
                                add(0xE0 | (codepoint >> 12));
                                add(0x80 | ((codepoint >> 6) & 0x3F));
                                add(0x80 | (codepoint & 0x3F));
                            }
                            else
                            {
                                // 4-byte characters: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                                add(0xF0 | (codepoint >> 18));
                                add(0x80 | ((codepoint >> 12) & 0x3F));
                                add(0x80 | ((codepoint >> 6) & 0x3F));
                                add(0x80 | (codepoint & 0x3F));
                            }

                            break;
                        }

                        // other characters after escape
                        default:
                            error_message = "invalid string: forbidden character after backslash";
                            return token_type::parse_error;
                    }

                    break;
                }

                // invalid control characters
                case 0x00:
                {
                    error_message = "invalid string: control character U+0000 (NUL) must be escaped to \\u0000";
                    return token_type::parse_error;
                }

                case 0x01:
                {
                    error_message = "invalid string: control character U+0001 (SOH) must be escaped to \\u0001";
                    return token_type::parse_error;
                }

                case 0x02:
                {
                    error_message = "invalid string: control character U+0002 (STX) must be escaped to \\u0002";
                    return token_type::parse_error;
                }

                case 0x03:
                {
                    error_message = "invalid string: control character U+0003 (ETX) must be escaped to \\u0003";
                    return token_type::parse_error;
                }

                case 0x04:
                {
                    error_message = "invalid string: control character U+0004 (EOT) must be escaped to \\u0004";
                    return token_type::parse_error;
                }

                case 0x05:
                {
                    error_message = "invalid string: control character U+0005 (ENQ) must be escaped to \\u0005";
                    return token_type::parse_error;
                }

                case 0x06:
                {
                    error_message = "invalid string: control character U+0006 (ACK) must be escaped to \\u0006";
                    return token_type::parse_error;
                }

                case 0x07:
                {
                    error_message = "invalid string: control character U+0007 (BEL) must be escaped to \\u0007";
                    return token_type::parse_error;
                }

                case 0x08:
                {
                    error_message = "invalid string: control character U+0008 (BS) must be escaped to \\u0008 or \\b";
                    return token_type::parse_error;
                }

                case 0x09:
                {
                    error_message = "invalid string: control character U+0009 (HT) must be escaped to \\u0009 or \\t";
                    return token_type::parse_error;
                }

                case 0x0A:
                {
                    error_message = "invalid string: control character U+000A (LF) must be escaped to \\u000A or \\n";
                    return token_type::parse_error;
                }

                case 0x0B:
                {
                    error_message = "invalid string: control character U+000B (VT) must be escaped to \\u000B";
                    return token_type::parse_error;
                }

                case 0x0C:
                {
                    error_message = "invalid string: control character U+000C (FF) must be escaped to \\u000C or \\f";
                    return token_type::parse_error;
                }

                case 0x0D:
                {
                    error_message = "invalid string: control character U+000D (CR) must be escaped to \\u000D or \\r";
                    return token_type::parse_error;
                }

                case 0x0E:
                {
                    error_message = "invalid string: control character U+000E (SO) must be escaped to \\u000E";
                    return token_type::parse_error;
                }

                case 0x0F:
                {
                    error_message = "invalid string: control character U+000F (SI) must be escaped to \\u000F";
                    return token_type::parse_error;
                }

                case 0x10:
                {
                    error_message = "invalid string: control character U+0010 (DLE) must be escaped to \\u0010";
                    return token_type::parse_error;
                }

                case 0x11:
                {
                    error_message = "invalid string: control character U+0011 (DC1) must be escaped to \\u0011";
                    return token_type::parse_error;
                }

                case 0x12:
                {
                    error_message = "invalid string: control character U+0012 (DC2) must be escaped to \\u0012";
                    return token_type::parse_error;
                }

                case 0x13:
                {
                    error_message = "invalid string: control character U+0013 (DC3) must be escaped to \\u0013";
                    return token_type::parse_error;
                }

                case 0x14:
                {
                    error_message = "invalid string: control character U+0014 (DC4) must be escaped to \\u0014";
                    return token_type::parse_error;
                }

                case 0x15:
                {
                    error_message = "invalid string: control character U+0015 (NAK) must be escaped to \\u0015";
                    return token_type::parse_error;
                }

                case 0x16:
                {
                    error_message = "invalid string: control character U+0016 (SYN) must be escaped to \\u0016";
                    return token_type::parse_error;
                }

                case 0x17:
                {
                    error_message = "invalid string: control character U+0017 (ETB) must be escaped to \\u0017";
                    return token_type::parse_error;
                }

                case 0x18:
                {
                    error_message = "invalid string: control character U+0018 (CAN) must be escaped to \\u0018";
                    return token_type::parse_error;
                }

                case 0x19:
                {
                    error_message = "invalid string: control character U+0019 (EM) must be escaped to \\u0019";
                    return token_type::parse_error;
                }

                case 0x1A:
                {
                    error_message = "invalid string: control character U+001A (SUB) must be escaped to \\u001A";
                    return token_type::parse_error;
                }

                case 0x1B:
                {
                    error_message = "invalid string: control character U+001B (ESC) must be escaped to \\u001B";
                    return token_type::parse_error;
                }

                case 0x1C:
                {
                    error_message = "invalid string: control character U+001C (FS) must be escaped to \\u001C";
                    return token_type::parse_error;
                }

                case 0x1D:
                {
                    error_message = "invalid string: control character U+001D (GS) must be escaped to \\u001D";
                    return token_type::parse_error;
                }

                case 0x1E:
                {
                    error_message = "invalid string: control character U+001E (RS) must be escaped to \\u001E";
                    return token_type::parse_error;
                }

                case 0x1F:
                {
                    error_message = "invalid string: control character U+001F (US) must be escaped to \\u001F";
                    return token_type::parse_error;
                }

                // U+0020..U+007F (except U+0022 (quote) and U+005C (backspace))
                case 0x20:
                case 0x21:
                case 0x23:
                case 0x24:
                case 0x25:
                case 0x26:
                case 0x27:
                case 0x28:
                case 0x29:
                case 0x2A:
                case 0x2B:
                case 0x2C:
                case 0x2D:
                case 0x2E:
                case 0x2F:
                case 0x30:
                case 0x31:
                case 0x32:
                case 0x33:
                case 0x34:
                case 0x35:
                case 0x36:
                case 0x37:
                case 0x38:
                case 0x39:
                case 0x3A:
                case 0x3B:
                case 0x3C:
                case 0x3D:
                case 0x3E:
                case 0x3F:
                case 0x40:
                case 0x41:
                case 0x42:
                case 0x43:
                case 0x44:
                case 0x45:
                case 0x46:
                case 0x47:
                case 0x48:
                case 0x49:
                case 0x4A:
                case 0x4B:
                case 0x4C:
                case 0x4D:
                case 0x4E:
                case 0x4F:
                case 0x50:
                case 0x51:
                case 0x52:
                case 0x53:
                case 0x54:
                case 0x55:
                case 0x56:
                case 0x57:
                case 0x58:
                case 0x59:
                case 0x5A:
                case 0x5B:
                case 0x5D:
                case 0x5E:
                case 0x5F:
                case 0x60:
                case 0x61:
                case 0x62:
                case 0x63:
                case 0x64:
                case 0x65:
                case 0x66:
                case 0x67:
                case 0x68:
                case 0x69:
                case 0x6A:
                case 0x6B:
                case 0x6C:
                case 0x6D:
                case 0x6E:
                case 0x6F:
                case 0x70:
                case 0x71:
                case 0x72:
                case 0x73:
                case 0x74:
                case 0x75:
                case 0x76:
                case 0x77:
                case 0x78:
                case 0x79:
                case 0x7A:
                case 0x7B:
                case 0x7C:
                case 0x7D:
                case 0x7E:
                case 0x7F:
                {
                    add(current);
                    break;
                }

                // U+0080..U+07FF: bytes C2..DF 80..BF
                case 0xC2:
                case 0xC3:
                case 0xC4:
                case 0xC5:
                case 0xC6:
                case 0xC7:
                case 0xC8:
                case 0xC9:
                case 0xCA:
                case 0xCB:
                case 0xCC:
                case 0xCD:
                case 0xCE:
                case 0xCF:
                case 0xD0:
                case 0xD1:
                case 0xD2:
                case 0xD3:
                case 0xD4:
                case 0xD5:
                case 0xD6:
                case 0xD7:
                case 0xD8:
                case 0xD9:
                case 0xDA:
                case 0xDB:
                case 0xDC:
                case 0xDD:
                case 0xDE:
                case 0xDF:
                {
                    if ((not next_byte_in_range({0x80, 0xBF})))
                    {
                        return token_type::parse_error;
                    }
                    break;
                }

                // U+0800..U+0FFF: bytes E0 A0..BF 80..BF
                case 0xE0:
                {
                    if ((not (next_byte_in_range({0xA0, 0xBF, 0x80, 0xBF}))))
                    {
                        return token_type::parse_error;
                    }
                    break;
                }

                // U+1000..U+CFFF: bytes E1..EC 80..BF 80..BF
                // U+E000..U+FFFF: bytes EE..EF 80..BF 80..BF
                case 0xE1:
                case 0xE2:
                case 0xE3:
                case 0xE4:
                case 0xE5:
                case 0xE6:
                case 0xE7:
                case 0xE8:
                case 0xE9:
                case 0xEA:
                case 0xEB:
                case 0xEC:
                case 0xEE:
                case 0xEF:
                {
                    if ((not (next_byte_in_range({0x80, 0xBF, 0x80, 0xBF}))))
                    {
                        return token_type::parse_error;
                    }
                    break;
                }

                // U+D000..U+D7FF: bytes ED 80..9F 80..BF
                case 0xED:
                {
                    if ((not (next_byte_in_range({0x80, 0x9F, 0x80, 0xBF}))))
                    {
                        return token_type::parse_error;
                    }
                    break;
                }

                // U+10000..U+3FFFF F0 90..BF 80..BF 80..BF
                case 0xF0:
                {
                    if ((not (next_byte_in_range({0x90, 0xBF, 0x80, 0xBF, 0x80, 0xBF}))))
                    {
                        return token_type::parse_error;
                    }
                    break;
                }

                // U+40000..U+FFFFF F1..F3 80..BF 80..BF 80..BF
                case 0xF1:
                case 0xF2:
                case 0xF3:
                {
                    if ((not (next_byte_in_range({0x80, 0xBF, 0x80, 0xBF, 0x80, 0xBF}))))
                    {
                        return token_type::parse_error;
                    }
                    break;
                }

                // U+100000..U+10FFFF F4 80..8F 80..BF 80..BF
                case 0xF4:
                {
                    if ((not (next_byte_in_range({0x80, 0x8F, 0x80, 0xBF, 0x80, 0xBF}))))
                    {
                        return token_type::parse_error;
                    }
                    break;
                }

                // remaining bytes (80..C1 and F5..FF) are ill-formed
                default:
                {
                    error_message = "invalid string: ill-formed UTF-8 byte";
                    return token_type::parse_error;
                }
            }
        }
    }

    static void strtof(float& f, const char* str, char** endptr) 
    {
        f = std::strtof(str, endptr);
    }

    static void strtof(double& f, const char* str, char** endptr) 
    {
        f = std::strtod(str, endptr);
    }

    static void strtof(long double& f, const char* str, char** endptr) 
    {
        f = std::strtold(str, endptr);
    }

  
    token_type scan_number()  // lgtm [cpp/use-of-goto]
    {
        // reset token_buffer to store the number's bytes
        reset();

        // the type of the parsed number; initially set to unsigned; will be
        // changed if minus sign, decimal point or exponent is read
        token_type number_type = token_type::value_unsigned;

        // state (init): we just found out we need to scan a number
        switch (current)
        {
            case '-':
            {
                add(current);
                goto scan_number_minus;
            }

            case '0':
            {
                add(current);
                goto scan_number_zero;
            }

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                add(current);
                goto scan_number_any1;
            }

            // LCOV_EXCL_START
            default:
            {
                // all other characters are rejected outside scan_number()
                assert(false);
            }
                // LCOV_EXCL_STOP
        }

scan_number_minus:
        // state: we just parsed a leading minus sign
        number_type = token_type::value_integer;
        switch (get())
        {
            case '0':
            {
                add(current);
                goto scan_number_zero;
            }

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                add(current);
                goto scan_number_any1;
            }

            default:
            {
                error_message = "invalid number; expected digit after '-'";
                return token_type::parse_error;
            }
        }

scan_number_zero:
        // state: we just parse a zero (maybe with a leading minus sign)
        switch (get())
        {
            case '.':
            {
                add(decimal_point_char);
                goto scan_number_decimal1;
            }

            case 'e':
            case 'E':
            {
                add(current);
                goto scan_number_exponent;
            }

            default:
                goto scan_number_done;
        }

scan_number_any1:
        // state: we just parsed a number 0-9 (maybe with a leading minus sign)
        switch (get())
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                add(current);
                goto scan_number_any1;
            }

            case '.':
            {
                add(decimal_point_char);
                goto scan_number_decimal1;
            }

            case 'e':
            case 'E':
            {
                add(current);
                goto scan_number_exponent;
            }

            default:
                goto scan_number_done;
        }

scan_number_decimal1:
        // state: we just parsed a decimal point
        number_type = token_type::value_float;
        switch (get())
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                add(current);
                goto scan_number_decimal2;
            }

            default:
            {
                error_message = "invalid number; expected digit after '.'";
                return token_type::parse_error;
            }
        }

scan_number_decimal2:
        // we just parsed at least one number after a decimal point
        switch (get())
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                add(current);
                goto scan_number_decimal2;
            }

            case 'e':
            case 'E':
            {
                add(current);
                goto scan_number_exponent;
            }

            default:
                goto scan_number_done;
        }

scan_number_exponent:
        // we just parsed an exponent
        number_type = token_type::value_float;
        switch (get())
        {
            case '+':
            case '-':
            {
                add(current);
                goto scan_number_sign;
            }

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                add(current);
                goto scan_number_any2;
            }

            default:
            {
                error_message =
                    "invalid number; expected '+', '-', or digit after exponent";
                return token_type::parse_error;
            }
        }

scan_number_sign:
        // we just parsed an exponent sign
        switch (get())
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                add(current);
                goto scan_number_any2;
            }

            default:
            {
                error_message = "invalid number; expected digit after exponent sign";
                return token_type::parse_error;
            }
        }

scan_number_any2:
        // we just parsed a number after the exponent or exponent sign
        switch (get())
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                add(current);
                goto scan_number_any2;
            }

            default:
                goto scan_number_done;
        }

scan_number_done:
        // unget the character after the number (we only read it to know that
        // we are done scanning a number)
        unget();

        char* endptr = nullptr;
        errno = 0;

        // try to parse integers first and fall back to floats
        if (number_type == token_type::value_unsigned)
        {
            const auto x = std::strtoull(token_buffer.data(), &endptr, 10);

            // we checked the number format before
            assert(endptr == token_buffer.data() + token_buffer.size());

            if (errno == 0)
            {
                value_unsigned = static_cast<Num_unsigned>(x);
                if (value_unsigned == x)
                {
                    return token_type::value_unsigned;
                }
            }
        }
        else if (number_type == token_type::value_integer)
        {
            const auto x = std::strtoll(token_buffer.data(), &endptr, 10);

            // we checked the number format before
            assert(endptr == token_buffer.data() + token_buffer.size());

            if (errno == 0)
            {
                value_integer = static_cast<Num_integer>(x);
                if (value_integer == x)
                {
                    return token_type::value_integer;
                }
            }
        }

        // this code is reached if we parse a floating-point number or if an
        // integer conversion above failed
        strtof(value_float, token_buffer.data(), &endptr);

        // we checked the number format before
        assert(endptr == token_buffer.data() + token_buffer.size());

        return token_type::value_float;
    }

    /*!
    @param[in] literal_text  the literal text to expect
    @param[in] length        the length of the passed literal text
    @param[in] return_type   the token type to return on success
    */
    token_type scan_literal(const char* literal_text, const std::size_t length,
                            token_type return_type)
    {
        assert(current == literal_text[0]);
        for (std::size_t i = 1; i < length; ++i)
        {
            if ((get() != literal_text[i]))
            {
                error_message = "invalid literal";
                return token_type::parse_error;
            }
        }
        return return_type;
    }

    /////////////////////
    // input management
    /////////////////////

    /// reset token_buffer; current character is beginning of token
    void reset() 
    {
        token_buffer.clear();
        token_string.clear();
        token_string.push_back(std::char_traits<char>::to_char_type(current));
    }

    /*
    @brief get next character from the input

    This function provides the interface to the used input adapter. It does
    not throw in case the input reached EOF, but returns a
    `std::char_traits<char>::eof()` in that case.  Stores the scanned characters
    for use in error messages.

    @return character read from the input
    */
    std::char_traits<char>::int_type get()
    {
        ++position.chars_read_total;
        ++position.chars_read_current_line;

        if (next_unget)
        {
            // just reset the next_unget variable and work with current
            next_unget = false;
        }
        else
        {
            current = ia->get_character();
        }

        if ((current != std::char_traits<char>::eof()))
        {
            token_string.push_back(std::char_traits<char>::to_char_type(current));
        }

        if (current == '\n')
        {
            ++position.lines_read;
            ++position.chars_read_current_line = 0;
        }

        return current;
    }

    /*!
    @brief unget current character (read it again on next get)

    We implement unget by setting variable next_unget to true. The input is not
    changed - we just simulate ungetting by modifying chars_read_total,
    chars_read_current_line, and token_string. The next call to get() will
    behave as if the unget character is read again.
    */
    void unget()
    {
        next_unget = true;

        --position.chars_read_total;

        // in case we "unget" a newline, we have to also decrement the lines_read
        if (position.chars_read_current_line == 0)
        {
            if (position.lines_read > 0)
            {
                --position.lines_read;
            }
        }
        else
        {
            --position.chars_read_current_line;
        }

        if ((current != std::char_traits<char>::eof()))
        {
            assert(token_string.size() != 0);
            token_string.pop_back();
        }
    }

    /// add a character to token_buffer
    void add(int c)
    {
        token_buffer.push_back(std::char_traits<char>::to_char_type(c));
    }

  public:
    /////////////////////
    // value getters
    /////////////////////

    /// return integer value
    constexpr Num_integer get_number_integer() const 
    {
        return value_integer;
    }

    /// return unsigned integer value
    constexpr Num_unsigned get_number_unsigned() const 
    {
        return value_unsigned;
    }

    /// return floating-point value
    constexpr Num_float get_number_float() const 
    {
        return value_float;
    }

    /// return current string value (implicitly resets the token; useful only once)
    String& get_string()
    {
        return token_buffer;
    }

    /////////////////////
    // diagnostics
    /////////////////////

    /// return position of last read token
    constexpr position_t get_position() const 
    {
        return position;
    }

    /// return the last read token (for errors only).  Will never contain EOF
    /// (an arbitrary value that is not a valid char value, often -1), because
    /// 255 may legitimately occur.  May contain NUL, which should be escaped.
    std::string get_token_string() const
    {
        // escape control characters
        std::string result;
        for (const auto c : token_string)
        {
            if ('\x00' <= c and c <= '\x1F')
            {
                // escape control characters
                char cs[9];
                (std::snprintf)(cs, 9, "<U+%.4X>", static_cast<unsigned char>(c));
                result += cs;
            }
            else
            {
                // add character as is
                result.push_back(c);
            }
        }

        return result;
    }

    /// return syntax error message
    constexpr const char* get_error_message() const 
    {
        return error_message;
    }

    /////////////////////
    // actual scanner
    /////////////////////

    /*!
    @brief skip the UTF-8 byte order mark
    @return true iff there is no BOM or the correct BOM has been skipped
    */
    bool skip_bom()
    {
        if (get() == 0xEF)
        {
            // check if we completely parse the BOM
            return get() == 0xBB and get() == 0xBF;
        }

        // the first character is not the beginning of the BOM; unget it to
        // process is later
        unget();
        return true;
    }

    token_type scan()
    {
        // initially, skip the BOM
        if (position.chars_read_total == 0 and not skip_bom())
        {
            error_message = "invalid BOM; must be 0xEF 0xBB 0xBF if given";
            return token_type::parse_error;
        }

        // read next character and ignore whitespace
        do
        {
            get();
        }
        while (current == ' ' or current == '\t' or current == '\n' or current == '\r');

        switch (current)
        {
            // structural characters
            case '[':
                return token_type::begin_array;
            case ']':
                return token_type::end_array;
            case '{':
                return token_type::begin_object;
            case '}':
                return token_type::end_object;
            case ':':
                return token_type::name_separator;
            case ',':
                return token_type::value_separator;

            // literals
            case 't':
                return scan_literal("true", 4, token_type::literal_true);
            case 'f':
                return scan_literal("false", 5, token_type::literal_false);
            case 'n':
                return scan_literal("null", 4, token_type::literal_null);

            // string
            case '\"':
                return scan_string();

            // number
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                return scan_number();

            // end of input (the null byte is needed when parsing from
            // string literals)
            case '\0':
            case std::char_traits<char>::eof():
                return token_type::end_of_input;

            // error
            default:
                error_message = "invalid literal";
                return token_type::parse_error;
        }
    }

  private:
    /// input adapter
    detail::input_adapter_t ia = nullptr;

    /// the current character
    std::char_traits<char>::int_type current = std::char_traits<char>::eof();

    /// whether the next get() call should just return current
    bool next_unget = false;

    /// the start position of the current token
    position_t position;

    /// raw input token string (for error messages)
    std::vector<char> token_string {};

    /// buffer for variable-length tokens (numbers, strings)
    String token_buffer {};

    /// a description of occurred Lexer errors
    const char* error_message = "";

    // number values
    Num_integer value_integer = 0;
    Num_unsigned value_unsigned = 0;
    Num_float value_float = 0;

    /// the decimal point
    const char decimal_point_char = '.';
};
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/input/Parser.hpp>


#include <cassert> // assert
#include <cmath> // isfinite
#include <cstdint> // uint8_t
#include <functional> // function
#include <string> // string
#include <utility> // move


#include <cstdint> // size_t
#include <utility> // declval


namespace nlohmann
{
namespace detail
{
template <typename T>
using null_function_t = decltype(std::declval<T&>().null());

template <typename T>
using boolean_function_t =
    decltype(std::declval<T&>().boolean(std::declval<bool>()));

template <typename T, typename Integer>
using number_integer_function_t =
    decltype(std::declval<T&>().num_integer(std::declval<Integer>()));

template <typename T, typename Unsigned>
using number_unsigned_function_t =
    decltype(std::declval<T&>().num_unsigned(std::declval<Unsigned>()));

template <typename T, typename Float, typename String>
using number_float_function_t = decltype(std::declval<T&>().num_float(
                                    std::declval<Float>(), std::declval<const String&>()));

template <typename T, typename String>
using string_function_t =
    decltype(std::declval<T&>().string(std::declval<String&>()));

template <typename T>
using start_object_function_t =
    decltype(std::declval<T&>().start_object(std::declval<std::size_t>()));

template <typename T, typename String>
using key_function_t =
    decltype(std::declval<T&>().key(std::declval<String&>()));

template <typename T>
using end_object_function_t = decltype(std::declval<T&>().end_object());

template <typename T>
using start_array_function_t =
    decltype(std::declval<T&>().start_array(std::declval<std::size_t>()));

template <typename T>
using end_array_function_t = decltype(std::declval<T&>().end_array());

template <typename T, typename Exception>
using parse_error_function_t = decltype(std::declval<T&>().parse_error(
        std::declval<std::size_t>(), std::declval<const std::string&>(),
        std::declval<const Exception&>()));

template <typename SAX, typename Basic_json>
struct is_sax
{
  private:
    static_assert(Is_basic_json<Basic_json>::value,
                  "Basic_json must be of type basic_json<...>");

    using Num_integer = typename Basic_json::Num_integer;
    using Num_unsigned = typename Basic_json::Num_unsigned;
    using Num_float = typename Basic_json::Num_float;
    using String = typename Basic_json::String;
    using exception_t = typename Basic_json::exception;

  public:
    static constexpr bool value =
        is_detected_exact<bool, null_function_t, SAX>::value &&
        is_detected_exact<bool, boolean_function_t, SAX>::value &&
        is_detected_exact<bool, number_integer_function_t, SAX,
        Num_integer>::value &&
        is_detected_exact<bool, number_unsigned_function_t, SAX,
        Num_unsigned>::value &&
        is_detected_exact<bool, number_float_function_t, SAX, Num_float,
        String>::value &&
        is_detected_exact<bool, string_function_t, SAX, String>::value &&
        is_detected_exact<bool, start_object_function_t, SAX>::value &&
        is_detected_exact<bool, key_function_t, SAX, String>::value &&
        is_detected_exact<bool, end_object_function_t, SAX>::value &&
        is_detected_exact<bool, start_array_function_t, SAX>::value &&
        is_detected_exact<bool, end_array_function_t, SAX>::value &&
        is_detected_exact<bool, parse_error_function_t, SAX, exception_t>::value;
};

template <typename SAX, typename Basic_json>
struct is_sax_static_asserts
{
  private:
    static_assert(Is_basic_json<Basic_json>::value,
                  "Basic_json must be of type basic_json<...>");

    using Num_integer = typename Basic_json::Num_integer;
    using Num_unsigned = typename Basic_json::Num_unsigned;
    using Num_float = typename Basic_json::Num_float;
    using String = typename Basic_json::String;
    using exception_t = typename Basic_json::exception;

  public:
    static_assert(is_detected_exact<bool, null_function_t, SAX>::value,
                  "Missing/invalid function: bool null()");
    static_assert(is_detected_exact<bool, boolean_function_t, SAX>::value,
                  "Missing/invalid function: bool boolean(bool)");
    static_assert(is_detected_exact<bool, boolean_function_t, SAX>::value,
                  "Missing/invalid function: bool boolean(bool)");
    static_assert(
        is_detected_exact<bool, number_integer_function_t, SAX,
        Num_integer>::value,
        "Missing/invalid function: bool num_integer(Num_integer)");
    static_assert(
        is_detected_exact<bool, number_unsigned_function_t, SAX,
        Num_unsigned>::value,
        "Missing/invalid function: bool num_unsigned(Num_unsigned)");
    static_assert(is_detected_exact<bool, number_float_function_t, SAX,
                  Num_float, String>::value,
                  "Missing/invalid function: bool num_float(Num_float, const String&)");
    static_assert(
        is_detected_exact<bool, string_function_t, SAX, String>::value,
        "Missing/invalid function: bool string(String&)");
    static_assert(is_detected_exact<bool, start_object_function_t, SAX>::value,
                  "Missing/invalid function: bool start_object(std::size_t)");
    static_assert(is_detected_exact<bool, key_function_t, SAX, String>::value,
                  "Missing/invalid function: bool key(String&)");
    static_assert(is_detected_exact<bool, end_object_function_t, SAX>::value,
                  "Missing/invalid function: bool end_object()");
    static_assert(is_detected_exact<bool, start_array_function_t, SAX>::value,
                  "Missing/invalid function: bool start_array(std::size_t)");
    static_assert(is_detected_exact<bool, end_array_function_t, SAX>::value,
                  "Missing/invalid function: bool end_array()");
    static_assert(
        is_detected_exact<bool, parse_error_function_t, SAX, exception_t>::value,
        "Missing/invalid function: bool parse_error(std::size_t, const "
        "std::string&, const exception&)");
};
}  // namespace detail
}  // namespace nlohmann


#include <cstddef>
#include <string>
#include <vector>


namespace nlohmann
{

/*!
@brief SAX interface

This class describes the SAX interface used by @ref nlohmann::json::sax_parse.
Each function is called in different situations while the input is parsed. The
boolean return value informs the Parser whether to continue processing the
input.
*/
template<typename Basic_json>
struct json_sax
{
    /// type for (signed) integers
    using Num_integer = typename Basic_json::Num_integer;
    /// type for unsigned integers
    using Num_unsigned = typename Basic_json::Num_unsigned;
    /// type for floating-point numbers
    using Num_float = typename Basic_json::Num_float;
    /// type for strings
    using String = typename Basic_json::String;

    /*!
    @brief a null value was read
    @return whether parsing should proceed
    */
    virtual bool null() = 0;

    /*!
    @brief a boolean value was read
    @param[in] val  boolean value
    @return whether parsing should proceed
    */
    virtual bool boolean(bool val) = 0;

    /*!
    @brief an integer number was read
    @param[in] val  integer value
    @return whether parsing should proceed
    */
    virtual bool num_integer(Num_integer val) = 0;

    /*!
    @brief an unsigned integer number was read
    @param[in] val  unsigned integer value
    @return whether parsing should proceed
    */
    virtual bool num_unsigned(Num_unsigned val) = 0;

    /*!
    @brief an floating-point number was read
    @param[in] val  floating-point value
    @param[in] s    raw token value
    @return whether parsing should proceed
    */
    virtual bool num_float(Num_float val, const String& s) = 0;

    /*!
    @brief a string was read
    @param[in] val  string value
    @return whether parsing should proceed
    @note It is safe to move the passed string.
    */
    virtual bool string(String& val) = 0;

    /*!
    @brief the beginning of an object was read
    @param[in] elements  number of object elements or -1 if unknown
    @return whether parsing should proceed
    @note binary formats may report the number of elements
    */
    virtual bool start_object(std::size_t elements) = 0;

    /*!
    @brief an object key was read
    @param[in] val  object key
    @return whether parsing should proceed
    @note It is safe to move the passed string.
    */
    virtual bool key(String& val) = 0;

    /*!
    @brief the end of an object was read
    @return whether parsing should proceed
    */
    virtual bool end_object() = 0;

    /*!
    @brief the beginning of an array was read
    @param[in] elements  number of array elements or -1 if unknown
    @return whether parsing should proceed
    @note binary formats may report the number of elements
    */
    virtual bool start_array(std::size_t elements) = 0;

    /*!
    @brief the end of an array was read
    @return whether parsing should proceed
    */
    virtual bool end_array() = 0;

    /*!
    @brief a parse error occurred
    @param[in] position    the position in the input where the error occurs
    @param[in] last_token  the last read token
    @param[in] ex          an exception object describing the error
    @return whether parsing should proceed (must return false)
    */
    virtual bool parse_error(std::size_t position,
                             const std::string& last_token,
                             const detail::exception& ex) = 0;

    virtual ~json_sax() = default;
};


namespace detail
{

template<typename Basic_json>
class json_sax_dom_parser
{
  public:
    using Num_integer = typename Basic_json::Num_integer;
    using Num_unsigned = typename Basic_json::Num_unsigned;
    using Num_float = typename Basic_json::Num_float;
    using String = typename Basic_json::String;

    /*!
    @param[in, out] r  reference to a JSON value that is manipulated while
                       parsing
    @param[in] allow_exceptions_  whether parse errors yield exceptions
    */
    explicit json_sax_dom_parser(Basic_json& r, const bool allow_exceptions_ = true)
        : root(r), allow_exceptions(allow_exceptions_)
    {}

    bool null()
    {
        handle_value(nullptr);
        return true;
    }

    bool boolean(bool val)
    {
        handle_value(val);
        return true;
    }

    bool num_integer(Num_integer val)
    {
        handle_value(val);
        return true;
    }

    bool num_unsigned(Num_unsigned val)
    {
        handle_value(val);
        return true;
    }

    bool num_float(Num_float val, const String& /*unused*/)
    {
        handle_value(val);
        return true;
    }

    bool string(String& val)
    {
        handle_value(val);
        return true;
    }

    bool start_object(std::size_t len)
    {
        ref_stack.push_back(handle_value(Basic_json::Value::object));

        if ((len != std::size_t(-1) and len > ref_stack.back()->max_size()))
        {
            JSON_THROW(out_of_range::create(408,
                                            "excessive object size: " + std::to_string(len)));
        }

        return true;
    }

    bool key(String& val)
    {
        // add null at given key and store the reference for later
        object_element = &(ref_stack.back()->value_.object->operator[](val));
        return true;
    }

    bool end_object()
    {
        ref_stack.pop_back();
        return true;
    }

    bool start_array(std::size_t len)
    {
        ref_stack.push_back(handle_value(Basic_json::Value::array));

        if ((len != std::size_t(-1) and len > ref_stack.back()->max_size()))
        {
            JSON_THROW(out_of_range::create(408,
                                            "excessive array size: " + std::to_string(len)));
        }

        return true;
    }

    bool end_array()
    {
        ref_stack.pop_back();
        return true;
    }

    bool parse_error(std::size_t /*unused*/, const std::string& /*unused*/,
                     const detail::exception& ex)
    {
        errored = true;
        if (allow_exceptions)
        {
            // determine the proper exception type from the id
            switch ((ex.id / 100) % 100)
            {
                case 1:
                    JSON_THROW(*reinterpret_cast<const detail::parse_error*>(&ex));
                case 4:
                    JSON_THROW(*reinterpret_cast<const detail::out_of_range*>(&ex));
                // LCOV_EXCL_START
                case 2:
                    JSON_THROW(*reinterpret_cast<const detail::invalid_iterator*>(&ex));
                case 3:
                    JSON_THROW(*reinterpret_cast<const detail::type_error*>(&ex));
                case 5:
                    JSON_THROW(*reinterpret_cast<const detail::other_error*>(&ex));
                default:
                    assert(false);
                    // LCOV_EXCL_STOP
            }
        }
        return false;
    }

    constexpr bool is_errored() const
    {
        return errored;
    }

  private:
    /*!
    @invariant If the ref stack is empty, then the passed value will be the new
               root.
    @invariant If the ref stack contains a value, then it is an array or an
               object to which we can add elements
    */
    template<typename Value>
    Basic_json* handle_value(Value&& v)
    {
        if (ref_stack.empty())
        {
            root = Basic_json(std::forward<Value>(v));
            return &root;
        }

        assert(ref_stack.back()->is_array() or ref_stack.back()->is_object());

        if (ref_stack.back()->is_array())
        {
            ref_stack.back()->value_.array->emplace_back(std::forward<Value>(v));
            return &(ref_stack.back()->value_.array->back());
        }
        else
        {
            assert(object_element);
            *object_element = Basic_json(std::forward<Value>(v));
            return object_element;
        }
    }

    /// the parsed JSON value
    Basic_json& root;
    /// stack to model hierarchy of values
    std::vector<Basic_json*> ref_stack;
    /// helper to hold the reference for the next object element
    Basic_json* object_element = nullptr;
    /// whether a syntax error occurred
    bool errored = false;
    /// whether to throw exceptions in case of errors
    const bool allow_exceptions = true;
};

template<typename Basic_json>
class json_sax_dom_callback_parser
{
  public:
    using Num_integer = typename Basic_json::Num_integer;
    using Num_unsigned = typename Basic_json::Num_unsigned;
    using Num_float = typename Basic_json::Num_float;
    using String = typename Basic_json::String;
    using parser_callback_t = typename Basic_json::parser_callback_t;
    using Parse_event = typename Basic_json::Parse_event;

    json_sax_dom_callback_parser(Basic_json& r,
                                 const parser_callback_t cb,
                                 const bool allow_exceptions_ = true)
        : root(r), callback(cb), allow_exceptions(allow_exceptions_)
    {
        keep_stack.push_back(true);
    }

    bool null()
    {
        handle_value(nullptr);
        return true;
    }

    bool boolean(bool val)
    {
        handle_value(val);
        return true;
    }

    bool num_integer(Num_integer val)
    {
        handle_value(val);
        return true;
    }

    bool num_unsigned(Num_unsigned val)
    {
        handle_value(val);
        return true;
    }

    bool num_float(Num_float val, const String& /*unused*/)
    {
        handle_value(val);
        return true;
    }

    bool string(String& val)
    {
        handle_value(val);
        return true;
    }

    bool start_object(std::size_t len)
    {
        // check callback for object start
        const bool keep = callback(static_cast<int>(ref_stack.size()), Parse_event::object_start, discarded);
        keep_stack.push_back(keep);

        auto val = handle_value(Basic_json::Value::object, true);
        ref_stack.push_back(val.second);

        // check object limit
        if (ref_stack.back())
        {
            if ((len != std::size_t(-1) and len > ref_stack.back()->max_size()))
            {
                JSON_THROW(out_of_range::create(408,
                                                "excessive object size: " + std::to_string(len)));
            }
        }

        return true;
    }

    bool key(String& val)
    {
        Basic_json k = Basic_json(val);

        // check callback for key
        const bool keep = callback(static_cast<int>(ref_stack.size()), Parse_event::key, k);
        key_keep_stack.push_back(keep);

        // add discarded value at given key and store the reference for later
        if (keep and ref_stack.back())
        {
            object_element = &(ref_stack.back()->value_.object->operator[](val) = discarded);
        }

        return true;
    }

    bool end_object()
    {
        if (ref_stack.back())
        {
            if (not callback(static_cast<int>(ref_stack.size()) - 1, Parse_event::object_end, *ref_stack.back()))
            {
                // discard object
                *ref_stack.back() = discarded;
            }
        }

        assert(not ref_stack.empty());
        assert(not keep_stack.empty());
        ref_stack.pop_back();
        keep_stack.pop_back();

        if (not ref_stack.empty() and ref_stack.back())
        {
            // remove discarded value
            if (ref_stack.back()->is_object())
            {
                for (auto it = ref_stack.back()->begin(); it != ref_stack.back()->end(); ++it)
                {
                    if (it->is_discarded())
                    {
                        ref_stack.back()->erase(it);
                        break;
                    }
                }
            }
        }

        return true;
    }

    bool start_array(std::size_t len)
    {
        const bool keep = callback(static_cast<int>(ref_stack.size()), Parse_event::array_start, discarded);
        keep_stack.push_back(keep);

        auto val = handle_value(Basic_json::Value::array, true);
        ref_stack.push_back(val.second);

        // check array limit
        if (ref_stack.back())
        {
            if ((len != std::size_t(-1) and len > ref_stack.back()->max_size()))
            {
                JSON_THROW(out_of_range::create(408,
                                                "excessive array size: " + std::to_string(len)));
            }
        }

        return true;
    }

    bool end_array()
    {
        bool keep = true;

        if (ref_stack.back())
        {
            keep = callback(static_cast<int>(ref_stack.size()) - 1, Parse_event::array_end, *ref_stack.back());
            if (not keep)
            {
                // discard array
                *ref_stack.back() = discarded;
            }
        }

        assert(not ref_stack.empty());
        assert(not keep_stack.empty());
        ref_stack.pop_back();
        keep_stack.pop_back();

        // remove discarded value
        if (not keep and not ref_stack.empty())
        {
            if (ref_stack.back()->is_array())
            {
                ref_stack.back()->value_.array->pop_back();
            }
        }

        return true;
    }

    bool parse_error(std::size_t /*unused*/, const std::string& /*unused*/,
                     const detail::exception& ex)
    {
        errored = true;
        if (allow_exceptions)
        {
            // determine the proper exception type from the id
            switch ((ex.id / 100) % 100)
            {
                case 1:
                    JSON_THROW(*reinterpret_cast<const detail::parse_error*>(&ex));
                case 4:
                    JSON_THROW(*reinterpret_cast<const detail::out_of_range*>(&ex));
                // LCOV_EXCL_START
                case 2:
                    JSON_THROW(*reinterpret_cast<const detail::invalid_iterator*>(&ex));
                case 3:
                    JSON_THROW(*reinterpret_cast<const detail::type_error*>(&ex));
                case 5:
                    JSON_THROW(*reinterpret_cast<const detail::other_error*>(&ex));
                default:
                    assert(false);
                    // LCOV_EXCL_STOP
            }
        }
        return false;
    }

    constexpr bool is_errored() const
    {
        return errored;
    }

  private:

    template<typename Value>
    std::pair<bool, Basic_json*> handle_value(Value&& v, const bool skip_callback = false)
    {
        assert(not keep_stack.empty());

        // do not handle this value if we know it would be added to a discarded
        // container
        if (not keep_stack.back())
        {
            return {false, nullptr};
        }

        // create value
        auto value = Basic_json(std::forward<Value>(v));

        // check callback
        const bool keep = skip_callback or callback(static_cast<int>(ref_stack.size()), Parse_event::value, value);

        // do not handle this value if we just learnt it shall be discarded
        if (not keep)
        {
            return {false, nullptr};
        }

        if (ref_stack.empty())
        {
            root = std::move(value);
            return {true, &root};
        }

        // skip this value if we already decided to skip the parent
        // (https://github.com/nlohmann/json/issues/971#issuecomment-413678360)
        if (not ref_stack.back())
        {
            return {false, nullptr};
        }

        // we now only expect arrays and objects
        assert(ref_stack.back()->is_array() or ref_stack.back()->is_object());

        if (ref_stack.back()->is_array())
        {
            ref_stack.back()->value_.array->push_back(std::move(value));
            return {true, &(ref_stack.back()->value_.array->back())};
        }
        else
        {
            // check if we should store an element for the current key
            assert(not key_keep_stack.empty());
            const bool store_element = key_keep_stack.back();
            key_keep_stack.pop_back();

            if (not store_element)
            {
                return {false, nullptr};
            }

            assert(object_element);
            *object_element = std::move(value);
            return {true, object_element};
        }
    }

    /// the parsed JSON value
    Basic_json& root;
    /// stack to model hierarchy of values
    std::vector<Basic_json*> ref_stack;
    /// stack to manage which values to keep
    std::vector<bool> keep_stack;
    /// stack to manage which object keys to keep
    std::vector<bool> key_keep_stack;
    /// helper to hold the reference for the next object element
    Basic_json* object_element = nullptr;
    /// whether a syntax error occurred
    bool errored = false;
    /// callback function
    const parser_callback_t callback = nullptr;
    /// whether to throw exceptions in case of errors
    const bool allow_exceptions = true;
    /// a discarded value for the callback
    Basic_json discarded = Basic_json::Value::discarded;
};

template<typename Basic_json>
class json_sax_acceptor
{
  public:
    using Num_integer = typename Basic_json::Num_integer;
    using Num_unsigned = typename Basic_json::Num_unsigned;
    using Num_float = typename Basic_json::Num_float;
    using String = typename Basic_json::String;

    bool null()
    {
        return true;
    }

    bool boolean(bool /*unused*/)
    {
        return true;
    }

    bool num_integer(Num_integer /*unused*/)
    {
        return true;
    }

    bool num_unsigned(Num_unsigned /*unused*/)
    {
        return true;
    }

    bool num_float(Num_float /*unused*/, const String& /*unused*/)
    {
        return true;
    }

    bool string(String& /*unused*/)
    {
        return true;
    }

    bool start_object(std::size_t  /*unused*/ = std::size_t(-1))
    {
        return true;
    }

    bool key(String& /*unused*/)
    {
        return true;
    }

    bool end_object()
    {
        return true;
    }

    bool start_array(std::size_t  /*unused*/ = std::size_t(-1))
    {
        return true;
    }

    bool end_array()
    {
        return true;
    }

    bool parse_error(std::size_t /*unused*/, const std::string& /*unused*/, const detail::exception& /*unused*/)
    {
        return false;
    }
};
}  // namespace detail

}  // namespace nlohmann


namespace nlohmann
{
namespace detail
{
////////////
// Parser //
////////////

/*!
@brief syntax analysis

This class implements a recursive decent Parser.
*/
template<typename Basic_json>
class Parser
{
    using Num_integer = typename Basic_json::Num_integer;
    using Num_unsigned = typename Basic_json::Num_unsigned;
    using Num_float = typename Basic_json::Num_float;
    using String = typename Basic_json::String;
    using lexer_t = Lexer<Basic_json>;
    using token_type = typename lexer_t::token_type;

  public:
    enum class Parse_event : uint8_t
    {
        object_start,
        object_end,

        array_start,
        array_end,

        key,
        value
    };

    using parser_callback_t =
        std::function<bool(int depth, Parse_event event, Basic_json& parsed)>;

    /// a Parser reading from an input adapter
    explicit Parser(detail::input_adapter_t&& adapter,
                    const parser_callback_t cb = nullptr,
                    const bool allow_exceptions_ = true)
        : callback(cb), m_lexer(std::move(adapter)), allow_exceptions(allow_exceptions_)
    {
        // read first token
        get_token();
    }


    void parse(const bool strict, Basic_json& result)
    {
        if (callback)
        {
            json_sax_dom_callback_parser<Basic_json> sdp(result, callback, allow_exceptions);
            sax_parse_internal(&sdp);
            result.assert_invariant();

            // in strict mode, input must be completely read
            if (strict and (get_token() != token_type::end_of_input))
            {
                sdp.parse_error(m_lexer.get_position(),
                                m_lexer.get_token_string(),
                                parse_error::create(101, m_lexer.get_position(),
                                                    exception_message(token_type::end_of_input, "value")));
            }

            // in case of an error, return discarded value
            if (sdp.is_errored())
            {
                result = Value::discarded;
                return;
            }

            // set top-level value to null if it was discarded by the callback
            // function
            if (result.is_discarded())
            {
                result = nullptr;
            }
        }
        else
        {
            json_sax_dom_parser<Basic_json> sdp(result, allow_exceptions);
            sax_parse_internal(&sdp);
            result.assert_invariant();

            // in strict mode, input must be completely read
            if (strict and (get_token() != token_type::end_of_input))
            {
                sdp.parse_error(m_lexer.get_position(),
                                m_lexer.get_token_string(),
                                parse_error::create(101, m_lexer.get_position(),
                                                    exception_message(token_type::end_of_input, "value")));
            }

            // in case of an error, return discarded value
            if (sdp.is_errored())
            {
                result = Value::discarded;
                return;
            }
        }
    }

    /*!
    @brief public accept interface

    @param[in] strict  whether to expect the last token to be EOF
    @return whether the input is a proper JSON text
    */
    bool accept(const bool strict = true)
    {
        json_sax_acceptor<Basic_json> sax_acceptor;
        return sax_parse(&sax_acceptor, strict);
    }

    template <typename SAX>
    bool sax_parse(SAX* sax, const bool strict = true)
    {
        (void)detail::is_sax_static_asserts<SAX, Basic_json> {};
        const bool result = sax_parse_internal(sax);

        // strict mode: next byte must be EOF
        if (result and strict and (get_token() != token_type::end_of_input))
        {
            return sax->parse_error(m_lexer.get_position(),
                                    m_lexer.get_token_string(),
                                    parse_error::create(101, m_lexer.get_position(),
                                            exception_message(token_type::end_of_input, "value")));
        }

        return result;
    }

  private:
    template <typename SAX>
    bool sax_parse_internal(SAX* sax)
    {
        // stack to remember the hierarchy of structured values we are parsing
        // true = array; false = object
        std::vector<bool> states;
        // value to avoid a goto (see comment where set to true)
        bool skip_to_state_evaluation = false;

        while (true)
        {
            if (not skip_to_state_evaluation)
            {
                // invariant: get_token() was called before each iteration
                switch (last_token)
                {
                    case token_type::begin_object:
                    {
                        if ((not sax->start_object(std::size_t(-1))))
                        {
                            return false;
                        }

                        // closing } -> we are done
                        if (get_token() == token_type::end_object)
                        {
                            if ((not sax->end_object()))
                            {
                                return false;
                            }
                            break;
                        }

                        // parse key
                        if ((last_token != token_type::value_string))
                        {
                            return sax->parse_error(m_lexer.get_position(),
                                                    m_lexer.get_token_string(),
                                                    parse_error::create(101, m_lexer.get_position(),
                                                            exception_message(token_type::value_string, "object key")));
                        }
                        if ((not sax->key(m_lexer.get_string())))
                        {
                            return false;
                        }

                        // parse separator (:)
                        if ((get_token() != token_type::name_separator))
                        {
                            return sax->parse_error(m_lexer.get_position(),
                                                    m_lexer.get_token_string(),
                                                    parse_error::create(101, m_lexer.get_position(),
                                                            exception_message(token_type::name_separator, "object separator")));
                        }

                        // remember we are now inside an object
                        states.push_back(false);

                        // parse values
                        get_token();
                        continue;
                    }

                    case token_type::begin_array:
                    {
                        if ((not sax->start_array(std::size_t(-1))))
                        {
                            return false;
                        }

                        // closing ] -> we are done
                        if (get_token() == token_type::end_array)
                        {
                            if ((not sax->end_array()))
                            {
                                return false;
                            }
                            break;
                        }

                        // remember we are now inside an array
                        states.push_back(true);

                        // parse values (no need to call get_token)
                        continue;
                    }

                    case token_type::value_float:
                    {
                        const auto res = m_lexer.get_number_float();

                        if ((not std::isfinite(res)))
                        {
                            return sax->parse_error(m_lexer.get_position(),
                                                    m_lexer.get_token_string(),
                                                    out_of_range::create(406, "number overflow parsing '" + m_lexer.get_token_string() + "'"));
                        }
                        else
                        {
                            if ((not sax->num_float(res, m_lexer.get_string())))
                            {
                                return false;
                            }
                            break;
                        }
                    }

                    case token_type::literal_false:
                    {
                        if ((not sax->boolean(false)))
                        {
                            return false;
                        }
                        break;
                    }

                    case token_type::literal_null:
                    {
                        if ((not sax->null()))
                        {
                            return false;
                        }
                        break;
                    }

                    case token_type::literal_true:
                    {
                        if ((not sax->boolean(true)))
                        {
                            return false;
                        }
                        break;
                    }

                    case token_type::value_integer:
                    {
                        if ((not sax->num_integer(m_lexer.get_number_integer())))
                        {
                            return false;
                        }
                        break;
                    }

                    case token_type::value_string:
                    {
                        if ((not sax->string(m_lexer.get_string())))
                        {
                            return false;
                        }
                        break;
                    }

                    case token_type::value_unsigned:
                    {
                        if ((not sax->num_unsigned(m_lexer.get_number_unsigned())))
                        {
                            return false;
                        }
                        break;
                    }

                    case token_type::parse_error:
                    {
                        // using "uninitialized" to avoid "expected" message
                        return sax->parse_error(m_lexer.get_position(),
                                                m_lexer.get_token_string(),
                                                parse_error::create(101, m_lexer.get_position(),
                                                        exception_message(token_type::uninitialized, "value")));
                    }

                    default: // the last token was unexpected
                    {
                        return sax->parse_error(m_lexer.get_position(),
                                                m_lexer.get_token_string(),
                                                parse_error::create(101, m_lexer.get_position(),
                                                        exception_message(token_type::literal_or_value, "value")));
                    }
                }
            }
            else
            {
                skip_to_state_evaluation = false;
            }

            // we reached this line after we successfully parsed a value
            if (states.empty())
            {
                // empty stack: we reached the end of the hierarchy: done
                return true;
            }
            else
            {
                if (states.back())  // array
                {
                    // comma -> next value
                    if (get_token() == token_type::value_separator)
                    {
                        // parse a new value
                        get_token();
                        continue;
                    }

                    // closing ]
                    if ((last_token == token_type::end_array))
                    {
                        if ((not sax->end_array()))
                        {
                            return false;
                        }

                        // We are done with this array. Before we can parse a
                        // new value, we need to evaluate the new state first.
                        // By setting skip_to_state_evaluation to false, we
                        // are effectively jumping to the beginning of this if.
                        assert(not states.empty());
                        states.pop_back();
                        skip_to_state_evaluation = true;
                        continue;
                    }
                    else
                    {
                        return sax->parse_error(m_lexer.get_position(),
                                                m_lexer.get_token_string(),
                                                parse_error::create(101, m_lexer.get_position(),
                                                        exception_message(token_type::end_array, "array")));
                    }
                }
                else  // object
                {
                    // comma -> next value
                    if (get_token() == token_type::value_separator)
                    {
                        // parse key
                        if ((get_token() != token_type::value_string))
                        {
                            return sax->parse_error(m_lexer.get_position(),
                                                    m_lexer.get_token_string(),
                                                    parse_error::create(101, m_lexer.get_position(),
                                                            exception_message(token_type::value_string, "object key")));
                        }
                        else
                        {
                            if ((not sax->key(m_lexer.get_string())))
                            {
                                return false;
                            }
                        }

                        // parse separator (:)
                        if ((get_token() != token_type::name_separator))
                        {
                            return sax->parse_error(m_lexer.get_position(),
                                                    m_lexer.get_token_string(),
                                                    parse_error::create(101, m_lexer.get_position(),
                                                            exception_message(token_type::name_separator, "object separator")));
                        }

                        // parse values
                        get_token();
                        continue;
                    }

                    // closing }
                    if ((last_token == token_type::end_object))
                    {
                        if ((not sax->end_object()))
                        {
                            return false;
                        }

                        // We are done with this object. Before we can parse a
                        // new value, we need to evaluate the new state first.
                        // By setting skip_to_state_evaluation to false, we
                        // are effectively jumping to the beginning of this if.
                        assert(not states.empty());
                        states.pop_back();
                        skip_to_state_evaluation = true;
                        continue;
                    }
                    else
                    {
                        return sax->parse_error(m_lexer.get_position(),
                                                m_lexer.get_token_string(),
                                                parse_error::create(101, m_lexer.get_position(),
                                                        exception_message(token_type::end_object, "object")));
                    }
                }
            }
        }
    }

    /// get next token from Lexer
    token_type get_token()
    {
        return (last_token = m_lexer.scan());
    }

    std::string exception_message(const token_type expected, const std::string& context)
    {
        std::string error_msg = "syntax error ";

        if (not context.empty())
        {
            error_msg += "while parsing " + context + " ";
        }

        error_msg += "- ";

        if (last_token == token_type::parse_error)
        {
            error_msg += std::string(m_lexer.get_error_message()) + "; last read: '" +
                         m_lexer.get_token_string() + "'";
        }
        else
        {
            error_msg += "unexpected " + std::string(lexer_t::token_type_name(last_token));
        }

        if (expected != token_type::uninitialized)
        {
            error_msg += "; expected " + std::string(lexer_t::token_type_name(expected));
        }

        return error_msg;
    }

  private:
    /// callback function
    const parser_callback_t callback = nullptr;
    /// the type of the last read token
    token_type last_token = token_type::uninitialized;
    /// the Lexer
    lexer_t m_lexer;
    /// whether to throw exceptions in case of errors
    const bool allow_exceptions = true;
};
}  // namespace detail
}  // namespace nlohmann

#include <cstddef> // ptrdiff_t
#include <limits>  // numeric_limits

namespace nlohmann
{
namespace detail
{

class primitive_iterator_t
{
  private:
    using difference_type = std::ptrdiff_t;
    static constexpr difference_type begin_value = 0;
    static constexpr difference_type end_value = begin_value + 1;

    /// iterator as signed integer type
    difference_type m_it = (std::numeric_limits<std::ptrdiff_t>::min)();

  public:
    constexpr difference_type get_value() const 
    {
        return m_it;
    }

    /// set iterator to a defined beginning
    void set_begin() 
    {
        m_it = begin_value;
    }

    /// set iterator to a defined past the end
    void set_end() 
    {
        m_it = end_value;
    }

    /// return whether the iterator can be dereferenced
    constexpr bool is_begin() const 
    {
        return m_it == begin_value;
    }

    /// return whether the iterator is at end
    constexpr bool is_end() const 
    {
        return m_it == end_value;
    }

    friend constexpr bool operator==(primitive_iterator_t lhs, primitive_iterator_t rhs) 
    {
        return lhs.m_it == rhs.m_it;
    }

    friend constexpr bool operator<(primitive_iterator_t lhs, primitive_iterator_t rhs) 
    {
        return lhs.m_it < rhs.m_it;
    }

    primitive_iterator_t operator+(difference_type n) 
    {
        auto result = *this;
        result += n;
        return result;
    }

    friend constexpr difference_type operator-(primitive_iterator_t lhs, primitive_iterator_t rhs) 
    {
        return lhs.m_it - rhs.m_it;
    }

    primitive_iterator_t& operator++() 
    {
        ++m_it;
        return *this;
    }

    primitive_iterator_t const operator++(int) 
    {
        auto result = *this;
        ++m_it;
        return result;
    }

    primitive_iterator_t& operator--() 
    {
        --m_it;
        return *this;
    }

    primitive_iterator_t const operator--(int) 
    {
        auto result = *this;
        --m_it;
        return result;
    }

    primitive_iterator_t& operator+=(difference_type n) 
    {
        m_it += n;
        return *this;
    }

    primitive_iterator_t& operator-=(difference_type n) 
    {
        m_it -= n;
        return *this;
    }
};
}  // namespace detail
}  // namespace nlohmann

namespace nlohmann
{
namespace detail
{
/*!
@brief an iterator value

@note This structure could easily be a union, but MSVC currently does not allow
unions members with complex constructors, see https://github.com/nlohmann/json/pull/105.
*/
template<typename Basic_json> struct internal_iterator
{
    /// iterator for JSON objects
    typename Basic_json::Object::iterator object_iterator {};
    /// iterator for JSON arrays
    typename Basic_json::Array::iterator array_iterator {};
    /// generic iterator for all other types
    primitive_iterator_t primitive_iterator {};
};
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/iterators/Iter_impl.hpp>


#include <ciso646> // not
#include <iterator> // iterator, random_access_iterator_tag, bidirectional_iterator_tag, advance, next
#include <type_traits> // conditional, is_const, remove_const




namespace nlohmann
{
namespace detail
{
// forward declare, to be able to friend it later on
template<typename IteratorType> class iteration_proxy;
template<typename IteratorType> class iteration_proxy_value;


template<typename Basic_json>
class Iter_impl
{
    /// allow basic_json to access private members
    friend Iter_impl<typename std::conditional<std::is_const<Basic_json>::value, typename std::remove_const<Basic_json>::type, const Basic_json>::type>;
    friend Basic_json;
    friend iteration_proxy<Iter_impl>;
    friend iteration_proxy_value<Iter_impl>;

    using Object = typename Basic_json::Object;
    using Array = typename Basic_json::Array;
    // make sure Basic_json is basic_json or const basic_json
    static_assert(Is_basic_json<typename std::remove_const<Basic_json>::type>::value,
                  "Iter_impl only accepts (const) basic_json");

  public:

    /// The std::iterator class template (used as a base class to provide typedefs) is deprecated in C++17.
    /// The C++ Standard has never required user-defined iterators to derive from std::iterator.
    /// A user-defined iterator should provide publicly accessible typedefs named
    /// iterator_category, value_type, difference_type, pointer, and reference.
    /// Note that value_type is required to be non-const, even for constant iterators.
    using iterator_category = std::bidirectional_iterator_tag;

    /// the type of the values when the iterator is dereferenced
    using value_type = typename Basic_json::value_type;
    /// a type to represent differences between iterators
    using difference_type = typename Basic_json::difference_type;
    /// defines a pointer to the type iterated over (value_type)
    using pointer = typename std::conditional<std::is_const<Basic_json>::value,
          typename Basic_json::const_pointer,
          typename Basic_json::pointer>::type;
    /// defines a reference to the type iterated over (value_type)
    using reference =
        typename std::conditional<std::is_const<Basic_json>::value,
        typename Basic_json::const_reference,
        typename Basic_json::reference>::type;

    /// default constructor
    Iter_impl() = default;

    /*!
    @brief constructor for a given JSON instance
    @param[in] object  pointer to a JSON object for this iterator
    @pre object != nullptr
    @post The iterator is initialized; i.e. `m_object != nullptr`.
    */
    explicit Iter_impl(pointer object)  : m_object(object)
    {
        assert(m_object != nullptr);

        switch (m_object->type_)
        {
            case Value::object:
            {
                m_it.object_iterator = typename Object::iterator();
                break;
            }

            case Value::array:
            {
                m_it.array_iterator = typename Array::iterator();
                break;
            }

            default:
            {
                m_it.primitive_iterator = primitive_iterator_t();
                break;
            }
        }
    }


    Iter_impl(const Iter_impl<typename std::remove_const<Basic_json>::type>& other) 
        : m_object(other.m_object), m_it(other.m_it) {}

    /*!
    @brief converting assignment
    @param[in,out] other  non-const iterator to copy from
    @return const/non-const iterator
    @note It is not checked whether @a other is initialized.
    */
    Iter_impl& operator=(const Iter_impl<typename std::remove_const<Basic_json>::type>& other) 
    {
        m_object = other.m_object;
        m_it = other.m_it;
        return *this;
    }

  private:
    /*!
    @brief set the iterator to the first value
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    void set_begin() 
    {
        assert(m_object != nullptr);

        switch (m_object->type_)
        {
            case Value::object:
            {
                m_it.object_iterator = m_object->value_.object->begin();
                break;
            }

            case Value::array:
            {
                m_it.array_iterator = m_object->value_.array->begin();
                break;
            }

            case Value::null:
            {
                // set to end so begin()==end() is true: null is empty
                m_it.primitive_iterator.set_end();
                break;
            }

            default:
            {
                m_it.primitive_iterator.set_begin();
                break;
            }
        }
    }

    /*!
    @brief set the iterator past the last value
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    void set_end() 
    {
        assert(m_object != nullptr);

        switch (m_object->type_)
        {
            case Value::object:
            {
                m_it.object_iterator = m_object->value_.object->end();
                break;
            }

            case Value::array:
            {
                m_it.array_iterator = m_object->value_.array->end();
                break;
            }

            default:
            {
                m_it.primitive_iterator.set_end();
                break;
            }
        }
    }

  public:
    /*!
    @brief return a reference to the value pointed to by the iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    reference operator*() const
    {
        assert(m_object != nullptr);

        switch (m_object->type_)
        {
            case Value::object:
            {
                assert(m_it.object_iterator != m_object->value_.object->end());
                return m_it.object_iterator->second;
            }

            case Value::array:
            {
                assert(m_it.array_iterator != m_object->value_.array->end());
                return *m_it.array_iterator;
            }

            case Value::null:
                JSON_THROW(invalid_iterator::create(214, "cannot get value"));

            default:
            {
                if ((m_it.primitive_iterator.is_begin()))
                {
                    return *m_object;
                }

                JSON_THROW(invalid_iterator::create(214, "cannot get value"));
            }
        }
    }

    /*!
    @brief dereference the iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    pointer operator->() const
    {
        assert(m_object != nullptr);

        switch (m_object->type_)
        {
            case Value::object:
            {
                assert(m_it.object_iterator != m_object->value_.object->end());
                return &(m_it.object_iterator->second);
            }

            case Value::array:
            {
                assert(m_it.array_iterator != m_object->value_.array->end());
                return &*m_it.array_iterator;
            }

            default:
            {
                if ((m_it.primitive_iterator.is_begin()))
                {
                    return m_object;
                }

                JSON_THROW(invalid_iterator::create(214, "cannot get value"));
            }
        }
    }

    /*!
    @brief post-increment (it++)
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    Iter_impl const operator++(int)
    {
        auto result = *this;
        ++(*this);
        return result;
    }

    /*!
    @brief pre-increment (++it)
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    Iter_impl& operator++()
    {
        assert(m_object != nullptr);

        switch (m_object->type_)
        {
            case Value::object:
            {
                std::advance(m_it.object_iterator, 1);
                break;
            }

            case Value::array:
            {
                std::advance(m_it.array_iterator, 1);
                break;
            }

            default:
            {
                ++m_it.primitive_iterator;
                break;
            }
        }

        return *this;
    }

    /*!
    @brief post-decrement (it--)
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    Iter_impl const operator--(int)
    {
        auto result = *this;
        --(*this);
        return result;
    }

    /*!
    @brief pre-decrement (--it)
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    Iter_impl& operator--()
    {
        assert(m_object != nullptr);

        switch (m_object->type_)
        {
            case Value::object:
            {
                std::advance(m_it.object_iterator, -1);
                break;
            }

            case Value::array:
            {
                std::advance(m_it.array_iterator, -1);
                break;
            }

            default:
            {
                --m_it.primitive_iterator;
                break;
            }
        }

        return *this;
    }

    /*!
    @brief  comparison: equal
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    bool operator==(const Iter_impl& other) const
    {
        // if objects are not the same, the comparison is undefined
        if ((m_object != other.m_object))
        {
            JSON_THROW(invalid_iterator::create(212, "cannot compare iterators of different containers"));
        }

        assert(m_object != nullptr);

        switch (m_object->type_)
        {
            case Value::object:
                return (m_it.object_iterator == other.m_it.object_iterator);

            case Value::array:
                return (m_it.array_iterator == other.m_it.array_iterator);

            default:
                return (m_it.primitive_iterator == other.m_it.primitive_iterator);
        }
    }

    /*!
    @brief  comparison: not equal
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    bool operator!=(const Iter_impl& other) const
    {
        return not operator==(other);
    }

    /*!
    @brief  comparison: smaller
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    bool operator<(const Iter_impl& other) const
    {
        // if objects are not the same, the comparison is undefined
        if ((m_object != other.m_object))
        {
            JSON_THROW(invalid_iterator::create(212, "cannot compare iterators of different containers"));
        }

        assert(m_object != nullptr);

        switch (m_object->type_)
        {
            case Value::object:
                JSON_THROW(invalid_iterator::create(213, "cannot compare order of object iterators"));

            case Value::array:
                return (m_it.array_iterator < other.m_it.array_iterator);

            default:
                return (m_it.primitive_iterator < other.m_it.primitive_iterator);
        }
    }

    /*!
    @brief  comparison: less than or equal
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    bool operator<=(const Iter_impl& other) const
    {
        return not other.operator < (*this);
    }

    /*!
    @brief  comparison: greater than
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    bool operator>(const Iter_impl& other) const
    {
        return not operator<=(other);
    }

    /*!
    @brief  comparison: greater than or equal
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    bool operator>=(const Iter_impl& other) const
    {
        return not operator<(other);
    }

    /*!
    @brief  add to iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    Iter_impl& operator+=(difference_type i)
    {
        assert(m_object != nullptr);

        switch (m_object->type_)
        {
            case Value::object:
                JSON_THROW(invalid_iterator::create(209, "cannot use offsets with object iterators"));

            case Value::array:
            {
                std::advance(m_it.array_iterator, i);
                break;
            }

            default:
            {
                m_it.primitive_iterator += i;
                break;
            }
        }

        return *this;
    }

    /*!
    @brief  subtract from iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    Iter_impl& operator-=(difference_type i)
    {
        return operator+=(-i);
    }

    /*!
    @brief  add to iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    Iter_impl operator+(difference_type i) const
    {
        auto result = *this;
        result += i;
        return result;
    }

    /*!
    @brief  addition of distance and iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    friend Iter_impl operator+(difference_type i, const Iter_impl& it)
    {
        auto result = it;
        result += i;
        return result;
    }

    /*!
    @brief  subtract from iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    Iter_impl operator-(difference_type i) const
    {
        auto result = *this;
        result -= i;
        return result;
    }

    /*!
    @brief  return difference
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    difference_type operator-(const Iter_impl& other) const
    {
        assert(m_object != nullptr);

        switch (m_object->type_)
        {
            case Value::object:
                JSON_THROW(invalid_iterator::create(209, "cannot use offsets with object iterators"));

            case Value::array:
                return m_it.array_iterator - other.m_it.array_iterator;

            default:
                return m_it.primitive_iterator - other.m_it.primitive_iterator;
        }
    }

    /*!
    @brief  access to successor
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    reference operator[](difference_type n) const
    {
        assert(m_object != nullptr);

        switch (m_object->type_)
        {
            case Value::object:
                JSON_THROW(invalid_iterator::create(208, "cannot use operator[] for object iterators"));

            case Value::array:
                return *std::next(m_it.array_iterator, n);

            case Value::null:
                JSON_THROW(invalid_iterator::create(214, "cannot get value"));

            default:
            {
                if ((m_it.primitive_iterator.get_value() == -n))
                {
                    return *m_object;
                }

                JSON_THROW(invalid_iterator::create(214, "cannot get value"));
            }
        }
    }

    /*!
    @brief  return the key of an object iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    const typename Object::key_type& key() const
    {
        assert(m_object != nullptr);

        if ((m_object->is_object()))
        {
            return m_it.object_iterator->first;
        }

        JSON_THROW(invalid_iterator::create(207, "cannot use key() for non-object iterators"));
    }

    /*!
    @brief  return the value of an iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    reference value() const
    {
        return operator*();
    }

  private:
    /// associated JSON instance
    pointer m_object = nullptr;
    /// the actual iterator of the associated instance
    internal_iterator<typename std::remove_const<Basic_json>::type> m_it;
};
}  // namespace detail
} // namespace nlohmann



#include <cstddef> // ptrdiff_t
#include <iterator> // reverse_iterator
#include <utility> // declval

namespace nlohmann
{
namespace detail
{

template<typename Base>
class Json_reverse_iterator : public std::reverse_iterator<Base>
{
  public:
    using difference_type = std::ptrdiff_t;
    /// shortcut to the reverse iterator adapter
    using base_iterator = std::reverse_iterator<Base>;
    /// the reference type for the pointed-to element
    using reference = typename Base::reference;

    /// create reverse iterator from iterator
    explicit Json_reverse_iterator(const typename base_iterator::iterator_type& it) 
        : base_iterator(it) {}

    /// create reverse iterator from base class
    explicit Json_reverse_iterator(const base_iterator& it)  : base_iterator(it) {}

    /// post-increment (it++)
    Json_reverse_iterator const operator++(int)
    {
        return static_cast<Json_reverse_iterator>(base_iterator::operator++(1));
    }

    /// pre-increment (++it)
    Json_reverse_iterator& operator++()
    {
        return static_cast<Json_reverse_iterator&>(base_iterator::operator++());
    }

    /// post-decrement (it--)
    Json_reverse_iterator const operator--(int)
    {
        return static_cast<Json_reverse_iterator>(base_iterator::operator--(1));
    }

    /// pre-decrement (--it)
    Json_reverse_iterator& operator--()
    {
        return static_cast<Json_reverse_iterator&>(base_iterator::operator--());
    }

    /// add to iterator
    Json_reverse_iterator& operator+=(difference_type i)
    {
        return static_cast<Json_reverse_iterator&>(base_iterator::operator+=(i));
    }

    /// add to iterator
    Json_reverse_iterator operator+(difference_type i) const
    {
        return static_cast<Json_reverse_iterator>(base_iterator::operator+(i));
    }

    /// subtract from iterator
    Json_reverse_iterator operator-(difference_type i) const
    {
        return static_cast<Json_reverse_iterator>(base_iterator::operator-(i));
    }

    /// return difference
    difference_type operator-(const Json_reverse_iterator& other) const
    {
        return base_iterator(*this) - base_iterator(other);
    }

    /// access to successor
    reference operator[](difference_type n) const
    {
        return *(this->operator+(n));
    }

    /// return the key of an object iterator
    auto key() const -> decltype(std::declval<Base>().key())
    {
        auto it = --this->base();
        return it.key();
    }

    /// return the value of an iterator
    reference value() const
    {
        auto it = --this->base();
        return it.operator * ();
    }
};
}  // namespace detail
}  // namespace nlohmann

#include <algorithm> // copy
#include <cstddef> // size_t
#include <ios> // streamsize
#include <iterator> // back_inserter
#include <memory> // shared_ptr, make_shared
#include <ostream> // basic_ostream
#include <string> // basic_string
#include <vector> // vector

namespace nlohmann
{
namespace detail
{
/// abstract output adapter interface
template<typename CharType> struct Output_adapter_protocol
{
    virtual void write_character(CharType c) = 0;
    virtual void write_characters(const CharType* s, std::size_t length) = 0;
    virtual ~Output_adapter_protocol() = default;
};

/// a type to simplify interfaces
template<typename CharType>
using output_adapter_t = std::shared_ptr<Output_adapter_protocol<CharType>>;

/// output adapter for byte vectors
template<typename CharType>
class Output_vector_adapter : public Output_adapter_protocol<CharType>
{
  public:
    explicit Output_vector_adapter(std::vector<CharType>& vec) 
        : v(vec)
    {}

    void write_character(CharType c) override
    {
        v.push_back(c);
    }

    void write_characters(const CharType* s, std::size_t length) override
    {
        std::copy(s, s + length, std::back_inserter(v));
    }

  private:
    std::vector<CharType>& v;
};

/// output adapter for output streams
template<typename CharType>
class Output_stream_adapter : public Output_adapter_protocol<CharType>
{
  public:
    explicit Output_stream_adapter(std::basic_ostream<CharType>& s) 
        : stream(s)
    {}

    void write_character(CharType c) override
    {
        stream.put(c);
    }

    void write_characters(const CharType* s, std::size_t length) override
    {
        stream.write(s, static_cast<std::streamsize>(length));
    }

  private:
    std::basic_ostream<CharType>& stream;
};

/// output adapter for basic_string
template<typename CharType, typename String = std::basic_string<CharType>>
class output_string_adapter : public Output_adapter_protocol<CharType>
{
  public:
    explicit output_string_adapter(String& s) 
        : str(s)
    {}

    void write_character(CharType c) override
    {
        str.push_back(c);
    }

    void write_characters(const CharType* s, std::size_t length) override
    {
        str.append(s, length);
    }

  private:
    String& str;
};

template<typename CharType, typename String = std::basic_string<CharType>>
class output_adapter
{
  public:
    output_adapter(std::vector<CharType>& vec)
        : oa(std::make_shared<Output_vector_adapter<CharType>>(vec)) {}

    output_adapter(std::basic_ostream<CharType>& s)
        : oa(std::make_shared<Output_stream_adapter<CharType>>(s)) {}

    output_adapter(String& s)
        : oa(std::make_shared<output_string_adapter<CharType, String>>(s)) {}

    operator output_adapter_t<CharType>()
    {
        return oa;
    }

  private:
    output_adapter_t<CharType> oa = nullptr;
};
}  // namespace detail
}  // namespace nlohmann


#include <algorithm> // generate_n
#include <array> // array
#include <cassert> // assert
#include <cmath> // ldexp
#include <cstddef> // size_t
#include <cstdint> // uint8_t, uint16_t, uint32_t, uint64_t
#include <cstdio> // snprintf
#include <cstring> // memcpy
#include <iterator> // back_inserter
#include <limits> // numeric_limits
#include <string> // char_traits, string
#include <utility> // make_pair, move




namespace nlohmann
{
namespace detail
{
///////////////////
// binary reader //
///////////////////

/*!
@brief deserialization of CBOR, MessagePack, and UBJSON values
*/
template<typename Basic_json, typename SAX = json_sax_dom_parser<Basic_json>>
class Binary_reader
{
    using Num_integer = typename Basic_json::Num_integer;
    using Num_unsigned = typename Basic_json::Num_unsigned;
    using Num_float = typename Basic_json::Num_float;
    using String = typename Basic_json::String;
    using json_sax_t = SAX;

  public:
    /*!
    @brief create a binary reader

    @param[in] adapter  input adapter to read from
    */
    explicit Binary_reader(input_adapter_t adapter) : ia(std::move(adapter))
    {
        (void)detail::is_sax_static_asserts<SAX, Basic_json> {};
        assert(ia);
    }

    /*!
    @param[in] format  the binary format to parse
    @param[in] sax_    a SAX event processor
    @param[in] strict  whether to expect the input to be consumed completed

    @return
    */
    bool sax_parse(const input_format_t format,
                   json_sax_t* sax_,
                   const bool strict = true)
    {
        sax = sax_;
        bool result = false;

        switch (format)
        {
            case input_format_t::bson:
                result = parse_bson_internal();
                break;

            case input_format_t::cbor:
                result = parse_cbor_internal();
                break;

            case input_format_t::msgpack:
                result = parse_msgpack_internal();
                break;

            case input_format_t::ubjson:
                result = parse_ubjson_internal();
                break;

            // LCOV_EXCL_START
            default:
                assert(false);
                // LCOV_EXCL_STOP
        }

        // strict mode: next byte must be EOF
        if (result and strict)
        {
            if (format == input_format_t::ubjson)
            {
                get_ignore_noop();
            }
            else
            {
                get();
            }

            if ((current != std::char_traits<char>::eof()))
            {
                return sax->parse_error(chars_read, get_token_string(),
                                        parse_error::create(110, chars_read, exception_message(format, "expected end of input; last byte: 0x" + get_token_string(), "value")));
            }
        }

        return result;
    }

    /*!
    @brief determine system byte order

    @return true if and only if system's byte order is little endian

    @note from http://stackoverflow.com/a/1001328/266378
    */
    static constexpr bool little_endianess(int num = 1) 
    {
        return (*reinterpret_cast<char*>(&num) == 1);
    }

  private:
    //////////
    // BSON //
    //////////

    /*!
    @brief Reads in a BSON-object and passes it to the SAX-Parser.
    @return whether a valid BSON-value was passed to the SAX Parser
    */
    bool parse_bson_internal()
    {
        std::int32_t document_size;
        get_number<std::int32_t, true>(input_format_t::bson, document_size);

        if ((not sax->start_object(std::size_t(-1))))
        {
            return false;
        }

        if ((not parse_bson_element_list(/*is_array*/false)))
        {
            return false;
        }

        return sax->end_object();
    }

    /*!
    @brief Parses a C-style string from the BSON input.
    @param[in, out] result  A reference to the string variable where the read
                            string is to be stored.
    @return `true` if the \x00-byte indicating the end of the string was
             encountered before the EOF; false` indicates an unexpected EOF.
    */
    bool get_bson_cstr(String& result)
    {
        auto out = std::back_inserter(result);
        while (true)
        {
            get();
            if ((not unexpect_eof(input_format_t::bson, "cstring")))
            {
                return false;
            }
            if (current == 0x00)
            {
                return true;
            }
            *out++ = static_cast<char>(current);
        }

        return true;
    }

    /*!
    @brief Parses a zero-terminated string of length @a len from the BSON
           input.
    @param[in] len  The length (including the zero-byte at the end) of the
                    string to be read.
    @param[in, out] result  A reference to the string variable where the read
                            string is to be stored.
    @tparam NumberType The type of the length @a len
    @pre len >= 1
    @return `true` if the string was successfully parsed
    */
    template<typename NumberType>
    bool get_bson_string(const NumberType len, String& result)
    {
        if ((len < 1))
        {
            auto last_token = get_token_string();
            return sax->parse_error(chars_read, last_token, parse_error::create(112, chars_read, exception_message(input_format_t::bson, "string length must be at least 1, is " + std::to_string(len), "string")));
        }

        return get_string(input_format_t::bson, len - static_cast<NumberType>(1), result) and get() != std::char_traits<char>::eof();
    }

    /*!
    @brief Read a BSON document element of the given @a element_type.
    @param[in] element_type The BSON element type, c.f. http://bsonspec.org/spec.html
    @param[in] element_type_parse_position The position in the input stream,
               where the `element_type` was read.
    @warning Not all BSON element types are supported yet. An unsupported
             @a element_type will give rise to a parse_error.114:
             Unsupported BSON record type 0x...
    @return whether a valid BSON-object/array was passed to the SAX Parser
    */
    bool parse_bson_element_internal(const int element_type,
                                     const std::size_t element_type_parse_position)
    {
        switch (element_type)
        {
            case 0x01: // double
            {
                double number;
                return get_number<double, true>(input_format_t::bson, number) and sax->num_float(static_cast<Num_float>(number), "");
            }

            case 0x02: // string
            {
                std::int32_t len;
                String value;
                return get_number<std::int32_t, true>(input_format_t::bson, len) and get_bson_string(len, value) and sax->string(value);
            }

            case 0x03: // object
            {
                return parse_bson_internal();
            }

            case 0x04: // array
            {
                return parse_bson_array();
            }

            case 0x08: // boolean
            {
                return sax->boolean(get() != 0);
            }

            case 0x0A: // null
            {
                return sax->null();
            }

            case 0x10: // int32
            {
                std::int32_t value;
                return get_number<std::int32_t, true>(input_format_t::bson, value) and sax->num_integer(value);
            }

            case 0x12: // int64
            {
                std::int64_t value;
                return get_number<std::int64_t, true>(input_format_t::bson, value) and sax->num_integer(value);
            }

            default: // anything else not supported (yet)
            {
                char cr[3];
                (std::snprintf)(cr, sizeof(cr), "%.2hhX", static_cast<unsigned char>(element_type));
                return sax->parse_error(element_type_parse_position, std::string(cr), parse_error::create(114, element_type_parse_position, "Unsupported BSON record type 0x" + std::string(cr)));
            }
        }
    }

    /*!
    @brief Read a BSON element list (as specified in the BSON-spec)

    The same binary layout is used for objects and arrays, hence it must be
    indicated with the argument @a is_array which one is expected
    (true --> array, false --> object).

    @param[in] is_array Determines if the element list being read is to be
                        treated as an object (@a is_array == false), or as an
                        array (@a is_array == true).
    @return whether a valid BSON-object/array was passed to the SAX Parser
    */
    bool parse_bson_element_list(const bool is_array)
    {
        String key;
        while (int element_type = get())
        {
            if ((not unexpect_eof(input_format_t::bson, "element list")))
            {
                return false;
            }

            const std::size_t element_type_parse_position = chars_read;
            if ((not get_bson_cstr(key)))
            {
                return false;
            }

            if (not is_array)
            {
                if (not sax->key(key))
                {
                    return false;
                }
            }

            if ((not parse_bson_element_internal(element_type, element_type_parse_position)))
            {
                return false;
            }

            // get_bson_cstr only appends
            key.clear();
        }

        return true;
    }

    /*!
    @brief Reads an array from the BSON input and passes it to the SAX-Parser.
    @return whether a valid BSON-array was passed to the SAX Parser
    */
    bool parse_bson_array()
    {
        std::int32_t document_size;
        get_number<std::int32_t, true>(input_format_t::bson, document_size);

        if ((not sax->start_array(std::size_t(-1))))
        {
            return false;
        }

        if ((not parse_bson_element_list(/*is_array*/true)))
        {
            return false;
        }

        return sax->end_array();
    }

    //////////
    // CBOR //
    //////////

    /*!
    @param[in] get_char  whether a new character should be retrieved from the
                         input (true, default) or whether the last read
                         character should be considered instead

    @return whether a valid CBOR value was passed to the SAX Parser
    */
    bool parse_cbor_internal(const bool get_char = true)
    {
        switch (get_char ? get() : current)
        {
            // EOF
            case std::char_traits<char>::eof():
                return unexpect_eof(input_format_t::cbor, "value");

            // Integer 0x00..0x17 (0..23)
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
            case 0x08:
            case 0x09:
            case 0x0A:
            case 0x0B:
            case 0x0C:
            case 0x0D:
            case 0x0E:
            case 0x0F:
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
                return sax->num_unsigned(static_cast<Num_unsigned>(current));

            case 0x18: // Unsigned integer (one-byte uint8_t follows)
            {
                uint8_t number;
                return get_number(input_format_t::cbor, number) and sax->num_unsigned(number);
            }

            case 0x19: // Unsigned integer (two-byte uint16_t follows)
            {
                uint16_t number;
                return get_number(input_format_t::cbor, number) and sax->num_unsigned(number);
            }

            case 0x1A: // Unsigned integer (four-byte uint32_t follows)
            {
                uint32_t number;
                return get_number(input_format_t::cbor, number) and sax->num_unsigned(number);
            }

            case 0x1B: // Unsigned integer (eight-byte uint64_t follows)
            {
                uint64_t number;
                return get_number(input_format_t::cbor, number) and sax->num_unsigned(number);
            }

            // Negative integer -1-0x00..-1-0x17 (-1..-24)
            case 0x20:
            case 0x21:
            case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2A:
            case 0x2B:
            case 0x2C:
            case 0x2D:
            case 0x2E:
            case 0x2F:
            case 0x30:
            case 0x31:
            case 0x32:
            case 0x33:
            case 0x34:
            case 0x35:
            case 0x36:
            case 0x37:
                return sax->num_integer(static_cast<int8_t>(0x20 - 1 - current));

            case 0x38: // Negative integer (one-byte uint8_t follows)
            {
                uint8_t number;
                return get_number(input_format_t::cbor, number) and sax->num_integer(static_cast<Num_integer>(-1) - number);
            }

            case 0x39: // Negative integer -1-n (two-byte uint16_t follows)
            {
                uint16_t number;
                return get_number(input_format_t::cbor, number) and sax->num_integer(static_cast<Num_integer>(-1) - number);
            }

            case 0x3A: // Negative integer -1-n (four-byte uint32_t follows)
            {
                uint32_t number;
                return get_number(input_format_t::cbor, number) and sax->num_integer(static_cast<Num_integer>(-1) - number);
            }

            case 0x3B: // Negative integer -1-n (eight-byte uint64_t follows)
            {
                uint64_t number;
                return get_number(input_format_t::cbor, number) and sax->num_integer(static_cast<Num_integer>(-1)
                        - static_cast<Num_integer>(number));
            }

            // UTF-8 string (0x00..0x17 bytes follow)
            case 0x60:
            case 0x61:
            case 0x62:
            case 0x63:
            case 0x64:
            case 0x65:
            case 0x66:
            case 0x67:
            case 0x68:
            case 0x69:
            case 0x6A:
            case 0x6B:
            case 0x6C:
            case 0x6D:
            case 0x6E:
            case 0x6F:
            case 0x70:
            case 0x71:
            case 0x72:
            case 0x73:
            case 0x74:
            case 0x75:
            case 0x76:
            case 0x77:
            case 0x78: // UTF-8 string (one-byte uint8_t for n follows)
            case 0x79: // UTF-8 string (two-byte uint16_t for n follow)
            case 0x7A: // UTF-8 string (four-byte uint32_t for n follow)
            case 0x7B: // UTF-8 string (eight-byte uint64_t for n follow)
            case 0x7F: // UTF-8 string (indefinite length)
            {
                String s;
                return get_cbor_string(s) and sax->string(s);
            }

            // array (0x00..0x17 data items follow)
            case 0x80:
            case 0x81:
            case 0x82:
            case 0x83:
            case 0x84:
            case 0x85:
            case 0x86:
            case 0x87:
            case 0x88:
            case 0x89:
            case 0x8A:
            case 0x8B:
            case 0x8C:
            case 0x8D:
            case 0x8E:
            case 0x8F:
            case 0x90:
            case 0x91:
            case 0x92:
            case 0x93:
            case 0x94:
            case 0x95:
            case 0x96:
            case 0x97:
                return get_cbor_array(static_cast<std::size_t>(current & 0x1F));

            case 0x98: // array (one-byte uint8_t for n follows)
            {
                uint8_t len;
                return get_number(input_format_t::cbor, len) and get_cbor_array(static_cast<std::size_t>(len));
            }

            case 0x99: // array (two-byte uint16_t for n follow)
            {
                uint16_t len;
                return get_number(input_format_t::cbor, len) and get_cbor_array(static_cast<std::size_t>(len));
            }

            case 0x9A: // array (four-byte uint32_t for n follow)
            {
                uint32_t len;
                return get_number(input_format_t::cbor, len) and get_cbor_array(static_cast<std::size_t>(len));
            }

            case 0x9B: // array (eight-byte uint64_t for n follow)
            {
                uint64_t len;
                return get_number(input_format_t::cbor, len) and get_cbor_array(static_cast<std::size_t>(len));
            }

            case 0x9F: // array (indefinite length)
                return get_cbor_array(std::size_t(-1));

            // map (0x00..0x17 pairs of data items follow)
            case 0xA0:
            case 0xA1:
            case 0xA2:
            case 0xA3:
            case 0xA4:
            case 0xA5:
            case 0xA6:
            case 0xA7:
            case 0xA8:
            case 0xA9:
            case 0xAA:
            case 0xAB:
            case 0xAC:
            case 0xAD:
            case 0xAE:
            case 0xAF:
            case 0xB0:
            case 0xB1:
            case 0xB2:
            case 0xB3:
            case 0xB4:
            case 0xB5:
            case 0xB6:
            case 0xB7:
                return get_cbor_object(static_cast<std::size_t>(current & 0x1F));

            case 0xB8: // map (one-byte uint8_t for n follows)
            {
                uint8_t len;
                return get_number(input_format_t::cbor, len) and get_cbor_object(static_cast<std::size_t>(len));
            }

            case 0xB9: // map (two-byte uint16_t for n follow)
            {
                uint16_t len;
                return get_number(input_format_t::cbor, len) and get_cbor_object(static_cast<std::size_t>(len));
            }

            case 0xBA: // map (four-byte uint32_t for n follow)
            {
                uint32_t len;
                return get_number(input_format_t::cbor, len) and get_cbor_object(static_cast<std::size_t>(len));
            }

            case 0xBB: // map (eight-byte uint64_t for n follow)
            {
                uint64_t len;
                return get_number(input_format_t::cbor, len) and get_cbor_object(static_cast<std::size_t>(len));
            }

            case 0xBF: // map (indefinite length)
                return get_cbor_object(std::size_t(-1));

            case 0xF4: // false
                return sax->boolean(false);

            case 0xF5: // true
                return sax->boolean(true);

            case 0xF6: // null
                return sax->null();

            case 0xF9: // Half-Precision Float (two-byte IEEE 754)
            {
                const int byte1_raw = get();
                if ((not unexpect_eof(input_format_t::cbor, "number")))
                {
                    return false;
                }
                const int byte2_raw = get();
                if ((not unexpect_eof(input_format_t::cbor, "number")))
                {
                    return false;
                }

                const auto byte1 = static_cast<unsigned char>(byte1_raw);
                const auto byte2 = static_cast<unsigned char>(byte2_raw);

                // code from RFC 7049, Appendix D, Figure 3:
                // As half-precision floating-point numbers were only added
                // to IEEE 754 in 2008, today's programming platforms often
                // still only have limited support for them. It is very
                // easy to include at least decoding support for them even
                // without such support. An example of a small decoder for
                // half-precision floating-point numbers in the C language
                // is shown in Fig. 3.
                const int half = (byte1 << 8) + byte2;
                const double val = [&half]
                {
                    const int exp = (half >> 10) & 0x1F;
                    const int mant = half & 0x3FF;
                    assert(0 <= exp and exp <= 32);
                    assert(0 <= mant and mant <= 1024);
                    switch (exp)
                    {
                        case 0:
                            return std::ldexp(mant, -24);
                        case 31:
                            return (mant == 0)
                            ? std::numeric_limits<double>::infinity()
                            : std::numeric_limits<double>::quiet_NaN();
                        default:
                            return std::ldexp(mant + 1024, exp - 25);
                    }
                }();
                return sax->num_float((half & 0x8000) != 0
                                         ? static_cast<Num_float>(-val)
                                         : static_cast<Num_float>(val), "");
            }

            case 0xFA: // Single-Precision Float (four-byte IEEE 754)
            {
                float number;
                return get_number(input_format_t::cbor, number) and sax->num_float(static_cast<Num_float>(number), "");
            }

            case 0xFB: // Double-Precision Float (eight-byte IEEE 754)
            {
                double number;
                return get_number(input_format_t::cbor, number) and sax->num_float(static_cast<Num_float>(number), "");
            }

            default: // anything else (0xFF is handled inside the other types)
            {
                auto last_token = get_token_string();
                return sax->parse_error(chars_read, last_token, parse_error::create(112, chars_read, exception_message(input_format_t::cbor, "invalid byte: 0x" + last_token, "value")));
            }
        }
    }

    /*!
    @brief reads a CBOR string

    This function first reads starting bytes to determine the expected
    string length and then copies this number of bytes into a string.
    Additionally, CBOR's strings with indefinite lengths are supported.

    @param[out] result  created string

    @return whether string creation completed
    */
    bool get_cbor_string(String& result)
    {
        if ((not unexpect_eof(input_format_t::cbor, "string")))
        {
            return false;
        }

        switch (current)
        {
            // UTF-8 string (0x00..0x17 bytes follow)
            case 0x60:
            case 0x61:
            case 0x62:
            case 0x63:
            case 0x64:
            case 0x65:
            case 0x66:
            case 0x67:
            case 0x68:
            case 0x69:
            case 0x6A:
            case 0x6B:
            case 0x6C:
            case 0x6D:
            case 0x6E:
            case 0x6F:
            case 0x70:
            case 0x71:
            case 0x72:
            case 0x73:
            case 0x74:
            case 0x75:
            case 0x76:
            case 0x77:
            {
                return get_string(input_format_t::cbor, current & 0x1F, result);
            }

            case 0x78: // UTF-8 string (one-byte uint8_t for n follows)
            {
                uint8_t len;
                return get_number(input_format_t::cbor, len) and get_string(input_format_t::cbor, len, result);
            }

            case 0x79: // UTF-8 string (two-byte uint16_t for n follow)
            {
                uint16_t len;
                return get_number(input_format_t::cbor, len) and get_string(input_format_t::cbor, len, result);
            }

            case 0x7A: // UTF-8 string (four-byte uint32_t for n follow)
            {
                uint32_t len;
                return get_number(input_format_t::cbor, len) and get_string(input_format_t::cbor, len, result);
            }

            case 0x7B: // UTF-8 string (eight-byte uint64_t for n follow)
            {
                uint64_t len;
                return get_number(input_format_t::cbor, len) and get_string(input_format_t::cbor, len, result);
            }

            case 0x7F: // UTF-8 string (indefinite length)
            {
                while (get() != 0xFF)
                {
                    String chunk;
                    if (not get_cbor_string(chunk))
                    {
                        return false;
                    }
                    result.append(chunk);
                }
                return true;
            }

            default:
            {
                auto last_token = get_token_string();
                return sax->parse_error(chars_read, last_token, parse_error::create(113, chars_read, exception_message(input_format_t::cbor, "expected length specification (0x60-0x7B) or indefinite string type (0x7F); last byte: 0x" + last_token, "string")));
            }
        }
    }

    /*!
    @param[in] len  the length of the array or std::size_t(-1) for an
                    array of indefinite size
    @return whether array creation completed
    */
    bool get_cbor_array(const std::size_t len)
    {
        if ((not sax->start_array(len)))
        {
            return false;
        }

        if (len != std::size_t(-1))
        {
            for (std::size_t i = 0; i < len; ++i)
            {
                if ((not parse_cbor_internal()))
                {
                    return false;
                }
            }
        }
        else
        {
            while (get() != 0xFF)
            {
                if ((not parse_cbor_internal(false)))
                {
                    return false;
                }
            }
        }

        return sax->end_array();
    }

    /*!
    @param[in] len  the length of the object or std::size_t(-1) for an
                    object of indefinite size
    @return whether object creation completed
    */
    bool get_cbor_object(const std::size_t len)
    {
        if (not (sax->start_object(len)))
        {
            return false;
        }

        String key;
        if (len != std::size_t(-1))
        {
            for (std::size_t i = 0; i < len; ++i)
            {
                get();
                if ((not get_cbor_string(key) or not sax->key(key)))
                {
                    return false;
                }

                if ((not parse_cbor_internal()))
                {
                    return false;
                }
                key.clear();
            }
        }
        else
        {
            while (get() != 0xFF)
            {
                if ((not get_cbor_string(key) or not sax->key(key)))
                {
                    return false;
                }

                if ((not parse_cbor_internal()))
                {
                    return false;
                }
                key.clear();
            }
        }

        return sax->end_object();
    }

    /////////////
    // MsgPack //
    /////////////

    /*!
    @return whether a valid MessagePack value was passed to the SAX Parser
    */
    bool parse_msgpack_internal()
    {
        switch (get())
        {
            // EOF
            case std::char_traits<char>::eof():
                return unexpect_eof(input_format_t::msgpack, "value");

            // positive fixint
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
            case 0x08:
            case 0x09:
            case 0x0A:
            case 0x0B:
            case 0x0C:
            case 0x0D:
            case 0x0E:
            case 0x0F:
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
            case 0x18:
            case 0x19:
            case 0x1A:
            case 0x1B:
            case 0x1C:
            case 0x1D:
            case 0x1E:
            case 0x1F:
            case 0x20:
            case 0x21:
            case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2A:
            case 0x2B:
            case 0x2C:
            case 0x2D:
            case 0x2E:
            case 0x2F:
            case 0x30:
            case 0x31:
            case 0x32:
            case 0x33:
            case 0x34:
            case 0x35:
            case 0x36:
            case 0x37:
            case 0x38:
            case 0x39:
            case 0x3A:
            case 0x3B:
            case 0x3C:
            case 0x3D:
            case 0x3E:
            case 0x3F:
            case 0x40:
            case 0x41:
            case 0x42:
            case 0x43:
            case 0x44:
            case 0x45:
            case 0x46:
            case 0x47:
            case 0x48:
            case 0x49:
            case 0x4A:
            case 0x4B:
            case 0x4C:
            case 0x4D:
            case 0x4E:
            case 0x4F:
            case 0x50:
            case 0x51:
            case 0x52:
            case 0x53:
            case 0x54:
            case 0x55:
            case 0x56:
            case 0x57:
            case 0x58:
            case 0x59:
            case 0x5A:
            case 0x5B:
            case 0x5C:
            case 0x5D:
            case 0x5E:
            case 0x5F:
            case 0x60:
            case 0x61:
            case 0x62:
            case 0x63:
            case 0x64:
            case 0x65:
            case 0x66:
            case 0x67:
            case 0x68:
            case 0x69:
            case 0x6A:
            case 0x6B:
            case 0x6C:
            case 0x6D:
            case 0x6E:
            case 0x6F:
            case 0x70:
            case 0x71:
            case 0x72:
            case 0x73:
            case 0x74:
            case 0x75:
            case 0x76:
            case 0x77:
            case 0x78:
            case 0x79:
            case 0x7A:
            case 0x7B:
            case 0x7C:
            case 0x7D:
            case 0x7E:
            case 0x7F:
                return sax->num_unsigned(static_cast<Num_unsigned>(current));

            // fixmap
            case 0x80:
            case 0x81:
            case 0x82:
            case 0x83:
            case 0x84:
            case 0x85:
            case 0x86:
            case 0x87:
            case 0x88:
            case 0x89:
            case 0x8A:
            case 0x8B:
            case 0x8C:
            case 0x8D:
            case 0x8E:
            case 0x8F:
                return get_msgpack_object(static_cast<std::size_t>(current & 0x0F));

            // fixarray
            case 0x90:
            case 0x91:
            case 0x92:
            case 0x93:
            case 0x94:
            case 0x95:
            case 0x96:
            case 0x97:
            case 0x98:
            case 0x99:
            case 0x9A:
            case 0x9B:
            case 0x9C:
            case 0x9D:
            case 0x9E:
            case 0x9F:
                return get_msgpack_array(static_cast<std::size_t>(current & 0x0F));

            // fixstr
            case 0xA0:
            case 0xA1:
            case 0xA2:
            case 0xA3:
            case 0xA4:
            case 0xA5:
            case 0xA6:
            case 0xA7:
            case 0xA8:
            case 0xA9:
            case 0xAA:
            case 0xAB:
            case 0xAC:
            case 0xAD:
            case 0xAE:
            case 0xAF:
            case 0xB0:
            case 0xB1:
            case 0xB2:
            case 0xB3:
            case 0xB4:
            case 0xB5:
            case 0xB6:
            case 0xB7:
            case 0xB8:
            case 0xB9:
            case 0xBA:
            case 0xBB:
            case 0xBC:
            case 0xBD:
            case 0xBE:
            case 0xBF:
            {
                String s;
                return get_msgpack_string(s) and sax->string(s);
            }

            case 0xC0: // nil
                return sax->null();

            case 0xC2: // false
                return sax->boolean(false);

            case 0xC3: // true
                return sax->boolean(true);

            case 0xCA: // float 32
            {
                float number;
                return get_number(input_format_t::msgpack, number) and sax->num_float(static_cast<Num_float>(number), "");
            }

            case 0xCB: // float 64
            {
                double number;
                return get_number(input_format_t::msgpack, number) and sax->num_float(static_cast<Num_float>(number), "");
            }

            case 0xCC: // uint 8
            {
                uint8_t number;
                return get_number(input_format_t::msgpack, number) and sax->num_unsigned(number);
            }

            case 0xCD: // uint 16
            {
                uint16_t number;
                return get_number(input_format_t::msgpack, number) and sax->num_unsigned(number);
            }

            case 0xCE: // uint 32
            {
                uint32_t number;
                return get_number(input_format_t::msgpack, number) and sax->num_unsigned(number);
            }

            case 0xCF: // uint 64
            {
                uint64_t number;
                return get_number(input_format_t::msgpack, number) and sax->num_unsigned(number);
            }

            case 0xD0: // int 8
            {
                int8_t number;
                return get_number(input_format_t::msgpack, number) and sax->num_integer(number);
            }

            case 0xD1: // int 16
            {
                int16_t number;
                return get_number(input_format_t::msgpack, number) and sax->num_integer(number);
            }

            case 0xD2: // int 32
            {
                int32_t number;
                return get_number(input_format_t::msgpack, number) and sax->num_integer(number);
            }

            case 0xD3: // int 64
            {
                int64_t number;
                return get_number(input_format_t::msgpack, number) and sax->num_integer(number);
            }

            case 0xD9: // str 8
            case 0xDA: // str 16
            case 0xDB: // str 32
            {
                String s;
                return get_msgpack_string(s) and sax->string(s);
            }

            case 0xDC: // array 16
            {
                uint16_t len;
                return get_number(input_format_t::msgpack, len) and get_msgpack_array(static_cast<std::size_t>(len));
            }

            case 0xDD: // array 32
            {
                uint32_t len;
                return get_number(input_format_t::msgpack, len) and get_msgpack_array(static_cast<std::size_t>(len));
            }

            case 0xDE: // map 16
            {
                uint16_t len;
                return get_number(input_format_t::msgpack, len) and get_msgpack_object(static_cast<std::size_t>(len));
            }

            case 0xDF: // map 32
            {
                uint32_t len;
                return get_number(input_format_t::msgpack, len) and get_msgpack_object(static_cast<std::size_t>(len));
            }

            // negative fixint
            case 0xE0:
            case 0xE1:
            case 0xE2:
            case 0xE3:
            case 0xE4:
            case 0xE5:
            case 0xE6:
            case 0xE7:
            case 0xE8:
            case 0xE9:
            case 0xEA:
            case 0xEB:
            case 0xEC:
            case 0xED:
            case 0xEE:
            case 0xEF:
            case 0xF0:
            case 0xF1:
            case 0xF2:
            case 0xF3:
            case 0xF4:
            case 0xF5:
            case 0xF6:
            case 0xF7:
            case 0xF8:
            case 0xF9:
            case 0xFA:
            case 0xFB:
            case 0xFC:
            case 0xFD:
            case 0xFE:
            case 0xFF:
                return sax->num_integer(static_cast<int8_t>(current));

            default: // anything else
            {
                auto last_token = get_token_string();
                return sax->parse_error(chars_read, last_token, parse_error::create(112, chars_read, exception_message(input_format_t::msgpack, "invalid byte: 0x" + last_token, "value")));
            }
        }
    }

    /*!
    @brief reads a MessagePack string

    This function first reads starting bytes to determine the expected
    string length and then copies this number of bytes into a string.

    @param[out] result  created string

    @return whether string creation completed
    */
    bool get_msgpack_string(String& result)
    {
        if ((not unexpect_eof(input_format_t::msgpack, "string")))
        {
            return false;
        }

        switch (current)
        {
            // fixstr
            case 0xA0:
            case 0xA1:
            case 0xA2:
            case 0xA3:
            case 0xA4:
            case 0xA5:
            case 0xA6:
            case 0xA7:
            case 0xA8:
            case 0xA9:
            case 0xAA:
            case 0xAB:
            case 0xAC:
            case 0xAD:
            case 0xAE:
            case 0xAF:
            case 0xB0:
            case 0xB1:
            case 0xB2:
            case 0xB3:
            case 0xB4:
            case 0xB5:
            case 0xB6:
            case 0xB7:
            case 0xB8:
            case 0xB9:
            case 0xBA:
            case 0xBB:
            case 0xBC:
            case 0xBD:
            case 0xBE:
            case 0xBF:
            {
                return get_string(input_format_t::msgpack, current & 0x1F, result);
            }

            case 0xD9: // str 8
            {
                uint8_t len;
                return get_number(input_format_t::msgpack, len) and get_string(input_format_t::msgpack, len, result);
            }

            case 0xDA: // str 16
            {
                uint16_t len;
                return get_number(input_format_t::msgpack, len) and get_string(input_format_t::msgpack, len, result);
            }

            case 0xDB: // str 32
            {
                uint32_t len;
                return get_number(input_format_t::msgpack, len) and get_string(input_format_t::msgpack, len, result);
            }

            default:
            {
                auto last_token = get_token_string();
                return sax->parse_error(chars_read, last_token, parse_error::create(113, chars_read, exception_message(input_format_t::msgpack, "expected length specification (0xA0-0xBF, 0xD9-0xDB); last byte: 0x" + last_token, "string")));
            }
        }
    }

    /*!
    @param[in] len  the length of the array
    @return whether array creation completed
    */
    bool get_msgpack_array(const std::size_t len)
    {
        if ((not sax->start_array(len)))
        {
            return false;
        }

        for (std::size_t i = 0; i < len; ++i)
        {
            if ((not parse_msgpack_internal()))
            {
                return false;
            }
        }

        return sax->end_array();
    }

    /*!
    @param[in] len  the length of the object
    @return whether object creation completed
    */
    bool get_msgpack_object(const std::size_t len)
    {
        if ((not sax->start_object(len)))
        {
            return false;
        }

        String key;
        for (std::size_t i = 0; i < len; ++i)
        {
            get();
            if ((not get_msgpack_string(key) or not sax->key(key)))
            {
                return false;
            }

            if ((not parse_msgpack_internal()))
            {
                return false;
            }
            key.clear();
        }

        return sax->end_object();
    }

    ////////////
    // UBJSON //
    ////////////

    /*!
    @param[in] get_char  whether a new character should be retrieved from the
                         input (true, default) or whether the last read
                         character should be considered instead

    @return whether a valid UBJSON value was passed to the SAX Parser
    */
    bool parse_ubjson_internal(const bool get_char = true)
    {
        return get_ubjson_value(get_char ? get_ignore_noop() : current);
    }


    bool get_ubjson_string(String& result, const bool get_char = true)
    {
        if (get_char)
        {
            get();  // TODO: may we ignore N here?
        }

        if ((not unexpect_eof(input_format_t::ubjson, "value")))
        {
            return false;
        }

        switch (current)
        {
            case 'U':
            {
                uint8_t len;
                return get_number(input_format_t::ubjson, len) and get_string(input_format_t::ubjson, len, result);
            }

            case 'i':
            {
                int8_t len;
                return get_number(input_format_t::ubjson, len) and get_string(input_format_t::ubjson, len, result);
            }

            case 'I':
            {
                int16_t len;
                return get_number(input_format_t::ubjson, len) and get_string(input_format_t::ubjson, len, result);
            }

            case 'l':
            {
                int32_t len;
                return get_number(input_format_t::ubjson, len) and get_string(input_format_t::ubjson, len, result);
            }

            case 'L':
            {
                int64_t len;
                return get_number(input_format_t::ubjson, len) and get_string(input_format_t::ubjson, len, result);
            }

            default:
                auto last_token = get_token_string();
                return sax->parse_error(chars_read, last_token, parse_error::create(113, chars_read, exception_message(input_format_t::ubjson, "expected length type specification (U, i, I, l, L); last byte: 0x" + last_token, "string")));
        }
    }

    /*!
    @param[out] result  determined size
    @return whether size determination completed
    */
    bool get_ubjson_size_value(std::size_t& result)
    {
        switch (get_ignore_noop())
        {
            case 'U':
            {
                uint8_t number;
                if ((not get_number(input_format_t::ubjson, number)))
                {
                    return false;
                }
                result = static_cast<std::size_t>(number);
                return true;
            }

            case 'i':
            {
                int8_t number;
                if ((not get_number(input_format_t::ubjson, number)))
                {
                    return false;
                }
                result = static_cast<std::size_t>(number);
                return true;
            }

            case 'I':
            {
                int16_t number;
                if ((not get_number(input_format_t::ubjson, number)))
                {
                    return false;
                }
                result = static_cast<std::size_t>(number);
                return true;
            }

            case 'l':
            {
                int32_t number;
                if ((not get_number(input_format_t::ubjson, number)))
                {
                    return false;
                }
                result = static_cast<std::size_t>(number);
                return true;
            }

            case 'L':
            {
                int64_t number;
                if ((not get_number(input_format_t::ubjson, number)))
                {
                    return false;
                }
                result = static_cast<std::size_t>(number);
                return true;
            }

            default:
            {
                auto last_token = get_token_string();
                return sax->parse_error(chars_read, last_token, parse_error::create(113, chars_read, exception_message(input_format_t::ubjson, "expected length type specification (U, i, I, l, L) after '#'; last byte: 0x" + last_token, "size")));
            }
        }
    }

    /*!
    @brief determine the type and size for a container

    In the optimized UBJSON format, a type and a size can be provided to allow
    for a more compact representation.

    @param[out] result  pair of the size and the type

    @return whether pair creation completed
    */
    bool get_ubjson_size_type(std::pair<std::size_t, int>& result)
    {
        result.first = String::npos; // size
        result.second = 0; // type

        get_ignore_noop();

        if (current == '$')
        {
            result.second = get();  // must not ignore 'N', because 'N' maybe the type
            if ((not unexpect_eof(input_format_t::ubjson, "type")))
            {
                return false;
            }

            get_ignore_noop();
            if ((current != '#'))
            {
                if ((not unexpect_eof(input_format_t::ubjson, "value")))
                {
                    return false;
                }
                auto last_token = get_token_string();
                return sax->parse_error(chars_read, last_token, parse_error::create(112, chars_read, exception_message(input_format_t::ubjson, "expected '#' after type information; last byte: 0x" + last_token, "size")));
            }

            return get_ubjson_size_value(result.first);
        }
        else if (current == '#')
        {
            return get_ubjson_size_value(result.first);
        }
        return true;
    }

    /*!
    @param prefix  the previously read or set type prefix
    @return whether value creation completed
    */
    bool get_ubjson_value(const int prefix)
    {
        switch (prefix)
        {
            case std::char_traits<char>::eof():  // EOF
                return unexpect_eof(input_format_t::ubjson, "value");

            case 'T':  // true
                return sax->boolean(true);
            case 'F':  // false
                return sax->boolean(false);

            case 'Z':  // null
                return sax->null();

            case 'U':
            {
                uint8_t number;
                return get_number(input_format_t::ubjson, number) and sax->num_unsigned(number);
            }

            case 'i':
            {
                int8_t number;
                return get_number(input_format_t::ubjson, number) and sax->num_integer(number);
            }

            case 'I':
            {
                int16_t number;
                return get_number(input_format_t::ubjson, number) and sax->num_integer(number);
            }

            case 'l':
            {
                int32_t number;
                return get_number(input_format_t::ubjson, number) and sax->num_integer(number);
            }

            case 'L':
            {
                int64_t number;
                return get_number(input_format_t::ubjson, number) and sax->num_integer(number);
            }

            case 'd':
            {
                float number;
                return get_number(input_format_t::ubjson, number) and sax->num_float(static_cast<Num_float>(number), "");
            }

            case 'D':
            {
                double number;
                return get_number(input_format_t::ubjson, number) and sax->num_float(static_cast<Num_float>(number), "");
            }

            case 'C':  // char
            {
                get();
                if ((not unexpect_eof(input_format_t::ubjson, "char")))
                {
                    return false;
                }
                if ((current > 127))
                {
                    auto last_token = get_token_string();
                    return sax->parse_error(chars_read, last_token, parse_error::create(113, chars_read, exception_message(input_format_t::ubjson, "byte after 'C' must be in range 0x00..0x7F; last byte: 0x" + last_token, "char")));
                }
                String s(1, static_cast<char>(current));
                return sax->string(s);
            }

            case 'S':  // string
            {
                String s;
                return get_ubjson_string(s) and sax->string(s);
            }

            case '[':  // array
                return get_ubjson_array();

            case '{':  // object
                return get_ubjson_object();

            default: // anything else
            {
                auto last_token = get_token_string();
                return sax->parse_error(chars_read, last_token, parse_error::create(112, chars_read, exception_message(input_format_t::ubjson, "invalid byte: 0x" + last_token, "value")));
            }
        }
    }

    /*!
    @return whether array creation completed
    */
    bool get_ubjson_array()
    {
        std::pair<std::size_t, int> size_and_type;
        if ((not get_ubjson_size_type(size_and_type)))
        {
            return false;
        }

        if (size_and_type.first != String::npos)
        {
            if ((not sax->start_array(size_and_type.first)))
            {
                return false;
            }

            if (size_and_type.second != 0)
            {
                if (size_and_type.second != 'N')
                {
                    for (std::size_t i = 0; i < size_and_type.first; ++i)
                    {
                        if ((not get_ubjson_value(size_and_type.second)))
                        {
                            return false;
                        }
                    }
                }
            }
            else
            {
                for (std::size_t i = 0; i < size_and_type.first; ++i)
                {
                    if ((not parse_ubjson_internal()))
                    {
                        return false;
                    }
                }
            }
        }
        else
        {
            if ((not sax->start_array(std::size_t(-1))))
            {
                return false;
            }

            while (current != ']')
            {
                if ((not parse_ubjson_internal(false)))
                {
                    return false;
                }
                get_ignore_noop();
            }
        }

        return sax->end_array();
    }

    /*!
    @return whether object creation completed
    */
    bool get_ubjson_object()
    {
        std::pair<std::size_t, int> size_and_type;
        if ((not get_ubjson_size_type(size_and_type)))
        {
            return false;
        }

        String key;
        if (size_and_type.first != String::npos)
        {
            if ((not sax->start_object(size_and_type.first)))
            {
                return false;
            }

            if (size_and_type.second != 0)
            {
                for (std::size_t i = 0; i < size_and_type.first; ++i)
                {
                    if ((not get_ubjson_string(key) or not sax->key(key)))
                    {
                        return false;
                    }
                    if ((not get_ubjson_value(size_and_type.second)))
                    {
                        return false;
                    }
                    key.clear();
                }
            }
            else
            {
                for (std::size_t i = 0; i < size_and_type.first; ++i)
                {
                    if ((not get_ubjson_string(key) or not sax->key(key)))
                    {
                        return false;
                    }
                    if ((not parse_ubjson_internal()))
                    {
                        return false;
                    }
                    key.clear();
                }
            }
        }
        else
        {
            if ((not sax->start_object(std::size_t(-1))))
            {
                return false;
            }

            while (current != '}')
            {
                if ((not get_ubjson_string(key, false) or not sax->key(key)))
                {
                    return false;
                }
                if ((not parse_ubjson_internal()))
                {
                    return false;
                }
                get_ignore_noop();
                key.clear();
            }
        }

        return sax->end_object();
    }

    ///////////////////////
    // Utility functions //
    ///////////////////////

    /*!
    @brief get next character from the input

    This function provides the interface to the used input adapter. It does
    not throw in case the input reached EOF, but returns a -'ve valued
    `std::char_traits<char>::eof()` in that case.

    @return character read from the input
    */
    int get()
    {
        ++chars_read;
        return (current = ia->get_character());
    }

    /*!
    @return character read from the input after ignoring all 'N' entries
    */
    int get_ignore_noop()
    {
        do
        {
            get();
        }
        while (current == 'N');

        return current;
    }

    /*
    @brief read a number from the input

    @tparam NumberType the type of the number
    @param[in] format   the current format (for diagnostics)
    @param[out] result  number of type @a NumberType

    @return whether conversion completed

    @note This function needs to respect the system's endianess, because
          bytes in CBOR, MessagePack, and UBJSON are stored in network order
          (big endian) and therefore need reordering on little endian systems.
    */
    template<typename NumberType, bool InputIsLittleEndian = false>
    bool get_number(const input_format_t format, NumberType& result)
    {
        // step 1: read input into array with system's byte order
        std::array<uint8_t, sizeof(NumberType)> vec;
        for (std::size_t i = 0; i < sizeof(NumberType); ++i)
        {
            get();
            if ((not unexpect_eof(format, "number")))
            {
                return false;
            }

            // reverse byte order prior to conversion if necessary
            if (is_little_endian && !InputIsLittleEndian)
            {
                vec[sizeof(NumberType) - i - 1] = static_cast<uint8_t>(current);
            }
            else
            {
                vec[i] = static_cast<uint8_t>(current); // LCOV_EXCL_LINE
            }
        }

        // step 2: convert array into number of type T and return
        std::memcpy(&result, vec.data(), sizeof(NumberType));
        return true;
    }

    /*!
    @brief create a string by reading characters from the input

    @tparam NumberType the type of the number
    @param[in] format the current format (for diagnostics)
    @param[in] len number of characters to read
    @param[out] result string created by reading @a len bytes

    @return whether string creation completed

    @note We can not reserve @a len bytes for the result, because @a len
          may be too large. Usually, @ref unexpect_eof() detects the end of
          the input before we run out of string memory.
    */
    template<typename NumberType>
    bool get_string(const input_format_t format,
                    const NumberType len,
                    String& result)
    {
        bool success = true;
        std::generate_n(std::back_inserter(result), len, [this, &success, &format]()
        {
            get();
            if ((not unexpect_eof(format, "string")))
            {
                success = false;
            }
            return static_cast<char>(current);
        });
        return success;
    }

    /*!
    @param[in] format   the current format (for diagnostics)
    @param[in] context  further context information (for diagnostics)
    @return whether the last read character is not EOF
    */
    bool unexpect_eof(const input_format_t format, const char* context) const
    {
        if ((current == std::char_traits<char>::eof()))
        {
            return sax->parse_error(chars_read, "<end of file>",
                                    parse_error::create(110, chars_read, exception_message(format, "unexpected end of input", context)));
        }
        return true;
    }

    /*!
    @return a string representation of the last read byte
    */
    std::string get_token_string() const
    {
        char cr[3];
        (std::snprintf)(cr, 3, "%.2hhX", static_cast<unsigned char>(current));
        return std::string{cr};
    }

    /*!
    @param[in] format   the current format
    @param[in] detail   a detailed error message
    @param[in] context  further contect information
    @return a message string to use in the parse_error exceptions
    */
    std::string exception_message(const input_format_t format,
                                  const std::string& detail,
                                  const std::string& context) const
    {
        std::string error_msg = "syntax error while parsing ";

        switch (format)
        {
            case input_format_t::cbor:
                error_msg += "CBOR";
                break;

            case input_format_t::msgpack:
                error_msg += "MessagePack";
                break;

            case input_format_t::ubjson:
                error_msg += "UBJSON";
                break;

            case input_format_t::bson:
                error_msg += "BSON";
                break;

            // LCOV_EXCL_START
            default:
                assert(false);
                // LCOV_EXCL_STOP
        }

        return error_msg + " " + context + ": " + detail;
    }

  private:
    /// input adapter
    input_adapter_t ia = nullptr;

    /// the current character
    int current = std::char_traits<char>::eof();

    /// the number of characters read
    std::size_t chars_read = 0;

    /// whether we can assume little endianess
    const bool is_little_endian = little_endianess();

    /// the SAX Parser
    json_sax_t* sax = nullptr;
};
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/output/Binary_writer.hpp>


#include <algorithm> // reverse
#include <array> // array
#include <cstdint> // uint8_t, uint16_t, uint32_t, uint64_t
#include <cstring> // memcpy
#include <limits> // numeric_limits

// #include <nlohmann/detail/input/Binary_reader.hpp>

// #include <nlohmann/detail/output/output_adapters.hpp>


namespace nlohmann
{
namespace detail
{
///////////////////
// binary writer //
///////////////////

/*!
@brief serialization to CBOR and MessagePack values
*/
template<typename Basic_json, typename CharType>
class Binary_writer
{
    using String = typename Basic_json::String;

  public:
    /*!
    @brief create a binary writer

    @param[in] adapter  output adapter to write to
    */
    explicit Binary_writer(output_adapter_t<CharType> adapter) : oa(adapter)
    {
        assert(oa);
    }

    /*!
    @param[in] j  JSON value to serialize
    @pre       j.type() == Value::object
    */
    void write_bson(const Basic_json& j)
    {
        switch (j.type())
        {
            case Value::object:
            {
                write_bson_object(*j.value_.object);
                break;
            }

            default:
            {
                JSON_THROW(type_error::create(317, "to serialize to BSON, top-level type must be object, but is " + std::string(j.type_name())));
            }
        }
    }

    /*!
    @param[in] j  JSON value to serialize
    */
    void write_cbor(const Basic_json& j)
    {
        switch (j.type())
        {
            case Value::null:
            {
                oa->write_character(to_char_type(0xF6));
                break;
            }

            case Value::boolean:
            {
                oa->write_character(j.value_.boolean
                                    ? to_char_type(0xF5)
                                    : to_char_type(0xF4));
                break;
            }

            case Value::num_integer:
            {
                if (j.value_.num_integer >= 0)
                {
                    // CBOR does not differentiate between positive signed
                    // integers and unsigned integers. Therefore, we used the
                    // code from the Value::num_unsigned case here.
                    if (j.value_.num_integer <= 0x17)
                    {
                        write_number(static_cast<uint8_t>(j.value_.num_integer));
                    }
                    else if (j.value_.num_integer <= (std::numeric_limits<uint8_t>::max)())
                    {
                        oa->write_character(to_char_type(0x18));
                        write_number(static_cast<uint8_t>(j.value_.num_integer));
                    }
                    else if (j.value_.num_integer <= (std::numeric_limits<uint16_t>::max)())
                    {
                        oa->write_character(to_char_type(0x19));
                        write_number(static_cast<uint16_t>(j.value_.num_integer));
                    }
                    else if (j.value_.num_integer <= (std::numeric_limits<uint32_t>::max)())
                    {
                        oa->write_character(to_char_type(0x1A));
                        write_number(static_cast<uint32_t>(j.value_.num_integer));
                    }
                    else
                    {
                        oa->write_character(to_char_type(0x1B));
                        write_number(static_cast<uint64_t>(j.value_.num_integer));
                    }
                }
                else
                {
                    // The conversions below encode the sign in the first
                    // byte, and the value is converted to a positive number.
                    const auto positive_number = -1 - j.value_.num_integer;
                    if (j.value_.num_integer >= -24)
                    {
                        write_number(static_cast<uint8_t>(0x20 + positive_number));
                    }
                    else if (positive_number <= (std::numeric_limits<uint8_t>::max)())
                    {
                        oa->write_character(to_char_type(0x38));
                        write_number(static_cast<uint8_t>(positive_number));
                    }
                    else if (positive_number <= (std::numeric_limits<uint16_t>::max)())
                    {
                        oa->write_character(to_char_type(0x39));
                        write_number(static_cast<uint16_t>(positive_number));
                    }
                    else if (positive_number <= (std::numeric_limits<uint32_t>::max)())
                    {
                        oa->write_character(to_char_type(0x3A));
                        write_number(static_cast<uint32_t>(positive_number));
                    }
                    else
                    {
                        oa->write_character(to_char_type(0x3B));
                        write_number(static_cast<uint64_t>(positive_number));
                    }
                }
                break;
            }

            case Value::num_unsigned:
            {
                if (j.value_.num_unsigned <= 0x17)
                {
                    write_number(static_cast<uint8_t>(j.value_.num_unsigned));
                }
                else if (j.value_.num_unsigned <= (std::numeric_limits<uint8_t>::max)())
                {
                    oa->write_character(to_char_type(0x18));
                    write_number(static_cast<uint8_t>(j.value_.num_unsigned));
                }
                else if (j.value_.num_unsigned <= (std::numeric_limits<uint16_t>::max)())
                {
                    oa->write_character(to_char_type(0x19));
                    write_number(static_cast<uint16_t>(j.value_.num_unsigned));
                }
                else if (j.value_.num_unsigned <= (std::numeric_limits<uint32_t>::max)())
                {
                    oa->write_character(to_char_type(0x1A));
                    write_number(static_cast<uint32_t>(j.value_.num_unsigned));
                }
                else
                {
                    oa->write_character(to_char_type(0x1B));
                    write_number(static_cast<uint64_t>(j.value_.num_unsigned));
                }
                break;
            }

            case Value::num_float:
            {
                oa->write_character(get_cbor_float_prefix(j.value_.num_float));
                write_number(j.value_.num_float);
                break;
            }

            case Value::string:
            {
                // step 1: write control byte and the string length
                const auto N = j.value_.string->size();
                if (N <= 0x17)
                {
                    write_number(static_cast<uint8_t>(0x60 + N));
                }
                else if (N <= (std::numeric_limits<uint8_t>::max)())
                {
                    oa->write_character(to_char_type(0x78));
                    write_number(static_cast<uint8_t>(N));
                }
                else if (N <= (std::numeric_limits<uint16_t>::max)())
                {
                    oa->write_character(to_char_type(0x79));
                    write_number(static_cast<uint16_t>(N));
                }
                else if (N <= (std::numeric_limits<uint32_t>::max)())
                {
                    oa->write_character(to_char_type(0x7A));
                    write_number(static_cast<uint32_t>(N));
                }
                // LCOV_EXCL_START
                else if (N <= (std::numeric_limits<uint64_t>::max)())
                {
                    oa->write_character(to_char_type(0x7B));
                    write_number(static_cast<uint64_t>(N));
                }
                // LCOV_EXCL_STOP

                // step 2: write the string
                oa->write_characters(
                    reinterpret_cast<const CharType*>(j.value_.string->c_str()),
                    j.value_.string->size());
                break;
            }

            case Value::array:
            {
                // step 1: write control byte and the array size
                const auto N = j.value_.array->size();
                if (N <= 0x17)
                {
                    write_number(static_cast<uint8_t>(0x80 + N));
                }
                else if (N <= (std::numeric_limits<uint8_t>::max)())
                {
                    oa->write_character(to_char_type(0x98));
                    write_number(static_cast<uint8_t>(N));
                }
                else if (N <= (std::numeric_limits<uint16_t>::max)())
                {
                    oa->write_character(to_char_type(0x99));
                    write_number(static_cast<uint16_t>(N));
                }
                else if (N <= (std::numeric_limits<uint32_t>::max)())
                {
                    oa->write_character(to_char_type(0x9A));
                    write_number(static_cast<uint32_t>(N));
                }
                // LCOV_EXCL_START
                else if (N <= (std::numeric_limits<uint64_t>::max)())
                {
                    oa->write_character(to_char_type(0x9B));
                    write_number(static_cast<uint64_t>(N));
                }
                // LCOV_EXCL_STOP

                // step 2: write each element
                for (const auto& el : *j.value_.array)
                {
                    write_cbor(el);
                }
                break;
            }

            case Value::object:
            {
                // step 1: write control byte and the object size
                const auto N = j.value_.object->size();
                if (N <= 0x17)
                {
                    write_number(static_cast<uint8_t>(0xA0 + N));
                }
                else if (N <= (std::numeric_limits<uint8_t>::max)())
                {
                    oa->write_character(to_char_type(0xB8));
                    write_number(static_cast<uint8_t>(N));
                }
                else if (N <= (std::numeric_limits<uint16_t>::max)())
                {
                    oa->write_character(to_char_type(0xB9));
                    write_number(static_cast<uint16_t>(N));
                }
                else if (N <= (std::numeric_limits<uint32_t>::max)())
                {
                    oa->write_character(to_char_type(0xBA));
                    write_number(static_cast<uint32_t>(N));
                }
                // LCOV_EXCL_START
                else if (N <= (std::numeric_limits<uint64_t>::max)())
                {
                    oa->write_character(to_char_type(0xBB));
                    write_number(static_cast<uint64_t>(N));
                }
                // LCOV_EXCL_STOP

                // step 2: write each element
                for (const auto& el : *j.value_.object)
                {
                    write_cbor(el.first);
                    write_cbor(el.second);
                }
                break;
            }

            default:
                break;
        }
    }

    /*!
    @param[in] j  JSON value to serialize
    */
    void write_msgpack(const Basic_json& j)
    {
        switch (j.type())
        {
            case Value::null: // nil
            {
                oa->write_character(to_char_type(0xC0));
                break;
            }

            case Value::boolean: // true and false
            {
                oa->write_character(j.value_.boolean
                                    ? to_char_type(0xC3)
                                    : to_char_type(0xC2));
                break;
            }

            case Value::num_integer:
            {
                if (j.value_.num_integer >= 0)
                {
                    // MessagePack does not differentiate between positive
                    // signed integers and unsigned integers. Therefore, we used
                    // the code from the Value::num_unsigned case here.
                    if (j.value_.num_unsigned < 128)
                    {
                        // positive fixnum
                        write_number(static_cast<uint8_t>(j.value_.num_integer));
                    }
                    else if (j.value_.num_unsigned <= (std::numeric_limits<uint8_t>::max)())
                    {
                        // uint 8
                        oa->write_character(to_char_type(0xCC));
                        write_number(static_cast<uint8_t>(j.value_.num_integer));
                    }
                    else if (j.value_.num_unsigned <= (std::numeric_limits<uint16_t>::max)())
                    {
                        // uint 16
                        oa->write_character(to_char_type(0xCD));
                        write_number(static_cast<uint16_t>(j.value_.num_integer));
                    }
                    else if (j.value_.num_unsigned <= (std::numeric_limits<uint32_t>::max)())
                    {
                        // uint 32
                        oa->write_character(to_char_type(0xCE));
                        write_number(static_cast<uint32_t>(j.value_.num_integer));
                    }
                    else if (j.value_.num_unsigned <= (std::numeric_limits<uint64_t>::max)())
                    {
                        // uint 64
                        oa->write_character(to_char_type(0xCF));
                        write_number(static_cast<uint64_t>(j.value_.num_integer));
                    }
                }
                else
                {
                    if (j.value_.num_integer >= -32)
                    {
                        // negative fixnum
                        write_number(static_cast<int8_t>(j.value_.num_integer));
                    }
                    else if (j.value_.num_integer >= (std::numeric_limits<int8_t>::min)() and
                             j.value_.num_integer <= (std::numeric_limits<int8_t>::max)())
                    {
                        // int 8
                        oa->write_character(to_char_type(0xD0));
                        write_number(static_cast<int8_t>(j.value_.num_integer));
                    }
                    else if (j.value_.num_integer >= (std::numeric_limits<int16_t>::min)() and
                             j.value_.num_integer <= (std::numeric_limits<int16_t>::max)())
                    {
                        // int 16
                        oa->write_character(to_char_type(0xD1));
                        write_number(static_cast<int16_t>(j.value_.num_integer));
                    }
                    else if (j.value_.num_integer >= (std::numeric_limits<int32_t>::min)() and
                             j.value_.num_integer <= (std::numeric_limits<int32_t>::max)())
                    {
                        // int 32
                        oa->write_character(to_char_type(0xD2));
                        write_number(static_cast<int32_t>(j.value_.num_integer));
                    }
                    else if (j.value_.num_integer >= (std::numeric_limits<int64_t>::min)() and
                             j.value_.num_integer <= (std::numeric_limits<int64_t>::max)())
                    {
                        // int 64
                        oa->write_character(to_char_type(0xD3));
                        write_number(static_cast<int64_t>(j.value_.num_integer));
                    }
                }
                break;
            }

            case Value::num_unsigned:
            {
                if (j.value_.num_unsigned < 128)
                {
                    // positive fixnum
                    write_number(static_cast<uint8_t>(j.value_.num_integer));
                }
                else if (j.value_.num_unsigned <= (std::numeric_limits<uint8_t>::max)())
                {
                    // uint 8
                    oa->write_character(to_char_type(0xCC));
                    write_number(static_cast<uint8_t>(j.value_.num_integer));
                }
                else if (j.value_.num_unsigned <= (std::numeric_limits<uint16_t>::max)())
                {
                    // uint 16
                    oa->write_character(to_char_type(0xCD));
                    write_number(static_cast<uint16_t>(j.value_.num_integer));
                }
                else if (j.value_.num_unsigned <= (std::numeric_limits<uint32_t>::max)())
                {
                    // uint 32
                    oa->write_character(to_char_type(0xCE));
                    write_number(static_cast<uint32_t>(j.value_.num_integer));
                }
                else if (j.value_.num_unsigned <= (std::numeric_limits<uint64_t>::max)())
                {
                    // uint 64
                    oa->write_character(to_char_type(0xCF));
                    write_number(static_cast<uint64_t>(j.value_.num_integer));
                }
                break;
            }

            case Value::num_float:
            {
                oa->write_character(get_msgpack_float_prefix(j.value_.num_float));
                write_number(j.value_.num_float);
                break;
            }

            case Value::string:
            {
                // step 1: write control byte and the string length
                const auto N = j.value_.string->size();
                if (N <= 31)
                {
                    // fixstr
                    write_number(static_cast<uint8_t>(0xA0 | N));
                }
                else if (N <= (std::numeric_limits<uint8_t>::max)())
                {
                    // str 8
                    oa->write_character(to_char_type(0xD9));
                    write_number(static_cast<uint8_t>(N));
                }
                else if (N <= (std::numeric_limits<uint16_t>::max)())
                {
                    // str 16
                    oa->write_character(to_char_type(0xDA));
                    write_number(static_cast<uint16_t>(N));
                }
                else if (N <= (std::numeric_limits<uint32_t>::max)())
                {
                    // str 32
                    oa->write_character(to_char_type(0xDB));
                    write_number(static_cast<uint32_t>(N));
                }

                // step 2: write the string
                oa->write_characters(
                    reinterpret_cast<const CharType*>(j.value_.string->c_str()),
                    j.value_.string->size());
                break;
            }

            case Value::array:
            {
                // step 1: write control byte and the array size
                const auto N = j.value_.array->size();
                if (N <= 15)
                {
                    // fixarray
                    write_number(static_cast<uint8_t>(0x90 | N));
                }
                else if (N <= (std::numeric_limits<uint16_t>::max)())
                {
                    // array 16
                    oa->write_character(to_char_type(0xDC));
                    write_number(static_cast<uint16_t>(N));
                }
                else if (N <= (std::numeric_limits<uint32_t>::max)())
                {
                    // array 32
                    oa->write_character(to_char_type(0xDD));
                    write_number(static_cast<uint32_t>(N));
                }

                // step 2: write each element
                for (const auto& el : *j.value_.array)
                {
                    write_msgpack(el);
                }
                break;
            }

            case Value::object:
            {
                // step 1: write control byte and the object size
                const auto N = j.value_.object->size();
                if (N <= 15)
                {
                    // fixmap
                    write_number(static_cast<uint8_t>(0x80 | (N & 0xF)));
                }
                else if (N <= (std::numeric_limits<uint16_t>::max)())
                {
                    // map 16
                    oa->write_character(to_char_type(0xDE));
                    write_number(static_cast<uint16_t>(N));
                }
                else if (N <= (std::numeric_limits<uint32_t>::max)())
                {
                    // map 32
                    oa->write_character(to_char_type(0xDF));
                    write_number(static_cast<uint32_t>(N));
                }

                // step 2: write each element
                for (const auto& el : *j.value_.object)
                {
                    write_msgpack(el.first);
                    write_msgpack(el.second);
                }
                break;
            }

            default:
                break;
        }
    }

    /*!
    @param[in] j  JSON value to serialize
    @param[in] use_count   whether to use '#' prefixes (optimized format)
    @param[in] use_type    whether to use '$' prefixes (optimized format)
    @param[in] add_prefix  whether prefixes need to be used for this value
    */
    void write_ubjson(const Basic_json& j, const bool use_count,
                      const bool use_type, const bool add_prefix = true)
    {
        switch (j.type())
        {
            case Value::null:
            {
                if (add_prefix)
                {
                    oa->write_character(to_char_type('Z'));
                }
                break;
            }

            case Value::boolean:
            {
                if (add_prefix)
                {
                    oa->write_character(j.value_.boolean
                                        ? to_char_type('T')
                                        : to_char_type('F'));
                }
                break;
            }

            case Value::num_integer:
            {
                write_number_with_ubjson_prefix(j.value_.num_integer, add_prefix);
                break;
            }

            case Value::num_unsigned:
            {
                write_number_with_ubjson_prefix(j.value_.num_unsigned, add_prefix);
                break;
            }

            case Value::num_float:
            {
                write_number_with_ubjson_prefix(j.value_.num_float, add_prefix);
                break;
            }

            case Value::string:
            {
                if (add_prefix)
                {
                    oa->write_character(to_char_type('S'));
                }
                write_number_with_ubjson_prefix(j.value_.string->size(), true);
                oa->write_characters(
                    reinterpret_cast<const CharType*>(j.value_.string->c_str()),
                    j.value_.string->size());
                break;
            }

            case Value::array:
            {
                if (add_prefix)
                {
                    oa->write_character(to_char_type('['));
                }

                bool prefix_required = true;
                if (use_type and not j.value_.array->empty())
                {
                    assert(use_count);
                    const CharType first_prefix = ubjson_prefix(j.front());
                    const bool same_prefix = std::all_of(j.begin() + 1, j.end(),
                                                         [this, first_prefix](const Basic_json & v)
                    {
                        return ubjson_prefix(v) == first_prefix;
                    });

                    if (same_prefix)
                    {
                        prefix_required = false;
                        oa->write_character(to_char_type('$'));
                        oa->write_character(first_prefix);
                    }
                }

                if (use_count)
                {
                    oa->write_character(to_char_type('#'));
                    write_number_with_ubjson_prefix(j.value_.array->size(), true);
                }

                for (const auto& el : *j.value_.array)
                {
                    write_ubjson(el, use_count, use_type, prefix_required);
                }

                if (not use_count)
                {
                    oa->write_character(to_char_type(']'));
                }

                break;
            }

            case Value::object:
            {
                if (add_prefix)
                {
                    oa->write_character(to_char_type('{'));
                }

                bool prefix_required = true;
                if (use_type and not j.value_.object->empty())
                {
                    assert(use_count);
                    const CharType first_prefix = ubjson_prefix(j.front());
                    const bool same_prefix = std::all_of(j.begin(), j.end(),
                                                         [this, first_prefix](const Basic_json & v)
                    {
                        return ubjson_prefix(v) == first_prefix;
                    });

                    if (same_prefix)
                    {
                        prefix_required = false;
                        oa->write_character(to_char_type('$'));
                        oa->write_character(first_prefix);
                    }
                }

                if (use_count)
                {
                    oa->write_character(to_char_type('#'));
                    write_number_with_ubjson_prefix(j.value_.object->size(), true);
                }

                for (const auto& el : *j.value_.object)
                {
                    write_number_with_ubjson_prefix(el.first.size(), true);
                    oa->write_characters(
                        reinterpret_cast<const CharType*>(el.first.c_str()),
                        el.first.size());
                    write_ubjson(el.second, use_count, use_type, prefix_required);
                }

                if (not use_count)
                {
                    oa->write_character(to_char_type('}'));
                }

                break;
            }

            default:
                break;
        }
    }

  private:
    //////////
    // BSON //
    //////////

    /*!
    @return The size of a BSON document entry header, including the id marker
            and the entry name size (and its null-terminator).
    */
    static std::size_t calc_bson_entry_header_size(const String& name)
    {
        const auto it = name.find(static_cast<typename String::value_type>(0));
        if ((it != Basic_json::String::npos))
        {
            JSON_THROW(out_of_range::create(409,
                                            "BSON key cannot contain code point U+0000 (at byte " + std::to_string(it) + ")"));
        }

        return /*id*/ 1ul + name.size() + /*zero-terminator*/1u;
    }

    /*!
    @brief Writes the given @a element_type and @a name to the output adapter
    */
    void write_bson_entry_header(const String& name,
                                 const std::uint8_t element_type)
    {
        oa->write_character(to_char_type(element_type)); // boolean
        oa->write_characters(
            reinterpret_cast<const CharType*>(name.c_str()),
            name.size() + 1u);
    }

    /*!
    @brief Writes a BSON element with key @a name and boolean value @a value
    */
    void write_bson_boolean(const String& name,
                            const bool value)
    {
        write_bson_entry_header(name, 0x08);
        oa->write_character(value ? to_char_type(0x01) : to_char_type(0x00));
    }

    /*!
    @brief Writes a BSON element with key @a name and double value @a value
    */
    void write_bson_double(const String& name,
                           const double value)
    {
        write_bson_entry_header(name, 0x01);
        write_number<double, true>(value);
    }

    /*!
    @return The size of the BSON-encoded string in @a value
    */
    static std::size_t calc_bson_string_size(const String& value)
    {
        return sizeof(std::int32_t) + value.size() + 1ul;
    }

    /*!
    @brief Writes a BSON element with key @a name and string value @a value
    */
    void write_bson_string(const String& name,
                           const String& value)
    {
        write_bson_entry_header(name, 0x02);

        write_number<std::int32_t, true>(static_cast<std::int32_t>(value.size() + 1ul));
        oa->write_characters(
            reinterpret_cast<const CharType*>(value.c_str()),
            value.size() + 1);
    }

    /*!
    @brief Writes a BSON element with key @a name and null value
    */
    void write_bson_null(const String& name)
    {
        write_bson_entry_header(name, 0x0A);
    }

    /*!
    @return The size of the BSON-encoded integer @a value
    */
    static std::size_t calc_bson_integer_size(const std::int64_t value)
    {
        if ((std::numeric_limits<std::int32_t>::min)() <= value and value <= (std::numeric_limits<std::int32_t>::max)())
        {
            return sizeof(std::int32_t);
        }
        else
        {
            return sizeof(std::int64_t);
        }
    }

    /*!
    @brief Writes a BSON element with key @a name and integer @a value
    */
    void write_bson_integer(const String& name,
                            const std::int64_t value)
    {
        if ((std::numeric_limits<std::int32_t>::min)() <= value and value <= (std::numeric_limits<std::int32_t>::max)())
        {
            write_bson_entry_header(name, 0x10); // int32
            write_number<std::int32_t, true>(static_cast<std::int32_t>(value));
        }
        else
        {
            write_bson_entry_header(name, 0x12); // int64
            write_number<std::int64_t, true>(static_cast<std::int64_t>(value));
        }
    }

    /*!
    @return The size of the BSON-encoded unsigned integer in @a j
    */
    static constexpr std::size_t calc_bson_unsigned_size(const std::uint64_t value) 
    {
        return (value <= static_cast<std::uint64_t>((std::numeric_limits<std::int32_t>::max)()))
               ? sizeof(std::int32_t)
               : sizeof(std::int64_t);
    }

    /*!
    @brief Writes a BSON element with key @a name and unsigned @a value
    */
    void write_bson_unsigned(const String& name,
                             const std::uint64_t value)
    {
        if (value <= static_cast<std::uint64_t>((std::numeric_limits<std::int32_t>::max)()))
        {
            write_bson_entry_header(name, 0x10 /* int32 */);
            write_number<std::int32_t, true>(static_cast<std::int32_t>(value));
        }
        else if (value <= static_cast<std::uint64_t>((std::numeric_limits<std::int64_t>::max)()))
        {
            write_bson_entry_header(name, 0x12 /* int64 */);
            write_number<std::int64_t, true>(static_cast<std::int64_t>(value));
        }
        else
        {
            JSON_THROW(out_of_range::create(407, "integer number " + std::to_string(value) + " cannot be represented by BSON as it does not fit int64"));
        }
    }

    /*!
    @brief Writes a BSON element with key @a name and object @a value
    */
    void write_bson_object_entry(const String& name,
                                 const typename Basic_json::Object& value)
    {
        write_bson_entry_header(name, 0x03); // object
        write_bson_object(value);
    }

    /*!
    @return The size of the BSON-encoded array @a value
    */
    static std::size_t calc_bson_array_size(const typename Basic_json::Array& value)
    {
        std::size_t embedded_document_size = 0ul;
        std::size_t array_index = 0ul;

        for (const auto& el : value)
        {
            embedded_document_size += calc_bson_element_size(std::to_string(array_index++), el);
        }

        return sizeof(std::int32_t) + embedded_document_size + 1ul;
    }

    /*!
    @brief Writes a BSON element with key @a name and array @a value
    */
    void write_bson_array(const String& name,
                          const typename Basic_json::Array& value)
    {
        write_bson_entry_header(name, 0x04); // array
        write_number<std::int32_t, true>(static_cast<std::int32_t>(calc_bson_array_size(value)));

        std::size_t array_index = 0ul;

        for (const auto& el : value)
        {
            write_bson_element(std::to_string(array_index++), el);
        }

        oa->write_character(to_char_type(0x00));
    }

    /*!
    @brief Calculates the size necessary to serialize the JSON value @a j with its @a name
    @return The calculated size for the BSON document entry for @a j with the given @a name.
    */
    static std::size_t calc_bson_element_size(const String& name,
            const Basic_json& j)
    {
        const auto header_size = calc_bson_entry_header_size(name);
        switch (j.type())
        {
            case Value::object:
                return header_size + calc_bson_object_size(*j.value_.object);

            case Value::array:
                return header_size + calc_bson_array_size(*j.value_.array);

            case Value::boolean:
                return header_size + 1ul;

            case Value::num_float:
                return header_size + 8ul;

            case Value::num_integer:
                return header_size + calc_bson_integer_size(j.value_.num_integer);

            case Value::num_unsigned:
                return header_size + calc_bson_unsigned_size(j.value_.num_unsigned);

            case Value::string:
                return header_size + calc_bson_string_size(*j.value_.string);

            case Value::null:
                return header_size + 0ul;

            // LCOV_EXCL_START
            default:
                assert(false);
                return 0ul;
                // LCOV_EXCL_STOP
        };
    }

    /*!
    @brief Serializes the JSON value @a j to BSON and associates it with the
           key @a name.
    @param name The name to associate with the JSON entity @a j within the
                current BSON document
    @return The size of the BSON entry
    */
    void write_bson_element(const String& name,
                            const Basic_json& j)
    {
        switch (j.type())
        {
            case Value::object:
                return write_bson_object_entry(name, *j.value_.object);

            case Value::array:
                return write_bson_array(name, *j.value_.array);

            case Value::boolean:
                return write_bson_boolean(name, j.value_.boolean);

            case Value::num_float:
                return write_bson_double(name, j.value_.num_float);

            case Value::num_integer:
                return write_bson_integer(name, j.value_.num_integer);

            case Value::num_unsigned:
                return write_bson_unsigned(name, j.value_.num_unsigned);

            case Value::string:
                return write_bson_string(name, *j.value_.string);

            case Value::null:
                return write_bson_null(name);

            // LCOV_EXCL_START
            default:
                assert(false);
                return;
                // LCOV_EXCL_STOP
        };
    }

    /*!
    @brief Calculates the size of the BSON serialization of the given
           JSON-object @a j.
    @param[in] j  JSON value to serialize
    @pre       j.type() == Value::object
    */
    static std::size_t calc_bson_object_size(const typename Basic_json::Object& value)
    {
        std::size_t document_size = std::accumulate(value.begin(), value.end(), 0ul,
                                    [](size_t result, const typename Basic_json::Object::value_type & el)
        {
            return result += calc_bson_element_size(el.first, el.second);
        });

        return sizeof(std::int32_t) + document_size + 1ul;
    }

    /*!
    @param[in] j  JSON value to serialize
    @pre       j.type() == Value::object
    */
    void write_bson_object(const typename Basic_json::Object& value)
    {
        write_number<std::int32_t, true>(static_cast<std::int32_t>(calc_bson_object_size(value)));

        for (const auto& el : value)
        {
            write_bson_element(el.first, el.second);
        }

        oa->write_character(to_char_type(0x00));
    }

    //////////
    // CBOR //
    //////////

    static constexpr CharType get_cbor_float_prefix(float /*unused*/)
    {
        return to_char_type(0xFA);  // Single-Precision Float
    }

    static constexpr CharType get_cbor_float_prefix(double /*unused*/)
    {
        return to_char_type(0xFB);  // Double-Precision Float
    }

    /////////////
    // MsgPack //
    /////////////

    static constexpr CharType get_msgpack_float_prefix(float /*unused*/)
    {
        return to_char_type(0xCA);  // float 32
    }

    static constexpr CharType get_msgpack_float_prefix(double /*unused*/)
    {
        return to_char_type(0xCB);  // float 64
    }

    ////////////
    // UBJSON //
    ////////////

    // UBJSON: write number (floating point)
    template<typename NumberType, typename std::enable_if<
                 std::is_floating_point<NumberType>::value, int>::type = 0>
    void write_number_with_ubjson_prefix(const NumberType n,
                                         const bool add_prefix)
    {
        if (add_prefix)
        {
            oa->write_character(get_ubjson_float_prefix(n));
        }
        write_number(n);
    }

    // UBJSON: write number (unsigned integer)
    template<typename NumberType, typename std::enable_if<
                 std::is_unsigned<NumberType>::value, int>::type = 0>
    void write_number_with_ubjson_prefix(const NumberType n,
                                         const bool add_prefix)
    {
        if (n <= static_cast<uint64_t>((std::numeric_limits<int8_t>::max)()))
        {
            if (add_prefix)
            {
                oa->write_character(to_char_type('i'));  // int8
            }
            write_number(static_cast<uint8_t>(n));
        }
        else if (n <= (std::numeric_limits<uint8_t>::max)())
        {
            if (add_prefix)
            {
                oa->write_character(to_char_type('U'));  // uint8
            }
            write_number(static_cast<uint8_t>(n));
        }
        else if (n <= static_cast<uint64_t>((std::numeric_limits<int16_t>::max)()))
        {
            if (add_prefix)
            {
                oa->write_character(to_char_type('I'));  // int16
            }
            write_number(static_cast<int16_t>(n));
        }
        else if (n <= static_cast<uint64_t>((std::numeric_limits<int32_t>::max)()))
        {
            if (add_prefix)
            {
                oa->write_character(to_char_type('l'));  // int32
            }
            write_number(static_cast<int32_t>(n));
        }
        else if (n <= static_cast<uint64_t>((std::numeric_limits<int64_t>::max)()))
        {
            if (add_prefix)
            {
                oa->write_character(to_char_type('L'));  // int64
            }
            write_number(static_cast<int64_t>(n));
        }
        else
        {
            JSON_THROW(out_of_range::create(407, "integer number " + std::to_string(n) + " cannot be represented by UBJSON as it does not fit int64"));
        }
    }

    // UBJSON: write number (signed integer)
    template<typename NumberType, typename std::enable_if<
                 std::is_signed<NumberType>::value and
                 not std::is_floating_point<NumberType>::value, int>::type = 0>
    void write_number_with_ubjson_prefix(const NumberType n,
                                         const bool add_prefix)
    {
        if ((std::numeric_limits<int8_t>::min)() <= n and n <= (std::numeric_limits<int8_t>::max)())
        {
            if (add_prefix)
            {
                oa->write_character(to_char_type('i'));  // int8
            }
            write_number(static_cast<int8_t>(n));
        }
        else if (static_cast<int64_t>((std::numeric_limits<uint8_t>::min)()) <= n and n <= static_cast<int64_t>((std::numeric_limits<uint8_t>::max)()))
        {
            if (add_prefix)
            {
                oa->write_character(to_char_type('U'));  // uint8
            }
            write_number(static_cast<uint8_t>(n));
        }
        else if ((std::numeric_limits<int16_t>::min)() <= n and n <= (std::numeric_limits<int16_t>::max)())
        {
            if (add_prefix)
            {
                oa->write_character(to_char_type('I'));  // int16
            }
            write_number(static_cast<int16_t>(n));
        }
        else if ((std::numeric_limits<int32_t>::min)() <= n and n <= (std::numeric_limits<int32_t>::max)())
        {
            if (add_prefix)
            {
                oa->write_character(to_char_type('l'));  // int32
            }
            write_number(static_cast<int32_t>(n));
        }
        else if ((std::numeric_limits<int64_t>::min)() <= n and n <= (std::numeric_limits<int64_t>::max)())
        {
            if (add_prefix)
            {
                oa->write_character(to_char_type('L'));  // int64
            }
            write_number(static_cast<int64_t>(n));
        }
        // LCOV_EXCL_START
        else
        {
            JSON_THROW(out_of_range::create(407, "integer number " + std::to_string(n) + " cannot be represented by UBJSON as it does not fit int64"));
        }
        // LCOV_EXCL_STOP
    }

    /*!
    @brief determine the type prefix of container values

    @note This function does not need to be 100% accurate when it comes to
          integer limits. In case a number exceeds the limits of int64_t,
          this will be detected by a later call to function
          write_number_with_ubjson_prefix. Therefore, we return 'L' for any
          value that does not fit the previous limits.
    */
    CharType ubjson_prefix(const Basic_json& j) const 
    {
        switch (j.type())
        {
            case Value::null:
                return 'Z';

            case Value::boolean:
                return j.value_.boolean ? 'T' : 'F';

            case Value::num_integer:
            {
                if ((std::numeric_limits<int8_t>::min)() <= j.value_.num_integer and j.value_.num_integer <= (std::numeric_limits<int8_t>::max)())
                {
                    return 'i';
                }
                if ((std::numeric_limits<uint8_t>::min)() <= j.value_.num_integer and j.value_.num_integer <= (std::numeric_limits<uint8_t>::max)())
                {
                    return 'U';
                }
                if ((std::numeric_limits<int16_t>::min)() <= j.value_.num_integer and j.value_.num_integer <= (std::numeric_limits<int16_t>::max)())
                {
                    return 'I';
                }
                if ((std::numeric_limits<int32_t>::min)() <= j.value_.num_integer and j.value_.num_integer <= (std::numeric_limits<int32_t>::max)())
                {
                    return 'l';
                }
                // no check and assume int64_t (see note above)
                return 'L';
            }

            case Value::num_unsigned:
            {
                if (j.value_.num_unsigned <= (std::numeric_limits<int8_t>::max)())
                {
                    return 'i';
                }
                if (j.value_.num_unsigned <= (std::numeric_limits<uint8_t>::max)())
                {
                    return 'U';
                }
                if (j.value_.num_unsigned <= (std::numeric_limits<int16_t>::max)())
                {
                    return 'I';
                }
                if (j.value_.num_unsigned <= (std::numeric_limits<int32_t>::max)())
                {
                    return 'l';
                }
                // no check and assume int64_t (see note above)
                return 'L';
            }

            case Value::num_float:
                return get_ubjson_float_prefix(j.value_.num_float);

            case Value::string:
                return 'S';

            case Value::array:
                return '[';

            case Value::object:
                return '{';

            default:  // discarded values
                return 'N';
        }
    }

    static constexpr CharType get_ubjson_float_prefix(float /*unused*/)
    {
        return 'd';  // float 32
    }

    static constexpr CharType get_ubjson_float_prefix(double /*unused*/)
    {
        return 'D';  // float 64
    }


    template<typename NumberType, bool OutputIsLittleEndian = false>
    void write_number(const NumberType n)
    {
        // step 1: write number to array of length NumberType
        std::array<CharType, sizeof(NumberType)> vec;
        std::memcpy(vec.data(), &n, sizeof(NumberType));

        // step 2: write array to output (with possible reordering)
        if (is_little_endian and not OutputIsLittleEndian)
        {
            // reverse byte order prior to conversion if necessary
            std::reverse(vec.begin(), vec.end());
        }

        oa->write_characters(vec.data(), sizeof(NumberType));
    }

  public:
    // The following to_char_type functions are implement the conversion
    // between uint8_t and CharType. In case CharType is not unsigned,
    // such a conversion is required to allow values greater than 128.
    // See <https://github.com/nlohmann/json/issues/1286> for a discussion.
    template < typename C = CharType,
               enable_if_t < std::is_signed<C>::value and std::is_signed<char>::value > * = nullptr >
    static constexpr CharType to_char_type(std::uint8_t x) 
    {
        return *reinterpret_cast<char*>(&x);
    }

    template < typename C = CharType,
               enable_if_t < std::is_signed<C>::value and std::is_unsigned<char>::value > * = nullptr >
    static CharType to_char_type(std::uint8_t x) 
    {
        static_assert(sizeof(std::uint8_t) == sizeof(CharType), "size of CharType must be equal to std::uint8_t");
        static_assert(std::is_pod<CharType>::value, "CharType must be POD");
        CharType result;
        std::memcpy(&result, &x, sizeof(x));
        return result;
    }

    template<typename C = CharType,
             enable_if_t<std::is_unsigned<C>::value>* = nullptr>
    static constexpr CharType to_char_type(std::uint8_t x) 
    {
        return x;
    }

    template < typename InputCharType, typename C = CharType,
               enable_if_t <
                   std::is_signed<C>::value and
                   std::is_signed<char>::value and
                   std::is_same<char, typename std::remove_cv<InputCharType>::type>::value
                   > * = nullptr >
    static constexpr CharType to_char_type(InputCharType x) 
    {
        return x;
    }

  private:
    /// whether we can assume little endianess
    const bool is_little_endian = Binary_reader<Basic_json>::little_endianess();

    /// the output
    output_adapter_t<CharType> oa = nullptr;
};
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/output/serializer.hpp>


#include <algorithm> // reverse, remove, fill, find, none_of
#include <array> // array
#include <cassert> // assert
#include <ciso646> // and, or
#include <clocale> // localeconv, lconv
#include <cmath> // labs, isfinite, isnan, signbit
#include <cstddef> // size_t, ptrdiff_t
#include <cstdint> // uint8_t
#include <cstdio> // snprintf
#include <limits> // numeric_limits
#include <string> // string
#include <type_traits> // is_same

// #include <nlohmann/detail/exceptions.hpp>

// #include <nlohmann/detail/conversions/to_chars.hpp>


#include <cassert> // assert
#include <ciso646> // or, and, not
#include <cmath>   // signbit, isfinite
#include <cstdint> // intN_t, uintN_t
#include <cstring> // memcpy, memmove

namespace nlohmann
{
namespace detail
{


namespace dtoa_impl
{

template <typename Target, typename Source>
Target reinterpret_bits(const Source source)
{
    static_assert(sizeof(Target) == sizeof(Source), "size mismatch");

    Target target;
    std::memcpy(&target, &source, sizeof(Source));
    return target;
}

struct diyfp // f * 2^e
{
    static constexpr int kPrecision = 64; // = q

    uint64_t f = 0;
    int e = 0;

    constexpr diyfp(uint64_t f_, int e_)  : f(f_), e(e_) {}

    /*!
    @brief returns x - y
    @pre x.e == y.e and x.f >= y.f
    */
    static diyfp sub(const diyfp& x, const diyfp& y) 
    {
        assert(x.e == y.e);
        assert(x.f >= y.f);

        return {x.f - y.f, x.e};
    }

    /*!
    @brief returns x * y
    @note The result is rounded. (Only the upper q bits are returned.)
    */
    static diyfp mul(const diyfp& x, const diyfp& y) 
    {
        static_assert(kPrecision == 64, "internal error");



        const uint64_t u_lo = x.f & 0xFFFFFFFF;
        const uint64_t u_hi = x.f >> 32;
        const uint64_t v_lo = y.f & 0xFFFFFFFF;
        const uint64_t v_hi = y.f >> 32;

        const uint64_t p0 = u_lo * v_lo;
        const uint64_t p1 = u_lo * v_hi;
        const uint64_t p2 = u_hi * v_lo;
        const uint64_t p3 = u_hi * v_hi;

        const uint64_t p0_hi = p0 >> 32;
        const uint64_t p1_lo = p1 & 0xFFFFFFFF;
        const uint64_t p1_hi = p1 >> 32;
        const uint64_t p2_lo = p2 & 0xFFFFFFFF;
        const uint64_t p2_hi = p2 >> 32;

        uint64_t Q = p0_hi + p1_lo + p2_lo;

        // The full product might now be computed as
        //
        // p_hi = p3 + p2_hi + p1_hi + (Q >> 32)
        // p_lo = p0_lo + (Q << 32)
        //
        // But in this particular case here, the full p_lo is not required.
        // Effectively we only need to add the highest bit in p_lo to p_hi (and
        // Q_hi + 1 does not overflow).

        Q += uint64_t{1} << (64 - 32 - 1); // round, ties up

        const uint64_t h = p3 + p2_hi + p1_hi + (Q >> 32);

        return {h, x.e + y.e + 64};
    }

    /*!
    @brief normalize x such that the significand is >= 2^(q-1)
    @pre x.f != 0
    */
    static diyfp normalize(diyfp x) 
    {
        assert(x.f != 0);

        while ((x.f >> 63) == 0)
        {
            x.f <<= 1;
            x.e--;
        }

        return x;
    }

    /*!
    @brief normalize x such that the result has the exponent E
    @pre e >= x.e and the upper e - x.e bits of x.f must be zero.
    */
    static diyfp normalize_to(const diyfp& x, const int target_exponent) 
    {
        const int delta = x.e - target_exponent;

        assert(delta >= 0);
        assert(((x.f << delta) >> delta) == x.f);

        return {x.f << delta, target_exponent};
    }
};

struct boundaries
{
    diyfp w;
    diyfp minus;
    diyfp plus;
};

/*!
Compute the (normalized) diyfp representing the input number 'value' and its
boundaries.

@pre value must be finite and positive
*/
template <typename FloatType>
boundaries compute_boundaries(FloatType value)
{
    assert(std::isfinite(value));
    assert(value > 0);

    // Convert the IEEE representation into a diyfp.
    //
    // If v is denormal:
    //      value = 0.F * 2^(1 - bias) = (          F) * 2^(1 - bias - (p-1))
    // If v is normalized:
    //      value = 1.F * 2^(E - bias) = (2^(p-1) + F) * 2^(E - bias - (p-1))

    static_assert(std::numeric_limits<FloatType>::is_iec559,
                  "internal error: dtoa_short requires an IEEE-754 floating-point implementation");

    constexpr int      kPrecision = std::numeric_limits<FloatType>::digits; // = p (includes the hidden bit)
    constexpr int      kBias      = std::numeric_limits<FloatType>::max_exponent - 1 + (kPrecision - 1);
    constexpr int      kMinExp    = 1 - kBias;
    constexpr uint64_t kHiddenBit = uint64_t{1} << (kPrecision - 1); // = 2^(p-1)

    using bits_type = typename std::conditional< kPrecision == 24, uint32_t, uint64_t >::type;

    const uint64_t bits = reinterpret_bits<bits_type>(value);
    const uint64_t E = bits >> (kPrecision - 1);
    const uint64_t F = bits & (kHiddenBit - 1);

    const bool is_denormal = (E == 0);
    const diyfp v = is_denormal
                    ? diyfp(F, kMinExp)
                    : diyfp(F + kHiddenBit, static_cast<int>(E) - kBias);



    const bool lower_boundary_is_closer = (F == 0 and E > 1);
    const diyfp m_plus = diyfp(2 * v.f + 1, v.e - 1);
    const diyfp m_minus = lower_boundary_is_closer
                          ? diyfp(4 * v.f - 1, v.e - 2)  // (B)
                          : diyfp(2 * v.f - 1, v.e - 1); // (A)

    // Determine the normalized w+ = m+.
    const diyfp w_plus = diyfp::normalize(m_plus);

    // Determine w- = m- such that e_(w-) = e_(w+).
    const diyfp w_minus = diyfp::normalize_to(m_minus, w_plus.e);

    return {diyfp::normalize(v), w_minus, w_plus};
}



constexpr int kAlpha = -60;
constexpr int kGamma = -32;

struct cached_power // c = f * 2^e ~= 10^k
{
    uint64_t f;
    int e;
    int k;
};

/*!
For a normalized diyfp w = f * 2^e, this function returns a (normalized) cached
power-of-ten c = f_c * 2^e_c, such that the exponent of the product w * c
satisfies (Definition 3.2 from [1])

     alpha <= e_c + e + q <= gamma.
*/
inline cached_power get_cached_power_for_binary_exponent(int e)
{
 

    constexpr int kCachedPowersSize = 79;
    constexpr int kCachedPowersMinDecExp = -300;
    constexpr int kCachedPowersDecStep = 8;

    static constexpr cached_power kCachedPowers[] =
    {
        { 0xAB70FE17C79AC6CA, -1060, -300 },
        { 0xFF77B1FCBEBCDC4F, -1034, -292 },
        { 0xBE5691EF416BD60C, -1007, -284 },
        { 0x8DD01FAD907FFC3C,  -980, -276 },
        { 0xD3515C2831559A83,  -954, -268 },
        { 0x9D71AC8FADA6C9B5,  -927, -260 },
        { 0xEA9C227723EE8BCB,  -901, -252 },
        { 0xAECC49914078536D,  -874, -244 },
        { 0x823C12795DB6CE57,  -847, -236 },
        { 0xC21094364DFB5637,  -821, -228 },
        { 0x9096EA6F3848984F,  -794, -220 },
        { 0xD77485CB25823AC7,  -768, -212 },
        { 0xA086CFCD97BF97F4,  -741, -204 },
        { 0xEF340A98172AACE5,  -715, -196 },
        { 0xB23867FB2A35B28E,  -688, -188 },
        { 0x84C8D4DFD2C63F3B,  -661, -180 },
        { 0xC5DD44271AD3CDBA,  -635, -172 },
        { 0x936B9FCEBB25C996,  -608, -164 },
        { 0xDBAC6C247D62A584,  -582, -156 },
        { 0xA3AB66580D5FDAF6,  -555, -148 },
        { 0xF3E2F893DEC3F126,  -529, -140 },
        { 0xB5B5ADA8AAFF80B8,  -502, -132 },
        { 0x87625F056C7C4A8B,  -475, -124 },
        { 0xC9BCFF6034C13053,  -449, -116 },
        { 0x964E858C91BA2655,  -422, -108 },
        { 0xDFF9772470297EBD,  -396, -100 },
        { 0xA6DFBD9FB8E5B88F,  -369,  -92 },
        { 0xF8A95FCF88747D94,  -343,  -84 },
        { 0xB94470938FA89BCF,  -316,  -76 },
        { 0x8A08F0F8BF0F156B,  -289,  -68 },
        { 0xCDB02555653131B6,  -263,  -60 },
        { 0x993FE2C6D07B7FAC,  -236,  -52 },
        { 0xE45C10C42A2B3B06,  -210,  -44 },
        { 0xAA242499697392D3,  -183,  -36 },
        { 0xFD87B5F28300CA0E,  -157,  -28 },
        { 0xBCE5086492111AEB,  -130,  -20 },
        { 0x8CBCCC096F5088CC,  -103,  -12 },
        { 0xD1B71758E219652C,   -77,   -4 },
        { 0x9C40000000000000,   -50,    4 },
        { 0xE8D4A51000000000,   -24,   12 },
        { 0xAD78EBC5AC620000,     3,   20 },
        { 0x813F3978F8940984,    30,   28 },
        { 0xC097CE7BC90715B3,    56,   36 },
        { 0x8F7E32CE7BEA5C70,    83,   44 },
        { 0xD5D238A4ABE98068,   109,   52 },
        { 0x9F4F2726179A2245,   136,   60 },
        { 0xED63A231D4C4FB27,   162,   68 },
        { 0xB0DE65388CC8ADA8,   189,   76 },
        { 0x83C7088E1AAB65DB,   216,   84 },
        { 0xC45D1DF942711D9A,   242,   92 },
        { 0x924D692CA61BE758,   269,  100 },
        { 0xDA01EE641A708DEA,   295,  108 },
        { 0xA26DA3999AEF774A,   322,  116 },
        { 0xF209787BB47D6B85,   348,  124 },
        { 0xB454E4A179DD1877,   375,  132 },
        { 0x865B86925B9BC5C2,   402,  140 },
        { 0xC83553C5C8965D3D,   428,  148 },
        { 0x952AB45CFA97A0B3,   455,  156 },
        { 0xDE469FBD99A05FE3,   481,  164 },
        { 0xA59BC234DB398C25,   508,  172 },
        { 0xF6C69A72A3989F5C,   534,  180 },
        { 0xB7DCBF5354E9BECE,   561,  188 },
        { 0x88FCF317F22241E2,   588,  196 },
        { 0xCC20CE9BD35C78A5,   614,  204 },
        { 0x98165AF37B2153DF,   641,  212 },
        { 0xE2A0B5DC971F303A,   667,  220 },
        { 0xA8D9D1535CE3B396,   694,  228 },
        { 0xFB9B7CD9A4A7443C,   720,  236 },
        { 0xBB764C4CA7A44410,   747,  244 },
        { 0x8BAB8EEFB6409C1A,   774,  252 },
        { 0xD01FEF10A657842C,   800,  260 },
        { 0x9B10A4E5E9913129,   827,  268 },
        { 0xE7109BFBA19C0C9D,   853,  276 },
        { 0xAC2820D9623BF429,   880,  284 },
        { 0x80444B5E7AA7CF85,   907,  292 },
        { 0xBF21E44003ACDD2D,   933,  300 },
        { 0x8E679C2F5E44FF8F,   960,  308 },
        { 0xD433179D9C8CB841,   986,  316 },
        { 0x9E19DB92B4E31BA9,  1013,  324 },
    };

    // This computation gives exactly the same results for k as
    //      k = ceil((kAlpha - e - 1) * 0.30102999566398114)
    // for |e| <= 1500, but doesn't require floating-point operations.
    // NB: log_10(2) ~= 78913 / 2^18
    assert(e >= -1500);
    assert(e <=  1500);
    const int f = kAlpha - e - 1;
    const int k = (f * 78913) / (1 << 18) + static_cast<int>(f > 0);

    const int index = (-kCachedPowersMinDecExp + k + (kCachedPowersDecStep - 1)) / kCachedPowersDecStep;
    assert(index >= 0);
    assert(index < kCachedPowersSize);
    static_cast<void>(kCachedPowersSize); // Fix warning.

    const cached_power cached = kCachedPowers[index];
    assert(kAlpha <= cached.e + e + 64);
    assert(kGamma >= cached.e + e + 64);

    return cached;
}

/*!
For n != 0, returns k, such that pow10 := 10^(k-1) <= n < 10^k.
For n == 0, returns 1 and sets pow10 := 1.
*/
inline int find_largest_pow10(const uint32_t n, uint32_t& pow10)
{
    // LCOV_EXCL_START
    if (n >= 1000000000)
    {
        pow10 = 1000000000;
        return 10;
    }
    // LCOV_EXCL_STOP
    else if (n >= 100000000)
    {
        pow10 = 100000000;
        return  9;
    }
    else if (n >= 10000000)
    {
        pow10 = 10000000;
        return  8;
    }
    else if (n >= 1000000)
    {
        pow10 = 1000000;
        return  7;
    }
    else if (n >= 100000)
    {
        pow10 = 100000;
        return  6;
    }
    else if (n >= 10000)
    {
        pow10 = 10000;
        return  5;
    }
    else if (n >= 1000)
    {
        pow10 = 1000;
        return  4;
    }
    else if (n >= 100)
    {
        pow10 = 100;
        return  3;
    }
    else if (n >= 10)
    {
        pow10 = 10;
        return  2;
    }
    else
    {
        pow10 = 1;
        return 1;
    }
}

inline void grisu2_round(char* buf, int len, uint64_t dist, uint64_t delta,
                         uint64_t rest, uint64_t ten_k)
{
    assert(len >= 1);
    assert(dist <= delta);
    assert(rest <= delta);
    assert(ten_k > 0);



    while (rest < dist
            and delta - rest >= ten_k
            and (rest + ten_k < dist or dist - rest > rest + ten_k - dist))
    {
        assert(buf[len - 1] != '0');
        buf[len - 1]--;
        rest += ten_k;
    }
}

/*!
Generates V = buffer * 10^decimal_exponent, such that M- <= V <= M+.
M- and M+ must be normalized and share the same exponent -60 <= e <= -32.
*/
inline void grisu2_digit_gen(char* buffer, int& length, int& decimal_exponent,
                             diyfp M_minus, diyfp w, diyfp M_plus)
{
    static_assert(kAlpha >= -60, "internal error");
    static_assert(kGamma <= -32, "internal error");



    assert(M_plus.e >= kAlpha);
    assert(M_plus.e <= kGamma);

    uint64_t delta = diyfp::sub(M_plus, M_minus).f; // (significand of (M+ - M-), implicit exponent is e)
    uint64_t dist  = diyfp::sub(M_plus, w      ).f; // (significand of (M+ - w ), implicit exponent is e)

    // Split M+ = f * 2^e into two parts p1 and p2 (note: e < 0):
    //
    //      M+ = f * 2^e
    //         = ((f div 2^-e) * 2^-e + (f mod 2^-e)) * 2^e
    //         = ((p1        ) * 2^-e + (p2        )) * 2^e
    //         = p1 + p2 * 2^e

    const diyfp one(uint64_t{1} << -M_plus.e, M_plus.e);

    auto p1 = static_cast<uint32_t>(M_plus.f >> -one.e); // p1 = f div 2^-e (Since -e >= 32, p1 fits into a 32-bit int.)
    uint64_t p2 = M_plus.f & (one.f - 1);                    // p2 = f mod 2^-e

    // 1)
    //
    // Generate the digits of the integral part p1 = d[n-1]...d[1]d[0]

    assert(p1 > 0);

    uint32_t pow10;
    const int k = find_largest_pow10(p1, pow10);



    int n = k;
    while (n > 0)
    {
        // Invariants:
        //      M+ = buffer * 10^n + (p1 + p2 * 2^e)    (buffer = 0 for n = k)
        //      pow10 = 10^(n-1) <= p1 < 10^n
        //
        const uint32_t d = p1 / pow10;  // d = p1 div 10^(n-1)
        const uint32_t r = p1 % pow10;  // r = p1 mod 10^(n-1)
        //
        //      M+ = buffer * 10^n + (d * 10^(n-1) + r) + p2 * 2^e
        //         = (buffer * 10 + d) * 10^(n-1) + (r + p2 * 2^e)
        //
        assert(d <= 9);
        buffer[length++] = static_cast<char>('0' + d); // buffer := buffer * 10 + d
        //
        //      M+ = buffer * 10^(n-1) + (r + p2 * 2^e)
        //
        p1 = r;
        n--;
        //
        //      M+ = buffer * 10^n + (p1 + p2 * 2^e)
        //      pow10 = 10^n
        //

        // Now check if enough digits have been generated.
        // Compute
        //
        //      p1 + p2 * 2^e = (p1 * 2^-e + p2) * 2^e = rest * 2^e
        //
        // Note:
        // Since rest and delta share the same exponent e, it suffices to
        // compare the significands.
        const uint64_t rest = (uint64_t{p1} << -one.e) + p2;
        if (rest <= delta)
        {
            // V = buffer * 10^n, with M- <= V <= M+.

            decimal_exponent += n;

            // We may now just stop. But instead look if the buffer could be
            // decremented to bring V closer to w.
            //
            // pow10 = 10^n is now 1 ulp in the decimal representation V.
            // The rounding procedure works with diyfp's with an implicit
            // exponent of e.
            //
            //      10^n = (10^n * 2^-e) * 2^e = ulp * 2^e
            //
            const uint64_t ten_n = uint64_t{pow10} << -one.e;
            grisu2_round(buffer, length, dist, delta, rest, ten_n);

            return;
        }

        pow10 /= 10;
        //
        //      pow10 = 10^(n-1) <= p1 < 10^n
        // Invariants restored.
    }

 

    assert(p2 > delta);

    int m = 0;
    for (;;)
    {
        // Invariant:
        //      M+ = buffer * 10^-m + 10^-m * (d[-m-1] / 10 + d[-m-2] / 10^2 + ...) * 2^e
        //         = buffer * 10^-m + 10^-m * (p2                                 ) * 2^e
        //         = buffer * 10^-m + 10^-m * (1/10 * (10 * p2)                   ) * 2^e
        //         = buffer * 10^-m + 10^-m * (1/10 * ((10*p2 div 2^-e) * 2^-e + (10*p2 mod 2^-e)) * 2^e
        //
        assert(p2 <= UINT64_MAX / 10);
        p2 *= 10;
        const uint64_t d = p2 >> -one.e;     // d = (10 * p2) div 2^-e
        const uint64_t r = p2 & (one.f - 1); // r = (10 * p2) mod 2^-e
        //
        //      M+ = buffer * 10^-m + 10^-m * (1/10 * (d * 2^-e + r) * 2^e
        //         = buffer * 10^-m + 10^-m * (1/10 * (d + r * 2^e))
        //         = (buffer * 10 + d) * 10^(-m-1) + 10^(-m-1) * r * 2^e
        //
        assert(d <= 9);
        buffer[length++] = static_cast<char>('0' + d); // buffer := buffer * 10 + d
        //
        //      M+ = buffer * 10^(-m-1) + 10^(-m-1) * r * 2^e
        //
        p2 = r;
        m++;
        //
        //      M+ = buffer * 10^-m + 10^-m * p2 * 2^e
        // Invariant restored.

        // Check if enough digits have been generated.
        //
        //      10^-m * p2 * 2^e <= delta * 2^e
        //              p2 * 2^e <= 10^m * delta * 2^e
        //                    p2 <= 10^m * delta
        delta *= 10;
        dist  *= 10;
        if (p2 <= delta)
        {
            break;
        }
    }

    // V = buffer * 10^-m, with M- <= V <= M+.

    decimal_exponent -= m;

    // 1 ulp in the decimal representation is now 10^-m.
    // Since delta and dist are now scaled by 10^m, we need to do the
    // same with ulp in order to keep the units in sync.
    //
    //      10^m * 10^-m = 1 = 2^-e * 2^e = ten_m * 2^e
    //
    const uint64_t ten_m = one.f;
    grisu2_round(buffer, length, dist, delta, p2, ten_m);


}

/*!
v = buf * 10^decimal_exponent
len is the length of the buffer (number of decimal digits)
The buffer must be large enough, i.e. >= max_digits10.
*/
inline void grisu2(char* buf, int& len, int& decimal_exponent,
                   diyfp m_minus, diyfp v, diyfp m_plus)
{
    assert(m_plus.e == m_minus.e);
    assert(m_plus.e == v.e);

    //  --------(-----------------------+-----------------------)--------    (A)
    //          m-                      v                       m+
    //
    //  --------------------(-----------+-----------------------)--------    (B)
    //                      m-          v                       m+
    //
    // First scale v (and m- and m+) such that the exponent is in the range
    // [alpha, gamma].

    const cached_power cached = get_cached_power_for_binary_exponent(m_plus.e);

    const diyfp c_minus_k(cached.f, cached.e); // = c ~= 10^-k

    // The exponent of the products is = v.e + c_minus_k.e + q and is in the range [alpha,gamma]
    const diyfp w       = diyfp::mul(v,       c_minus_k);
    const diyfp w_minus = diyfp::mul(m_minus, c_minus_k);
    const diyfp w_plus  = diyfp::mul(m_plus,  c_minus_k);


    const diyfp M_minus(w_minus.f + 1, w_minus.e);
    const diyfp M_plus (w_plus.f  - 1, w_plus.e );

    decimal_exponent = -cached.k; // = -(-k) = k

    grisu2_digit_gen(buf, len, decimal_exponent, M_minus, w, M_plus);
}

/*!
v = buf * 10^decimal_exponent
len is the length of the buffer (number of decimal digits)
The buffer must be large enough, i.e. >= max_digits10.
*/
template <typename FloatType>
void grisu2(char* buf, int& len, int& decimal_exponent, FloatType value)
{
    static_assert(diyfp::kPrecision >= std::numeric_limits<FloatType>::digits + 3,
                  "internal error: not enough precision");

    assert(std::isfinite(value));
    assert(value > 0);

 
    const boundaries w = compute_boundaries(value);
#endif

    grisu2(buf, len, decimal_exponent, w.minus, w.w, w.plus);
}


inline char* append_exponent(char* buf, int e)
{
    assert(e > -1000);
    assert(e <  1000);

    if (e < 0)
    {
        e = -e;
        *buf++ = '-';
    }
    else
    {
        *buf++ = '+';
    }

    auto k = static_cast<uint32_t>(e);
    if (k < 10)
    {
        // Always print at least two digits in the exponent.
        // This is for compatibility with printf("%g").
        *buf++ = '0';
        *buf++ = static_cast<char>('0' + k);
    }
    else if (k < 100)
    {
        *buf++ = static_cast<char>('0' + k / 10);
        k %= 10;
        *buf++ = static_cast<char>('0' + k);
    }
    else
    {
        *buf++ = static_cast<char>('0' + k / 100);
        k %= 100;
        *buf++ = static_cast<char>('0' + k / 10);
        k %= 10;
        *buf++ = static_cast<char>('0' + k);
    }

    return buf;
}

/*!
@brief prettify v = buf * 10^decimal_exponent

If v is in the range [10^min_exp, 10^max_exp) it will be printed in fixed-point
notation. Otherwise it will be printed in exponential notation.

@pre min_exp < 0
@pre max_exp > 0
*/
inline char* format_buffer(char* buf, int len, int decimal_exponent,
                           int min_exp, int max_exp)
{
    assert(min_exp < 0);
    assert(max_exp > 0);

    const int k = len;
    const int n = len + decimal_exponent;

    // v = buf * 10^(n-k)
    // k is the length of the buffer (number of decimal digits)
    // n is the position of the decimal point relative to the start of the buffer.

    if (k <= n and n <= max_exp)
    {
        // digits[000]
        // len <= max_exp + 2

        std::memset(buf + k, '0', static_cast<size_t>(n - k));
        // Make it look like a floating-point number (#362, #378)
        buf[n + 0] = '.';
        buf[n + 1] = '0';
        return buf + (n + 2);
    }

    if (0 < n and n <= max_exp)
    {
        // dig.its
        // len <= max_digits10 + 1

        assert(k > n);

        std::memmove(buf + (n + 1), buf + n, static_cast<size_t>(k - n));
        buf[n] = '.';
        return buf + (k + 1);
    }

    if (min_exp < n and n <= 0)
    {
        // 0.[000]digits
        // len <= 2 + (-min_exp - 1) + max_digits10

        std::memmove(buf + (2 + -n), buf, static_cast<size_t>(k));
        buf[0] = '0';
        buf[1] = '.';
        std::memset(buf + 2, '0', static_cast<size_t>(-n));
        return buf + (2 + (-n) + k);
    }

    if (k == 1)
    {
        // dE+123
        // len <= 1 + 5

        buf += 1;
    }
    else
    {
        // d.igitsE+123
        // len <= max_digits10 + 1 + 5

        std::memmove(buf + 2, buf + 1, static_cast<size_t>(k - 1));
        buf[1] = '.';
        buf += 1 + k;
    }

    *buf++ = 'e';
    return append_exponent(buf, n - 1);
}

} // namespace dtoa_impl

/*!
@brief generates a decimal representation of the floating-point number value in [first, last).

The format of the resulting decimal representation is similar to printf's %g
format. Returns an iterator pointing past-the-end of the decimal representation.

@note The input number must be finite, i.e. NaN's and Inf's are not supported.
@note The buffer must be large enough.
@note The result is NOT null-terminated.
*/
template <typename FloatType>
char* to_chars(char* first, const char* last, FloatType value)
{
    static_cast<void>(last); // maybe unused - fix warning
    assert(std::isfinite(value));

    // Use signbit(value) instead of (value < 0) since signbit works for -0.
    if (std::signbit(value))
    {
        value = -value;
        *first++ = '-';
    }

    if (value == 0) // +-0
    {
        *first++ = '0';
        // Make it look like a floating-point number (#362, #378)
        *first++ = '.';
        *first++ = '0';
        return first;
    }

    assert(last - first >= std::numeric_limits<FloatType>::max_digits10);

    // Compute v = buffer * 10^decimal_exponent.
    // The decimal digits are stored in the buffer, which needs to be interpreted
    // as an unsigned decimal integer.
    // len is the length of the buffer, i.e. the number of decimal digits.
    int len = 0;
    int decimal_exponent = 0;
    dtoa_impl::grisu2(first, len, decimal_exponent, value);

    assert(len <= std::numeric_limits<FloatType>::max_digits10);

    // Format the buffer like printf("%.*g", prec, value)
    constexpr int kMinExp = -4;
    // Use digits10 here to increase compatibility with version 2.
    constexpr int kMaxExp = std::numeric_limits<FloatType>::digits10;

    assert(last - first >= kMaxExp + 2);
    assert(last - first >= 2 + (-kMinExp - 1) + std::numeric_limits<FloatType>::max_digits10);
    assert(last - first >= std::numeric_limits<FloatType>::max_digits10 + 6);

    return dtoa_impl::format_buffer(first, len, decimal_exponent, kMinExp, kMaxExp);
}

} // namespace detail
} // namespace nlohmann



namespace nlohmann
{
namespace detail
{
///////////////////
// serialization //
///////////////////

/// how to treat decoding errors
enum class error_handler_t
{
    strict,  ///< throw a type_error exception in case of invalid UTF-8
    replace, ///< replace invalid UTF-8 sequences with U+FFFD
    ignore   ///< ignore invalid UTF-8 sequences
};

template<typename Basic_json>
class serializer
{
    using String = typename Basic_json::String;
    using Num_float = typename Basic_json::Num_float;
    using Num_integer = typename Basic_json::Num_integer;
    using Num_unsigned = typename Basic_json::Num_unsigned;
    static constexpr uint8_t UTF8_ACCEPT = 0;
    static constexpr uint8_t UTF8_REJECT = 1;

  public:
    /*!
    @param[in] s  output stream to serialize to
    @param[in] ichar  indentation character to use
    @param[in] error_handler_  how to react on decoding errors
    */
    serializer(output_adapter_t<char> s, const char ichar,
               error_handler_t error_handler_ = error_handler_t::strict)
        : o(std::move(s))
        , loc(std::localeconv())
        , thousands_sep(loc->thousands_sep == nullptr ? '\0' : * (loc->thousands_sep))
        , decimal_point(loc->decimal_point == nullptr ? '\0' : * (loc->decimal_point))
        , indent_char(ichar)
        , indent_string(512, indent_char)
        , error_handler(error_handler_)
    {}

    // delete because of pointer members
    serializer(const serializer&) = delete;
    serializer& operator=(const serializer&) = delete;
    serializer(serializer&&) = delete;
    serializer& operator=(serializer&&) = delete;
    ~serializer() = default;

    void dump(const Basic_json& val, const bool pretty_print,
              const bool ensure_ascii,
              const unsigned int indent_step,
              const unsigned int current_indent = 0)
    {
        switch (val.type_)
        {
            case Value::object:
            {
                if (val.value_.object->empty())
                {
                    o->write_characters("{}", 2);
                    return;
                }

                if (pretty_print)
                {
                    o->write_characters("{\n", 2);

                    // variable to hold indentation for recursive calls
                    const auto new_indent = current_indent + indent_step;
                    if ((indent_string.size() < new_indent))
                    {
                        indent_string.resize(indent_string.size() * 2, ' ');
                    }

                    // first n-1 elements
                    auto i = val.value_.object->cbegin();
                    for (std::size_t cnt = 0; cnt < val.value_.object->size() - 1; ++cnt, ++i)
                    {
                        o->write_characters(indent_string.c_str(), new_indent);
                        o->write_character('\"');
                        dump_escaped(i->first, ensure_ascii);
                        o->write_characters("\": ", 3);
                        dump(i->second, true, ensure_ascii, indent_step, new_indent);
                        o->write_characters(",\n", 2);
                    }

                    // last element
                    assert(i != val.value_.object->cend());
                    assert(std::next(i) == val.value_.object->cend());
                    o->write_characters(indent_string.c_str(), new_indent);
                    o->write_character('\"');
                    dump_escaped(i->first, ensure_ascii);
                    o->write_characters("\": ", 3);
                    dump(i->second, true, ensure_ascii, indent_step, new_indent);

                    o->write_character('\n');
                    o->write_characters(indent_string.c_str(), current_indent);
                    o->write_character('}');
                }
                else
                {
                    o->write_character('{');

                    // first n-1 elements
                    auto i = val.value_.object->cbegin();
                    for (std::size_t cnt = 0; cnt < val.value_.object->size() - 1; ++cnt, ++i)
                    {
                        o->write_character('\"');
                        dump_escaped(i->first, ensure_ascii);
                        o->write_characters("\":", 2);
                        dump(i->second, false, ensure_ascii, indent_step, current_indent);
                        o->write_character(',');
                    }

                    // last element
                    assert(i != val.value_.object->cend());
                    assert(std::next(i) == val.value_.object->cend());
                    o->write_character('\"');
                    dump_escaped(i->first, ensure_ascii);
                    o->write_characters("\":", 2);
                    dump(i->second, false, ensure_ascii, indent_step, current_indent);

                    o->write_character('}');
                }

                return;
            }

            case Value::array:
            {
                if (val.value_.array->empty())
                {
                    o->write_characters("[]", 2);
                    return;
                }

                if (pretty_print)
                {
                    o->write_characters("[\n", 2);

                    // variable to hold indentation for recursive calls
                    const auto new_indent = current_indent + indent_step;
                    if ((indent_string.size() < new_indent))
                    {
                        indent_string.resize(indent_string.size() * 2, ' ');
                    }

                    // first n-1 elements
                    for (auto i = val.value_.array->cbegin();
                            i != val.value_.array->cend() - 1; ++i)
                    {
                        o->write_characters(indent_string.c_str(), new_indent);
                        dump(*i, true, ensure_ascii, indent_step, new_indent);
                        o->write_characters(",\n", 2);
                    }

                    // last element
                    assert(not val.value_.array->empty());
                    o->write_characters(indent_string.c_str(), new_indent);
                    dump(val.value_.array->back(), true, ensure_ascii, indent_step, new_indent);

                    o->write_character('\n');
                    o->write_characters(indent_string.c_str(), current_indent);
                    o->write_character(']');
                }
                else
                {
                    o->write_character('[');

                    // first n-1 elements
                    for (auto i = val.value_.array->cbegin();
                            i != val.value_.array->cend() - 1; ++i)
                    {
                        dump(*i, false, ensure_ascii, indent_step, current_indent);
                        o->write_character(',');
                    }

                    // last element
                    assert(not val.value_.array->empty());
                    dump(val.value_.array->back(), false, ensure_ascii, indent_step, current_indent);

                    o->write_character(']');
                }

                return;
            }

            case Value::string:
            {
                o->write_character('\"');
                dump_escaped(*val.value_.string, ensure_ascii);
                o->write_character('\"');
                return;
            }

            case Value::boolean:
            {
                if (val.value_.boolean)
                {
                    o->write_characters("true", 4);
                }
                else
                {
                    o->write_characters("false", 5);
                }
                return;
            }

            case Value::num_integer:
            {
                dump_integer(val.value_.num_integer);
                return;
            }

            case Value::num_unsigned:
            {
                dump_integer(val.value_.num_unsigned);
                return;
            }

            case Value::num_float:
            {
                dump_float(val.value_.num_float);
                return;
            }

            case Value::discarded:
            {
                o->write_characters("<discarded>", 11);
                return;
            }

            case Value::null:
            {
                o->write_characters("null", 4);
                return;
            }
        }
    }

  private:

    void dump_escaped(const String& s, const bool ensure_ascii)
    {
        uint32_t codepoint;
        uint8_t state = UTF8_ACCEPT;
        std::size_t bytes = 0;  // number of bytes written to string_buffer

        // number of bytes written at the point of the last valid byte
        std::size_t bytes_after_last_accept = 0;
        std::size_t undumped_chars = 0;

        for (std::size_t i = 0; i < s.size(); ++i)
        {
            const auto byte = static_cast<uint8_t>(s[i]);

            switch (decode(state, codepoint, byte))
            {
                case UTF8_ACCEPT:  // decode found a new code point
                {
                    switch (codepoint)
                    {
                        case 0x08: // backspace
                        {
                            string_buffer[bytes++] = '\\';
                            string_buffer[bytes++] = 'b';
                            break;
                        }

                        case 0x09: // horizontal tab
                        {
                            string_buffer[bytes++] = '\\';
                            string_buffer[bytes++] = 't';
                            break;
                        }

                        case 0x0A: // newline
                        {
                            string_buffer[bytes++] = '\\';
                            string_buffer[bytes++] = 'n';
                            break;
                        }

                        case 0x0C: // formfeed
                        {
                            string_buffer[bytes++] = '\\';
                            string_buffer[bytes++] = 'f';
                            break;
                        }

                        case 0x0D: // carriage return
                        {
                            string_buffer[bytes++] = '\\';
                            string_buffer[bytes++] = 'r';
                            break;
                        }

                        case 0x22: // quotation mark
                        {
                            string_buffer[bytes++] = '\\';
                            string_buffer[bytes++] = '\"';
                            break;
                        }

                        case 0x5C: // reverse solidus
                        {
                            string_buffer[bytes++] = '\\';
                            string_buffer[bytes++] = '\\';
                            break;
                        }

                        default:
                        {
                            // escape control characters (0x00..0x1F) or, if
                            // ensure_ascii parameter is used, non-ASCII characters
                            if ((codepoint <= 0x1F) or (ensure_ascii and (codepoint >= 0x7F)))
                            {
                                if (codepoint <= 0xFFFF)
                                {
                                    (std::snprintf)(string_buffer.data() + bytes, 7, "\\u%04x",
                                                    static_cast<uint16_t>(codepoint));
                                    bytes += 6;
                                }
                                else
                                {
                                    (std::snprintf)(string_buffer.data() + bytes, 13, "\\u%04x\\u%04x",
                                                    static_cast<uint16_t>(0xD7C0 + (codepoint >> 10)),
                                                    static_cast<uint16_t>(0xDC00 + (codepoint & 0x3FF)));
                                    bytes += 12;
                                }
                            }
                            else
                            {
                                // copy byte to buffer (all previous bytes
                                // been copied have in default case above)
                                string_buffer[bytes++] = s[i];
                            }
                            break;
                        }
                    }

                    // write buffer and reset index; there must be 13 bytes
                    // left, as this is the maximal number of bytes to be
                    // written ("\uxxxx\uxxxx\0") for one code point
                    if (string_buffer.size() - bytes < 13)
                    {
                        o->write_characters(string_buffer.data(), bytes);
                        bytes = 0;
                    }

                    // remember the byte position of this accept
                    bytes_after_last_accept = bytes;
                    undumped_chars = 0;
                    break;
                }

                case UTF8_REJECT:  // decode found invalid UTF-8 byte
                {
                    switch (error_handler)
                    {
                        case error_handler_t::strict:
                        {
                            std::string sn(3, '\0');
                            (std::snprintf)(&sn[0], sn.size(), "%.2X", byte);
                            JSON_THROW(type_error::create(316, "invalid UTF-8 byte at index " + std::to_string(i) + ": 0x" + sn));
                        }

                        case error_handler_t::ignore:
                        case error_handler_t::replace:
                        {
                            // in case we saw this character the first time, we
                            // would like to read it again, because the byte
                            // may be OK for itself, but just not OK for the
                            // previous sequence
                            if (undumped_chars > 0)
                            {
                                --i;
                            }

                            // reset length buffer to the last accepted index;
                            // thus removing/ignoring the invalid characters
                            bytes = bytes_after_last_accept;

                            if (error_handler == error_handler_t::replace)
                            {
                                // add a replacement character
                                if (ensure_ascii)
                                {
                                    string_buffer[bytes++] = '\\';
                                    string_buffer[bytes++] = 'u';
                                    string_buffer[bytes++] = 'f';
                                    string_buffer[bytes++] = 'f';
                                    string_buffer[bytes++] = 'f';
                                    string_buffer[bytes++] = 'd';
                                }
                                else
                                {
                                    string_buffer[bytes++] = detail::Binary_writer<Basic_json, char>::to_char_type('\xEF');
                                    string_buffer[bytes++] = detail::Binary_writer<Basic_json, char>::to_char_type('\xBF');
                                    string_buffer[bytes++] = detail::Binary_writer<Basic_json, char>::to_char_type('\xBD');
                                }
                                bytes_after_last_accept = bytes;
                            }

                            undumped_chars = 0;

                            // continue processing the string
                            state = UTF8_ACCEPT;
                            break;
                        }
                    }
                    break;
                }

                default:  // decode found yet incomplete multi-byte code point
                {
                    if (not ensure_ascii)
                    {
                        // code point will not be escaped - copy byte to buffer
                        string_buffer[bytes++] = s[i];
                    }
                    ++undumped_chars;
                    break;
                }
            }
        }

        // we finished processing the string
        if ((state == UTF8_ACCEPT))
        {
            // write buffer
            if (bytes > 0)
            {
                o->write_characters(string_buffer.data(), bytes);
            }
        }
        else
        {
            // we finish reading, but do not accept: string was incomplete
            switch (error_handler)
            {
                case error_handler_t::strict:
                {
                    std::string sn(3, '\0');
                    (std::snprintf)(&sn[0], sn.size(), "%.2X", static_cast<uint8_t>(s.back()));
                    JSON_THROW(type_error::create(316, "incomplete UTF-8 string; last byte: 0x" + sn));
                }

                case error_handler_t::ignore:
                {
                    // write all accepted bytes
                    o->write_characters(string_buffer.data(), bytes_after_last_accept);
                    break;
                }

                case error_handler_t::replace:
                {
                    // write all accepted bytes
                    o->write_characters(string_buffer.data(), bytes_after_last_accept);
                    // add a replacement character
                    if (ensure_ascii)
                    {
                        o->write_characters("\\ufffd", 6);
                    }
                    else
                    {
                        o->write_characters("\xEF\xBF\xBD", 3);
                    }
                    break;
                }
            }
        }
    }

    /*!
    @brief dump an integer

    Dump a given integer to output stream @a o. Works internally with
    @a number_buffer.

    @param[in] x  integer number (signed or unsigned) to dump
    @tparam NumberType either @a Num_integer or @a Num_unsigned
    */
    template<typename NumberType, detail::enable_if_t<
                 std::is_same<NumberType, Num_unsigned>::value or
                 std::is_same<NumberType, Num_integer>::value,
                 int> = 0>
    void dump_integer(NumberType x)
    {
        // special case for "0"
        if (x == 0)
        {
            o->write_character('0');
            return;
        }

        const bool is_negative = std::is_same<NumberType, Num_integer>::value and not (x >= 0);  // see issue #755
        std::size_t i = 0;

        while (x != 0)
        {
            // spare 1 byte for '\0'
            assert(i < number_buffer.size() - 1);

            const auto digit = std::labs(static_cast<long>(x % 10));
            number_buffer[i++] = static_cast<char>('0' + digit);
            x /= 10;
        }

        if (is_negative)
        {
            // make sure there is capacity for the '-'
            assert(i < number_buffer.size() - 2);
            number_buffer[i++] = '-';
        }

        std::reverse(number_buffer.begin(), number_buffer.begin() + i);
        o->write_characters(number_buffer.data(), i);
    }

    /*!
    @brief dump a floating-point number

    Dump a given floating-point number to output stream @a o. Works internally
    with @a number_buffer.

    @param[in] x  floating-point number to dump
    */
    void dump_float(Num_float x)
    {
        // NaN / inf
        if (not std::isfinite(x))
        {
            o->write_characters("null", 4);
            return;
        }

        // If Num_float is an IEEE-754 single or double precision number,
        // use the Grisu2 algorithm to produce short numbers which are
        // guaranteed to round-trip, using strtof and strtod, resp.
        //
        // NB: The test below works if <long double> == <double>.
        static constexpr bool is_ieee_single_or_double
            = (std::numeric_limits<Num_float>::is_iec559 and std::numeric_limits<Num_float>::digits == 24 and std::numeric_limits<Num_float>::max_exponent == 128) or
              (std::numeric_limits<Num_float>::is_iec559 and std::numeric_limits<Num_float>::digits == 53 and std::numeric_limits<Num_float>::max_exponent == 1024);

        dump_float(x, std::integral_constant<bool, is_ieee_single_or_double>());
    }

    void dump_float(Num_float x, std::true_type /*is_ieee_single_or_double*/)
    {
        char* begin = number_buffer.data();
        char* end = ::nlohmann::detail::to_chars(begin, begin + number_buffer.size(), x);

        o->write_characters(begin, static_cast<size_t>(end - begin));
    }

    void dump_float(Num_float x, std::false_type /*is_ieee_single_or_double*/)
    {
        // get number of digits for a float -> text -> float round-trip
        static constexpr auto d = std::numeric_limits<Num_float>::max_digits10;

        // the actual conversion
        std::ptrdiff_t len = (std::snprintf)(number_buffer.data(), number_buffer.size(), "%.*g", d, x);

        // negative value indicates an error
        assert(len > 0);
        // check if buffer was large enough
        assert(static_cast<std::size_t>(len) < number_buffer.size());

        // erase thousands separator
        if (thousands_sep != '\0')
        {
            const auto end = std::remove(number_buffer.begin(),
                                         number_buffer.begin() + len, thousands_sep);
            std::fill(end, number_buffer.end(), '\0');
            assert((end - number_buffer.begin()) <= len);
            len = (end - number_buffer.begin());
        }

        // convert decimal point to '.'
        if (decimal_point != '\0' and decimal_point != '.')
        {
            const auto dec_pos = std::find(number_buffer.begin(), number_buffer.end(), decimal_point);
            if (dec_pos != number_buffer.end())
            {
                *dec_pos = '.';
            }
        }

        o->write_characters(number_buffer.data(), static_cast<std::size_t>(len));

        // determine if need to append ".0"
        const bool value_is_int_like =
            std::none_of(number_buffer.begin(), number_buffer.begin() + len + 1,
                         [](char c)
        {
            return (c == '.' or c == 'e');
        });

        if (value_is_int_like)
        {
            o->write_characters(".0", 2);
        }
    }


    static uint8_t decode(uint8_t& state, uint32_t& codep, const uint8_t byte) 
    {
        static const std::array<uint8_t, 400> utf8d =
        {
            {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 00..1F
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 20..3F
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 40..5F
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 60..7F
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, // 80..9F
                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, // A0..BF
                8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // C0..DF
                0xA, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 0x3, // E0..EF
                0xB, 0x6, 0x6, 0x6, 0x5, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, // F0..FF
                0x0, 0x1, 0x2, 0x3, 0x5, 0x8, 0x7, 0x1, 0x1, 0x1, 0x4, 0x6, 0x1, 0x1, 0x1, 0x1, // s0..s0
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, // s1..s2
                1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, // s3..s4
                1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, // s5..s6
                1, 3, 1, 1, 1, 1, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 // s7..s8
            }
        };

        const uint8_t type = utf8d[byte];

        codep = (state != UTF8_ACCEPT)
                ? (byte & 0x3fu) | (codep << 6)
                : static_cast<uint32_t>(0xff >> type) & (byte);

        state = utf8d[256u + state * 16u + type];
        return state;
    }

  private:
    /// the output of the serializer
    output_adapter_t<char> o = nullptr;

    /// a (hopefully) large enough character buffer
    std::array<char, 64> number_buffer{{}};

    /// the locale
    const std::lconv* loc = nullptr;
    /// the locale's thousand separator character
    const char thousands_sep = '\0';
    /// the locale's decimal point character
    const char decimal_point = '\0';

    /// string buffer
    std::array<char, 512> string_buffer{{}};

    /// the indentation character
    const char indent_char;
    /// the indentation string
    String indent_string;

    /// error_handler how to react on decoding errors
    const error_handler_t error_handler;
};
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/json_ref.hpp>


#include <initializer_list>
#include <utility>



namespace nlohmann
{
namespace detail
{
template<typename Basic_json>
class json_ref
{
  public:
    using value_type = Basic_json;

    json_ref(value_type&& value)
        : owned_value(std::move(value)), value_ref(&owned_value), is_rvalue(true)
    {}

    json_ref(const value_type& value)
        : value_ref(const_cast<value_type*>(&value)), is_rvalue(false)
    {}

    json_ref(std::initializer_list<json_ref> init)
        : owned_value(init), value_ref(&owned_value), is_rvalue(true)
    {}

    template <
        class... Args,
        enable_if_t<std::is_constructible<value_type, Args...>::value, int> = 0 >
    json_ref(Args && ... args)
        : owned_value(std::forward<Args>(args)...), value_ref(&owned_value),
          is_rvalue(true) {}

    // class should be movable only
    json_ref(json_ref&&) = default;
    json_ref(const json_ref&) = delete;
    json_ref& operator=(const json_ref&) = delete;
    json_ref& operator=(json_ref&&) = delete;
    ~json_ref() = default;

    value_type moved_or_copied() const
    {
        if (is_rvalue)
        {
            return std::move(*value_ref);
        }
        return *value_ref;
    }

    value_type const& operator*() const
    {
        return *static_cast<value_type const*>(value_ref);
    }

    value_type const* operator->() const
    {
        return static_cast<value_type const*>(value_ref);
    }

  private:
    mutable value_type owned_value = nullptr;
    value_type* value_ref = nullptr;
    const bool is_rvalue;
};
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/Json_ptr.hpp>


#include <cassert> // assert
#include <numeric> // accumulate
#include <string> // string
#include <vector> // vector


namespace nlohmann
{
template<typename Basic_json>
class Json_ptr
{
    // allow basic_json to access private members
    NLOHMANN_BASIC_JSON_TPL_DECLARATION
    friend class basic_json;

  public:

    explicit Json_ptr(const std::string& s = "")
        : reference_tokens(split(s))
    {}


    std::string to_string() const
    {
        return std::accumulate(reference_tokens.begin(), reference_tokens.end(),
                               std::string{},
                               [](const std::string & a, const std::string & b)
        {
            return a + "/" + escape(b);
        });
    }

    /// @copydoc to_string()
    operator std::string() const
    {
        return to_string();
    }


    static int array_index(const std::string& s)
    {
        std::size_t processed_chars = 0;
        const int res = std::stoi(s, &processed_chars);

        // check if the string was completely read
        if ((processed_chars != s.size()))
        {
            JSON_THROW(detail::out_of_range::create(404, "unresolved reference token '" + s + "'"));
        }

        return res;
    }

  private:
    /*!
    @brief remove and return last reference pointer
    @throw out_of_range.405 if JSON pointer has no parent
    */
    std::string pop_back()
    {
        if ((is_root()))
        {
            JSON_THROW(detail::out_of_range::create(405, "JSON pointer has no parent"));
        }

        auto last = reference_tokens.back();
        reference_tokens.pop_back();
        return last;
    }

    /// return whether pointer points to the root document
    bool is_root() const 
    {
        return reference_tokens.empty();
    }

    Json_ptr top() const
    {
        if ((is_root()))
        {
            JSON_THROW(detail::out_of_range::create(405, "JSON pointer has no parent"));
        }

        Json_ptr result = *this;
        result.reference_tokens = {reference_tokens[0]};
        return result;
    }

 
    Basic_json& get_and_create(Basic_json& j) const
    {
        using size_type = typename Basic_json::size_type;
        auto result = &j;

        // in case no reference tokens exist, return a reference to the JSON value
        // j which will be overwritten by a primitive value
        for (const auto& reference_token : reference_tokens)
        {
            switch (result->type_)
            {
                case detail::Value::null:
                {
                    if (reference_token == "0")
                    {
                        // start a new array if reference token is 0
                        result = &result->operator[](0);
                    }
                    else
                    {
                        // start a new object otherwise
                        result = &result->operator[](reference_token);
                    }
                    break;
                }

                case detail::Value::object:
                {
                    // create an entry in the object
                    result = &result->operator[](reference_token);
                    break;
                }

                case detail::Value::array:
                {
                    // create an entry in the array
                    JSON_TRY
                    {
                        result = &result->operator[](static_cast<size_type>(array_index(reference_token)));
                    }
                    JSON_CATCH(std::invalid_argument&)
                    {
                        JSON_THROW(detail::parse_error::create(109, 0, "array index '" + reference_token + "' is not a number"));
                    }
                    break;
                }


                default:
                    JSON_THROW(detail::type_error::create(313, "invalid value to unflatten"));
            }
        }

        return *result;
    }


    Basic_json& get_unchecked(Basic_json* ptr) const
    {
        using size_type = typename Basic_json::size_type;
        for (const auto& reference_token : reference_tokens)
        {
            // convert null values to arrays or objects before continuing
            if (ptr->type_ == detail::Value::null)
            {
                // check if reference token is a number
                const bool nums =
                    std::all_of(reference_token.begin(), reference_token.end(),
                                [](const char x)
                {
                    return (x >= '0' and x <= '9');
                });

                // change value to array for numbers or "-" or to object otherwise
                *ptr = (nums or reference_token == "-")
                       ? detail::Value::array
                       : detail::Value::object;
            }

            switch (ptr->type_)
            {
                case detail::Value::object:
                {
                    // use unchecked object access
                    ptr = &ptr->operator[](reference_token);
                    break;
                }

                case detail::Value::array:
                {
                    // error condition (cf. RFC 6901, Sect. 4)
                    if ((reference_token.size() > 1 and reference_token[0] == '0'))
                    {
                        JSON_THROW(detail::parse_error::create(106, 0,
                                                               "array index '" + reference_token +
                                                               "' must not begin with '0'"));
                    }

                    if (reference_token == "-")
                    {
                        // explicitly treat "-" as index beyond the end
                        ptr = &ptr->operator[](ptr->value_.array->size());
                    }
                    else
                    {
                        // convert array index to number; unchecked access
                        JSON_TRY
                        {
                            ptr = &ptr->operator[](
                                static_cast<size_type>(array_index(reference_token)));
                        }
                        JSON_CATCH(std::invalid_argument&)
                        {
                            JSON_THROW(detail::parse_error::create(109, 0, "array index '" + reference_token + "' is not a number"));
                        }
                    }
                    break;
                }

                default:
                    JSON_THROW(detail::out_of_range::create(404, "unresolved reference token '" + reference_token + "'"));
            }
        }

        return *ptr;
    }

 
    Basic_json& get_checked(Basic_json* ptr) const
    {
        using size_type = typename Basic_json::size_type;
        for (const auto& reference_token : reference_tokens)
        {
            switch (ptr->type_)
            {
                case detail::Value::object:
                {
                    // note: at performs range check
                    ptr = &ptr->at(reference_token);
                    break;
                }

                case detail::Value::array:
                {
                    if ((reference_token == "-"))
                    {
                        // "-" always fails the range check
                        JSON_THROW(detail::out_of_range::create(402,
                                                                "array index '-' (" + std::to_string(ptr->value_.array->size()) +
                                                                ") is out of range"));
                    }

                    // error condition (cf. RFC 6901, Sect. 4)
                    if ((reference_token.size() > 1 and reference_token[0] == '0'))
                    {
                        JSON_THROW(detail::parse_error::create(106, 0,
                                                               "array index '" + reference_token +
                                                               "' must not begin with '0'"));
                    }

                    // note: at performs range check
                    JSON_TRY
                    {
                        ptr = &ptr->at(static_cast<size_type>(array_index(reference_token)));
                    }
                    JSON_CATCH(std::invalid_argument&)
                    {
                        JSON_THROW(detail::parse_error::create(109, 0, "array index '" + reference_token + "' is not a number"));
                    }
                    break;
                }

                default:
                    JSON_THROW(detail::out_of_range::create(404, "unresolved reference token '" + reference_token + "'"));
            }
        }

        return *ptr;
    }


    const Basic_json& get_unchecked(const Basic_json* ptr) const
    {
        using size_type = typename Basic_json::size_type;
        for (const auto& reference_token : reference_tokens)
        {
            switch (ptr->type_)
            {
                case detail::Value::object:
                {
                    // use unchecked object access
                    ptr = &ptr->operator[](reference_token);
                    break;
                }

                case detail::Value::array:
                {
                    if ((reference_token == "-"))
                    {
                        // "-" cannot be used for const access
                        JSON_THROW(detail::out_of_range::create(402,
                                                                "array index '-' (" + std::to_string(ptr->value_.array->size()) +
                                                                ") is out of range"));
                    }

                    // error condition (cf. RFC 6901, Sect. 4)
                    if ((reference_token.size() > 1 and reference_token[0] == '0'))
                    {
                        JSON_THROW(detail::parse_error::create(106, 0,
                                                               "array index '" + reference_token +
                                                               "' must not begin with '0'"));
                    }

                    // use unchecked array access
                    JSON_TRY
                    {
                        ptr = &ptr->operator[](
                            static_cast<size_type>(array_index(reference_token)));
                    }
                    JSON_CATCH(std::invalid_argument&)
                    {
                        JSON_THROW(detail::parse_error::create(109, 0, "array index '" + reference_token + "' is not a number"));
                    }
                    break;
                }

                default:
                    JSON_THROW(detail::out_of_range::create(404, "unresolved reference token '" + reference_token + "'"));
            }
        }

        return *ptr;
    }


    const Basic_json& get_checked(const Basic_json* ptr) const
    {
        using size_type = typename Basic_json::size_type;
        for (const auto& reference_token : reference_tokens)
        {
            switch (ptr->type_)
            {
                case detail::Value::object:
                {
                    // note: at performs range check
                    ptr = &ptr->at(reference_token);
                    break;
                }

                case detail::Value::array:
                {
                    if ((reference_token == "-"))
                    {
                        // "-" always fails the range check
                        JSON_THROW(detail::out_of_range::create(402,
                                                                "array index '-' (" + std::to_string(ptr->value_.array->size()) +
                                                                ") is out of range"));
                    }

                    // error condition (cf. RFC 6901, Sect. 4)
                    if ((reference_token.size() > 1 and reference_token[0] == '0'))
                    {
                        JSON_THROW(detail::parse_error::create(106, 0,
                                                               "array index '" + reference_token +
                                                               "' must not begin with '0'"));
                    }

                    // note: at performs range check
                    JSON_TRY
                    {
                        ptr = &ptr->at(static_cast<size_type>(array_index(reference_token)));
                    }
                    JSON_CATCH(std::invalid_argument&)
                    {
                        JSON_THROW(detail::parse_error::create(109, 0, "array index '" + reference_token + "' is not a number"));
                    }
                    break;
                }

                default:
                    JSON_THROW(detail::out_of_range::create(404, "unresolved reference token '" + reference_token + "'"));
            }
        }

        return *ptr;
    }


    static std::vector<std::string> split(const std::string& reference_string)
    {
        std::vector<std::string> result;

        // special case: empty reference string -> no reference tokens
        if (reference_string.empty())
        {
            return result;
        }

        // check if nonempty reference string begins with slash
        if ((reference_string[0] != '/'))
        {
            JSON_THROW(detail::parse_error::create(107, 1,
                                                   "JSON pointer must be empty or begin with '/' - was: '" +
                                                   reference_string + "'"));
        }

        // extract the reference tokens:
        // - slash: position of the last read slash (or end of string)
        // - start: position after the previous slash
        for (
            // search for the first slash after the first character
            std::size_t slash = reference_string.find_first_of('/', 1),
            // set the beginning of the first reference token
            start = 1;
            // we can stop if start == 0 (if slash == std::string::npos)
            start != 0;
            // set the beginning of the next reference token
            // (will eventually be 0 if slash == std::string::npos)
            start = (slash == std::string::npos) ? 0 : slash + 1,
            // find next slash
            slash = reference_string.find_first_of('/', start))
        {
            // use the text between the beginning of the reference token
            // (start) and the last slash (slash).
            auto reference_token = reference_string.substr(start, slash - start);

            // check reference tokens are properly escaped
            for (std::size_t pos = reference_token.find_first_of('~');
                    pos != std::string::npos;
                    pos = reference_token.find_first_of('~', pos + 1))
            {
                assert(reference_token[pos] == '~');

                // ~ must be followed by 0 or 1
                if ((pos == reference_token.size() - 1 or
                                  (reference_token[pos + 1] != '0' and
                                   reference_token[pos + 1] != '1')))
                {
                    JSON_THROW(detail::parse_error::create(108, 0, "escape character '~' must be followed with '0' or '1'"));
                }
            }

            // finally, store the reference token
            unescape(reference_token);
            result.push_back(reference_token);
        }

        return result;
    }


    static void replace_substring(std::string& s, const std::string& f,
                                  const std::string& t)
    {
        assert(not f.empty());
        for (auto pos = s.find(f);                // find first occurrence of f
                pos != std::string::npos;         // make sure f was found
                s.replace(pos, f.size(), t),      // replace with t, and
                pos = s.find(f, pos + t.size()))  // find next occurrence of f
        {}
    }

    /// escape "~" to "~0" and "/" to "~1"
    static std::string escape(std::string s)
    {
        replace_substring(s, "~", "~0");
        replace_substring(s, "/", "~1");
        return s;
    }

    /// unescape "~1" to tilde and "~0" to slash (order is important!)
    static void unescape(std::string& s)
    {
        replace_substring(s, "~1", "/");
        replace_substring(s, "~0", "~");
    }

 
    static void flatten(const std::string& reference_string,
                        const Basic_json& value,
                        Basic_json& result)
    {
        switch (value.type_)
        {
            case detail::Value::array:
            {
                if (value.value_.array->empty())
                {
                    // flatten empty array as null
                    result[reference_string] = nullptr;
                }
                else
                {
                    // iterate array and use index as reference string
                    for (std::size_t i = 0; i < value.value_.array->size(); ++i)
                    {
                        flatten(reference_string + "/" + std::to_string(i),
                                value.value_.array->operator[](i), result);
                    }
                }
                break;
            }

            case detail::Value::object:
            {
                if (value.value_.object->empty())
                {
                    // flatten empty object as null
                    result[reference_string] = nullptr;
                }
                else
                {
                    // iterate object and use keys as reference string
                    for (const auto& element : *value.value_.object)
                    {
                        flatten(reference_string + "/" + escape(element.first), element.second, result);
                    }
                }
                break;
            }

            default:
            {
                // add primitive value with its reference string
                result[reference_string] = value;
                break;
            }
        }
    }


    static Basic_json
    unflatten(const Basic_json& value)
    {
        if ((not value.is_object()))
        {
            JSON_THROW(detail::type_error::create(314, "only objects can be unflattened"));
        }

        Basic_json result;

        // iterate the JSON object values
        for (const auto& element : *value.value_.object)
        {
            if ((not element.second.is_primitive()))
            {
                JSON_THROW(detail::type_error::create(315, "values in object must be primitive"));
            }

            // assign value to reference pointed to by JSON pointer; Note that if
            // the JSON pointer is "" (i.e., points to the whole value), function
            // get_and_create returns a reference to result itself. An assignment
            // will then create a primitive value.
            Json_ptr(element.first).get_and_create(result) = element.second;
        }

        return result;
    }

    friend bool operator==(Json_ptr const& lhs,
                           Json_ptr const& rhs) 
    {
        return (lhs.reference_tokens == rhs.reference_tokens);
    }

    friend bool operator!=(Json_ptr const& lhs,
                           Json_ptr const& rhs) 
    {
        return not (lhs == rhs);
    }

    /// the reference tokens
    std::vector<std::string> reference_tokens;
};
}  // namespace nlohmann

// #include <nlohmann/adl_serializer.hpp>


#include <utility>




namespace nlohmann
{

template<typename, typename>
struct adl_serializer
{
    /*!
    @brief convert a JSON value to any value type

    This function is usually called by the `get()` function of the
    @ref basic_json class (either explicit or via conversion operators).

    @param[in] j        JSON value to read from
    @param[in,out] val  value to write to
    */
    template<typename Basic_json, typename ValueType>
    static auto from_json(Basic_json&& j, ValueType& val) (
        (::nlohmann::from_json(std::forward<Basic_json>(j), val)))
    -> decltype(::nlohmann::from_json(std::forward<Basic_json>(j), val), void())
    {
        ::nlohmann::from_json(std::forward<Basic_json>(j), val);
    }

    /*!
    @brief convert any value type to a JSON value

    This function is usually called by the constructors of the @ref basic_json
    class.

    @param[in,out] j  JSON value to write to
    @param[in] val    value to read from
    */
    template <typename Basic_json, typename ValueType>
    static auto to_json(Basic_json& j, ValueType&& val) (
        (::nlohmann::to_json(j, std::forward<ValueType>(val))))
    -> decltype(::nlohmann::to_json(j, std::forward<ValueType>(val)), void())
    {
        ::nlohmann::to_json(j, std::forward<ValueType>(val));
    }
};

}  // namespace nlohmann

namespace nlohmann
{


NLOHMANN_BASIC_JSON_TPL_DECLARATION
class basic_json
{
private:
	template<detail::Value> friend struct detail::external_constructor;
    friend ::nlohmann::Json_ptr<basic_json>;
    friend ::nlohmann::detail::Parser<basic_json>;
    friend ::nlohmann::detail::serializer<basic_json>;
    template<typename Basic_json>
    friend class ::nlohmann::detail::Iter_impl;
    template<typename Basic_json, typename CharType>
    friend class ::nlohmann::detail::Binary_writer;
    template<typename Basic_json, typename SAX>
    friend class ::nlohmann::detail::Binary_reader;
    template<typename Basic_json>
    friend class ::nlohmann::detail::json_sax_dom_parser;
    template<typename Basic_json>
    friend class ::nlohmann::detail::json_sax_dom_callback_parser;

    /// workaround type for MSVC
    using basic_json_t = NLOHMANN_BASIC_JSON_TPL;

    // convenience aliases for types residing in namespace detail;
    using Lexer = ::nlohmann::detail::Lexer<basic_json>;
    using Parser = ::nlohmann::detail::Parser<basic_json>;

    using primitive_iterator_t = ::nlohmann::detail::primitive_iterator_t;
    template<typename Basic_json>
    using internal_iterator = ::nlohmann::detail::internal_iterator<Basic_json>;
    template<typename Basic_json>
    using Iter_impl = ::nlohmann::detail::Iter_impl<Basic_json>;
    template<typename Iterator>
    using iteration_proxy = ::nlohmann::detail::iteration_proxy<Iterator>;
    template<typename Base> using Json_reverse_iterator = ::nlohmann::detail::Json_reverse_iterator<Base>;

    template<typename CharType>
    using output_adapter_t = ::nlohmann::detail::output_adapter_t<CharType>;

    using Binary_reader = ::nlohmann::detail::Binary_reader<basic_json>;
    template<typename CharType> using Binary_writer = ::nlohmann::detail::Binary_writer<basic_json, CharType>;

    using serializer = ::nlohmann::detail::serializer<basic_json>;
public:
	using Value = detail::Value;
    /// JSON Pointer, see @ref nlohmann::Json_ptr
    using Json_ptr = ::nlohmann::Json_ptr<basic_json>;
    template<typename T, typename SFINAE>
    using json_serializer = Serializer<T, SFINAE>;
    /// how to treat decoding errors
    using error_handler_t = detail::error_handler_t;
    /// helper type for initializer lists of basic_json values
    using Initializer_list = std::initializer_list<detail::json_ref<basic_json>>;

    using input_format_t = detail::input_format_t;
    /// SAX interface type, see @ref nlohmann::json_sax
    using json_sax_t = json_sax<basic_json>;

    ////////////////
    // exceptions //
    ////////////////

    /// @name exceptions
    /// Classes to implement user-defined exceptions.
    /// @{

    /// @copydoc detail::exception
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

    /// @}


    /////////////////////
    // container types //
    /////////////////////

    /// @name container types
    /// The canonic container types to use @ref basic_json like any other STL
    /// container.
    /// @{

    /// the type of elements in a basic_json container
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
    using allocator_type = Allocator<basic_json>;

    /// the type of an element pointer
    using pointer = typename std::allocator_traits<allocator_type>::pointer;
    /// the type of an element const pointer
    using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

    using iterator = Iter_impl<basic_json>;
    using const_iterator = Iter_impl<const basic_json>;

    using reverse_iterator = Json_reverse_iterator<typename basic_json::iterator>;
    using const_reverse_iterator = Json_reverse_iterator<typename basic_json::const_iterator>;

    static allocator_type get_allocator()
    {
        return allocator_type();
    }

    using Object = Object<String,
          basic_json,
          std::less<>,
          Allocator<std::pair<const String,
          basic_json>>>;

    using Array = Array<basic_json, Allocator<basic_json>>;
    using String = String;
    using Boolean = Boolean;
    using Num_integer = Num_integer;
    using Num_unsigned = Num_unsigned;
    using Num_float = Num_float;
private:
    template<typename T, typename... Args>
    static T* create(Args&& ... args)
    {
        Allocator<T> alloc;
        using Allocator_traits = std::allocator_traits<Allocator<T>>;

        auto deleter = [&](T * object)
        {
            Allocator_traits::deallocate(alloc, object, 1);
        };
        std::unique_ptr<T, decltype(deleter)> object(Allocator_traits::allocate(alloc, 1), deleter);
        Allocator_traits::construct(alloc, object.get(), std::forward<Args>(args)...);
        assert(object != nullptr);
        return object.release();
    }

 
    union Json_value
    {
        Object* object;
        Array* array;
        String* string;
        Boolean boolean;
        Num_integer num_integer;
        Num_unsigned num_unsigned;
        Num_float num_float;

        Json_value() = default;
        Json_value(Boolean v)  : boolean(v) {}
        Json_value(Num_integer v)  : num_integer(v) {}
        Json_value(Num_unsigned v)  : num_unsigned(v) {}
        Json_value(Num_float v)  : num_float(v) {}

        Json_value(Value t)
        {
            switch (t)
            {
                case Value::object:
                {
                    object = create<Object>();
                    break;
                }
                case Value::array:
                {
                    array = create<Array>();
                    break;
                }
                case Value::string:
                {
                    string = create<String>("");
                    break;
                }
                case Value::boolean:
                {
                    boolean = Boolean(false);
                    break;
                }
                case Value::num_integer:
                {
                    num_integer = Num_integer(0);
                    break;
                }
                case Value::num_unsigned:
                {
                    num_unsigned = Num_unsigned(0);
                    break;
                }
                case Value::num_float:
                {
                    num_float = Num_float(0.0);
                    break;
                }
                case Value::null:
                {
                    object = nullptr; 
                    break;
                }
                default:
                {
                    object = nullptr;  
                    if ((t == Value::null))
                    {
			throw "Expatient."
                    }
                    break;
                }
            }
        }

        Json_value(const String& value)
        {
            string = create<String>(value);
        }

        Json_value(String&& value)
        {
            string = create<String>(std::move(value));
        }

        Json_value(const Object& value)
        {
            object = create<Object>(value);
        }

        Json_value(Object&& value)
        {
            object = create<Object>(std::move(value));
        }

        Json_value(const Array& value)
        {
            array = create<Array>(value);
        }

        Json_value(Array&& value)
        {
            array = create<Array>(std::move(value));
        }

        void destroy(Value t) 
        {
            switch (t)
            {
                case Value::object:
                {
                    Allocator<Object> alloc;
                    std::allocator_traits<decltype(alloc)>::destroy(alloc, object);
                    std::allocator_traits<decltype(alloc)>::deallocate(alloc, object, 1);
                    break;
                }
                case Value::array:
                {
                    Allocator<Array> alloc;
                    std::allocator_traits<decltype(alloc)>::destroy(alloc, array);
                    std::allocator_traits<decltype(alloc)>::deallocate(alloc, array, 1);
                    break;
                }
                case Value::string:
                {
                    Allocator<String> alloc;
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
        assert(type_ != Value::object or value_.object != nullptr);
        assert(type_ != Value::array or value_.array != nullptr);
        assert(type_ != Value::string or value_.string != nullptr);
    }
public:
    using Parse_event = typename Parser::Parse_event;

 
    using parser_callback_t = typename Parser::parser_callback_t;


	basic_json(const Value v) : type_(v), value_(v) { assert_invariant();}

    basic_json(std::nullptr_t = nullptr)  : basic_json(Value::null)
    {
        assert_invariant();
    }

    template <typename Compatible,
              typename U = detail::uncvref_t<Compatible>,
              detail::enable_if_t<
                  not detail::Is_basic_json<U>::value and 
				  detail::Is_compatible_type<basic_json_t, U>::value, 
			  int> = 0>
    basic_json(Compatible && val) ((
				Serializer<U>::to_json(std::declval<basic_json_t&>(),
					std::forward<Compatible>(val))))
    {
        Serializer<U>::to_json(*this, std::forward<Compatible>(val));
        assert_invariant();
    }

 
    template <typename Basic_json,
              detail::enable_if_t<
                  detail::Is_basic_json<Basic_json>::value and 
				  not std::is_same<basic_json, Basic_json>::value, 
			  int> = 0>
    basic_json(const Basic_json& val)
    {
        using other_boolean_t = typename Basic_json::Boolean;
        using other_number_float_t = typename Basic_json::Num_float;
        using other_number_integer_t = typename Basic_json::Num_integer;
        using other_number_unsigned_t = typename Basic_json::Num_unsigned;
        using other_string_t = typename Basic_json::String;
        using other_object_t = typename Basic_json::Object;
        using other_array_t = typename Basic_json::Array;

        switch (val.type())
        {
            case Value::boolean:
                Serializer<other_boolean_t>::to_json(*this, val.template get<other_boolean_t>());
                break;
            case Value::num_float:
                Serializer<other_number_float_t>::to_json(*this, val.template get<other_number_float_t>());
                break;
            case Value::num_integer:
                Serializer<other_number_integer_t>::to_json(*this, val.template get<other_number_integer_t>());
                break;
            case Value::num_unsigned:
                Serializer<other_number_unsigned_t>::to_json(*this, val.template get<other_number_unsigned_t>());
                break;
            case Value::string:
                Serializer<other_string_t>::to_json(*this, val.template get_ref<const other_string_t&>());
                break;
            case Value::object:
                Serializer<other_object_t>::to_json(*this, val.template get_ref<const other_object_t&>());
                break;
            case Value::array:
                Serializer<other_array_t>::to_json(*this, val.template get_ref<const other_array_t&>());
                break;
            case Value::null:
                *this = nullptr;
                break;
            case Value::discarded:
                type_ = Value::discarded;
                break;
        }
        assert_invariant();
    }

  
    basic_json(Initializer_list init,
               bool type_deduction = true,
               Value manual_type = Value::array)
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
            if (manual_type == Value::array)
            {
                is_an_object = false;
            }

            // if object is wanted but impossible, throw an exception
            if ((manual_type == Value::object and not is_an_object))
            {
                JSON_THROW(type_error::create(301, "cannot create object from initializer list"));
            }
        }

        if (is_an_object)
        {
            // the initializer list is a list of pairs -> create object
            type_ = Value::object;
            value_ = Value::object;

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
            type_ = Value::array;
            value_.array = create<Array>(init.begin(), init.end());
        }

        assert_invariant();
    }

 
    static basic_json array(Initializer_list init = {})
    {
        return basic_json(init, false, Value::array);
    }

  
    static basic_json object(Initializer_list init = {})
    {
        return basic_json(init, false, Value::object);
    }

 
    basic_json(size_type cnt, const basic_json& val)
        : type_(Value::array)
    {
        value_.array = create<Array>(cnt, val);
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
        if ((first.m_object != last.m_object))
        {
            JSON_THROW(invalid_iterator::create(201, "iterators are not compatible"));
        }

        // copy type from first iterator
        type_ = first.m_object->type_;

        // check if iterator range is complete for primitive values
        switch (type_)
        {
            case Value::boolean:
            case Value::num_float:
            case Value::num_integer:
            case Value::num_unsigned:
            case Value::string:
            {
                if ((not first.m_it.primitive_iterator.is_begin()
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
            case Value::num_integer:
            {
                value_.num_integer = first.m_object->value_.num_integer;
                break;
            }

            case Value::num_unsigned:
            {
                value_.num_unsigned = first.m_object->value_.num_unsigned;
                break;
            }

            case Value::num_float:
            {
                value_.num_float = first.m_object->value_.num_float;
                break;
            }

            case Value::boolean:
            {
                value_.boolean = first.m_object->value_.boolean;
                break;
            }

            case Value::string:
            {
                value_ = *first.m_object->value_.string;
                break;
            }

            case Value::object:
            {
                value_.object = create<Object>(first.m_it.object_iterator,
                                                  last.m_it.object_iterator);
                break;
            }

            case Value::array:
            {
                value_.array = create<Array>(first.m_it.array_iterator,
                                                last.m_it.array_iterator);
                break;
            }

            default:
                JSON_THROW(invalid_iterator::create(206, "cannot construct with iterators from " +
                                                    std::string(first.m_object->type_name())));
        }

        assert_invariant();
    }


    basic_json(const detail::json_ref<basic_json>& ref)
        : basic_json(ref.moved_or_copied())
    {}

 
    basic_json(const basic_json& other) : type_(other.type_)
    {
        // check of passed value is valid
        other.assert_invariant();

        switch (type_)
        {
            case Value::object:
            {
                value_ = *other.value_.object;
                break;
            }

            case Value::array:
            {
                value_ = *other.value_.array;
                break;
            }

            case Value::string:
            {
                value_ = *other.value_.string;
                break;
            }

            case Value::boolean:
            {
                value_ = other.value_.boolean;
                break;
            }

            case Value::num_integer:
            {
                value_ = other.value_.num_integer;
                break;
            }

            case Value::num_unsigned:
            {
                value_ = other.value_.num_unsigned;
                break;
            }

            case Value::num_float:
            {
                value_ = other.value_.num_float;
                break;
            }

            default:
                break;
        }

        assert_invariant();
    }

 
    basic_json(basic_json&& other)  : type_(std::move(other.type_)),
          value_(std::move(other.value_))
    {
        // check that passed value is valid
        other.assert_invariant();

        // invalidate payload
        other.type_ = Value::null;
        other.value_ = {};

        assert_invariant();
    }

 
    basic_json& operator=(basic_json other)  (
        std::is_nothrow_move_constructible<Value>::value and
        std::is_nothrow_move_assignable<Value>::value and
        std::is_nothrow_move_constructible<Json_value>::value and
        std::is_nothrow_move_assignable<Json_value>::value
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
  public:

    String dump(const int indent = -1,
                  const char indent_char = ' ',
                  const bool ensure_ascii = false,
                  const error_handler_t error_handler = error_handler_t::strict) const
    {
        String result;
        serializer s(detail::output_adapter<char, String>(result), indent_char, error_handler);

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

 
    constexpr Value type() const { return type_; }

    constexpr bool is_primitive() const 
    {
        return is_null() or is_string() or is_boolean() or is_number();
    }

  
    constexpr bool is_structured() const 
    {
        return is_array() or is_object();
    }

  
    constexpr bool is_null() const { return (type_ == Value::null); }

 
    constexpr bool is_boolean() const 
    {
        return (type_ == Value::boolean);
    }
   
    constexpr bool is_number() const 
    {
        return is_number_integer() or is_number_float();
    }

    constexpr bool is_number_integer() const 
    {
        return (type_ == Value::num_integer or type_ == Value::num_unsigned);
    }
 
    constexpr bool is_number_unsigned() const 
    {
        return (type_ == Value::num_unsigned);
    }
 
    constexpr bool is_number_float() const 
	{
		return (type_ == Value::num_float);
    }

    constexpr bool is_object() const 
    {
        return (type_ == Value::object);
    }
 
    constexpr bool is_array() const { return (type_ == Value::array); }
  
    constexpr bool is_string() const 
    {
        return (type_ == Value::string);
    }

    constexpr bool is_discarded() const 
    {
        return (type_ == Value::discarded);
    }

 
    constexpr operator Value() const { return type_; }
  private:

    /// get a boolean (explicit)
    Boolean get_impl(Boolean* /*unused*/) const
    {
        if ((is_boolean()))
        {
            return value_.boolean;
        }

        JSON_THROW(type_error::create(302, "type must be boolean, but is " + std::string(type_name())));
    }

    Object* get_impl_ptr(Object* /*unused*/) 
    {
        return is_object() ? value_.object : nullptr;
    }

    constexpr const Object* get_impl_ptr(const Object* /*unused*/) const 
    {
        return is_object() ? value_.object : nullptr;
    }

    Array* get_impl_ptr(Array* /*unused*/) 
    {
        return is_array() ? value_.array : nullptr;
    }

    constexpr const Array* get_impl_ptr(const Array* /*unused*/) const 
    {
        return is_array() ? value_.array : nullptr;
    }

    String* get_impl_ptr(String* /*unused*/) 
    {
        return is_string() ? value_.string : nullptr;
    }

    constexpr const String* get_impl_ptr(const String* /*unused*/) const 
    {
        return is_string() ? value_.string : nullptr;
    }

    Boolean* get_impl_ptr(Boolean* /*unused*/) 
    {
        return is_boolean() ? &value_.boolean : nullptr;
    }

    constexpr const Boolean* get_impl_ptr(const Boolean* /*unused*/) const 
    {
        return is_boolean() ? &value_.boolean : nullptr;
    }

    Num_integer* get_impl_ptr(Num_integer* /*unused*/) 
    {
        return is_number_integer() ? &value_.num_integer : nullptr;
    }

    constexpr const Num_integer* get_impl_ptr(const Num_integer* /*unused*/) const 
    {
        return is_number_integer() ? &value_.num_integer : nullptr;
    }

    Num_unsigned* get_impl_ptr(Num_unsigned* /*unused*/) 
    {
        return is_number_unsigned() ? &value_.num_unsigned : nullptr;
    }

    constexpr const Num_unsigned* get_impl_ptr(const Num_unsigned* /*unused*/) const 
    {
        return is_number_unsigned() ? &value_.num_unsigned : nullptr;
    }

    Num_float* get_impl_ptr(Num_float* /*unused*/) 
    {
        return is_number_float() ? &value_.num_float : nullptr;
    }

    constexpr const Num_float* get_impl_ptr(const Num_float* /*unused*/) const 
    {
        return is_number_float() ? &value_.num_float : nullptr;
    }


    template<typename ReferenceType, typename ThisType>
    static ReferenceType get_ref_impl(ThisType& obj)
    {
        // delegate the call to get_ptr<>()
        auto ptr = obj.template get_ptr<typename std::add_pointer<ReferenceType>::type>();

        if ((ptr != nullptr))
        {
            return *ptr;
        }

        JSON_THROW(type_error::create(303, "incompatible ReferenceType for get_ref, actual type is " + std::string(obj.type_name())));
    }

  public:
  
    template<typename Basic_json, detail::enable_if_t<
                 std::is_same<typename std::remove_const<Basic_json>::type, basic_json_t>::value,
                 int> = 0>
    basic_json get() const
    {
        return *this;
    }

 
    template<typename Basic_json, detail::enable_if_t<
                 not std::is_same<Basic_json, basic_json>::value and
                 detail::Is_basic_json<Basic_json>::value, int> = 0>
    Basic_json get() const
    {
        return *this;
    }

    template<typename ValueTypeCV, typename ValueType = detail::uncvref_t<ValueTypeCV>,
             detail::enable_if_t <
                 not detail::Is_basic_json<ValueType>::value and
                 detail::has_from_json<basic_json_t, ValueType>::value and
                 not detail::has_non_default_from_json<basic_json_t, ValueType>::value,
                 int> = 0>
    ValueType get() const ((
                                       Serializer<ValueType>::from_json(std::declval<const basic_json_t&>(), std::declval<ValueType&>())))
    {
        // we cannot static_assert on ValueTypeCV being non-const, because
        // there is support for get<const basic_json_t>(), which is why we
        // still need the uncvref
        static_assert(not std::is_reference<ValueTypeCV>::value,
                      "get() cannot be used with reference types, you might want to use get_ref()");
        static_assert(std::is_default_constructible<ValueType>::value,
                      "types must be DefaultConstructible when used with get()");

        ValueType ret;
        Serializer<ValueType>::from_json(*this, ret);
        return ret;
    }


    template<typename ValueTypeCV, typename ValueType = detail::uncvref_t<ValueTypeCV>,
             detail::enable_if_t<not std::is_same<basic_json_t, ValueType>::value and
                                 detail::has_non_default_from_json<basic_json_t, ValueType>::value,
                                 int> = 0>
    ValueType get() const ((
                                       Serializer<ValueTypeCV>::from_json(std::declval<const basic_json_t&>())))
    {
        static_assert(not std::is_reference<ValueTypeCV>::value,
                      "get() cannot be used with reference types, you might want to use get_ref()");
        return Serializer<ValueTypeCV>::from_json(*this);
    }


    template<typename ValueType,
             detail::enable_if_t <
                 not detail::Is_basic_json<ValueType>::value and
                 detail::has_from_json<basic_json_t, ValueType>::value,
                 int> = 0>
    ValueType & get_to(ValueType& v) const ((
                Serializer<ValueType>::from_json(std::declval<const basic_json_t&>(), v)))
    {
        Serializer<ValueType>::from_json(*this, v);
        return v;
    }



    template<typename PointerType, typename std::enable_if<
                 std::is_pointer<PointerType>::value, int>::type = 0>
    auto get_ptr()  -> decltype(std::declval<basic_json_t&>().get_impl_ptr(std::declval<PointerType>()))
    {
        // delegate the call to get_impl_ptr<>()
        return get_impl_ptr(static_cast<PointerType>(nullptr));
    }


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
                   not std::is_same<ValueType, typename String::value_type>::value and
                   not detail::Is_basic_json<ValueType>::value

#ifndef _MSC_VER  // fix for issue #167 operator<< ambiguity under VS2015
                   and not std::is_same<ValueType, std::initializer_list<typename String::value_type>>::value
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
        if ((is_array()))
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
        if ((is_array()))
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

 
    reference at(const typename Object::key_type& key)
    {
        // at only works for objects
        if ((is_object()))
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

 
    const_reference at(const typename Object::key_type& key) const
    {
        // at only works for objects
        if ((is_object()))
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
            type_ = Value::array;
            value_.array = create<Array>();
            assert_invariant();
        }

        // operator[] only works for arrays
        if ((is_array()))
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
        if ((is_array()))
        {
            return value_.array->operator[](idx);
        }

        JSON_THROW(type_error::create(305, "cannot use operator[] with a numeric argument with " + std::string(type_name())));
    }

  
    reference operator[](const typename Object::key_type& key)
    {
        // implicitly convert null value to an empty object
        if (is_null())
        {
            type_ = Value::object;
            value_.object = create<Object>();
            assert_invariant();
        }

        // operator[] only works for objects
        if ((is_object()))
        {
            return value_.object->operator[](key);
        }

        JSON_THROW(type_error::create(305, "cannot use operator[] with a string argument with " + std::string(type_name())));
    }

 
    const_reference operator[](const typename Object::key_type& key) const
    {
        // const operator[] only works for objects
        if ((is_object()))
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
            type_ = Value::object;
            value_ = Value::object;
            assert_invariant();
        }

        // at only works for objects
        if ((is_object()))
        {
            return value_.object->operator[](key);
        }

        JSON_THROW(type_error::create(305, "cannot use operator[] with a string argument with " + std::string(type_name())));
    }

    template<typename T>
    const_reference operator[](T* key) const
    {
        // at only works for objects
        if ((is_object()))
        {
            assert(value_.object->find(key) != value_.object->end());
            return value_.object->find(key)->second;
        }

        JSON_THROW(type_error::create(305, "cannot use operator[] with a string argument with " + std::string(type_name())));
    }

 
    template<class ValueType, typename std::enable_if<
                 std::is_convertible<basic_json_t, ValueType>::value, int>::type = 0>
    ValueType value(const typename Object::key_type& key, const ValueType& default_value) const
    {
        // at only works for objects
        if ((is_object()))
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
    @copydoc basic_json::value(const typename Object::key_type&, const ValueType&) const
    */
    String value(const typename Object::key_type& key, const char* default_value) const
    {
        return value(key, String(default_value));
    }

    template<class ValueType, typename std::enable_if<
                 std::is_convertible<basic_json_t, ValueType>::value, int>::type = 0>
    ValueType value(const Json_ptr& ptr, const ValueType& default_value) const
    {
        // at only works for objects
        if ((is_object()))
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
    @copydoc basic_json::value(const Json_ptr&, ValueType) const
    */
    String value(const Json_ptr& ptr, const char* default_value) const
    {
        return value(ptr, String(default_value));
    }

 
    reference front()
    {
        return *begin();
    }

    /*!
    @copydoc basic_json::front()
    */
    const_reference front() const
    {
        return *cbegin();
    }

  
    reference back()
    {
        auto tmp = end();
        --tmp;
        return *tmp;
    }

    /*!
    @copydoc basic_json::back()
    */
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
        if ((this != pos.m_object))
        {
            JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
        }

        IteratorType result = end();

        switch (type_)
        {
            case Value::boolean:
            case Value::num_float:
            case Value::num_integer:
            case Value::num_unsigned:
            case Value::string:
            {
                if ((not pos.m_it.primitive_iterator.is_begin()))
                {
                    JSON_THROW(invalid_iterator::create(205, "iterator out of range"));
                }

                if (is_string())
                {
                    Allocator<String> alloc;
                    std::allocator_traits<decltype(alloc)>::destroy(alloc, value_.string);
                    std::allocator_traits<decltype(alloc)>::deallocate(alloc, value_.string, 1);
                    value_.string = nullptr;
                }

                type_ = Value::null;
                assert_invariant();
                break;
            }

            case Value::object:
            {
                result.m_it.object_iterator = value_.object->erase(pos.m_it.object_iterator);
                break;
            }

            case Value::array:
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
        if ((this != first.m_object or this != last.m_object))
        {
            JSON_THROW(invalid_iterator::create(203, "iterators do not fit current value"));
        }

        IteratorType result = end();

        switch (type_)
        {
            case Value::boolean:
            case Value::num_float:
            case Value::num_integer:
            case Value::num_unsigned:
            case Value::string:
            {
                if ((not first.m_it.primitive_iterator.is_begin()
                                or not last.m_it.primitive_iterator.is_end()))
                {
                    JSON_THROW(invalid_iterator::create(204, "iterators out of range"));
                }

                if (is_string())
                {
                    Allocator<String> alloc;
                    std::allocator_traits<decltype(alloc)>::destroy(alloc, value_.string);
                    std::allocator_traits<decltype(alloc)>::deallocate(alloc, value_.string, 1);
                    value_.string = nullptr;
                }

                type_ = Value::null;
                assert_invariant();
                break;
            }

            case Value::object:
            {
                result.m_it.object_iterator = value_.object->erase(first.m_it.object_iterator,
                                              last.m_it.object_iterator);
                break;
            }

            case Value::array:
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

  
    size_type erase(const typename Object::key_type& key)
    {
        // this erase only works for objects
        if ((is_object()))
        {
            return value_.object->erase(key);
        }

        JSON_THROW(type_error::create(307, "cannot use erase() with " + std::string(type_name())));
    }


    void erase(const size_type idx)
    {
        // this erase only works for arrays
        if ((is_array()))
        {
            if ((idx >= size()))
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

    /*!
    @brief find an element in a JSON object
    @copydoc find(KeyT&&)
    */
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

    /// @}


  
    iterator begin() 
    {
        iterator result(this);
        result.set_begin();
        return result;
    }

    /*!
    @copydoc basic_json::cbegin()
    */
    const_iterator begin() const 
    {
        return cbegin();
    }

   
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

    /*!
    @copydoc basic_json::cend()
    */
    const_iterator end() const 
    {
        return cend();
    }

  
    const_iterator cend() const 
    {
        const_iterator result(this);
        result.set_end();
        return result;
    }

  
    reverse_iterator rbegin() 
    {
        return reverse_iterator(end());
    }

    /*!
    @copydoc basic_json::crbegin()
    */
    const_reverse_iterator rbegin() const 
    {
        return crbegin();
    }

   
    reverse_iterator rend() 
    {
        return reverse_iterator(begin());
    }

    /*!
    @copydoc basic_json::crend()
    */
    const_reverse_iterator rend() const 
    {
        return crend();
    }

   
    const_reverse_iterator crbegin() const 
    {
        return const_reverse_iterator(cend());
    }

   
    const_reverse_iterator crend() const 
    {
        return const_reverse_iterator(cbegin());
    }

  public:
  
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
            case Value::null:
                return true;
            case Value::array:
                return value_.array->empty();
            case Value::object:
                return value_.object->empty();
            default:
                return false;
        }
    }

 
    size_type size() const 
    {
        switch (type_)
        {
            case Value::null:
                return 0;
            case Value::array:
                return value_.array->size();
            case Value::object:
                return value_.object->size();
            default:
                return 1;
        }
    }

  
    size_type max_size() const 
    {
        switch (type_)
        {
            case Value::array:
                return value_.array->max_size();
            case Value::object:
                return value_.object->max_size();
            default:
                return size();
        }
    }

  
    void clear() 
    {
        switch (type_)
        {
            case Value::num_integer:
            {
                value_.num_integer = 0;
                break;
            }
            case Value::num_unsigned:
            {
                value_.num_unsigned = 0;
                break;
            }
            case Value::num_float:
            {
                value_.num_float = 0.0;
                break;
            }
            case Value::boolean:
            {
                value_.boolean = false;
                break;
            }
            case Value::string:
            {
                value_.string->clear();
                break;
            }
            case Value::array:
            {
                value_.array->clear();
                break;
            }
            case Value::object:
            {
                value_.object->clear();
                break;
            }
            default:
                break;
        }
    }

  
    void push_back(basic_json&& val)
    {
        // push_back only works for null objects or arrays
        if ((not(is_null() or is_array())))
        {
            JSON_THROW(type_error::create(308, "cannot use push_back() with " + std::string(type_name())));
        }

        // transform null object into an array
        if (is_null())
        {
            type_ = Value::array;
            value_ = Value::array;
            assert_invariant();
        }

        // add element to array (move semantics)
        value_.array->push_back(std::move(val));
        // invalidate object
        val.type_ = Value::null;
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
        if ((not(is_null() or is_array())))
        {
            JSON_THROW(type_error::create(308, "cannot use push_back() with " + std::string(type_name())));
        }

        // transform null object into an array
        if (is_null())
        {
            type_ = Value::array;
            value_ = Value::array;
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

  
    void push_back(const typename Object::value_type& val)
    {
        // push_back only works for null objects or objects
        if ((not(is_null() or is_object())))
        {
            JSON_THROW(type_error::create(308, "cannot use push_back() with " + std::string(type_name())));
        }

        // transform null object into an object
        if (is_null())
        {
            type_ = Value::object;
            value_ = Value::object;
            assert_invariant();
        }

        // add element to array
        value_.object->insert(val);
    }

    /*!
    @brief add an object to an object
    @copydoc push_back(const typename Object::value_type&)
    */
    reference operator+=(const typename Object::value_type& val)
    {
        push_back(val);
        return *this;
    }

  
    void push_back(Initializer_list init)
    {
        if (is_object() and init.size() == 2 and (*init.begin())->is_string())
        {
            basic_json&& key = init.begin()->moved_or_copied();
            push_back(typename Object::value_type(
                          std::move(key.get_ref<String&>()), (init.begin() + 1)->moved_or_copied()));
        }
        else
        {
            push_back(basic_json(init));
        }
    }

    /*!
    @brief add an object to an object
    @copydoc push_back(Initializer_list)
    */
    reference operator+=(Initializer_list init)
    {
        push_back(init);
        return *this;
    }

  
    template<class... Args>
    void emplace_back(Args&& ... args)
    {
        // emplace_back only works for null objects or arrays
        if ((not(is_null() or is_array())))
        {
            JSON_THROW(type_error::create(311, "cannot use emplace_back() with " + std::string(type_name())));
        }

        // transform null object into an array
        if (is_null())
        {
            type_ = Value::array;
            value_ = Value::array;
            assert_invariant();
        }

        // add element to array (perfect forwarding)
        value_.array->emplace_back(std::forward<Args>(args)...);
    }

  
    template<class... Args>
    std::pair<iterator, bool> emplace(Args&& ... args)
    {
        // emplace only works for null objects or arrays
        if ((not(is_null() or is_object())))
        {
            JSON_THROW(type_error::create(311, "cannot use emplace() with " + std::string(type_name())));
        }

        // transform null object into an object
        if (is_null())
        {
            type_ = Value::object;
            value_ = Value::object;
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
        if ((is_array()))
        {
            // check if iterator pos fits to this JSON value
            if ((pos.m_object != this))
            {
                JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
            }

            // insert to array and return iterator
            return insert_iterator(pos, val);
        }

        JSON_THROW(type_error::create(309, "cannot use insert() with " + std::string(type_name())));
    }

    /*!
    @brief inserts element
    @copydoc insert(const_iterator, const basic_json&)
    */
    iterator insert(const_iterator pos, basic_json&& val)
    {
        return insert(pos, val);
    }

 
    iterator insert(const_iterator pos, size_type cnt, const basic_json& val)
    {
        // insert only works for arrays
        if ((is_array()))
        {
            // check if iterator pos fits to this JSON value
            if ((pos.m_object != this))
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
        if ((not is_array()))
        {
            JSON_THROW(type_error::create(309, "cannot use insert() with " + std::string(type_name())));
        }

        // check if iterator pos fits to this JSON value
        if ((pos.m_object != this))
        {
            JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
        }

        // check if range iterators belong to the same JSON object
        if ((first.m_object != last.m_object))
        {
            JSON_THROW(invalid_iterator::create(210, "iterators do not fit"));
        }

        if ((first.m_object == this))
        {
            JSON_THROW(invalid_iterator::create(211, "passed iterators may not belong to container"));
        }

        // insert to array and return iterator
        return insert_iterator(pos, first.m_it.array_iterator, last.m_it.array_iterator);
    }

  
    iterator insert(const_iterator pos, Initializer_list ilist)
    {
        // insert only works for arrays
        if ((not is_array()))
        {
            JSON_THROW(type_error::create(309, "cannot use insert() with " + std::string(type_name())));
        }

        // check if iterator pos fits to this JSON value
        if ((pos.m_object != this))
        {
            JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
        }

        // insert to array and return iterator
        return insert_iterator(pos, ilist.begin(), ilist.end());
    }

  
    void insert(const_iterator first, const_iterator last)
    {
        // insert only works for objects
        if ((not is_object()))
        {
            JSON_THROW(type_error::create(309, "cannot use insert() with " + std::string(type_name())));
        }

        // check if range iterators belong to the same JSON object
        if ((first.m_object != last.m_object))
        {
            JSON_THROW(invalid_iterator::create(210, "iterators do not fit"));
        }

        // passed iterators must belong to objects
        if ((not first.m_object->is_object()))
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
            type_ = Value::object;
            value_.object = create<Object>();
            assert_invariant();
        }

        if ((not is_object()))
        {
            JSON_THROW(type_error::create(312, "cannot use update() with " + std::string(type_name())));
        }
        if ((not j.is_object()))
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
            type_ = Value::object;
            value_.object = create<Object>();
            assert_invariant();
        }

        if ((not is_object()))
        {
            JSON_THROW(type_error::create(312, "cannot use update() with " + std::string(type_name())));
        }

        // check if range iterators belong to the same JSON object
        if ((first.m_object != last.m_object))
        {
            JSON_THROW(invalid_iterator::create(210, "iterators do not fit"));
        }

        // passed iterators must belong to objects
        if ((not first.m_object->is_object()
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
        std::is_nothrow_move_constructible<Value>::value and
        std::is_nothrow_move_assignable<Value>::value and
        std::is_nothrow_move_constructible<Json_value>::value and
        std::is_nothrow_move_assignable<Json_value>::value
    )
    {
        std::swap(type_, other.type_);
        std::swap(value_, other.value_);
        assert_invariant();
    }

 
    void swap(Array& other)
    {
        // swap only works for arrays
        if ((is_array()))
        {
            std::swap(*(value_.array), other);
        }
        else
        {
            JSON_THROW(type_error::create(310, "cannot use swap() with " + std::string(type_name())));
        }
    }

  
    void swap(Object& other)
    {
        // swap only works for objects
        if ((is_object()))
        {
            std::swap(*(value_.object), other);
        }
        else
        {
            JSON_THROW(type_error::create(310, "cannot use swap() with " + std::string(type_name())));
        }
    }

  
    void swap(String& other)
    {
        // swap only works for strings
        if ((is_string()))
        {
            std::swap(*(value_.string), other);
        }
        else
        {
            JSON_THROW(type_error::create(310, "cannot use swap() with " + std::string(type_name())));
        }
    }

    /// @}

  public:
  
    friend bool operator==(const_reference lhs, const_reference rhs) 
    {
        const auto lhs_type = lhs.type();
        const auto rhs_type = rhs.type();

        if (lhs_type == rhs_type)
        {
            switch (lhs_type)
            {
                case Value::array:
                    return (*lhs.value_.array == *rhs.value_.array);

                case Value::object:
                    return (*lhs.value_.object == *rhs.value_.object);

                case Value::null:
                    return true;

                case Value::string:
                    return (*lhs.value_.string == *rhs.value_.string);

                case Value::boolean:
                    return (lhs.value_.boolean == rhs.value_.boolean);

                case Value::num_integer:
                    return (lhs.value_.num_integer == rhs.value_.num_integer);

                case Value::num_unsigned:
                    return (lhs.value_.num_unsigned == rhs.value_.num_unsigned);

                case Value::num_float:
                    return (lhs.value_.num_float == rhs.value_.num_float);

                default:
                    return false;
            }
        }
        else if (lhs_type == Value::num_integer and rhs_type == Value::num_float)
        {
            return (static_cast<Num_float>(lhs.value_.num_integer) == rhs.value_.num_float);
        }
        else if (lhs_type == Value::num_float and rhs_type == Value::num_integer)
        {
            return (lhs.value_.num_float == static_cast<Num_float>(rhs.value_.num_integer));
        }
        else if (lhs_type == Value::num_unsigned and rhs_type == Value::num_float)
        {
            return (static_cast<Num_float>(lhs.value_.num_unsigned) == rhs.value_.num_float);
        }
        else if (lhs_type == Value::num_float and rhs_type == Value::num_unsigned)
        {
            return (lhs.value_.num_float == static_cast<Num_float>(rhs.value_.num_unsigned));
        }
        else if (lhs_type == Value::num_unsigned and rhs_type == Value::num_integer)
        {
            return (static_cast<Num_integer>(lhs.value_.num_unsigned) == rhs.value_.num_integer);
        }
        else if (lhs_type == Value::num_integer and rhs_type == Value::num_unsigned)
        {
            return (lhs.value_.num_integer == static_cast<Num_integer>(rhs.value_.num_unsigned));
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
                case Value::array:
                    return (*lhs.value_.array) < (*rhs.value_.array);

                case Value::object:
                    return *lhs.value_.object < *rhs.value_.object;

                case Value::null:
                    return false;

                case Value::string:
                    return *lhs.value_.string < *rhs.value_.string;

                case Value::boolean:
                    return lhs.value_.boolean < rhs.value_.boolean;

                case Value::num_integer:
                    return lhs.value_.num_integer < rhs.value_.num_integer;

                case Value::num_unsigned:
                    return lhs.value_.num_unsigned < rhs.value_.num_unsigned;

                case Value::num_float:
                    return lhs.value_.num_float < rhs.value_.num_float;

                default:
                    return false;
            }
        }
        else if (lhs_type == Value::num_integer and rhs_type == Value::num_float)
        {
            return static_cast<Num_float>(lhs.value_.num_integer) < rhs.value_.num_float;
        }
        else if (lhs_type == Value::num_float and rhs_type == Value::num_integer)
        {
            return lhs.value_.num_float < static_cast<Num_float>(rhs.value_.num_integer);
        }
        else if (lhs_type == Value::num_unsigned and rhs_type == Value::num_float)
        {
            return static_cast<Num_float>(lhs.value_.num_unsigned) < rhs.value_.num_float;
        }
        else if (lhs_type == Value::num_float and rhs_type == Value::num_unsigned)
        {
            return lhs.value_.num_float < static_cast<Num_float>(rhs.value_.num_unsigned);
        }
        else if (lhs_type == Value::num_integer and rhs_type == Value::num_unsigned)
        {
            return lhs.value_.num_integer < static_cast<Num_integer>(rhs.value_.num_unsigned);
        }
        else if (lhs_type == Value::num_unsigned and rhs_type == Value::num_integer)
        {
            return static_cast<Num_integer>(lhs.value_.num_unsigned) < rhs.value_.num_integer;
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

    /*!
    @brief serialize to stream
    @deprecated This stream operator is deprecated and will be removed in
                future 4.0.0 of the library. Please use
                @ref operator<<(std::ostream&, const basic_json&)
                instead; that is, replace calls like `j >> o;` with `o << j;`.
    @since version 1.0.0; deprecated since version 3.0.0
    */
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
        Parser(i, cb, allow_exceptions).parse(true, result);
        return result;
    }

    static bool accept(detail::input_adapter&& i)
    {
        return Parser(i).accept(true);
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
                return Parser(std::move(i)).sax_parse(sax, strict);
            default:
                return detail::Binary_reader<basic_json, SAX>(std::move(i)).sax_parse(format, sax, strict);
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
        Parser(detail::input_adapter(first, last), cb, allow_exceptions).parse(true, result);
        return result;
    }

    template<class IteratorType, typename std::enable_if<
                 std::is_base_of<
                     std::random_access_iterator_tag,
                     typename std::iterator_traits<IteratorType>::iterator_category>::value, int>::type = 0>
    static bool accept(IteratorType first, IteratorType last)
    {
        return Parser(detail::input_adapter(first, last)).accept(true);
    }

    template<class IteratorType, class SAX, typename std::enable_if<
                 std::is_base_of<
                     std::random_access_iterator_tag,
                     typename std::iterator_traits<IteratorType>::iterator_category>::value, int>::type = 0>
    static bool sax_parse(IteratorType first, IteratorType last, SAX* sax)
    {
        return Parser(detail::input_adapter(first, last)).sax_parse(sax);
    }

    /*!
    @brief deserialize from stream
    @deprecated This stream operator is deprecated and will be removed in
                version 4.0.0 of the library. Please use
                @ref operator>>(std::istream&, basic_json&)
                instead; that is, replace calls like `j << i;` with `i >> j;`.
    @since version 1.0.0; deprecated since version 3.0.0
    */
    JSON_DEPRECATED
    friend std::istream& operator<<(basic_json& j, std::istream& i)
    {
        return operator>>(i, j);
    }

   
    friend std::istream& operator>>(std::istream& i, basic_json& j)
    {
        Parser(detail::input_adapter(i)).parse(false, j);
        return i;
    }

   
    const char* type_name() const 
    {
        {
            switch (type_)
            {
                case Value::null:
                    return "null";
                case Value::object:
                    return "object";
                case Value::array:
                    return "array";
                case Value::string:
                    return "string";
                case Value::boolean:
                    return "boolean";
                case Value::discarded:
                    return "discarded";
                default:
                    return "number";
            }
        }
    }


  private:
    Value type_ = Value::null;
    Json_value value_ = {};
  public:
   
    static std::vector<uint8_t> to_cbor(const basic_json& j)
    {
        std::vector<uint8_t> result;
        to_cbor(j, result);
        return result;
    }

    static void to_cbor(const basic_json& j, detail::output_adapter<uint8_t> o)
    {
        Binary_writer<uint8_t>(o).write_cbor(j);
    }

    static void to_cbor(const basic_json& j, detail::output_adapter<char> o)
    {
        Binary_writer<char>(o).write_cbor(j);
    }

   
    static std::vector<uint8_t> to_msgpack(const basic_json& j)
    {
        std::vector<uint8_t> result;
        to_msgpack(j, result);
        return result;
    }

    static void to_msgpack(const basic_json& j, detail::output_adapter<uint8_t> o)
    {
        Binary_writer<uint8_t>(o).write_msgpack(j);
    }

    static void to_msgpack(const basic_json& j, detail::output_adapter<char> o)
    {
        Binary_writer<char>(o).write_msgpack(j);
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
        Binary_writer<uint8_t>(o).write_ubjson(j, use_size, use_type);
    }

    static void to_ubjson(const basic_json& j, detail::output_adapter<char> o,
                          const bool use_size = false, const bool use_type = false)
    {
        Binary_writer<char>(o).write_ubjson(j, use_size, use_type);
    }


  
    static std::vector<uint8_t> to_bson(const basic_json& j)
    {
        std::vector<uint8_t> result;
        to_bson(j, result);
        return result;
    }

 
    static void to_bson(const basic_json& j, detail::output_adapter<uint8_t> o)
    {
        Binary_writer<uint8_t>(o).write_bson(j);
    }

    /*!
    @copydoc to_bson(const basic_json&, detail::output_adapter<uint8_t>)
    */
    static void to_bson(const basic_json& j, detail::output_adapter<char> o)
    {
        Binary_writer<char>(o).write_bson(j);
    }


  
    static basic_json from_cbor(detail::input_adapter&& i,
                                const bool strict = true,
                                const bool allow_exceptions = true)
    {
        basic_json result;
        detail::json_sax_dom_parser<basic_json> sdp(result, allow_exceptions);
        const bool res = Binary_reader(detail::input_adapter(i)).sax_parse(input_format_t::cbor, &sdp, strict);
        return res ? result : basic_json(Value::discarded);
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
        const bool res = Binary_reader(detail::input_adapter(std::forward<A1>(a1), std::forward<A2>(a2))).sax_parse(input_format_t::cbor, &sdp, strict);
        return res ? result : basic_json(Value::discarded);
    }


    static basic_json from_msgpack(detail::input_adapter&& i,
                                   const bool strict = true,
                                   const bool allow_exceptions = true)
    {
        basic_json result;
        detail::json_sax_dom_parser<basic_json> sdp(result, allow_exceptions);
        const bool res = Binary_reader(detail::input_adapter(i)).sax_parse(input_format_t::msgpack, &sdp, strict);
        return res ? result : basic_json(Value::discarded);
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
        const bool res = Binary_reader(detail::input_adapter(std::forward<A1>(a1), std::forward<A2>(a2))).sax_parse(input_format_t::msgpack, &sdp, strict);
        return res ? result : basic_json(Value::discarded);
    }

 
    static basic_json from_ubjson(detail::input_adapter&& i,
                                  const bool strict = true,
                                  const bool allow_exceptions = true)
    {
        basic_json result;
        detail::json_sax_dom_parser<basic_json> sdp(result, allow_exceptions);
        const bool res = Binary_reader(detail::input_adapter(i)).sax_parse(input_format_t::ubjson, &sdp, strict);
        return res ? result : basic_json(Value::discarded);
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
        const bool res = Binary_reader(detail::input_adapter(std::forward<A1>(a1), std::forward<A2>(a2))).sax_parse(input_format_t::ubjson, &sdp, strict);
        return res ? result : basic_json(Value::discarded);
    }

  
    static basic_json from_bson(detail::input_adapter&& i,
                                const bool strict = true,
                                const bool allow_exceptions = true)
    {
        basic_json result;
        detail::json_sax_dom_parser<basic_json> sdp(result, allow_exceptions);
        const bool res = Binary_reader(detail::input_adapter(i)).sax_parse(input_format_t::bson, &sdp, strict);
        return res ? result : basic_json(Value::discarded);
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
        const bool res = Binary_reader(detail::input_adapter(std::forward<A1>(a1), std::forward<A2>(a2))).sax_parse(input_format_t::bson, &sdp, strict);
        return res ? result : basic_json(Value::discarded);
    }



  
    reference operator[](const Json_ptr& ptr)
    {
        return ptr.get_unchecked(this);
    }

  
    const_reference operator[](const Json_ptr& ptr) const
    {
        return ptr.get_unchecked(this);
    }

   
    reference at(const Json_ptr& ptr)
    {
        return ptr.get_checked(this);
    }

   
    const_reference at(const Json_ptr& ptr) const
    {
        return ptr.get_checked(this);
    }

  
    basic_json flatten() const
    {
        basic_json result(Value::object);
        Json_ptr::flatten("", *this, result);
        return result;
    }

   
    basic_json unflatten() const
    {
        return Json_ptr::unflatten(*this);
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
        const auto operation_add = [&result](Json_ptr & ptr, basic_json val)
        {
            // adding to the root of the target document means replacing it
            if (ptr.is_root())
            {
                result = val;
            }
            else
            {
                // make sure the top element of the pointer exists
                Json_ptr top_pointer = ptr.top();
                if (top_pointer != ptr)
                {
                    result.at(top_pointer);
                }

                // get reference to parent of JSON pointer ptr
                const auto last_path = ptr.pop_back();
                basic_json& parent = result[ptr];

                switch (parent.type_)
                {
                    case Value::null:
                    case Value::object:
                    {
                        // use operator[] to add value
                        parent[last_path] = val;
                        break;
                    }

                    case Value::array:
                    {
                        if (last_path == "-")
                        {
                            // special case: append to back
                            parent.push_back(val);
                        }
                        else
                        {
                            const auto idx = Json_ptr::array_index(last_path);
                            if ((static_cast<size_type>(idx) > parent.size()))
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
        const auto operation_remove = [&result](Json_ptr & ptr)
        {
            // get reference to parent of JSON pointer ptr
            const auto last_path = ptr.pop_back();
            basic_json& parent = result.at(ptr);

            // remove child
            if (parent.is_object())
            {
                // perform range check
                auto it = parent.find(last_path);
                if ((it != parent.end()))
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
                parent.erase(static_cast<size_type>(Json_ptr::array_index(last_path)));
            }
        };

        // type check: top level value must be an array
        if ((not json_patch.is_array()))
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
                if ((it == val.value_.object->end()))
                {
                    JSON_THROW(parse_error::create(105, 0, error_msg + " must have member '" + member + "'"));
                }

                // check if result is of type string
                if ((string_type and not it->second.is_string()))
                {
                    JSON_THROW(parse_error::create(105, 0, error_msg + " must have string member '" + member + "'"));
                }

                // no error: return value
                return it->second;
            };

            // type check: every element of the array must be an object
            if ((not val.is_object()))
            {
                JSON_THROW(parse_error::create(104, 0, "JSON patch must be an array of objects"));
            }

            // collect mandatory members
            const std::string op = get_value("op", "op", true);
            const std::string path = get_value(op, "path", true);
            Json_ptr ptr(path);

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
                    Json_ptr from_ptr(from_path);

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
                    const Json_ptr from_ptr(from_path);

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
                    if ((not success))
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
        basic_json result(Value::array);

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
                case Value::array:
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

                case Value::object:
                {
                    // first pass: traverse this object's elements
                    for (auto it = source.cbegin(); it != source.cend(); ++it)
                    {
                        // escape the key name to be used in a JSON patch
                        const auto key = Json_ptr::escape(it.key());

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
                            const auto key = Json_ptr::escape(it.key());
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

    /// @}
};
} // namespace nlohmann

///////////////////////
// nonmember support //
///////////////////////

// specialization of std::swap, and std::hash
namespace std
{

/// hash value for JSON objects
template<>
struct hash<nlohmann::json>
{
    /*!
    @brief return a hash value for a JSON object

    @since version 1.0.0
    */
    std::size_t operator()(const nlohmann::json& j) const
    {
        // a naive hashing via the string representation
        const auto& h = hash<nlohmann::json::String>();
        return h(j.dump());
    }
};

/// specialization for std::less<Value>
/// @note: do not remove the space after '<',
///        see https://github.com/nlohmann/json/pull/679
template<>
struct less< ::nlohmann::detail::Value>
{
    /*!
    @brief compare two Value enum values
    @since version 3.0.0
    */
    bool operator()(nlohmann::detail::Value lhs,
                    nlohmann::detail::Value rhs) const 
    {
        return nlohmann::detail::operator<(lhs, rhs);
    }
};

/*!
@brief exchanges the values of two JSON objects

@since version 1.0.0
*/
template<>
inline void swap<nlohmann::json>(nlohmann::json& j1, nlohmann::json& j2) (
    is_nothrow_move_constructible<nlohmann::json>::value and
    is_nothrow_move_assignable<nlohmann::json>::value
)
{
    j1.swap(j2);
}

} // namespace std

/*!
@brief user-defined string literal for JSON values

This operator implements a user-defined string literal for JSON objects. It
can be used by adding `"_json"` to a string literal and returns a JSON object
if no parse error occurred.

@param[in] s  a string representation of a JSON object
@param[in] n  the length of string @a s
@return a JSON object

@since version 1.0.0
*/
inline nlohmann::json operator "" _json(const char* s, std::size_t n)
{
    return nlohmann::json::parse(s, s + n);
}

/*!
@brief user-defined string literal for JSON pointer

This operator implements a user-defined string literal for JSON Pointers. It
can be used by adding `"_json_pointer"` to a string literal and returns a JSON pointer
object if no parse error occurred.

@param[in] s  a string representation of a JSON Pointer
@param[in] n  the length of string @a s
@return a JSON pointer object

@since version 2.0.0
*/
inline nlohmann::json::Json_ptr operator "" _json_pointer(const char* s, std::size_t n)
{
    return nlohmann::json::Json_ptr(std::string(s, n));
}

#endif
