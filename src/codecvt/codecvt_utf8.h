#ifndef GUARD_CODECVT_UTF8_H
#define GUARD_CODECVT_UTF8_H 1

#include <locale>
#include <algorithm>
#include <cassert>

#include "codecvt_specializations.h"
#include "codecvt_mode.h"
#include "utf_conversion_helpers.h"


namespace std {

//
// The facet shall convert between UTF-8 multibyte sequences and UCS2 or
// UCS4 (depending on the size of Elem) within the program.
//
// - Endianness shall not affect how multibyte sequences are read or written.
//
// - The multibyte sequences may be written as either a text or a binary file.
//
template <class Elem,
          unsigned long Maxcode = max_unicode_codepoint(),
          codecvt_mode Mode = (codecvt_mode)0>
class codecvt_utf8
  : public codecvt<Elem, char, mbstate_t>
{
 public:
	static_assert(is_wide_character<Elem>::value,
	              "Element type must be a wide character type");
	static_assert(Maxcode <= 0x7ffffffful,
	              "max code must be less than or equal to 0x7ffffffu");

	typedef Elem      intern_type;
	typedef char      extern_type;
	typedef mbstate_t state_type;

	explicit
	codecvt_utf8(size_t refs = 0)
	: codecvt<Elem, char, mbstate_t>(refs)
	{ }

	~codecvt_utf8()
	{ }

 protected:
	inline bool generate_bom() const
	{ return (Mode & generate_header); }

	inline bool consume_bom() const
	{ return (Mode & consume_header); }

	inline
	constexpr char32_t max_encodable() const
	{
		return std::min(
		         static_cast<uint32_t>(0x7fffffffu),
		         std::min(static_cast<uint32_t>(Maxcode),
		                  static_cast<uint32_t>(numeric_limits<Elem>::max())));
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
		namespace utf8 = utf8_conversion;
		namespace utf16 = utf16_conversion;

		codecvt_base::result rc = codecvt_base::ok;

		assert(from_begin <= from_end);
		assert(to_begin <= to_end);

		from_last = from_begin;
		to_last = to_begin;

		if (this->generate_bom() && (from_last < from_end))
		{
			state.__value.__wch = bom_value();
			char val = utf8::extract_leader_byte(state);

			*to_last = val;
			++to_last;
		}

		while (  (to_last < to_end)
		      && (from_last < from_end)
		      && rc == codecvt_base::ok)
		{
			if (state.__count == 0)
			{
				if (static_cast<uint32_t>(*from_last) > this->max_encodable())
					return codecvt_base::error;
				else if (utf16::is_surrogate(*from_last))
					return codecvt_base::error;

				state.__value.__wch = *from_last;
				char val = utf8::extract_leader_byte(state);

				if (state.__count < 0)
				{
					rc = codecvt_base::error;
					break;
				} else
				{
					*to_last = val;
					++to_last;
					++from_last;
				}
			}

			rc = this->do_unshift(state, to_last, to_end, to_last);
		}

		if (rc == codecvt_base::ok)
			this->do_unshift(state, to_last, to_end, to_last);

		return rc;
	}

	virtual codecvt_base::result
	do_unshift(mbstate_t & state,
	           char * to_begin,
	           char * to_end,
	           char * & to_last) const override
	{
		namespace utf8 = utf8_conversion;

		if (  (state.__count < 0)
		   || (state.__count > std::max(this->do_max_length(), 3)))
			return codecvt_base::error;

		if ( (  ((Mode & generate_header) != generate_header)
		     || (state.__value.__wch != bom_value()) )
		   && ( state.__value.__wch > Maxcode) )
			return codecvt_base::error;

		to_last = to_begin;

		while ((to_last < to_end) && (state.__count > 0))
		{
			*to_last = utf8::next_byte(state);
			++to_last;
			--state.__count;
		}

		return ( (state.__count == 0) ?
		         codecvt_base::ok :
		         codecvt_base::partial );
	}

	virtual codecvt_base::result
	do_in(mbstate_t &state,
	      const char * from_begin,
	      const char *from_end,
	      const char * & from_last,
	      intern_type * to_begin,
	      intern_type * to_end,
	      intern_type * & to_last) const override
	{
		namespace utf8 = utf8_conversion;

		assert(from_begin <= from_end);
		assert(to_begin <= to_end);

		from_last = from_begin;
		to_last = to_begin;

		if (  (state.__count == 0)
		   && this->consume_bom()
		   && (from_end - from_last) > 2)
		{
			if (  (static_cast<uint8_t>(from_last[0]) == 0xefu)
			   && (static_cast<uint8_t>(from_last[1]) == 0xbbu)
			   && (static_cast<uint8_t>(from_last[2]) == 0xbfu) )
			{
				from_last += 3;
			}
		}

		while ( (from_last < from_end) && (to_last < to_end) )
		{
			if ( ! utf8::update_mbstate(state, *from_last))
				return codecvt_base::error;

			if (state.__count == 0)
			{

				if (state.__value.__wch > this->max_encodable())
					return codecvt_base::error;
				else
				{
					*to_last = state.__value.__wch;
					++to_last;
				}
			}
			++from_last;
		}

		return (((state.__count != 0) || (from_last < from_end) ) ?
		        codecvt_base::partial : codecvt_base::ok );
	}

	virtual int
	do_length(mbstate_t & state,
	          const char * from_begin,
	          const char * from_end,
	          size_t max) const override
	{
		namespace utf8 = utf8_conversion;

		size_t count = 0;
		const char * from_last = from_begin;

		if (  (state.__count == 0)
		   && this->consume_bom()
		   && (from_end - from_last) > 2)
		{
			if (  (static_cast<uint8_t>(from_last[0]) == 0xefu)
			   && (static_cast<uint8_t>(from_last[1]) == 0xbbu)
			   && (static_cast<uint8_t>(from_last[2]) == 0xbfu) )
			{
				from_last += 3;
			}
		}

		while (  (from_last < from_end)
		      && (count < max)
		      && utf8::update_mbstate(state, *from_last))
		{
			++from_last;
			if (state.__count == 0)
			{
				if (state.__value.__wch > Maxcode)
					break;
			}
		}

		return (from_last - from_begin);
	}

	virtual int
	do_encoding() const noexcept override
	{ return 0; }

	virtual bool
	do_always_noconv() const noexcept override
	{ return false; }

	virtual int
	do_max_length() const noexcept override
	{
		return ( (this->consume_bom() ?
		          utf8_conversion::bytes_needed(bom_value()) : 0)
		       + utf8_conversion::bytes_needed(this->max_encodable()));
	}
};

//
// massive number of specializations here - for all combos of the
// codecvt_mode enum plus all three wide char types. Unfortunately,
// we can't burry the functions defined in the class in a source
// file, but we can at least provide some coverage of the common
// cases.
//
FOR_ALL_CVT_MODES(EXTERN_CVT_TEMPLATE, codecvt_utf8, wchar_t, 0x7f);
FOR_ALL_CVT_MODES(EXTERN_CVT_TEMPLATE, codecvt_utf8, wchar_t, 0xff);
FOR_ALL_CVT_MODES(EXTERN_CVT_TEMPLATE, codecvt_utf8, wchar_t, 0xffff);
FOR_ALL_CVT_MODES(EXTERN_CVT_TEMPLATE, codecvt_utf8, wchar_t, 0x10ffff);

FOR_ALL_CVT_MODES(EXTERN_CVT_TEMPLATE, codecvt_utf8, char16_t, 0x7f);
FOR_ALL_CVT_MODES(EXTERN_CVT_TEMPLATE, codecvt_utf8, char16_t, 0xff);
FOR_ALL_CVT_MODES(EXTERN_CVT_TEMPLATE, codecvt_utf8, char16_t, 0xffff);
FOR_ALL_CVT_MODES(EXTERN_CVT_TEMPLATE, codecvt_utf8, char16_t, 0x10ffff);

FOR_ALL_CVT_MODES(EXTERN_CVT_TEMPLATE, codecvt_utf8, char32_t, 0x7f);
FOR_ALL_CVT_MODES(EXTERN_CVT_TEMPLATE, codecvt_utf8, char32_t, 0xff);
FOR_ALL_CVT_MODES(EXTERN_CVT_TEMPLATE, codecvt_utf8, char32_t, 0xffff);
FOR_ALL_CVT_MODES(EXTERN_CVT_TEMPLATE, codecvt_utf8, char32_t, 0x10ffff);
FOR_ALL_CVT_MODES(EXTERN_CVT_TEMPLATE, codecvt_utf8, char32_t, 0x3ffffff);
FOR_ALL_CVT_MODES(EXTERN_CVT_TEMPLATE, codecvt_utf8, char32_t, 0x7fffffff);

} // namespace std

#endif // GUARD_CODECVT_UTF8_H

