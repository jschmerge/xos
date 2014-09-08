#include "utfconv.h"
#include <cstdio>
#include <cstring>
#include <iostream>

int main()
{
	std::basic_string<char> complexString
	  = u8"\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068\u6559\u6388";

	wchar_t buffer[30];

	fputs("====> ", stdout);
	for (const auto & c : complexString)
		printf("%02hhx ", c);
	putchar('\n');

	codecvt_utf8<wchar_t> cnv;

	printf("-> %d\n", cnv.max_length());

	codecvt_utf8<wchar_t, 0x7fffffff> cnv2;
	printf("-> %d\n", cnv2.max_length());

	std::mbstate_t mbs = std::mbstate_t();

	printf("%zd %d ", complexString.length(),
	       cnv.length(mbs, complexString.data(),
	                  complexString.data() + 3, 20));

	printf("%d\n", cnv.length(mbs, complexString.data() + 3,
	                  complexString.data() + complexString.length(), 20));

	const char * next_in = nullptr;
	wchar_t * next_out = nullptr;
	memset(&mbs, 0, sizeof(mbs));
	cnv.in(mbs, complexString.data(),
	       complexString.data() + complexString.length() + 1,
	       next_in, buffer, buffer + 30, next_out);

	for (int i = 0; i < next_out - buffer; ++i)
		printf("%08x ", buffer[i]);

	//codecvt_utf8<char16_t, 0xffff> xcnv;
	return 0;
}
