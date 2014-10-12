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
	bool rc = false;
	
	if (  (static_cast<unsigned char>(c) < 0xfe)
	   && ( (static_cast<unsigned char>(c) & 0xc0ul) != 0x80ul) )
		rc = true;

	return rc;
}

inline constexpr size_t utf8_codepoint_length (char c)
{
	return ( (static_cast<unsigned char>(c) < 0x80ul) ? 1 :
	       ( (static_cast<unsigned char>(c) < 0xc0ul) ? 0 :
	       ( (static_cast<unsigned char>(c) < 0xe0ul) ? 2 :
	       ( (static_cast<unsigned char>(c) < 0xf0ul) ? 3 :
	       ( (static_cast<unsigned char>(c) < 0xf8ul) ? 4 :
	       ( (static_cast<unsigned char>(c) < 0xfcul) ? 5 :
	       ( (static_cast<unsigned char>(c) < 0xfeul) ? 6 :
	          0 ) ) ) ) ) ) );
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

constexpr inline unsigned int data_shift(int byte_number)
{
	return ((byte_number - 1) * utf8_continuation_bits_per_byte);
}

constexpr inline unsigned int data_mask(int byte_number)
{
	return (0x3ful << data_shift(byte_number));
}

constexpr inline char next_utf8_byte(const std::mbstate_t & s)
{
	return ( 0x80
	       | (data_mask(s.__count) & s.__value.__wch) >> data_shift(s.__count));
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
#if 0
	static_assert(max_code <=
	              std::numeric_limits<typename std::make_unsigned<Elem>::type>::max(),
	              "Max code is greater than internal character "
	              "representation type");
#endif
	// suck this stuff in from codecvt
	using typename std::codecvt<Elem, char, std::mbstate_t>::state_type;
	using typename std::codecvt<Elem, char, std::mbstate_t>::extern_type;
	using typename std::codecvt<Elem, char, std::mbstate_t>::intern_type;
	using typename std::codecvt<Elem, char, std::mbstate_t>::result;

	// just use base ctor
	using std::codecvt<Elem, char, std::mbstate_t>::codecvt;

	virtual ~codecvt_utf8() { }

	static constexpr unsigned int utf_length_limits[6] = {
		0x80, 0x800, 0x10000, 0x200000, 0x4000000, 0x80000000
	};

	static constexpr int utf8_bytes_needed (unsigned int c)
	{
		return ( (static_cast<unsigned int>(c) < utf_length_limits[0]) ? 1 :
		       ( (static_cast<unsigned int>(c) < utf_length_limits[1]) ? 2 :
		       ( (static_cast<unsigned int>(c) < utf_length_limits[2]) ? 3 :
		       ( (static_cast<unsigned int>(c) < utf_length_limits[3]) ? 4 :
		       ( (static_cast<unsigned int>(c) < utf_length_limits[4]) ? 5 :
		       ( (static_cast<unsigned int>(c) < utf_length_limits[5]) ? 6 :
		          -1 ) ) ) ) ) );
	}

 protected:
	int do_max_length() const noexcept override
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

		return ( ( (state.__count != 0) || (from_next < from_end) ) ?
			     std::codecvt_base::partial :
		         std::codecvt_base::ok );
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
	              extern_type * & to_next) const override
	{
		assert(from <= from_end);
		assert(to <= to_limit);

		from_next = from;
		to_next = to;

		while ((to_next < to_limit) && (from_next < from_end))
		{
			if (state.__count != 0)
			{
				*to_next = next_utf8_byte(state);
				++to_next;
				--state.__count;
			} else
			{
				unsigned char val = 0;
				state.__value.__wch = *from_next;

				if (state.__value.__wch < utf_length_limits[0])
				{
					val = state.__value.__wch;

					state.__value.__wch = 0;
					state.__count = 0;
				} else if (state.__value.__wch < utf_length_limits[1])
				{
					val = (0xc0 | (state.__value.__wch >> 6));
					state.__count = 1;
				} else if (state.__value.__wch < utf_length_limits[2])
				{
					val = (0xe0 | (state.__value.__wch >> 12));
					state.__count = 2;
				} else if (state.__value.__wch < utf_length_limits[3])
				{
					val = (0xf0 | (state.__value.__wch >> 18));
					state.__count = 3;
				} else if (state.__value.__wch < utf_length_limits[4])
				{
					val = (0xf8 | (state.__value.__wch >> 24));
					state.__count = 4;
				} else if (state.__value.__wch < utf_length_limits[5])
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

		while ((to_next < to_limit) && state.__count > 0)
		{
			*to_next = next_utf8_byte(state);
			++to_next;
			--state.__count;
		}

		return ( (  (state.__count == 0)
		         && (from_next == from_end)) ?
		         std::codecvt_base::ok :
		         std::codecvt_base::partial );
	}

	//////////////////////////////////////////////////////////////////
	result do_unshift(state_type & state,
	                  extern_type * to,
	                  extern_type * to_limit,
	                  extern_type * & to_next) const override
	{
		assert(to <= to_limit);

		for (to_next = to; (  (to_next < to_limit)
		                   && (state.__count != 0) ); ++to_next)
		{
			intern_type outValue =
				(intern_type(0x3f) << ((state.__count - 1) * 6));
			outValue &= state.__value.__wch;
			outValue >>= ((state.__count -1) * 6);
			*to_next = static_cast<extern_type>(outValue & extern_type(~0));
			*to_next |= 0x80;
			--state.__count;
		}

		return ( (state.__count == 0) ?
		         std::codecvt_base::ok :
		         std::codecvt_base::partial );
	}
};

template<unsigned long max_code, codecvt_mode Mode>
class codecvt_utf8<char, max_code, Mode>
  : public std::codecvt<char, char, std::mbstate_t>
{
 public:
	// suck this stuff in from codecvt
	using typename std::codecvt<char, char, std::mbstate_t>::state_type;
	using typename std::codecvt<char, char, std::mbstate_t>::extern_type;
	using typename std::codecvt<char, char, std::mbstate_t>::intern_type;
	using typename std::codecvt<char, char, std::mbstate_t>::result;

	// just use base ctor
	using std::codecvt<char, char, std::mbstate_t>::codecvt;

	virtual ~codecvt_utf8() { }

 protected:
	int do_max_length() const noexcept override
	{ return sizeof(char); }
	
	bool do_always_noconv() const noexcept override
	{ return true; }

	int do_encoding() const noexcept override
	{ return 1; }

	result do_in(state_type &, const extern_type * from,
	             const extern_type *, const extern_type * & from_next,
	             intern_type * to, intern_type *,
	             intern_type * & to_next) const override
	{
		from_next = from;
		to_next = to;
		return std::codecvt_base::noconv;
	}

	int do_length(state_type &, const char * from, const char * from_end,
	              std::size_t max) const override
	{ return std::min(static_cast<std::size_t>(from_end - from), max); }

	result do_out(state_type &,
	              const intern_type * from, const intern_type *,
	              const intern_type * & from_next, extern_type * to,
	              extern_type *, extern_type * & to_next) const override
	{
		from_next = from;
		to_next = to;
		return std::codecvt_base::noconv;
	}

	//////////////////////////////////////////////////////////////////
	result do_unshift(state_type &, extern_type * to_start, extern_type *,
	                  extern_type * & to_limit) const override
	{
		to_limit = to_start;
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
