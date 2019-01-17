#pragma once
#include <type_traits>

namespace zm
{
template <typename ...>
using void_t = void;

template <typename T>
using decay_t = typename std::decay<T>::type;

#define HAS_STATIC_FUN(name ,func)                                                        \
template <typename T, typename ...Args>                                                   \
struct HasStaticMemberFunc##name                                                          \
{                                                                                         \
  template <typename U>                                                                   \
  static auto                                                                             \
    check(int) -> decltype(decay_t<U>::##func(std::declval<Args>()...), std::true_type()) \
  {                                                                                       \
    return std::bool_constant<true>();                                                    \
  };                                                                                      \
template <typename U>                                                                     \
static auto                                                                               \
  check(...) -> std::false_type                                                           \
{                                                                                         \
  return std::bool_constant<false>();                                                     \
};                                                                                        \
static constexpr bool value = std::is_same<decltype(check<T>(0)), std::true_type>::value; \
};

#define HAS_STATIC_FUN(name ,func)                                                        \
template <typename T, typename ...Args>                                                   \
struct HasMemberFunc##name                                                                \
{                                                                                         \
  template <typename U>                                                                   \
  static auto                                                                             \
    check(int) -> decltype(std::declval<U>().##func(std::declval<Args>()...), std::true_type()) \
  {                                                                                       \
    return std::bool_constant<true>();                                                    \
  };                                                                                      \
template <typename U>                                                                     \
static auto                                                                               \
  check(...) -> std::false_type                                                           \
{                                                                                         \
  return std::bool_constant<false>();                                                     \
};                                                                                        \
static constexpr bool value = std::is_same<decltype(check<T>(0)), std::true_type>::value; \
};
}