#include "utfconv.h"
#include <cstdio>

int main()
{
	std::basic_string<char> complexString
	  = u8"\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068\u6559\u6388";

	codecvt_utf8<wchar_t> cnv;

	printf("-> %d\n", cnv.max_length());

	codecvt_utf8<wchar_t, 0x7fffffff> cnv2;
	printf("-> %d\n", cnv2.max_length());

	std::mbstate_t mbs;

	printf("%zd %d\n", complexString.length(),
	       cnv.length(mbs, complexString.data(),
	                  complexString.data() + complexString.length(), 20));
	return 0;
}
