#include "utfconv.h"
#include <cstdio>
#include <cassert>
#include <cstring>
#include <iostream>

const char * string_utf8
  = u8"\U00011111\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068\u6559\u6388";

const char16_t * string_utf16
  = u"\U00011111\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068\u6559\u6388";

const char32_t * string_utf32
  = U"\U00011111\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068\u6559\u6388";


int main()
{
	char32_t buffer[100];

	for (const char32_t * lp = string_utf32; *lp; ++lp)
		printf("%x ", *lp);
	putchar('\n');

	for (const char * p = string_utf8; *p; ++p)
		printf("%02hhx ", *p);
	putchar('\n');

	codecvt_utf8<char32_t> cnv;

	std::mbstate_t mbs = std::mbstate_t();
	const char * next_in = nullptr;
	char32_t * next_out = nullptr;
	memset(&mbs, 0, sizeof(mbs));
	int x = cnv.in(mbs, string_utf8,
	       string_utf8 + strlen(string_utf8) + 1,
	       next_in, buffer, buffer + 100, next_out);
	printf("===> %d\n", x);

	for (int i = 0; i < (next_out - buffer - 1); ++i)
	{
		assert(static_cast<char32_t>(buffer[i]) == string_utf32[i]);
		printf("%x ", buffer[i]);
	}
	putchar('\n');

	memset(&mbs, 0, sizeof(mbs));
	char buf2[100];
	const char32_t * last = nullptr;
	char * last_out = nullptr;
	x = cnv.out(mbs, buffer, next_out, last, buf2, buf2 + 100, last_out);
	printf("===> %d\n", x);

	for (int i = 0; i < (last_out - buf2); ++i)
	{
		assert(buf2[i] == string_utf8[i]);
		printf("%02hhx ", buf2[i]);
	}
	putchar('\n');
	printf("%s\n", buf2);

	return 0;
}
