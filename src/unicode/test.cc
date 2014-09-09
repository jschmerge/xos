#include "utfconv.h"
#include <cstdio>
#include <cstring>
#include <iostream>

int main()
{
	std::basic_string<char> complexString
	  = "\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068\u6559\u6388";

	wchar_t buffer[30];

	for (const auto & c : complexString)
		printf("%02hhx ", c);
	putchar('\n');

	codecvt_utf8<wchar_t> cnv;

/*
	printf("-> %d\n", cnv.max_length());
*/

	codecvt_utf8<wchar_t, 0x7fffffff> cnv2;
/*
	printf("-> %d\n", cnv2.max_length());
*/

/*
	printf("%zd %d ", complexString.length(),
	       cnv.length(mbs, complexString.data(),
	                  complexString.data() + 3, 20));

	printf("%d\n", cnv.length(mbs, complexString.data() + 3,
	                  complexString.data() + complexString.length(), 20));
*/

	std::mbstate_t mbs = std::mbstate_t();
	const char * next_in = nullptr;
	wchar_t * next_out = nullptr;
	memset(&mbs, 0, sizeof(mbs));
	cnv.in(mbs, complexString.data(),
	       complexString.data() + complexString.length() + 1,
	       next_in, buffer, buffer + 30, next_out);

	for (int i = 0; i < next_out - buffer; ++i)
		printf("%x ", buffer[i]);
	putchar('\n');

	memset(&mbs, 0, sizeof(mbs));
	char buf2[40];
	const wchar_t * last = nullptr;
	char * last_out = nullptr;
	cnv.out(mbs, buffer, next_out, last, buf2, buf2 + 40, last_out);

	for (char * p = buf2; p < last_out; ++p)
		printf("%02hhx ", *p);
	putchar('\n');

//	std::cout << complexString << std::endl;
	  //L"\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068\u6559\u6388\n";
	return 0;
}
