#ifndef GUARD_STDWORKAROUNDS_H
#define GUARD_STDWORKAROUNDS_H 1

#include <cwchar>
#include <locale>

namespace std {

template<> class codecvt<char16_t, char, mbstate_t>
  : public __codecvt_abstract_base<char16_t, char, mbstate_t>
{
 public:
	typedef char16_t          intern_type;
	typedef char              extern_type;
	typedef mbstate_t         state_type;

	static locale::id id;

	explicit codecvt(size_t __refs = 0)
	  : __codecvt_abstract_base<char16_t, char, mbstate_t>(__refs) { }

	explicit codecvt(__c_locale, size_t __refs = 0)
	  : __codecvt_abstract_base<char16_t, char, mbstate_t>(__refs) { }

 protected:
	virtual ~codecvt() { }

	virtual result
	do_out(mbstate_t&, const char16_t*, const char16_t*, const char16_t*&,
	       char*, char *, char*&) const = 0;

	virtual result do_unshift(mbstate_t&, char*, char*, char*&) const = 0;

	virtual result
	do_in(mbstate_t&, const char*, const char*, const char*&,
	      char16_t*, char16_t*, char16_t*&) const = 0;

	virtual int do_encoding() const noexcept = 0;

	virtual bool do_always_noconv() const noexcept = 0;

	virtual int
	do_length(mbstate_t&, const char*, const char*, size_t) const = 0;

	virtual int do_max_length() const noexcept = 0;
};

template<> class codecvt<char32_t, char, mbstate_t>
  : public __codecvt_abstract_base<char32_t, char, mbstate_t>
{
 public:
	typedef char32_t          intern_type;
	typedef char              extern_type;
	typedef mbstate_t         state_type;

	static locale::id id;

	explicit codecvt(size_t __refs = 0)
	  : __codecvt_abstract_base<char32_t, char, mbstate_t>(__refs) { }

	explicit codecvt(__c_locale, size_t __refs = 0)
	  : __codecvt_abstract_base<char32_t, char, mbstate_t>(__refs) { }

 protected:
	virtual ~codecvt() { }

	virtual result
	do_out(mbstate_t&, const char32_t*, const char32_t*, const char32_t*&,
	       char*, char *, char*&) const = 0;

	virtual result do_unshift(mbstate_t&, char*, char*, char*&) const = 0;

	virtual result
	do_in(mbstate_t&, const char*, const char*, const char*&,
	      char32_t*, char32_t*, char32_t*&) const = 0;

	virtual int do_encoding() const noexcept = 0;

	virtual bool do_always_noconv() const noexcept = 0;

	virtual int
	do_length(mbstate_t&, const char*, const char*, size_t) const = 0;

	virtual int do_max_length() const noexcept = 0;
};

} // namespace std

#endif // GUARD_STDWORKAROUNDS_H
