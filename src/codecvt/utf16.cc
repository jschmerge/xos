/*
0 - 0xffff -> 0 - 0xffff

0x10000 - 0x10ffff -> {0xd800 - 0xdbff , 0xdc000 - dffff}
*/

#include <utility>
#include <stdexcept>

constexpr char32_t ten_bit_mask = 0x3ffu;
constexpr char32_t magic_value = 0x10000u;
constexpr char32_t encoding_pattern = 0xd800dc00;

constexpr char16_t surrogate_range_begin = 0xd800u;
constexpr char16_t surrogate_high_end  = 0xdbffu;
constexpr char16_t surrogate_low_begin = 0xdc00u;
constexpr char16_t surrogate_range_end = 0xdfffu;

enum class endian { big, little };

char32_t encode(char32_t c32)
{
	char32_t ret;
	if (c32 < magic_value)
		ret = c32;
	else
	{
		c32 -= magic_value;
		ret = encoding_pattern;
		ret |= ((c32 & (ten_bit_mask << 10) ) << 6); // upper
		ret |= (c32 & ten_bit_mask); // lower
	}

	return ret;
}

template <endian order = endian::big>
char32_t decode(char16_t * s)
{
	char32_t high = 0, low = 0, ret = 0;

	if (order == endian::big)
	{
		if (s[0] < surrogate_range_begin || s[0] > surrogate_range_end)
		{
			ret = s[0];
		} else if (s[0] < surrogate_low_begin)
		{
			high = s[0] & ten_bit_mask;
			high <<= 10;

			if (s[1] < surrogate_low_begin || s[1] > surrogate_range_end)
				throw std::runtime_error("Bad sequence on surogate");
			else
				low = s[1] & ten_bit_mask;
		} else
		{
			throw std::runtime_error("Bad sequence");
		}
	} else
	{
		// little endian
		if (s[0] < surrogate_range_begin || s[0] > surrogate_range_end)
		{
			ret = s[0];
		} else if (s[0] > surrogate_high_end)
		{
			low = s[0] & ten_bit_mask;

			if (s[1] < surrogate_range_begin || s[1] > surrogate_high_end)
				throw std::runtime_error("Bad sequence on surogate");
			else
			{
				high = s[1] & ten_bit_mask;
				high <<= 10;
			}
		} else
		{
			throw std::runtime_error("Bad sequence");
		}
	}

	ret |= low;
	ret |= high;

	if ((low != 0) || (high != 0))
		ret += magic_value;

	return ret;
}
