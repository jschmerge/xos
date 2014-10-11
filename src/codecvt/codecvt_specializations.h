#ifndef GUARD_CODECVT_SPECIALIZATIONS_H
#define GUARD_CODECVT_SPECIALIZATIONS_H 1

#include <locale>
#include <cassert>

namespace std {

//
// The specialization codecvt<char16_t, char, mbstate_t> converts between
// the UTF-16 and UTF-8 encoding forms.
//
template<> class codecvt<char16_t, char, mbstate_t>
  : public __codecvt_abstract_base<char16_t, char, mbstate_t>
{
 public:
	typedef codecvt_base::result result;
	typedef char16_t             intern_type;
	typedef char                 extern_type;
	typedef mbstate_t            state_type;

	static locale::id id;

	explicit
	codecvt(size_t __refs = 0)
	: __codecvt_abstract_base<char16_t, char, mbstate_t>(__refs)
	{ }

	// Constructor is for compatibility with the codecvt<char, char, mbstate_t>
	// and codecvt<char, wchar_t, mbstate_t> specializations
	explicit
	codecvt(__c_locale, size_t __refs = 0)
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
	do_length(mbstate_t &, const char *, const char *, size_t) const override;

	virtual int
	do_encoding() const noexcept override;

	virtual bool
	do_always_noconv() const noexcept override;

	virtual int
	do_max_length() const noexcept override;
};

//
// The specialization codecvt<char32_t, char, mbstate_t> converts between
// the UTF-32 and UTF-8 encoding forms.
//
// Note regarding implementation: this specialization can convert values
// greater than the maximum defined unicode codepoint (0x10ffff), as such
// it can return utf8 strings that are as long as six bytes for large char32
// values. The return value of max_length reflects this.
//
// The design choice to allow for larger values than the maximum codepoint
// size was made because the c++11 standard adds the codecvt_utf8 class
// which allows for the clamping of the maximum encoded value to the
// proper unicode range - use that explicitly if you need to deal with
// unknown ranges.
//
template<> class codecvt<char32_t, char, mbstate_t>
  : public __codecvt_abstract_base<char32_t, char, mbstate_t>
{
 public:
	typedef codecvt_base::result result;
	typedef char32_t             intern_type;
	typedef char                 extern_type;
	typedef mbstate_t            state_type;

	static locale::id id;

	explicit
	codecvt(size_t __refs = 0)
	: __codecvt_abstract_base<char32_t, char, mbstate_t>(__refs)
	{ }

	// Constructor is for compatibility with the codecvt<char, char, mbstate_t>
	// and codecvt<char, wchar_t, mbstate_t> specializations
	explicit
	codecvt(__c_locale, size_t __refs = 0)
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
	do_encoding() const noexcept override;

	virtual bool
	do_always_noconv() const noexcept override;

	virtual int
	do_max_length() const noexcept override;
};

} // namespace std

#endif // GUARD_CODECVT_SPECIALIZATIONS_H
