#ifndef GUARD_CODECVT_UTF16_H
#define GUARD_CODECVT_UTF16_H 1

#include <locale>
#include <limits>
#include <cassert>
#include "codecvt_specializations.h"
#include "codecvt_mode.h"
#include "utf_conversion_helpers.h"

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
	typedef Elem                 intern_type;
	typedef char                 extern_type;
	typedef mbstate_t            state_type;
	typedef codecvt_base::result result;

	explicit codecvt_utf16(size_t refs = 0)
	: codecvt<intern_type, extern_type, mbstate_t>(refs)
	{ }

	~codecvt_utf16()
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

	virtual codecvt_base::result
	do_out(mbstate_t & state,
	       const intern_type * from_begin,
	       const intern_type * from_end,
	       const intern_type * & from_last,
	       char * to_begin,
	       char * to_end,
	       char * & to_last) const override
	{
		namespace utf16 = utf16_conversion;
		result res = codecvt_base::ok;

		assert(from_begin <= from_end);
		assert(to_begin <= to_end);

		from_last = from_begin;
		to_last = to_begin;

		while (  (res == codecvt_base::ok)
		      && (from_last < from_end)
		      && (to_last < to_end) )
		{
			if (  (static_cast<uint32_t>(*from_last) > this->max_encodable())
			   || (utf16::is_surrogate(*from_last)) )
			{
				res = codecvt_base::error;
			}
			else if (state.__count == 0)
			{
				if (utf16::set_mbstate(state, *from_last,
				                       this->little_endian_out()))
				{
					++from_last;
				} else
				{
					res = codecvt_base::error;
				}
			}

			if (res == codecvt_base::ok)
				res = this->do_unshift(state, to_last, to_end, to_last);
		}

		return res;
	}

	virtual codecvt_base::result
	do_unshift(mbstate_t & state,
	           char * to_begin,
	           char * to_end,
	           char * & to_last) const override
	{
		assert(to_begin <= to_end);

		to_last = to_begin;

//		printf("--> ");
		for (; (to_last < to_end) && (state.__count != 0);
		     --state.__count, ++to_last)
		{
//			printf("%02hhx ", state.__value.__wchb[state.__count - 1]);
			*to_last = state.__value.__wchb[state.__count - 1];
		}

		return ( (state.__count == 0) ?
		         codecvt_base::ok :
		         codecvt_base::partial );
	}

/*
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
	          size_t) const override;
*/

	virtual int
	do_encoding() const noexcept override
	{ return 0; }

	virtual bool
	do_always_noconv() const noexcept override
	{ return false; }

/*
	virtual int
	do_max_length() const noexcept override;
*/
};

//
// massive number of specializations here - for all combos of the
// codecvt_mode enum plus all three wide char types. Unfortunately,
// we can't burry the functions defined in the class in a source
// file, but we can at least provide some coverage of the common
// cases.
//
extern template class codecvt_utf16<wchar_t, max_unicode_codepoint()>;
extern template class codecvt_utf16<wchar_t, max_unicode_codepoint(),
	little_endian>;
extern template class codecvt_utf16<wchar_t, max_unicode_codepoint(),
	generate_header>;
extern template class codecvt_utf16<wchar_t, max_unicode_codepoint(),
	codecvt_mode(generate_header | little_endian)>;
extern template class codecvt_utf16<wchar_t, max_unicode_codepoint(),
	consume_header>;
extern template class codecvt_utf16<wchar_t, max_unicode_codepoint(),
	codecvt_mode(consume_header | little_endian)>;
extern template class codecvt_utf16<wchar_t, max_unicode_codepoint(),
	codecvt_mode(consume_header | generate_header)>;
extern template class codecvt_utf16<wchar_t, max_unicode_codepoint(),
	codecvt_mode(consume_header | generate_header | little_endian)>;

extern template class codecvt_utf16<char16_t, max_unicode_codepoint()>;
extern template class codecvt_utf16<char16_t, max_unicode_codepoint(),
	little_endian>;
extern template class codecvt_utf16<char16_t, max_unicode_codepoint(),
	generate_header>;
extern template class codecvt_utf16<char16_t, max_unicode_codepoint(),
	codecvt_mode(generate_header | little_endian)>;
extern template class codecvt_utf16<char16_t, max_unicode_codepoint(),
	consume_header>;
extern template class codecvt_utf16<char16_t, max_unicode_codepoint(),
	codecvt_mode(consume_header | little_endian)>;
extern template class codecvt_utf16<char16_t, max_unicode_codepoint(),
	codecvt_mode(consume_header | generate_header)>;
extern template class codecvt_utf16<char16_t, max_unicode_codepoint(),
	codecvt_mode(consume_header | generate_header | little_endian)>;

extern template class codecvt_utf16<char32_t, max_unicode_codepoint()>;
extern template class codecvt_utf16<char32_t, max_unicode_codepoint(),
	little_endian>;
extern template class codecvt_utf16<char32_t, max_unicode_codepoint(),
	generate_header>;
extern template class codecvt_utf16<char32_t, max_unicode_codepoint(),
	codecvt_mode(generate_header | little_endian)>;
extern template class codecvt_utf16<char32_t, max_unicode_codepoint(),
	consume_header>;
extern template class codecvt_utf16<char32_t, max_unicode_codepoint(),
	codecvt_mode(consume_header | little_endian)>;
extern template class codecvt_utf16<char32_t, max_unicode_codepoint(),
	codecvt_mode(consume_header | generate_header)>;
extern template class codecvt_utf16<char32_t, max_unicode_codepoint(),
	codecvt_mode(consume_header | generate_header | little_endian)>;

} // namespace std

#endif // GUARD_CODECVT_UTF16_H
