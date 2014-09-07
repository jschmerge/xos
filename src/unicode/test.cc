#include "utfconv.h"
#include <cstdio>

int main()
{
	codecvt_utf8<wchar_t> cnv;
	printf("-> %d\n", cnv.max_length());

	codecvt_utf8<wchar_t, 0x7fffffff> cnv2;
	printf("-> %d\n", cnv2.max_length());
	return 0;
}
