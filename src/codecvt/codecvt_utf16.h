#ifndef GUARD_CODECVT_UTF16_H
#define GUARD_CODECVT_UTF16_H 1

#include <locale>
#include "codecvt_specializations.h"
#include "codecvt_mode.h"

namespace std {

//
// For the facet codecvt_utf16:
//
// - The facet shall convert between UTF-16 multibyte sequences and UCS2 or
//   UCS4 (depending on the size of Elem) within the program.
// - Multibyte sequences shall be read or written according to the Mode
//   flag, as set out above.
// - The multibyte sequences may be written only as a binary file.
//   Attempting to write to a text file produces undefined behavior.
//
// This is used to output utf-16 encoded data
//
template <typename Elem, unsigned long Maxcode = 0x10ffff,
          codecvt_mode Mode = (codecvt_mode) 0>
class codecvt_utf16 : public codecvt<Elem, char, mbstate_t>
{
 public:
	explicit codecvt_utf16(size_t refs = 0);
	~codecvt_utf16();
};

} // namespace std

#endif // GUARD_CODECVT_UTF16_H
