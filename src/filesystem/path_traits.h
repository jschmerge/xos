#ifndef GUARD_PATH_TRAITS_H
#define GUARD_PATH_TRAITS_H 1

#include <cstdint>
#include <type_traits>
#include <string>

namespace filesystem {
inline namespace v1 {
namespace path_traits {

/// [8.2.2] - encodable path character types
///
/// For member function arguments that take character sequences representing
/// paths and for member functions returning strings, value type and encoding
/// conversion is performed if the value type of the argument or return
/// differs from path::value_type . Encoding and method of conversion for the
/// argument or return value to be converted to is determined by its value
/// type:
/// 	* char : Encoding is the native narrow encoding (4.10). Conversion,
///			if any, is operating system dependent.
///
/// 		[Note: For POSIX based operating systems path::value_type is char
///			so no conversion from char value type arguments or to char value
///			type returns is performed. For Windows based operating systems,
///			the native narrow encoding is determined by calling a Windows API
///			function. —end note]
///
/// 		[Note: This results in behavior identical to other C and C++
///			standard library functions that perform file operations using
///			narrow character strings to identify paths. Changing this behavior
///			would be surprising and error prone. —end note]
///
/// 	* wchar_t : Encoding is the native wide encoding (4.10). Conversion
///			method is unspecified.
///
/// 		[Note: For Windows based operating systems path::value_type is
///			wchar_t so no conversion from wchar_t value type arguments or to
///			wchar_t value type returns is performed. —end note]
///
/// 	* char16_t : Encoding is UTF-16. Conversion method is unspecified.
///
/// 	* char32_t : Encoding is UTF-32. Conversion method is unspecified.
///
/// If the encoding being converted to has no representation for source
///	characters, the resulting converted characters, if any, are unspecified.
template <typename C> struct is_path_char_t_encodable : std::false_type { };

template <> struct is_path_char_t_encodable<char>     : std::true_type  { };
template <> struct is_path_char_t_encodable<wchar_t>  : std::true_type  { };
template <> struct is_path_char_t_encodable<char16_t> : std::true_type  { };
template <> struct is_path_char_t_encodable<char32_t> : std::true_type  { };

/// [8.3] - path requirements [path.req]
/// In addition to the requirements (5), function template parameters named
/// Source shall be one of:
///		* basic_string<EcharT, traits, Allocator> . A function argument
///			const Source& source shall have an effective range
///			[source.begin(), source.end()).
///		* A type meeting the input iterator requirements that iterates over
///			a NTCTS. The value type shall be an encoded character type. A
///			function argument const Source& source shall have an effective
///			range [source ,end) where end is the first iterator value with an
///			element value equal to iterator_traits<Source>::value_type().
///		* A character array that after array-to-pointer decay results in a
///			pointer to the start of a NTCTS. The value type shall be an
///			encoded character type. A function argument const Source& source
///			shall have an effective range [source, end) where end is the
///			first iterator value with an element value equal to
///			iterator_traits<decay<Source>::type>::value_type() .
///
///			[Note: See path conversions (8.2) for how these value types and
///			their encodings convert to path::value_type and its encoding.
///			—end note]
///
///		Arguments of type Source shall not be null pointers.
///
template <typename T> struct is_path_initializer : std::false_type { };

template <typename EcharT, typename traits, typename Allocator>
struct is_path_initializer<std::basic_string<EcharT, traits, Allocator>>
  : std::true_type {
	typedef typename std::remove_cv<EcharT>::type value_type;
};

// pointer specialization
template <typename EcharT> struct is_path_initializer<EcharT*> {
	typedef typename std::decay<
	          typename std::remove_cv<EcharT>::type>::type value_type;

	static constexpr value_type eos = value_type();

	static constexpr bool value = is_path_char_t_encodable<value_type>::value;
};

// array specialization
template <typename EcharT> struct is_path_initializer<EcharT[]> {
	typedef typename std::decay<
	          typename std::remove_cv<EcharT>::type>::type value_type;

	static constexpr value_type eos = value_type();

	static constexpr bool value = is_path_char_t_encodable<value_type>::value;
};

// array specialization, with extent
template <typename EcharT, std::size_t N> struct is_path_initializer<EcharT[N]> {
	typedef typename std::decay<
	          typename std::remove_cv<EcharT>::type>::type value_type;

	static constexpr value_type eos = value_type();

	static constexpr bool value = is_path_char_t_encodable<value_type>::value;
};

// container specialization
template <typename EcharT, template <typename...> class C, typename ...Args>
struct is_path_initializer<C<EcharT, Args...>> {
	typedef typename std::remove_cv<
	          typename C<EcharT, Args...>::value_type>::type value_type;

	typedef decltype(
	  ++std::declval<typename C<EcharT, Args...>::iterator>()) iterator_type;

	static constexpr bool value = is_path_char_t_encodable<value_type>::value;
};

template <> struct is_path_initializer<std::nullptr_t> : std::false_type { };


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

	using filesystem::path_traits::is_path_initializer;
	using filesystem::path_traits::is_path_char_t_encodable;

	static_assert(is_path_char_t_encodable<char>::value, "foo");


	static_assert(is_path_initializer<const char*>::value,
	              "is_path_initializer failed");

	static_assert(is_path_initializer<const volatile char*>::value,
	              "is_path_initializer failed");

	static_assert(is_path_initializer<char*>::value,
	              "is_path_initializer failed");

	static_assert(is_path_initializer<const char[]>::value,
	              "is_path_initializer failed");

	static_assert(is_path_initializer<const char[10]>::value,
	              "is_path_initializer failed");

	static_assert(is_path_initializer<std::basic_string<volatile char>>::value,
	              "is_path_initializer failed");

	static_assert(is_path_initializer<std::vector<char>>::value,
	              "is_path_initializer failed");

	static_assert(is_path_initializer<std::basic_string<char>>::value,
	              "is_path_initializer failed");

	static_assert(is_path_initializer<std::vector<char>>::value,
	              "is_path_initializer failed");

	static_assert(is_path_initializer<
	                std::vector<char, std::allocator<char>>>::value,
	              "is_path_initializer failed");

	static_assert(is_path_initializer<std::list<char>>::value,
	              "is_path_initializer failed");

  } // namespace detail

#endif // DO_ASSERTS

#endif // GUARD_PATH_TRAITS_H
