#ifndef GUARD_CODECVT_SPECIALIZATIONS_H
#define GUARD_CODECVT_SPECIALIZATIONS_H 1

#include <locale>
#include <cassert>

#include "utf8conv.h"

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
	  : __codecvt_abstract_base<char16_t, char, mbstate_t>(__refs) { }

	explicit codecvt(__c_locale, size_t __refs = 0)
	  : __codecvt_abstract_base<char16_t, char, mbstate_t>(__refs) { }

 protected:
	virtual
	~codecvt() { }

	virtual result
	do_out(mbstate_t & state,
	       const char16_t * from_begin,
	       const char16_t * from_end,
	       const char16_t * & from_next,
	       char * to_begin,
	       char * to_end,
	       char * & to_next) const override
	{
		namespace utf8 = utf8_conversion;
		namespace utf16 = utf16_conversion;

		result res = ok;

		from_next = from_begin;
		to_next = to_begin;

		while ((to_next < to_end) && (from_next < from_end) && (res == ok))
		{
			if (utf16::update_mbstate(state, *from_next))
				++from_next;
			else
				res = error;

			if ((state.__count == 0) && (res == ok))
			{
				// have a complete character
				char val = utf8::extract_leader_byte(state);
				if (state.__count < 0)
					res = error;

				*to_next = val;
				++to_next;
			}

			if (state.__count > 0)
			{
				res = do_unshift(state, to_next, to_end, to_next);
			}
		}

		return res;
	}

	virtual result
	do_unshift(mbstate_t & state,
	           char * to_begin,
	           char * to_end,
	           char * & to_next) const
	{
		namespace utf8 = utf8_conversion;

		assert((state.__count) >= 0 && (state.__count < do_max_length()));

		to_next = to_begin;

		while ((to_next < to_end) && state.__count > 0)
		{
			*to_next = utf8::next_byte(state);
			++to_next;
			--state.__count;
		}

		return ( (state.__count == 0) ?  ok : partial );
	}

	virtual result
	do_in(mbstate_t & state,
	      const char * from_begin,
	      const char * from_end,
	      const char * & from_next,
	      char16_t * to_begin,
	      char16_t * to_end,
	      char16_t * & to_next) const
	{
		namespace utf8 = utf8_conversion;
		namespace utf16 = utf16_conversion;
		result res = ok;

		assert(from_begin <= from_end);
		assert(to_begin <= to_end);

		from_next = from_begin;
		to_next = to_begin;

		while ( (from_next < from_end) && (to_next < to_end) )
		{
			if (state.__count < 0)
			{
				*to_next = state.__value.__wch;
				++to_next;
				state.__count = 0;
			}

			if (utf8::update_mbstate(state, *from_next))
			{
				++from_next;
			} else
			{
				return res = error;
				break;
			}

			if (state.__count == 0)
			{
				*to_next = utf16::extract_leader_value(state);

				++to_next;
			}
		}

		if ((state.__count < 0) && (to_next < to_end))
		{
			*to_next = state.__value.__wch;
			++to_next;
			state.__count = 0;
		}

		return res;
	}

	virtual int
	do_encoding() const noexcept
	{ return 0; }

	virtual bool
	do_always_noconv() const noexcept
	{ return false; }

	virtual int
	do_length(mbstate_t & state,
	          const char * from_begin,
	          const char * from_end,
	          size_t max) const override
	{
		namespace utf8 = utf8_conversion;

		size_t count = 0;
		const char * i = from_begin;

		while (  (i < from_end)
		      && (count < max)
		      && utf8::update_mbstate(state, *i))
		{
			if (state.__count == 0)
			{
				if (state.__value.__wch < 0x10000u)
				{
					++count;
				} else if (state.__value.__wch <= 0x10ffff)
				{
					count += 2;
				} else
				{
					break;
				}
				state.__value.__wch = 0;
			}
			++i;
		}

		return (i - from_begin);
	}

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
	do_out(mbstate_t & state,
	       const char32_t * from_begin,
	       const char32_t * from_end,
	       const char32_t * & from_next,
	       char * to_begin,
	       char * to_end,
	       char * & to_next) const override
	{
		namespace utf8 = utf8_conversion;

		assert(from_begin <= from_end);
		assert(to_begin <= to_end);

		from_next = from_begin;
		to_next = to_begin;

		while ((to_next < to_end) && (from_next < from_end))
		{
			if (state.__count == 0)
			{
				state.__value.__wch = *from_next;
				char val = utf8::extract_leader_byte(state);

				if (state.__count < 0)
					return error;

				*to_next = val;
				++to_next;
				++from_next;
			}

			do_unshift(state, to_next, to_end, to_next);
		}

		do_unshift(state, to_next, to_end, to_next);

		return ( (  (state.__count == 0)
		         && (from_next == from_end)) ?
		         ok : partial );
	}

	virtual result
	do_unshift(mbstate_t & state,
	           char * to_begin,
	           char * to_end,
	           char * & to_next) const override
	{
		namespace utf8 = utf8_conversion;

		assert((state.__count) >= 0 && (state.__count < do_max_length()));

		to_next = to_begin;

		while ((to_next < to_end) && state.__count > 0)
		{
			*to_next = utf8::next_byte(state);
			++to_next;
			--state.__count;
		}

		return ( (state.__count == 0) ?  ok : partial );
	}

	virtual result
	do_in(mbstate_t & state,
	      const char * from_begin,
	      const char * from_end,
	      const char * & from_next,
	      char32_t * to_begin,
	      char32_t * to_end,
	      char32_t * & to_next) const
	{
		namespace utf8 = utf8_conversion;
		assert(from_begin <= from_end);
		assert(to_begin <= to_end);

		for (from_next = from_begin, to_next = to_begin;
		     (from_next < from_end) && (to_next < to_end);
		     ++from_next)
		{
			if ( ! utf8::update_mbstate(state, *from_next))
				return error;

			if (state.__count == 0)
			{
				*to_next = state.__value.__wch;
				++to_next;
			}
		}

		return ( ( (state.__count != 0) || (from_next < from_end) ) ?
			     partial :
		         ok );
	}

	virtual int
	do_encoding() const noexcept override
	{ return 0; }

	virtual bool
	do_always_noconv() const noexcept override
	{ return false; }

	virtual int
	do_length(mbstate_t & state,
	          const char * from_begin,
	          const char * from_end,
	          size_t max) const override
	{
		namespace utf8 = utf8_conversion;

		size_t count = 0;
		const char * i = from_begin;

		while (  (i < from_end)
		      && (count < max)
		      && utf8::update_mbstate(state, *i))
		{
			if (state.__count == 0)
				++count;
			++i;
		}

		return (i - from_begin);
	}

	virtual int
	do_max_length() const noexcept override
	{
		namespace utf8 = utf8_conversion;
		return utf8::bytes_needed(utf8::max_encodable_value());
	}
};

} // namespace std

#endif // GUARD_CODECVT_SPECIALIZATIONS_H
