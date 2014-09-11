#include "utfconv.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <iostream>

const char * string_utf8
  = u8"a\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068\u6559\u6388"
      "\U0010FF00  \U00011111 \U0010FFFF";

const char16_t * string_utf16
  = u"a\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068\u6559\u6388"
     "\U0010FF00  \U00011111 \U0010FFFF";

const char32_t * string_utf32
  = U"a\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068\u6559\u6388"
     "\U0010FF00  \U00011111 \U0010FFFF";

typedef std::codecvt<char32_t, char, std::mbstate_t>::result result;

template <class CHAR_T = char32_t, unsigned int limit = 0x10FFFF>
void testStuff()
{
	static_assert(std::numeric_limits<CHAR_T>::max() >= limit,
	              "Upper limit for char values must be less "
	              "than charater type max");
	CHAR_T wc[4];
	char nc[8];

	const CHAR_T * last_in = nullptr;
	char * last_out = nullptr;
	codecvt_utf8<CHAR_T, limit> cnv;
	
	for (wc[0] = 0;
	     static_cast<unsigned int>(wc[0]) < (limit + 1); ++wc[0])
	{
		if ((wc[0] & 0xFFFFF) == 0)
			printf("Processing plane 0x%08x\n", wc[0]);

		std::mbstate_t mbs = std::mbstate_t();
		memset(nc, 0, 8);
		result r = cnv.out(mbs, wc, wc + 1, last_in, nc, nc + 8, last_out);

		switch (r)
		{
		 case std::codecvt_base::ok: break;
		 case std::codecvt_base::partial:
			printf("Partial conversion of charater 0x%08x; %ld chars written\n",
			       wc[0], last_out - nc); break;
		 case std::codecvt_base::error:
			printf("Conversion error of charater 0x%08x\n", wc[0]); break;
		 case std::codecvt_base::noconv:
			printf("No conversion (error) of charater 0x%08x\n", wc[0]); break;
		}

		assert(r == std::codecvt_base::ok);
		assert(mbs.__count == 0);

		const char * last_in2 = nullptr;
		CHAR_T * last_out2 = nullptr;
		memset(&mbs, 0, sizeof(mbs));

		r = cnv.in(mbs, nc, last_out, last_in2, wc + 1, wc + 2, last_out2);
		switch (r)
		{
		 case std::codecvt_base::ok: break;

		 case std::codecvt_base::partial:
			printf("Partial reverse conversion of charater 0x%08x -> 0x%08x; "
			       "%ld chars written\n", wc[0], wc[1],
			        last_out2 - wc + 1); break;
		 case std::codecvt_base::error:
			printf("Conversion error of charater 0x%08x\n", wc[0]); break;

		 case std::codecvt_base::noconv:
			printf("No conversion (error) of charater 0x%08x\n", wc[0]); break;
		}

		assert((r == std::codecvt_base::ok));

		assert(wc[0] == wc[1]);
		assert(mbs.__count == 0);

		if (wc[0] == std::numeric_limits<CHAR_T>::max())
			break;
	}
}

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
		printf("%x ", buffer[i]);
	}
	putchar('\n');
	for (int i = 0; i < (next_out - buffer - 1); ++i)
	{
		assert(static_cast<char32_t>(buffer[i]) == string_utf32[i]);
	}

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

	testStuff<wchar_t>();
	testStuff<char32_t, 0x0fffffff>();
	testStuff<char16_t, 0xffff>();
	return 0;
}
