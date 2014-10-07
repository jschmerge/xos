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
	for (char32_t i = 0; i < 0x100ffff; ++i)
	{
		auto res = cvt.out(state, &i, (&i) + 1, doneptr, outbuf, end, ptr2);
		if ((i % 0x100000) == 0 || ((i < 0x10000) && ((i % 0x100) == 0)))
			printf("processing mega-plane 0x%08x, current len = %ld\n",
			       i, ptr2 - outbuf);

		assert(res == std::codecvt_base::ok);

		int length = ptr2 - outbuf;

		state = std::mbstate_t();
		int l2 = cvt.length(state, outbuf, ptr2, 20);

		assert(length == l2);


		char32_t redecoded = 0, * lastptr = nullptr;
		const char * ptr1 = nullptr;
		state = std::mbstate_t();
		res = cvt.in(state, outbuf, outbuf + length, ptr1,
		             &redecoded, (&redecoded) + 1, lastptr);

		assert(res == std::codecvt_base::ok);
		assert(i == redecoded);
	}
}
