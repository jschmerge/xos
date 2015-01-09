#ifndef GUARD_CODECVT_MODE_H
#define GUARD_CODECVT_MODE_H 1

#include <type_traits>

namespace std {

template <typename T> struct is_character : std::false_type { };
template <> struct is_character<char>     : std::true_type { };
template <> struct is_character<wchar_t>  : std::true_type { };
template <> struct is_character<char16_t> : std::true_type { };
template <> struct is_character<char32_t> : std::true_type { };

template <typename T> struct is_wide_character : std::false_type { };
template <> struct is_wide_character<wchar_t>  : std::true_type { };
template <> struct is_wide_character<char16_t> : std::true_type { };
template <> struct is_wide_character<char32_t> : std::true_type { };

constexpr char32_t max_unicode_codepoint()
{ return 0x10ffffu; }

//
// enum codecvt_mode
//
// - If (Mode & consume_header), the facet shall consume an initial header
//   sequence, if present, when reading a multibyte sequence to determine
//   the endianness of the subsequent multibyte sequence to be read.
// - If (Mode & generate_header), the facet shall generate an initial header
//   sequence when writing a multibyte sequence to advertise the endianness
//   of the subsequent multibyte sequence to be written.
// - If (Mode & little_endian), the facet shall generate a multibyte
//   sequence in little-endian order, as opposed to the default big-endian
//   order.
//
enum codecvt_mode
{
	consume_header = 4,
	generate_header = 2,
	little_endian = 1
};

inline constexpr codecvt_mode operator & (codecvt_mode a, codecvt_mode b)
{
	return static_cast<codecvt_mode>(
	static_cast<unsigned>(a) & static_cast<unsigned>(b));
}

inline constexpr codecvt_mode operator | (codecvt_mode a, codecvt_mode b)
{
	return static_cast<codecvt_mode>(
	static_cast<unsigned>(a) | static_cast<unsigned>(b));
}

inline constexpr codecvt_mode operator ^ (codecvt_mode a, codecvt_mode b)
{
	return static_cast<codecvt_mode>(
	static_cast<unsigned>(a) ^ static_cast<unsigned>(b));
}

inline constexpr codecvt_mode operator ~ (codecvt_mode a)
	{ return static_cast<codecvt_mode>(~static_cast<unsigned>(a)); }

inline codecvt_mode & operator &= (codecvt_mode & a, const codecvt_mode & b)
	{ a = (a & b); return a; }

inline codecvt_mode & operator |= (codecvt_mode & a, const codecvt_mode & b)
	{ a = (a | b); return a; }

inline codecvt_mode & operator ^= (codecvt_mode & a, const codecvt_mode & b)
	{ a = (a ^ b); return a; }


} // namespace std

#endif // GUARD_CODECVT_MODE_H
