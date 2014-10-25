#ifndef GUARD_UTF
#define GUARD_UTF 1

#include <cstdint>
#include <cwchar>
#include <cstring>
#include <iterator>

constexpr char32_t bom_value()
{
	return 0xfeffu;
}

namespace utf8_conversion {

constexpr uint32_t length_limits[6] = {
	0x00000080ul,
	0x00000800ul,
	0x00010000ul,
	0x00200000ul,
	0x04000000ul,
	0x80000000ul
};

constexpr uint8_t leader_prefix[6] = {
	0x00u,
	0xc0u,
	0xe0u,
	0xf0u,
	0xf8u,
	0xfcu,
};

constexpr uint8_t one_byte_limit   = 0x80u;
constexpr uint8_t invalid_limit    = 0xc0u;
constexpr uint8_t two_byte_limit   = 0xe0u;
constexpr uint8_t three_byte_limit = 0xf0u;
constexpr uint8_t four_byte_limit  = 0xf8u;
constexpr uint8_t five_byte_limit  = 0xfcu;
constexpr uint8_t six_byte_limit   = 0xfeu;

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

inline char extract_leader_byte(mbstate_t & state)
{
	char leader = 0;

	for (int i = 0;
	     i < (std::end(length_limits) - std::begin(length_limits)); ++i)
	{
		// on error, we set __count to -1; - see do_out
		state.__count = -1;

		if (state.__value.__wch < length_limits[i])
		{
			unsigned int shift = (continuation_bits_per_byte * i);
			leader = (leader_prefix[i] | (state.__value.__wch >> shift));
			state.__count = i;

			if (i == 0) state.__value.__wch = 0;

			break;
		}
	}

	return leader;
}

//
// decode
//
constexpr uint32_t max_encodable_value() { return 0x7fffffff; }

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

	if (s.__count < 0)
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
		}
		else
		{
			s.__value.__wch = leader_bits(c, s.__count);
			s.__count--;
		}
	}
	else
	{
		s.__value.__wch <<= continuation_bits_per_byte;
		s.__value.__wch |= continuation_bits(c);
		s.__count--;
	}

	return rc;
}

} // namespace utf8_conversion


namespace utf16_conversion {

constexpr char32_t surrogate_data_bits = 10;
constexpr char32_t ten_bit_mask = ( (1 << surrogate_data_bits) - 1);
constexpr char32_t surrogate_data_bitmask = ( (1 << surrogate_data_bits) - 1);
constexpr char32_t surrogate_transform_value = 0x10000u;

constexpr char16_t surrogate_min = 0xd800u;
constexpr char16_t surrogate_max = 0xdfffu;
constexpr char16_t low_surrogate_min = 0xdc00u;

constexpr uint32_t max_encodable_value() { return 0x10ffffu; }

inline constexpr bool is_surrogate(uint32_t c)
{
	return ((c >= surrogate_min) && (c <= surrogate_max));
}

inline bool update_mbstate(std::mbstate_t & s, char16_t c)
{
	bool rc = true;
	if (s.__count == 0)
	{
		if (c < surrogate_min || c > surrogate_max)
		{
			s.__value.__wch = c;
			s.__count = 0; // superfluous
		}
		else if (c < low_surrogate_min)
		{
			s.__value.__wch = (c & surrogate_data_bitmask);
			s.__value.__wch <<= surrogate_data_bits;
			s.__count = -1;
		}
		else
		{
			rc = false;
		}
	}
	else if (s.__count == -1)
	{
		if (c >= low_surrogate_min && c <= surrogate_max)
		{
			s.__value.__wch |= (c & ten_bit_mask);
			s.__value.__wch += surrogate_transform_value;
			s.__count = 0;
		}
		else
		{
			rc = false;
		}
	}

	return rc;
}

inline bool update_mbstate(std::mbstate_t & s, char c, bool le)
{
	bool rc = true;

	if (le)
	{
		switch (s.__count)
		{
		 case 0:
			s.__value.__wch = 0;
			s.__value.__wchb[1] = c;
			++s.__count;
			break;

		 case 1:
			s.__value.__wchb[0] = c;
			if (  (static_cast<uint8_t>(c) < 0xd8u)
			   || (static_cast<uint8_t>(c) > 0xdfu) )
			{
				std::wint_t tmp = static_cast<uint8_t>(s.__value.__wchb[1]);
				tmp |= (static_cast<uint8_t>(s.__value.__wchb[0]) << 8);
				s.__value.__wch = tmp;
				s.__count = 0;
			}
			else
			{
				++s.__count;
			}
			break;

		 case 2:
			s.__value.__wchb[3] = c;
			++s.__count;
			break;

		 case 3:
			if (  (static_cast<uint8_t>(c) >= 0xdcu)
			   && (static_cast<uint8_t>(c) <= 0xdfu) )
			{
				s.__value.__wchb[2] = c;

				std::wint_t tmp = static_cast<uint8_t>(s.__value.__wchb[3]);
				tmp |= (static_cast<uint8_t>(s.__value.__wchb[2] & 0x03) << 8);
				tmp |= (static_cast<uint8_t>(s.__value.__wchb[1] ) << 10);
				tmp |= (static_cast<uint8_t>(s.__value.__wchb[0] & 0x03) << 18);
				tmp += 0x10000u;
				s.__value.__wch = tmp;

				s.__count = 0;
			}
			else
			{
				rc = false;
			}
			break;
		}
	}
	else
	{
		switch (s.__count)
		{
		 case 0:
			s.__value.__wch = 0;
			s.__value.__wchb[0] = c;
			++s.__count;
			break;

		 case 1:
			s.__value.__wchb[1] = c;
			if (  (static_cast<uint8_t>(s.__value.__wchb[0]) < 0xd8u)
			   || (static_cast<uint8_t>(s.__value.__wchb[0]) > 0xdfu) )
			{
				std::wint_t tmp = static_cast<uint8_t>(s.__value.__wchb[1]);
				tmp |= (static_cast<uint8_t>(s.__value.__wchb[0]) << 8);
				s.__value.__wch = tmp;
				s.__count = 0;
			}
			else
			{
				++s.__count;
			}
			break;

		 case 2:
			if (  (static_cast<uint8_t>(c) >= 0xdcu)
			   && (static_cast<uint8_t>(c) <= 0xdfu) )
			{
				s.__value.__wchb[2] = c;
				++s.__count;
			}
			else
			{
				rc = false;
			}
			break;

		 case 3:
			s.__value.__wchb[3] = c;
			s.__count = 0;
			break;
		}
	}

	return rc;
}

constexpr char16_t high_surrogate_value(char32_t c)
{
	return (surrogate_min
		   | ( ( (c - surrogate_transform_value) >> surrogate_data_bits)
	           & surrogate_data_bitmask));
}

constexpr char16_t low_surrogate_value(char32_t c)
{
	return (low_surrogate_min
		   | ( (c - surrogate_transform_value) & surrogate_data_bitmask));
}

inline char16_t extract_leader_value(std::mbstate_t & s)
{
	char16_t value = 0;

	if (s.__value.__wch < surrogate_transform_value)
	{
		value = s.__value.__wch;
	}
	else
	{
		value = high_surrogate_value(s.__value.__wch);
		s.__value.__wch = low_surrogate_value(s.__value.__wch);
		s.__count = -1;
	}

	return value;
}

inline bool set_mbstate(std::mbstate_t & s, char32_t c, bool little_endian)
{
	bool rc = true;
	char16_t tmp = L'\0';

	memset(&s, 0, sizeof(s));

	if (is_surrogate(c) || (c > max_encodable_value()))
	{
		rc = false;
	}
	else if (c > surrogate_transform_value)
	{
		tmp = low_surrogate_value(c);
		if (little_endian)
		{
			s.__value.__wchb[0] = ((tmp >> 8) & 0xffu);
			s.__value.__wchb[1] = (tmp & 0xffu);

			tmp = high_surrogate_value(c);
			s.__value.__wchb[2] = ((tmp >> 8) & 0xffu);
			s.__value.__wchb[3] = (tmp & 0xffu);
		}
		else
		{
			s.__value.__wchb[0] = (tmp & 0xffu);
			s.__value.__wchb[1] = ((tmp >> 8) & 0xffu);

			tmp = high_surrogate_value(c);
			s.__value.__wchb[2] = (tmp & 0xffu);
			s.__value.__wchb[3] = ((tmp >> 8) & 0xffu);
		}
		
		s.__count = 4;
	}
	else
	{
		tmp = c;

		if (little_endian)
		{
			s.__value.__wchb[0] = ((tmp >> 8) & 0xffu);
			s.__value.__wchb[1] = (tmp & 0xffu);
		}
		else
		{
			s.__value.__wchb[0] = (tmp & 0xffu);
			s.__value.__wchb[1] = ((tmp >> 8) & 0xffu);
		}

		s.__count = 2;
	}

	return rc;
}

} // namespace utf16_conversion

#endif // GUARD_UTF
