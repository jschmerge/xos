
#include <cstdint>
#include <cwchar>

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
constexpr uint32_t max_encodable_value()
{ return 0x7fffffff; }

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

