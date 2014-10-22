#ifndef GUARD_CODECVT_UTF8_UTF16_H
#define GUARD_CODECVT_UTF8_UTF16_H 1

#include <locale>
#include <limits>
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
	typedef Elem                 intern_type;
	typedef char                 extern_type;
	typedef mbstate_t            state_type;
	typedef codecvt_base::result result;

	explicit codecvt_utf8_utf16(size_t refs = 0)
	: codecvt<intern_type, extern_type, mbstate_t>(refs)
	{ }

	~codecvt_utf8_utf16()
	{ }

 protected:
	inline bool generate_bom() const
	{ return ((Mode & generate_header) == generate_header); }

	inline bool consume_bom() const
	{ return ((Mode & consume_header) == consume_header); }

	inline bool little_endian_out() const
	{ return ((Mode & little_endian) == little_endian); }

	inline
	constexpr char32_t max_encodable() const
	{
		return std::min(
		         static_cast<uint32_t>(0x7fffffffu),
		         std::min(static_cast<uint32_t>(Maxcode),
		                  static_cast<uint32_t>(
		                    numeric_limits<Elem>::max())));
	}

#if 0
	virtual codecvt_base::result
	do_out(mbstate_t & state,
	       const intern_type * from_begin,
	       const intern_type * from_end,
	       const intern_type * & from_last,
	       char * to_begin,
	       char * to_end,
	       char * & to_last) const override;

	virtual codecvt_base::result
	do_unshift(mbstate_t & state,
	           char * to_begin,
	           char * to_end,
	           char * & to_last) const override;

	virtual codecvt_base::result
	do_in(mbstate_t & state,
	      const char * from_begin,
	      const char * from_end,
	      const char * & from_last,
	      intern_type * to_begin,
	      intern_type * to_end,
	      intern_type * & to_last) const override;

	virtual int
	do_length(mbstate_t & state,
	          const char * from_begin,
	          const char * from_end,
	          size_t max) const override;
#endif

	virtual int
	do_encoding() const noexcept override
	{ return 0; }

	virtual bool
	do_always_noconv() const noexcept override
	{ return false; }

	virtual int
	do_max_length() const noexcept override
	{
		int val = 4;
		if (this->consume_bom())
			val += 3;
		return val;
	}
};

} // namespace std

#endif // GUARD_CODECVT_UTF8_UTF16_H
