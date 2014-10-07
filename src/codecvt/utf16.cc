/*
0 - 0xffff -> 0 - 0xffff

0x10000 - 0x10ffff -> {0xd800 - 0xdbff , 0xdc000 - dffff}
*/

/*

This was taken from the clang libc++ codebase and edited for formatting:

                              Valid UTF ranges
    UTF-32           UTF-16                 UTF-8               # of cp's
                  1st      2nd     1st    2nd    3rd    4th
000000-00007F 0000-007F            00-7F                        127
000080-0007FF 0080-07FF            C2-DF, 80-BF                 1920
000800-000FFF 0800-0FFF            E0-E0, A0-BF, 80-BF          2048
001000-00CFFF 1000-CFFF            E1-EC, 80-BF, 80-BF          49152
00D000-00D7FF D000-D7FF            ED-ED, 80-9F, 80-BF          2048
00D800-00DFFF             invalid
00E000-00FFFF E000-FFFF            EE-EF, 80-BF, 80-BF          8192
010000-03FFFF D800-D8BF, DC00-DFFF F0-F0, 90-BF, 80-BF, 80-BF   196608
040000-0FFFFF D8C0-DBBF, DC00-DFFF F1-F3, 80-BF, 80-BF, 80-BF   786432
100000-10FFFF DBC0-DBFF, DC00-DFFF F4-F4, 80-8F, 80-BF, 80-BF   65536
*/

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
