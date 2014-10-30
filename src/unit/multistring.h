#ifndef GUARD_MULTISTRING_H
#define GUARD_MULTISTRING_H 1

#include <string>

struct multistring {
	multistring(const char * _ns, const wchar_t * _ws,
	            const char16_t * _s16, const char32_t * _s32)
	: ns(_ns), ws(_ws), s16(_s16), s32(_s32)
	{ }

	multistring(const char * _ns, size_t _ns_len,
	            const wchar_t * _ws, size_t _ws_len,
	            const char16_t * _s16, size_t _s16_len,
	            const char32_t * _s32, size_t _s32_len)
	: ns(_ns, _ns_len)
	, ws(_ws, _ws_len)
	, s16(_s16, _s16_len)
	, s32(_s32, _s32_len)
	{ }

	template <typename T>
	const std::basic_string<T> & get() const;

	std::string    ns;
	std::wstring   ws;
	std::u16string s16;
	std::u32string s32;
};

#define TRIPLE_CAT_(a_, b_, c_) a_ ## b_ ## c_
#define TRIPLE_CAT(a_, b_, c_) TRIPLE_CAT_(a_, b_, c_)
#define NARROW(s_) TRIPLE_CAT(, s_, )
#define WIDE(s_) TRIPLE_CAT(L, s_, )
#define UTF8(s_) TRIPLE_CAT(u8, s_, )
#define UTF16(s_) TRIPLE_CAT(u, s_, )
#define UTF32(s_) TRIPLE_CAT(U, s_, )

#define DEF_MULTISTRING(name, literalval) \
	multistring name(NARROW(literalval), \
	                 (sizeof(NARROW(literalval)) - 1)/ sizeof(char), \
	                 WIDE(literalval), \
	                 (sizeof(WIDE(literalval)) - 1)/ sizeof(wchar_t), \
	                 UTF16(literalval), \
	                 (sizeof(UTF16(literalval)) - 1)/ sizeof(char16_t), \
	                 UTF32(literalval), \
	                 (sizeof(UTF32(literalval)) - 1)/ sizeof(char32_t))

template<>
inline const std::basic_string<char> & multistring::get<char>() const
{ return ns; }

template<>
inline const std::basic_string<wchar_t> & multistring::get<wchar_t>() const
{ return ws; }

template<>
inline const std::basic_string<char16_t> & multistring::get<char16_t>() const
{ return s16; }

template<>
inline const std::basic_string<char32_t> & multistring::get<char32_t>() const
{ return s32; }

template <typename T>
const std::basic_string<char> byte_oriented_string(bool little_endian);

#endif // GUARD_MULTISTRING_H
