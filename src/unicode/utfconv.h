#ifndef GUARD_CODECVT_H
#define GUARD_CODECVT_H 1

#include <cwchar>
#include <cassert>
#include <stdexcept>
#include <limits>
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

enum codecvt_mode
{
	consume_header = 4,
	generate_header = 2,
	little_endian = 1
};
 
inline bool is_utf8_codepoint_start (char c)
{
	unsigned char val = static_cast<unsigned char>(c);
	bool rc = false;
	
// 1011 1111
	if (val <= 0x7f || ((val >= 0xc0) && (val <= 0xfd)))
		rc = true;

	return rc;
}

inline size_t utf8_codepoint_length (char c)
{
	unsigned char val = static_cast<unsigned char>(c);
	if (val <= 0x7f)      return 1;
	else if (val <= 0xbf) return 0;
	else if (val <= 0xdf) return 2;
	else if (val <= 0xef) return 3;
	else if (val <= 0xf7) return 4;
	else if (val <= 0xfb) return 5;
	else if (val <= 0xfd) return 6;

	return 0;
}

inline bool utf8_update_mbstate(std::mbstate_t & s, const char c)
{
	if (is_utf8_codepoint_start(c))
	{
		// handle error
		if (s.__count != 0)
			return false;

		s.__count = utf8_codepoint_length(c);

		if (s.__count == 1)
		{
			s.__value.__wch = c;
			s.__count = 0;
		} else if (s.__count == 0)
		{
			return false;
		} else
		{
			s.__value.__wch = (c & ((1ul << (s.__count + 1) ) - 1));
			s.__count--;
		}
	} else
	{
		// handle error
		if (s.__count <= 0)
			return false;

		s.__value.__wch <<= 6;
		s.__value.__wch |= (0x0000003ful & static_cast<unsigned long>(c));
		s.__count--;
	}

	return true;
}


template<class Elem, unsigned long max_code = 0x10ffff,
         codecvt_mode Mode = (codecvt_mode)0>
class codecvt_utf8 : public std::codecvt<Elem, char, std::mbstate_t>
{
	static_assert(max_code <= std::numeric_limits<Elem>::max(),
	              "Max code is greater than internal character "
	              "representation type");
 public:
	// suck this stuff in from codecvt
	using typename std::codecvt<Elem, char, std::mbstate_t>::state_type;
	using typename std::codecvt<Elem, char, std::mbstate_t>::extern_type;
	using typename std::codecvt<Elem, char, std::mbstate_t>::intern_type;
	using typename std::codecvt<Elem, char, std::mbstate_t>::result;

	explicit codecvt_utf8(std::size_t refs = 0)
	  : std::codecvt<Elem, char, std::mbstate_t>(refs) { }

	~codecvt_utf8() { }

 protected:
	bool do_always_noconv() const noexcept override { return false; }

	int do_encoding() const noexcept override { return 0; }

	result do_in(state_type & state,
	             const extern_type * from,
	             const extern_type * from_end,
	             const extern_type * & from_next,
	             intern_type * to,
	             intern_type * to_limit,
	             intern_type * & to_next) const override
	{
		assert(from <= from_end);
		assert(to <= to_limit);

		for (from_next = from, to_next = to;
		     (from_next < from_end) && (to_next < to_limit);
		     ++from_next)
		{
			if ( ! utf8_update_mbstate(state, *from_next))
				return std::codecvt_base::error;

			if (state.__count == 0)
			{
				if (state.__value.__wch > max_code)
					return std::codecvt_base::error;

				*to_next = state.__value.__wch;
				++to_next;
			}
		}

		if (state.__count != 0)
			return std::codecvt_base::partial;
		else if (from_next < from_end)
			return std::codecvt_base::partial;
		else
			return std::codecvt_base::ok;
	}

	int do_length(state_type & state,
	              const char * from,
	              const char * from_end,
	              std::size_t max) const override
	{
		assert(from <= from_end);
		std::size_t count = 0;
		const extern_type * iter = nullptr;

		for (iter = from; (iter < from_end) && (count < max); ++iter)
		{
			if ( ! utf8_update_mbstate(state, *iter))
				break;

			if (state.__count == 0)
				++count;
		}

		return (iter - from);
	}

	constexpr int do_max_length() const noexcept override
	{
		return (           (max_code <=       0x7f) ? 1 :
		         (         (max_code <=      0x7ff) ? 2 :
		           (       (max_code <=     0xffff) ? 3 :
		             (     (max_code <=   0x1fffff) ? 4 :
		               (   (max_code <=  0x3ffffff) ? 5 :
		                 ( (max_code <= 0x7fffffff) ? 6 : -1 ) ) ) ) ) );
	}

	result do_out(state_type & ,//state,
	              const intern_type * ,//from,
	              const intern_type * ,//from_end,
	              const intern_type * & ,//from_next,
	              extern_type * ,//to,
	              extern_type * ,//to_limit,
	              extern_type * & //to_next)
	) const override
	{
		return std::codecvt_base::noconv;
	}

	result do_unshift(state_type & ,//state,
	                  extern_type * ,//to,
	                  extern_type * ,//to_limit,
	                  extern_type * & //to_next
	) const override
	{
		return std::codecvt_base::noconv;
	}
};

#if 0 
template<class Elem,
         unsigned long Maxcode = 0x10ffff,
         codecvt_mode Mode = (codecvt_mode)0>
class codecvt_utf16 : public std::codecvt<Elem, char, std::mbstate_t> {
 public:
    explicit codecvt_utf16(std::size_t refs = 0);
    ~codecvt_utf16();
};

template<class Elem,
         unsigned long Maxcode = 0x10ffff,
         codecvt_mode Mode = (codecvt_mode)0>
class codecvt_utf8_utf16 : public std::codecvt<Elem, char, std::mbstate_t> {
 public:
    explicit codecvt_utf8_utf16(std::size_t refs = 0);
    ~codecvt_utf8_utf16();
};
#endif

#endif // GUARD_CODECVT_H
