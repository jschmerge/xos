#ifndef GUARD_PATH_TRAITS_H
#define GUARD_PATH_TRAITS_H 1

#include <string>

namespace filesystem {
inline namespace v1 {
namespace path_traits {

/// [8.2.2] - encodable path characters
template <typename C>
struct is_path_encodable { static constexpr bool value = false; };

template <>
struct is_path_encodable<char> { static constexpr bool value = true; };

template <>
struct is_path_encodable<wchar_t> { static constexpr bool value = true; };

template <>
struct is_path_encodable<char16_t> { static constexpr bool value = true; };

template <>
struct is_path_encodable<char32_t> { static constexpr bool value = true; };

/// [8.3] - source requirements
template <typename C, typename T, typename A,
          template<typename, typename, typename> class STR>
struct is_path_source
{
	static constexpr bool value =
	  is_path_encodable<typename STR<C, T, A>::value_type>::value;
};

} // namespace path_traits
} // inline namespace v1
} // namespace filesystem

#endif // GUARD_PATH_TRAITS_H
