#ifndef GUARD_UTF8_H
#define GUARD_UTF8_H 1

#include <locale>
#include <cassert>

#include "utf8conv.h"

#if 0
namespace utf8_conversion {

constexpr uint32_t length_limits[] = {
	0x00000080ul,
	0x00000800ul,
	0x00010000ul,
	0x00200000ul,
	0x04000000ul,
	0x80000000ul
};

constexpr uint8_t one_byte_limit   = 0x80u;
constexpr uint8_t invalid_limit    = 0xc0u;
constexpr uint8_t two_byte_limit   = 0xe0u;
constexpr uint8_t three_byte_limit = 0xf0u;
constexpr uint8_t four_byte_limit  = 0xf8u;
constexpr uint8_t five_byte_limit  = 0xfcu;
constexpr uint8_t six_byte_limit   = 0xfeu;

constexpr uint8_t leader_prefix2    = 0xc0u;
constexpr uint8_t leader_prefix3    = 0xe0u;
constexpr uint8_t leader_prefix4    = 0xf0u;
constexpr uint8_t leader_prefix5    = 0xf8u;
constexpr uint8_t leader_prefix6    = 0xfcu;
//
// encode
//
constexpr unsigned int continuation_bits_per_byte = 6;
constexpr uint8_t continuation_byte_value_mask = 0x3fu;
constexpr uint8_t continuation_byte_prefix = 0x80u;

constexpr inline uint32_t data_shift(int byte_number)
{
	return ((byte_number - 1) * continuation_bits_per_byte);
}

constexpr inline uint32_t data_mask(int byte_number)
{
	return (continuation_byte_value_mask << data_shift(byte_number));
}

constexpr inline char next_byte(const std::mbstate_t & s)
{
	return ( continuation_byte_prefix
	       | ( data_mask(s.__count)
	         & s.__value.__wch) >> data_shift(s.__count));
}

inline char remove_leader_byte(mbstate_t & state)
{
	// Someone smarter than me can probably figure out how to write
	// this elegantly as a constexpr function
	char leader = 0;
	if (state.__value.__wch < length_limits[0])
	{
		leader = state.__value.__wch;
		state.__value.__wch = 0;
		state.__count = 0;
	} else if (state.__value.__wch < length_limits[1])
	{
		leader = (leader_prefix2 | (state.__value.__wch >> 6));
		state.__count = 1;
	} else if (state.__value.__wch < length_limits[2])
	{
		leader = (leader_prefix3 | (state.__value.__wch >> 12));
		state.__count = 2;
	} else if (state.__value.__wch < length_limits[3])
	{
		leader = (leader_prefix4 | (state.__value.__wch >> 18));
		state.__count = 3;
	} else if (state.__value.__wch < length_limits[4])
	{
		leader = (leader_prefix5 | (state.__value.__wch >> 24));
		state.__count = 4;
	} else if (state.__value.__wch < length_limits[5])
	{
		leader = (leader_prefix6 | (state.__value.__wch >> 30));
		state.__count = 5;
	} else
	{
		// on error, we set __count to -1; - see do_out
		state.__count = -1;
	}

	return leader;
}

//
// decode
//
constexpr uint32_t max_encodable_value = 0x7fffffff;
constexpr uint8_t continuation_byte_prefix_mask = 0xc0u;

constexpr size_t bytes_needed (uint32_t c)
{
	return ( (static_cast<uint32_t>(c) < length_limits[0]) ? 1 :
	       ( (static_cast<uint32_t>(c) < length_limits[1]) ? 2 :
	       ( (static_cast<uint32_t>(c) < length_limits[2]) ? 3 :
	       ( (static_cast<uint32_t>(c) < length_limits[3]) ? 4 :
	       ( (static_cast<uint32_t>(c) < length_limits[4]) ? 5 :
	       ( (static_cast<uint32_t>(c) < length_limits[5]) ? 6 :
	          -1 ) ) ) ) ) );
}

inline constexpr size_t codepoint_length (char c)
{
	return ( (static_cast<uint8_t>(c) < one_byte_limit   ) ? 1 :
	       ( (static_cast<uint8_t>(c) < invalid_limit    ) ? 0 :
	       ( (static_cast<uint8_t>(c) < two_byte_limit   ) ? 2 :
	       ( (static_cast<uint8_t>(c) < three_byte_limit ) ? 3 :
	       ( (static_cast<uint8_t>(c) < four_byte_limit  ) ? 4 :
	       ( (static_cast<uint8_t>(c) < five_byte_limit  ) ? 5 :
	       ( (static_cast<uint8_t>(c) < six_byte_limit   ) ? 6 :
	          0 ) ) ) ) ) ) );
}

inline bool is_codepoint_start (char c)
{
	bool rc = false;
	
	if (  (static_cast<unsigned char>(c) < six_byte_limit)
	   && ( continuation_byte_prefix != ( static_cast<unsigned char>(c)
	                                    & continuation_byte_prefix_mask) ) )
	{
		rc = true;
	}

	return rc;
}

constexpr unsigned char leader_bits(char c, int count)
{
	return (c & ((1ul << (7 - count) ) - 1));
}

constexpr inline uint32_t continuation_bits(char c)
{
	return ( (1ul << continuation_bits_per_byte) - 1)
	       & static_cast<unsigned long>(c);
}

inline bool update_mbstate(std::mbstate_t & s, const char c)
{
	bool rc = true;

	if (s.__count <= 0)
	{
		// handle error
		rc = false;
	}
	else if (is_codepoint_start(c))
	{
		s.__count = codepoint_length(c);

		if (s.__count == 1)
		{
			s.__value.__wch = c;
			s.__count = 0;
		} else
		{
			s.__value.__wch = leader_bits(c, s.__count);
			s.__count--;
		}
	} else
	{
		s.__value.__wch <<= continuation_bits_per_byte;
		s.__value.__wch |= continuation_bits(c);
		s.__count--;
	}

	return rc;
}

} // namespace utf8_conversion
#endif

//////////////////////////////////////////////////////////////////////
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
	do_out(mbstate_t&,
	       const char16_t*, const char16_t*, const char16_t*&,
	       char*, char *, char*&) const;

	virtual result
	do_unshift(mbstate_t&, char*, char*, char*&) const;

	virtual result
	do_in(mbstate_t&,
	      const char*, const char*, const char*&,
	      char16_t*, char16_t*, char16_t*&) const;

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
				} else if (state.__value.__wch < 0x10ffff)
				{
					if ((count + 2) >= max)
						count += 2;
				} else
				{
					// error
				}
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
			if (state.__count != 0)
			{
				*to_next = utf8::next_byte(state);
				++to_next;
				--state.__count;
			} else
			{
				state.__value.__wch = *from_next;
				char val = utf8::remove_leader_byte(state);

				if (state.__count < 0)
					return codecvt_base::error;

				*to_next = val;
				++to_next;
				++from_next;
			}
		}

		do_unshift(state, to_next, to_end, to_next);

		return ( (  (state.__count == 0)
		         && (from_next == from_end)) ?
		         codecvt_base::ok :
		         codecvt_base::partial );
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

		return ( (state.__count == 0) ?
		          codecvt_base::ok :
		          codecvt_base::partial );
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
				return std::codecvt_base::error;

			if (state.__count == 0)
			{
				*to_next = state.__value.__wch;
				++to_next;
			}
		}

		return ( ( (state.__count != 0) || (from_next < from_end) ) ?
			     std::codecvt_base::partial :
		         std::codecvt_base::ok );
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

#endif // GUARD_UTF8_H
