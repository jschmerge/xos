#include <cwchar>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>
#include <chrono>
#include <clocale>

#include "../utility/bithacks.h"
#include "../time/timeutil.h"

size_t utf8length(char c)
{
	// what the heck is this?
	//
	// The number of bytes taken by a UTF-8 encoded character is equal to the
	// number of 1 bits a the front of the first value What we're doing here
	// is taking that byte, putting it in the most significant byte of a u32
	// then flipping that value, so the leading ones become zero, then
	// forcing all bits to 1 after the leading 0's, reversing it again
	// then doing a popcnt on it
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
	if (retval == 0) retval = 1;

	return retval;
}

size_t utf8length2(char c)
{
	// what the heck is this?
	uint32_t shiftval = ~0;
	uint32_t value = static_cast<uint32_t>(c);
	value |= (value >> 1);
	value |= (value >> 2);
	value |= (value >> 4);
	shiftval &= value;

	size_t retval = __builtin_popcount(~shiftval);

	// "10xxxxxx"b is a continuation byte
	if (retval == 1) throw std::runtime_error("bad encoding");

	// for the "0xxxxxxx"b case of plain ascii
	if (retval == 0) retval = 1;
	//retval += (retval == 0);

	return retval;
}

size_t utf8strlen(const std::basic_string<char> & str)
{
	size_t sz = 0;
	size_t char_length = 0;
	for (std::basic_string<char>::const_iterator i = str.begin();
	     i != str.end(); i += char_length)
	{
		char_length = utf8length(*i);
		++sz;
	}
	return sz;
}

size_t utf8strlen2(const std::basic_string<char> & str)
{
	size_t sz = 0;
	size_t char_length = 0;
	for (std::basic_string<char>::const_iterator i = str.begin();
	     i != str.end(); i += char_length)
	{
		char_length = utf8length(*i);
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
	std::basic_string<wchar_t> simpleWString = L"foo bar";
	std::basic_string<char16_t> simpleU16String = u"foo bar";
	std::basic_string<char32_t> simpleU32String = U"foo bar";

	printf("char length = %zd\n", utf8strlen(simpleString));
	stringInfo(simpleString);
//	stringInfo(simpleWString);
//	stringInfo(simpleU16String);
//	stringInfo(simpleU32String);


	std::basic_string<char> complexString
	  = u8"\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068\u6559\u6388";
	std::basic_string<wchar_t> complexWString
	  = L"\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068\u6559\u6388";
	std::basic_string<char16_t> complexU16String
	  = u"\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068\u6559\u6388";
	std::basic_string<char32_t> complexU32String
	  = U"\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068\u6559\u6388";

	printf("char length = %zd\n", utf8strlen(complexString));
	printf("char length2 = %zd\n", utf8strlen2(complexString));
	stringInfo(complexString);
//	stringInfo(complexWString);
//	stringInfo(complexU16String);

	for (int q = 0; q < 10; ++q)
	{
	const unsigned reps = 100000000;
	size_t x = 0;
	auto begin = PosixClock<CLOCK_REALTIME>::now();
	for (unsigned i = 0; i < reps; ++i)
	{
		x += utf8strlen(simpleString);
		x += utf8strlen(complexString);
	}
	auto end = PosixClock<CLOCK_REALTIME>::now();

	auto d = (end - begin);

	printf("%zd\n", x / reps);
	printf("time utfstrlen %'ld\n", d.count());
///////////////
	x = 0;
	begin = PosixClock<CLOCK_REALTIME>::now();
	for (unsigned i = 0; i < reps; ++i)
	{
		x += utf8strlen2(simpleString);
		x += utf8strlen2(complexString);
	}
	end = PosixClock<CLOCK_REALTIME>::now();

	d = (end - begin);

	printf("%zd\n", x / reps);
	printf("time utfstrlen2 %'ld\n", d.count());
	}
	
	return 0;
}
