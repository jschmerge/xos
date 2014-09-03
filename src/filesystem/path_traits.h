#ifndef GUARD_PATH_TRAITS_H
#define GUARD_PATH_TRAITS_H 1

#include <type_traits>

namespace filesystem {
inline namespace v1 {
namespace path_traits {

/// [8.2.2] - encodable path characters
template <typename C> struct is_path_encodable : std::false_type { };

template <> struct is_path_encodable<char> : std::true_type { };
template <> struct is_path_encodable<wchar_t> : std::true_type { };
template <> struct is_path_encodable<char16_t> : std::true_type { };
template <> struct is_path_encodable<char32_t> : std::true_type { };

/// [8.3] - source requirements

template <typename T> struct is_path_array_source : std::false_type { };

template <typename T>
struct is_path_array_source<T*> {
	typedef typename std::decay<
		typename std::remove_cv<T>::type>::type value_type;
	static constexpr bool value = is_path_encodable<value_type>::value;
};

template <typename T>
struct is_path_array_source<T[]> {
	typedef typename std::remove_cv<
		typename std::decay<T>::type>::type value_type;
	static constexpr bool value = is_path_encodable<value_type>::value;
};

template <typename C>
struct is_path_container_source
{
//	typedef typename C::iterator iterator_type;

	typedef typename std::decay<
		typename std::remove_cv<typename C::value_type>::type>::type value_type;
	static constexpr bool value = is_path_encodable<value_type>::value;
};

template <typename T>
struct is_path_source : std::false_type { };


} // namespace path_traits
} // inline namespace v1
} // namespace filesystem

#endif // GUARD_PATH_TRAITS_H
