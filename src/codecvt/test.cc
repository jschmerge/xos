#include "codecvt.h"

class u32cvt : public std::codecvt<char32_t, char, std::mbstate_t>
{
 public:
	using codecvt<char32_t, char, std::mbstate_t>::codecvt;
	virtual ~u32cvt() { };
};


int main()
{
	u32cvt cvt;
	const char32_t * doneptr = nullptr;;

	char outbuf[64];

	char * end = outbuf + 64, * ptr2 = nullptr;

	auto state = std::mbstate_t();
	//for (char32_t i = 0; i < 0x10ffff; ++i)
	for (char32_t i = 0; i < 0x7fffffff; ++i)
	{
		auto res = cvt.out(state, &i, (&i) + 1, doneptr, outbuf, end, ptr2);
		if ((i % 0x100000) == 0 || ((i < 0x10000) && ((i % 0x100) == 0)))
			printf("processing mega-plane 0x%08x, current len = %ld\n",
			       i, ptr2 - outbuf);

		assert(res == std::codecvt_base::ok);

		size_t length = ptr2 - outbuf;

		state = std::mbstate_t();
		auto l2 = cvt.length(state, outbuf, ptr2, 1);

		printf("Length %d %zu\n", l2, length);
	}
}
