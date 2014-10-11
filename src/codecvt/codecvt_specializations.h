#ifndef GUARD_CODECVT_SPECIALIZATIONS_H
#define GUARD_CODECVT_SPECIALIZATIONS_H 1

#include <locale>
#include <cassert>

#include "utf_conversion_helpers.h"

namespace std {

//
// The specialization codecvt<char16_t, char, mbstate_t> converts between
// the UTF-16 and UTF-8 encoding forms.
//
template<> class codecvt<char16_t, char, mbstate_t>
  : public __codecvt_abstract_base<char16_t, char, mbstate_t>
{
 public:
	typedef char16_t          intern_type;
	typedef char              extern_type;
	typedef mbstate_t         state_type;

	static locale::id id;

	explicit codecvt(size_t __refs = 0)
	: __codecvt_abstract_base<char16_t, char, mbstate_t>(__refs)
	{ }

	explicit codecvt(__c_locale, size_t __refs = 0)
	: __codecvt_abstract_base<char16_t, char, mbstate_t>(__refs)
	{ }

 protected:
	virtual
	~codecvt()
	{ }

	virtual result
	do_out(mbstate_t &, const char16_t *, const char16_t *, const char16_t * &,
	       char *, char *, char * &) const override;

	virtual result
	do_unshift(mbstate_t &, char *, char *, char * &) const override;

	virtual result
	do_in(mbstate_t &, const char *, const char *, const char * &,
	      char16_t *, char16_t *, char16_t * &) const override;

	virtual int
	do_encoding() const noexcept override
	{ return 0; }

	virtual bool
	do_always_noconv() const noexcept override
	{ return false; }

	virtual int
	do_length(mbstate_t &, const char *, const char *, size_t) const override;

	virtual int
	do_max_length() const noexcept
	{
		// max_length is defined to be the maximum return value from
		// do_length(state, begin, end, 1) ... so max number of bytes it
		// could take to produce a single char16_t. There are two
		// interpretations of this:
		//
		// 1) max number of char's to produce a valid unicode codepoint that is
		//    encodable as utf16 (4)
		//
		// 2) max number of char's to produce a unicode codepoint that is in
		//    the basic multilingual plain (3)
		//
		// Since this function is most likely used for determining buffer
		// sizes, we're erring on the side of caution and choosing 4
		return 4;
	}
};

//
// The specialization codecvt<char32_t, char, mbstate_t> converts between
// the UTF-32 and UTF-8 encoding forms.
//
template<> class codecvt<char32_t, char, mbstate_t>
  : public __codecvt_abstract_base<char32_t, char, mbstate_t>
{
 public:
	typedef char32_t          intern_type;
	typedef char              extern_type;
	typedef mbstate_t         state_type;

	static locale::id id;

	explicit codecvt(size_t __refs = 0)
	: __codecvt_abstract_base<char32_t, char, mbstate_t>(__refs)
	{ }

	explicit codecvt(__c_locale, size_t __refs = 0)
	: __codecvt_abstract_base<char32_t, char, mbstate_t>(__refs)
	{ }

 protected:
	virtual ~codecvt()
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
	do_encoding() const noexcept override
	{ return 0; }

	virtual bool
	do_always_noconv() const noexcept override
	{ return false; }

	virtual int
	do_max_length() const noexcept override
	{
		namespace utf8 = utf8_conversion;
		return utf8::bytes_needed(utf8::max_encodable_value());
	}
};

} // namespace std

#endif // GUARD_CODECVT_SPECIALIZATIONS_H
