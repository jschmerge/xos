#include <cwchar>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>
#include <chrono>
#include <clocale>

#include "../utility/bithacks.h"
#include "../time/timeutil.h"

struct utf8length {
	size_t operator () (char c) const
	{
		// what the heck is this?
		//
		// The number of bytes taken by a UTF-8 encoded character is equal
		// to the number of 1 bits a the front of the first value What
		// we're doing here is taking that byte, putting it in the most
		// significant byte of a u32 then flipping that value, so the leading
		// ones become zero, then forcing all bits to 1 after the leading 0's,
		// reversing it again then doing a popcnt on it
		uint32_t shiftval = static_cast<unsigned char>(c);
		const uint32_t mask = 0x00FFFFFF;

		shiftval <<= 24;
		shiftval = ~shiftval;
		shiftval |= (shiftval >> 1);
		shiftval |= (shiftval >> 2);
		shiftval |= (shiftval >> 4);
		shiftval |= mask;

		//printf("%02hhx %08x %08x\n", c, shiftval, mask);

		size_t retval = __builtin_popcount(~shiftval);

		// "10xxxxxx"b is a continuation byte
		if (retval == 1) throw std::runtime_error("bad encoding");

		// for the "0xxxxxxx"b case of plain ascii
		//if (retval == 0) retval = 1;
		retval += (retval == 0);

	//	printf("---> %zd\n", retval);

		return retval;
	}
};

struct utf8length2 {
	size_t operator () (char c) const
	{
		signed char shiftval = 0x80;
		unsigned char val = static_cast<unsigned char>(c);


#if 0
		if (val < static_cast<unsigned char>(shiftval))           // 0xxxxxxx
			return 1;
		else if (val < static_cast<unsigned char>(shiftval >> 1)) // 10xxxxxx
			throw std::runtime_error("bad encoding");
		else if (val < static_cast<unsigned char>(shiftval >> 2)) // 110xxxxx
			return 2;
		else if (val < static_cast<unsigned char>(shiftval >> 3)) // 1110xxxx
			return 3;
		else if (val < static_cast<unsigned char>(shiftval >> 4)) // 11110xxx
			return 4;
		else if (val < static_cast<unsigned char>(shiftval >> 5)) // 111110xx
			return 5;
		else if (val < static_cast<unsigned char>(shiftval >> 6)) // 1111110x
			return 6;
		throw std::runtime_error("bad encoding");
#endif
		int i = 0;
		for (i = 0; i < 7 && val >= static_cast<unsigned char>(shiftval >> i);
		     ++i)
		{ }

		if (i == 1 || i > 6) {
			throw std::runtime_error("bad encoding");
		}

		//if (i == 0) i = 1;
		i += (i == 0);

		return i;
	}
};

struct utf8length3 {
	inline size_t operator () (char c) const
	{
		signed char shiftval = 0x80;
		unsigned char val = static_cast<unsigned char>(c);
		if (val < static_cast<unsigned char>(shiftval))
			return 1;
		else if (val < static_cast<unsigned char>(shiftval >> 1)) // 10xxxxxx
			{ }
		else if (val < static_cast<unsigned char>(shiftval >> 2))
			return 2;
		else if (val < static_cast<unsigned char>(shiftval >> 3))
			return 3;
		else if (val < static_cast<unsigned char>(shiftval >> 4))
			return 4;
		else if (val < static_cast<unsigned char>(shiftval >> 5))
			return 5;
		else if (val < static_cast<unsigned char>(shiftval >> 6))
			return 6;

		throw std::runtime_error("bad encoding");
	}
};

template <typename LENFUNC>
size_t utf8strlen(const std::basic_string<char> & str)
{
	LENFUNC length;
	size_t sz = 0;
	size_t char_length = 0;
	for (std::basic_string<char>::const_iterator i = str.begin();
	     i != str.end(); i += char_length)
	{
		char_length = length(*i);
		++sz;
	}
	return sz;
}

template <class C>
void stringInfo(const std::basic_string<C> & str)
{
	const char * p = reinterpret_cast<const char *>(str.c_str());

	printf("----------\nFunction = %s\ndata = %s\n", __PRETTY_FUNCTION__, p);
	printf("data.length() = %zd\n", str.length());
	printf("sizeof(C) * length = %zd\n", str.length() * sizeof(C));
	printf("strlen() of data = %zd\n", strlen(p));
	printf("wcslen() of data = %zd\n", wcslen(reinterpret_cast<const wchar_t*>(p)));

	for (size_t i = 0; i < ((str.length() + 1) * sizeof(C)); ++i)
		printf("%02hhx ", p[i]);

	putchar('\n');
}

int main()
{
	setlocale (LC_ALL, "en_GB.UTF-8");
	std::basic_string<char> simpleString = u8"foo bar";

	stringInfo(simpleString);

	std::basic_string<char> complexString
	  = u8"\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068\u6559\u6388";

	stringInfo(complexString);

	for (int q = 0; q < 3; ++q)
	{
		const unsigned reps = 100000000;
		size_t x = 0;
		auto begin = PosixClock<CLOCK_REALTIME>::now();
#if 0
		for (unsigned i = 0; i < reps; ++i)
		{
			x += utf8strlen<utf8length>(simpleString);
			x += utf8strlen<utf8length>(complexString);
		}
#endif
		auto end = PosixClock<CLOCK_REALTIME>::now();

		auto d = (end - begin);

		printf("%zd\n", x / reps);
		printf("time utfstrlen %'ld\n", d.count());
///////////////
		x = 0;
		begin = PosixClock<CLOCK_REALTIME>::now();
		for (unsigned i = 0; i < reps; ++i)
		{
			x += utf8strlen<utf8length2>(simpleString);
			x += utf8strlen<utf8length2>(complexString);
		}
		end = PosixClock<CLOCK_REALTIME>::now();

		d = (end - begin);

		printf("%zd\n", x / reps);
		printf("time utfstrlen2 %'ld\n", d.count());
///////////////
		x = 0;
		begin = PosixClock<CLOCK_REALTIME>::now();
		for (unsigned i = 0; i < reps; ++i)
		{
			x += utf8strlen<utf8length3>(simpleString);
			x += utf8strlen<utf8length3>(complexString);
		}
		end = PosixClock<CLOCK_REALTIME>::now();

		d = (end - begin);

		printf("%zd\n", x / reps);
		printf("time utfstrlen3 %'ld\n", d.count());
	}
	
	return 0;
}
