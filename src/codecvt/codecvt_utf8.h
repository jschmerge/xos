#ifndef GUARD_CODECVT_UTF8_H
#define GUARD_CODECVT_UTF8_H 1

#include <locale>
#include "codecvt_specializations.h"
#include "codecvt_mode.h"

namespace std {

//
// The facet shall convert between UTF-8 multibyte sequences and UCS2 or
// UCS4 (depending on the size of Elem) within the program.
//
// - Endianness shall not affect how multibyte sequences are read or written.
//
// - The multibyte sequences may be written as either a text or a binary file.
//
template <class Elem, unsigned long Maxcode = 0x10ffff,
          codecvt_mode Mode = (codecvt_mode)0>
class codecvt_utf8 : public codecvt<Elem, char, mbstate_t>
{
 public:
	explicit codecvt_utf8(size_t refs = 0)
	  : codecvt<Elem, char, mbstate_t>(refs)
		{ }

	~codecvt_utf8()
		{ }
};

} // namespace std

#endif // GUARD_CODECVT_UTF8_H

