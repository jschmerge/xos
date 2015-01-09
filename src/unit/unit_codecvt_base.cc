#include "unit_codecvt_base.h"

FOR_ALL_CVT_MODES(INSTANTIATE_DECLS, std::codecvt_utf8, wchar_t, 0x7f);
FOR_ALL_CVT_MODES(INSTANTIATE_DECLS, std::codecvt_utf8, wchar_t, 0xff);
FOR_ALL_CVT_MODES(INSTANTIATE_DECLS, std::codecvt_utf8, wchar_t, 0xffff);
FOR_ALL_CVT_MODES(INSTANTIATE_DECLS, std::codecvt_utf8, wchar_t, 0x10ffff);

FOR_ALL_CVT_MODES(INSTANTIATE_DECLS, std::codecvt_utf8, char16_t, 0x7f);
FOR_ALL_CVT_MODES(INSTANTIATE_DECLS, std::codecvt_utf8, char16_t, 0xff);
FOR_ALL_CVT_MODES(INSTANTIATE_DECLS, std::codecvt_utf8, char16_t, 0xffff);
FOR_ALL_CVT_MODES(INSTANTIATE_DECLS, std::codecvt_utf8, char16_t, 0x10ffff);

FOR_ALL_CVT_MODES(INSTANTIATE_DECLS, std::codecvt_utf8, char32_t, 0x7f);
FOR_ALL_CVT_MODES(INSTANTIATE_DECLS, std::codecvt_utf8, char32_t, 0xff);
FOR_ALL_CVT_MODES(INSTANTIATE_DECLS, std::codecvt_utf8, char32_t, 0xffff);
FOR_ALL_CVT_MODES(INSTANTIATE_DECLS, std::codecvt_utf8, char32_t, 0x10ffff);
FOR_ALL_CVT_MODES(INSTANTIATE_DECLS, std::codecvt_utf8, char32_t, 0x3ffffff);
FOR_ALL_CVT_MODES(INSTANTIATE_DECLS, std::codecvt_utf8, char32_t, 0x7fffffff);

