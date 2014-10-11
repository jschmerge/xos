#ifndef GUARD_CODECVT_UTF8_UTF16_H
#define GUARD_CODECVT_UTF8_UTF16_H 1

#include <locale>
#include "codecvt_specializations.h"
#include "codecvt_mode.h"

namespace std {

//
// For the facet codecvt_utf8_utf16:
//
// - The facet shall convert between UTF-8 multibyte sequences and UTF-16
//   (one or two 16-bit codes) within the program.
// - Endianness shall not affect how multibyte sequences are read or written.
// - The multibyte sequences may be written as either a text or a binary file.
//
template <typename Elem, unsigned long Maxcode = 0x10ffff,
          codecvt_mode Mode = (codecvt_mode) 0>
class codecvt_utf8_utf16 : public codecvt<Elem, char, mbstate_t>
{
 public:
	explicit codecvt_utf8_utf16(size_t refs = 0);
	~codecvt_utf8_utf16();
};

} // namespace std

#endif // GUARD_CODECVT_UTF8_UTF16_H
