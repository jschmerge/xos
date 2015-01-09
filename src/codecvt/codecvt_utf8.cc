#include "codecvt_utf8.h"

namespace std {

#define INSTANTIATE_EXTERNS(CHAR_T, MAX) \
	template class codecvt_utf8<CHAR_T, MAX, cvt_mode0>; \
	template class codecvt_utf8<CHAR_T, MAX, cvt_mode1>; \
	template class codecvt_utf8<CHAR_T, MAX, cvt_mode2>; \
	template class codecvt_utf8<CHAR_T, MAX, cvt_mode3>; \
	template class codecvt_utf8<CHAR_T, MAX, cvt_mode4>; \
	template class codecvt_utf8<CHAR_T, MAX, cvt_mode5>; \
	template class codecvt_utf8<CHAR_T, MAX, cvt_mode6>; \
	template class codecvt_utf8<CHAR_T, MAX, cvt_mode7>

INSTANTIATE_EXTERNS(wchar_t, 0x7f);
INSTANTIATE_EXTERNS(wchar_t, 0xff);
INSTANTIATE_EXTERNS(wchar_t, 0xffff);
INSTANTIATE_EXTERNS(wchar_t, max_unicode_codepoint());

INSTANTIATE_EXTERNS(char16_t, 0x7f);
INSTANTIATE_EXTERNS(char16_t, 0xff);
INSTANTIATE_EXTERNS(char16_t, 0xffff);
INSTANTIATE_EXTERNS(char16_t, max_unicode_codepoint());

INSTANTIATE_EXTERNS(char32_t, 0x7f);
INSTANTIATE_EXTERNS(char32_t, 0xff);
INSTANTIATE_EXTERNS(char32_t, 0xffff);
INSTANTIATE_EXTERNS(char32_t, max_unicode_codepoint());
INSTANTIATE_EXTERNS(char32_t, 0x7fffffff);

} // namespace std
