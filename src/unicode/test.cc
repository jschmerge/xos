#include "utfconv.h"
#include <cstdio>

int main()
{
	codecvt_utf8<wchar_t> cnv;
	printf("-> %d\n", cnv.max_length());

	codecvt_utf8<wchar_t, 0x7fffffff> cnv2;
	printf("-> %d\n", cnv2.max_length());

	std::mbstate_t state = std::mbstate_t();

	printf("%zd { %zd %zd { %zd %zd } }\n", sizeof(state), sizeof(state.__count),
	       sizeof(state.__value), sizeof(state.__value.__wch),
	       sizeof(state.__value.__wchb));
	return 0;
}
