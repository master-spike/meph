#include <cstddef>
#include <type_traits>

namespace meph::meta
{

// count elements in parameter pack
template <typename... Args> struct count;

template <> struct count<>
{
  static constexpr std::size_t value = 0;
};

template <typename T, typename... Ts> struct count<T, Ts...>
{
  static constexpr std::size_t value = 1 + count<Ts...>::value;
};

// index of element in parameter pack
template <typename... Args> struct index;

template <typename U, typename... Ts> struct index<U, U, Ts...>
{
  static constexpr std::size_t value = 0;
};

template <typename U, typename T, typename... Ts> struct index<U, T, Ts...>
{
  static constexpr std::size_t value = 1 + index<U, Ts...>::value;
};

template <typename... Args> struct all_distinct;

template <> struct all_distinct<>
{
  static constexpr bool value = true;
};

template <typename T, typename... Ts> struct all_distinct<T, Ts...>
{
  static constexpr bool value = (!std::is_same_v<T, Ts> && ...) && all_distinct<Ts...>::value;
};

namespace concepts
{

template <typename... Args>
concept all_distinct = meph::meta::all_distinct<Args...>::value;

} // namespace concepts

} // namespace meph::meta
