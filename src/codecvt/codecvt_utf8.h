#ifndef GUARD_CODECVT_UTF8_H
#define GUARD_CODECVT_UTF8_H 1

#include <locale>
#include <algorithm>

#include "codecvt_specializations.h"
#include "codecvt_mode.h"

namespace std {

// helper class mix-in for codecvt_utf8
template <typename T, unsigned long Maxcode>
struct codecvt_utf8_helper
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
  , protected codecvt_utf8_helper<wchar_t, Maxcode>
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
	do_out(mbstate_t &, const wchar_t *, const wchar_t *, const wchar_t * &,
	       char *, char *, char * &) const override;

	virtual result
	do_unshift(mbstate_t &, char *, char *, char * &) const override;

	virtual result
	do_in(mbstate_t &, const char *, const char *, const char * &,
	      wchar_t *, wchar_t *, wchar_t * &) const override;

	virtual int
	do_length(mbstate_t &, const char *, const char *, size_t) const override;

	virtual int
	do_encoding() const noexcept override;

	virtual bool
	do_always_noconv() const noexcept override;

	virtual int
	do_max_length() const noexcept override;
};

template <unsigned long Maxcode, codecvt_mode Mode>
class codecvt_utf8<char16_t, Maxcode, Mode>
  : public codecvt<char16_t, char, mbstate_t>
  , protected codecvt_utf8_helper<char16_t, Maxcode>
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
	do_out(mbstate_t &, const char16_t *, const char16_t *, const char16_t * &,
	       char *, char *, char * &) const override;

	virtual result
	do_unshift(mbstate_t &, char *, char *, char * &) const override;

	virtual result
	do_in(mbstate_t &, const char *, const char *, const char * &,
	      char16_t *, char16_t *, char16_t * &) const override;

	virtual int
	do_length(mbstate_t &, const char *, const char *, size_t) const override;

	virtual int
	do_encoding() const noexcept override;

	virtual bool
	do_always_noconv() const noexcept override;

	virtual int
	do_max_length() const noexcept override;
};

template <unsigned long Maxcode, codecvt_mode Mode>
class codecvt_utf8<char32_t, Maxcode, Mode>
  : public codecvt<char32_t, char, mbstate_t>
  , protected codecvt_utf8_helper<char32_t, Maxcode>
{
 public:
	explicit
	codecvt_utf8(size_t refs = 0)
	: codecvt<char32_t, char, mbstate_t>(refs)
	{ }

	~codecvt_utf8()
	{ }

	virtual result
	do_out(mbstate_t &, const char32_t *, const char32_t *, const char32_t * &,
	       char *, char *, char * &) const override;

	virtual result
	do_unshift(mbstate_t &, char *, char *, char * &) const override;

	virtual result
	do_in(mbstate_t &, const char *, const char *, const char * &,
	      char32_t *, char32_t *, char32_t * &) const override;

	virtual int
	do_length(mbstate_t &, const char *, const char *, size_t) const override;

	virtual int
	do_encoding() const noexcept override;

	virtual bool
	do_always_noconv() const noexcept override;

	virtual int
	do_max_length() const noexcept override;
};

} // namespace std

#endif // GUARD_CODECVT_UTF8_H

