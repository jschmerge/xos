#ifndef GUARD_MULTISTRING_H
#define GUARD_MULTISTRING_H 1

#include <string>

struct multistring {
	multistring(const char * _ns, const wchar_t * _ws,
	            const char16_t * _s16, const char32_t * _s32)
	: ns(_ns), ws(_ws), s16(_s16), s32(_s32)
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
	multistring name(NARROW(literalval), WIDE(literalval), \
	                 UTF16(literalval), UTF32(literalval))


#endif // GUARD_MULTISTRING_H
