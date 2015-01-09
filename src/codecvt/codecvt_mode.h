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

namespace cvt_mode_constants {
  constexpr std::codecvt_mode cvt_0 =
	( std::little_endian ^ std::little_endian );

  constexpr std::codecvt_mode cvt_1 =
	( std::little_endian | std::little_endian );

  constexpr std::codecvt_mode cvt_2 =
	( std::generate_header | std::generate_header );

  constexpr std::codecvt_mode cvt_3 =
	( std::little_endian | std::generate_header );

  constexpr std::codecvt_mode cvt_4 =
	( std::consume_header | std::consume_header );

  constexpr std::codecvt_mode cvt_5 =
	( std::little_endian | std::consume_header );

  constexpr std::codecvt_mode cvt_6 =
	( std::generate_header | std::consume_header );

  constexpr std::codecvt_mode cvt_7 =
	( std::little_endian | std::generate_header | std::consume_header );
}

#define FOR_ALL_CVT_MODES(FN, CVT, CHAR_T, MAX) \
	FN(CVT, CHAR_T, MAX, 0); \
	FN(CVT, CHAR_T, MAX, 1); \
	FN(CVT, CHAR_T, MAX, 2); \
	FN(CVT, CHAR_T, MAX, 3); \
	FN(CVT, CHAR_T, MAX, 4); \
	FN(CVT, CHAR_T, MAX, 5); \
	FN(CVT, CHAR_T, MAX, 6); \
	FN(CVT, CHAR_T, MAX, 7);

#define EXTERN_CVT_TEMPLATE(CVT, CHAR_T, MAX, MODE) \
	extern template class CVT<CHAR_T, MAX, cvt_mode_constants::cvt_ ## MODE>

#define INSTANTIATE_CVT_TEMPLATE(CVT, CHAR_T, MAX, MODE) \
	template class CVT<CHAR_T, MAX, cvt_mode_constants::cvt_ ## MODE>;


#endif // GUARD_CODECVT_MODE_H
