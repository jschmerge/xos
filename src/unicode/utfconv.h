#ifndef GUARD_CODECVT_H
#define GUARD_CODECVT_H 1

#include <cwchar>
#include <cassert>
#include <limits>
#include <locale>

#include "stdworkarounds.h"

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
	if (val < 0x80)      return 1;
	else if (val < 0xc0) return 0;
	else if (val < 0xe0) return 2;
	else if (val < 0xf0) return 3;
	else if (val < 0xf8) return 4;
	else if (val < 0xfc) return 5;
	else if (val < 0xfe) return 6;

	return 0;
}


constexpr unsigned char utf8_leader_bits(char c, int count)
{
	return (c & ((1ul << (7 - count) ) - 1));
}

const unsigned int utf8_continuation_bits_per_byte = 6;

constexpr inline unsigned int utf8_continuation_bits(char c)
{
	return ( (1ul << utf8_continuation_bits_per_byte) - 1)
	       & static_cast<unsigned long>(c);
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
			s.__value.__wch = utf8_leader_bits(c, s.__count);
			s.__count--;
		}
	} else
	{
		// handle error
		if (s.__count <= 0)
			return false;

		s.__value.__wch <<= utf8_continuation_bits_per_byte;
		s.__value.__wch |= utf8_continuation_bits(c);
		s.__count--;
	}

	return true;
}

template<class Elem, unsigned long max_code = 0x10ffff,
         codecvt_mode Mode = (codecvt_mode)0>
class codecvt_utf8 : public std::codecvt<Elem, char, std::mbstate_t>
{
 public:
	static_assert(max_code <= std::numeric_limits<Elem>::max(),
	              "Max code is greater than internal character "
	              "representation type");
	// suck this stuff in from codecvt
	using typename std::codecvt<Elem, char, std::mbstate_t>::state_type;
	using typename std::codecvt<Elem, char, std::mbstate_t>::extern_type;
	using typename std::codecvt<Elem, char, std::mbstate_t>::intern_type;
	using typename std::codecvt<Elem, char, std::mbstate_t>::result;

	// just use base ctor
	using std::codecvt<Elem, char, std::mbstate_t>::codecvt;

	virtual ~codecvt_utf8() { }

	static constexpr int utf8_bytes_needed (intern_type c)
	{
		return ( (c <       0x80) ? 1 :
		       ( (c <      0x800) ? 2 :
		       ( (c <    0x10000) ? 3 :
		       ( (c <   0x200000) ? 4 :
		       ( (c <  0x4000000) ? 5 :
		       ( (c < 0x80000000) ? 6 : -1 ) ) ) ) ) );
	}

 protected:
	constexpr int do_max_length() const noexcept override
	{ return utf8_bytes_needed(max_code); }
	
	bool do_always_noconv() const noexcept override
	{ return false; }

	int do_encoding() const noexcept override
	{ return 0; }

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

	//////////////////////////////////////////////////////////////////
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

	//////////////////////////////////////////////////////////////////
	result do_out(state_type & state,
	              const intern_type * from,
	              const intern_type * from_end,
	              const intern_type * & from_next,
	              extern_type * to,
	              extern_type * to_limit,
	              extern_type * & to_next) const
	{
		assert(from <= from_end);
		assert(to <= to_limit);

		from_next = from;
		to_next = to;

		while ((to_next < to_limit) && (from_next < from_end))
		{
			if (state.__count != 0)
			{
				intern_type outValue
				    = (intern_type(0x3f) << ((state.__count - 1) * utf8_continuation_bits_per_byte));
				outValue &= state.__value.__wch;
				outValue >>= ((state.__count -1) * utf8_continuation_bits_per_byte);
				*to_next = static_cast<extern_type>(outValue & extern_type(~0));
				*to_next |= 0x80;
				++to_next;
				--state.__count;
			} else
			{
				unsigned char val = 0;
				state.__value.__wch = *from_next;

				if (state.__value.__wch < 0x80)
				{
					val = state.__value.__wch;

					state.__value.__wch = 0;
					state.__count = 0;
				} else if (state.__value.__wch < 0x800)
				{
					val = (0xc0 | (state.__value.__wch >> 6));
					state.__count = 1;
				} else if (state.__value.__wch < 0x10000)
				{
					val = (0xe0 | (state.__value.__wch >> 12));
					state.__count = 2;
				} else if (state.__value.__wch < 0x200000)
				{
					val = (0xf0 | (state.__value.__wch >> 18));
					state.__count = 3;
				} else if (state.__value.__wch < 0x4000000)
				{
					val = (0xf8 | (state.__value.__wch >> 24));
					state.__count = 4;
				} else if (state.__value.__wch < 0x80000000)
				{
					val = (0xfc | (state.__value.__wch >> 30));
					state.__count = 5;
				} else
					return std::codecvt_base::error;

				*to_next = val;
				++to_next;
				++from_next;
			}
		}

		return std::codecvt_base::noconv;
	}

	//////////////////////////////////////////////////////////////////
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
