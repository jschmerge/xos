#ifndef GUARD_CODECVT_UTF8_H
#define GUARD_CODECVT_UTF8_H 1

#include <locale>
#include <algorithm>

#include "codecvt_specializations.h"
#include "codecvt_mode.h"
#include "utf_conversion_helpers.h"

namespace std {

// helper class mix-in for codecvt_utf8
template <typename T, unsigned long Maxcode>
struct helper
{
	inline
	constexpr uint32_t max_encodable() const
	{
		return std::min(static_cast<uint32_t>(Maxcode),
		                static_cast<uint32_t>(numeric_limits<T>::max()));
	}
};


//
// The facet shall convert between UTF-8 multibyte sequences and UCS2 or
// UCS4 (depending on the size of Elem) within the program.
//
// - Endianness shall not affect how multibyte sequences are read or written.
//
// - The multibyte sequences may be written as either a text or a binary file.
//
template <class Elem, unsigned long Maxcode = max_unicode_codepoint(),
          codecvt_mode Mode = (codecvt_mode)0>
class codecvt_utf8;

template <unsigned long Maxcode, codecvt_mode Mode>
class codecvt_utf8<wchar_t, Maxcode, Mode>
  : public codecvt<wchar_t, char, mbstate_t>
  , protected helper<wchar_t, Maxcode>
{
 public:
	explicit
	codecvt_utf8(size_t refs = 0)
	: codecvt<wchar_t, char, mbstate_t>(refs)
	{ }

	~codecvt_utf8()
	{ }

 protected:
	virtual result
	do_out(mbstate_t & state,
	       const wchar_t * from_begin,
	       const wchar_t * from_end,
	       const wchar_t * & from_last,
	       char * to_begin,
	       char * to_end,
	       char * & to_last) const override;

	virtual result
	do_unshift(mbstate_t & state,
	           char * to_begin,
	           char * to_end,
	           char * & to_last) const override;

	virtual result
	do_in(mbstate_t &state,
	      const char * from_begin,
	      const char *from_end,
	      const char * & from_last,
	      wchar_t * to_begin,
	      wchar_t * to_end,
	      wchar_t * & to_last) const override;

	virtual int
	do_length(mbstate_t & state,
	          const char * from_begin,
	          const char * from_end,
	          size_t max) const override;

	virtual int
	do_encoding() const noexcept override
	{ return 0; }

	virtual bool
	do_always_noconv() const noexcept override
	{ return false; }

	virtual int
	do_max_length() const noexcept override
	{
		return ( utf8_conversion::bytes_needed(bom_value())
		       + utf8_conversion::bytes_needed(this->max_encodable()));
	}
};

template <unsigned long Maxcode, codecvt_mode Mode>
class codecvt_utf8<char16_t, Maxcode, Mode>
  : public codecvt<char16_t, char, mbstate_t>
  , protected helper<char16_t, Maxcode>
{
 public:
	explicit
	codecvt_utf8(size_t refs = 0)
	: codecvt<char16_t, char, mbstate_t>(refs)
	{ }

	~codecvt_utf8()
	{ }

 protected:
	virtual result
	do_out(mbstate_t & state,
	       const wchar_t * from_begin,
	       const wchar_t * from_end,
	       const wchar_t * & from_last,
	       char * to_begin,
	       char * to_end,
	       char * & to_last) const override;

	virtual result
	do_unshift(mbstate_t & state,
	           char * to_begin,
	           char * to_end,
	           char * & to_last) const override;

	virtual result
	do_in(mbstate_t &state,
	      const char * from_begin,
	      const char *from_end,
	      const char * & from_last,
	      wchar_t * to_begin,
	      wchar_t * to_end,
	      wchar_t * & to_last) const override;

	virtual int
	do_length(mbstate_t & state,
	          const char * from_begin,
	          const char * from_end,
	          size_t max) const override;

	virtual int
	do_encoding() const noexcept override
	{ return 0; }

	virtual bool
	do_always_noconv() const noexcept override
	{ return false; }

	virtual int
	do_max_length() const noexcept override
	{
		return ( utf8_conversion::bytes_needed(bom_value())
		       + utf8_conversion::bytes_needed(this->max_encodable()));
	}
};

template <unsigned long Maxcode, codecvt_mode Mode>
class codecvt_utf8<char32_t, Maxcode, Mode>
  : public codecvt<char32_t, char, mbstate_t>
  , protected helper<char32_t, Maxcode>
{
 public:
	explicit
	codecvt_utf8(size_t refs = 0)
	: codecvt<char32_t, char, mbstate_t>(refs)
	{ }

	~codecvt_utf8()
	{ }

 protected:
	virtual result
	do_out(mbstate_t & state,
	       const wchar_t * from_begin,
	       const wchar_t * from_end,
	       const wchar_t * & from_last,
	       char * to_begin,
	       char * to_end,
	       char * & to_last) const override;

	virtual result
	do_unshift(mbstate_t & state,
	           char * to_begin,
	           char * to_end,
	           char * & to_last) const override;

	virtual result
	do_in(mbstate_t &state,
	      const char * from_begin,
	      const char *from_end,
	      const char * & from_last,
	      wchar_t * to_begin,
	      wchar_t * to_end,
	      wchar_t * & to_last) const override;

	virtual int
	do_length(mbstate_t & state,
	          const char * from_begin,
	          const char * from_end,
	          size_t max) const override;

	virtual int
	do_encoding() const noexcept override
	{ return 0; }

	virtual bool
	do_always_noconv() const noexcept override
	{ return false; }

	virtual int
	do_max_length() const noexcept override
	{
		return ( utf8_conversion::bytes_needed(bom_value())
		       + utf8_conversion::bytes_needed(this->max_encodable()));
	}
};

} // namespace std

#endif // GUARD_CODECVT_UTF8_H

