#ifndef TLP_TYPE_H
#define TLP_TYPE_H

namespace tlp
{
template<typename T, T v>
struct integral_constant
{
  typedef T value_type;
  typedef integral_constant type;
  
  constexpr operator value_type() const { return value; }
  constexpr value_type operator()() const { return value; }
  
  static constexpr T value = v;
};
 
template<bool B>
using bool_t = integral_constant<bool, B>;
  
template<int8_t V>
using int8_t = integral_constant<int8_t, V>;
  
template<int16_t V>
using int16_t = integral_constant<int16_t, V>;
  
template<int32_t V>
using int32_t = integral_constant<int32_t, V>;
  
template<int64_t V>
using int64_t = integral_constant<int64_t, V>;
  
template<uint8_t V>
using uint8_t = integral_constant<uint8_t, V>;
  
template<uint16_t V>
using uint16_t = integral_constant<uint16_t, V>;
  
template<uint32_t V>
using uint32_t = integral_constant<uint32_t, V>;
  
template<uint64_t V>
using uint64_t = integral_constant<uint64_t, V>;
  
template<float V>
using float_t = integral_constant<float, V>;
  
template<double V>
using double_t = integral_constant<double, V>;
  
template<long double V>
using long_doube_t = integral_constant<long double, V>;
  
template<void V>
using void_t = integral_constant<void, V>;
  
using true_type = bool_t<true>;
using false_type = bool_t<false>;
  
using null_t = integral_constant<nullptr_t, nullptr>;
  
template<typename T>
struct remove_const
{
  typedef T type;
};

template<typename T>
struct remove_cost<const T>
{
  typedef T type;
};
  
tempplate<typename T>
remove_volatile
{
  typedef T type;
};

template<typename T>
remove_volatile<volatile T>
{
  typedef T type;
};
  
template<typename T>
struct remove_cv
{
  typedef typename remove_volatile<typeame remove_const<T>::type> type;
};
  
template<typename T, typename U>
struct is_same : false_type
{};
  
template<typename T>
struct is_same<T, T> : true_type
{};
  
template<bool B, typename T, typename F>
struct conditional
{
  typedef T type;
};

template<typename T, typename F>
struct conditional<false, T, F>
{
  typedef F type;
};
 
template<bool B, typename T = void>
struct enable_if
{};

template<typename T>
struct enable_if<true, T>
{
  typedef T type;
};
  
template<bool B, typename T>
using enable_if_t = enable_if<B, T>::type;
 
}

#endif
