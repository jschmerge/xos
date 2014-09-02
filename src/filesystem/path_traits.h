#ifndef GUARD_PATH_TRAITS_H
#define GUARD_PATH_TRAITS_H 1

#include <cstdint>
#include <type_traits>

namespace filesystem {
inline namespace v1 {
namespace path_traits {

/// [8.2.2] - encodable path character types
template <typename C> struct is_path_encodable : std::false_type { };

template <> struct is_path_encodable<char>     : std::true_type  { };
template <> struct is_path_encodable<wchar_t>  : std::true_type  { };
template <> struct is_path_encodable<char16_t> : std::true_type  { };
template <> struct is_path_encodable<char32_t> : std::true_type  { };

/// [8.3] - source requirements
template <typename T> struct is_pathable : std::false_type { };

// pointer specialization
template <typename T> struct is_pathable<T*> {
	typedef typename std::decay<
	          typename std::remove_cv<T>::type>::type value_type;

	static constexpr bool value = is_path_encodable<value_type>::value;
};

// array specialization
template <typename T> struct is_pathable<T[]> {
	typedef typename std::decay<
	          typename std::remove_cv<T>::type>::type value_type;

	static constexpr bool value = is_path_encodable<value_type>::value;
};

// array specialization, with extent
template <typename T, std::size_t N> struct is_pathable<T[N]> {
	typedef typename std::decay<
	          typename std::remove_cv<T>::type>::type value_type;

	static constexpr bool value = is_path_encodable<value_type>::value;
};

// container specialization
template <typename T, template <typename...> class C, typename ...Args>
struct is_pathable<C<T, Args...>> {
	typedef typename std::remove_cv<
	          typename C<T, Args...>::value_type>::type value_type;

	typedef decltype(
	  ++std::declval<typename C<T, Args...>::iterator>()) iterator_type;

	static constexpr bool value = is_path_encodable<value_type>::value;
};

} // namespace path_traits
} // inline namespace v1
} // namespace filesystem

#define DO_ASSERTS 1
#if DO_ASSERTS
#	include <vector>
#	include <list>
#	include <forward_list>
#	include <string>


  namespace detail {

	using filesystem::path_traits::is_pathable;
	using filesystem::path_traits::is_path_encodable;

	static_assert(is_path_encodable<char>::value, "foo");


	static_assert(is_pathable<const char*>::value,
	              "is_pathable failed");

	static_assert(is_pathable<const volatile char*>::value,
	              "is_pathable failed");

	static_assert(is_pathable<char*>::value,
	              "is_pathable failed");

	static_assert(is_pathable<const char[]>::value,
	              "is_pathable failed");

	static_assert(is_pathable<const char[10]>::value,
	              "is_pathable failed");

	static_assert(is_pathable<std::basic_string<volatile char>>::value,
	              "is_pathable failed");

	static_assert(is_pathable<std::vector<char>>::value,
	              "is_pathable failed");

	static_assert(is_pathable<std::basic_string<char>>::value,
	              "is_pathable failed");

	static_assert(is_pathable<std::vector<char>>::value,
	              "is_pathable failed");

	static_assert(is_pathable<std::vector<char, std::allocator<char>>>::value,
	              "is_pathable failed");

	static_assert(is_pathable<std::list<char>>::value,
	              "is_pathable failed");

  } // namespace detail

#endif // DO_ASSERTS

#endif // GUARD_PATH_TRAITS_H
