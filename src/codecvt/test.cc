#include "codecvt.h"
#include <cstring>
#include <cassert>

class u16cvt : public std::codecvt<char16_t, char, std::mbstate_t>
{
 public:
	using codecvt<char16_t, char, std::mbstate_t>::codecvt;
	virtual ~u16cvt() { };
};

class u32cvt : public std::codecvt<char32_t, char, std::mbstate_t>
{
 public:
	using codecvt<char32_t, char, std::mbstate_t>::codecvt;
	virtual ~u32cvt() { };
};

const char * code2str(std::codecvt_base::result r)
{
	switch (r)
	{
	 case std::codecvt_base::ok:
		return "ok";
	 case std::codecvt_base::error:
		return "error";
	 case std::codecvt_base::partial:
		return "partial";
	 case std::codecvt_base::noconv:
		return "noconv";
	}
}

void checku16()
{
	u16cvt cvt;
	std::u32string s32 = U"a\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068"
	                      "\u6559\u6388\U0010FF00  \U00011111  \U0010FFFF";
	std::u16string s16 = u"a\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068"
	                      "\u6559\u6388\U0010FF00  \U00011111  \U0010FFFF";
	std::string s8 =    u8"a\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068"
	                      "\u6559\u6388\U0010FF00  \U00011111  \U0010FFFF";
	char buffer[64];
	const char16_t * from_end = nullptr;
	char * to_end = nullptr;

	memset(buffer, 0, 64);

	for (auto c : s32) { printf("%08x ", c); } putchar('\n');
	printf("s32 size = %zu\n", s32.length());
	for (auto c : s16) { printf("%04x ", c); } putchar('\n');
	printf("s16 size = %zu\n", s16.length());
	for (auto c : s8) { printf("%02hhx ", c); } putchar('\n');
	printf("s8 size = %zu\n", s8.length());


	std::mbstate_t s = std::mbstate_t();
	auto r = cvt.out(s, s16.data(), s16.data() + s16.size(), from_end,
	                 buffer, buffer + 64, to_end);

	printf("Conversion yielded '%s'\n", code2str(r));

	for (char * p = buffer; p != to_end; ++p)
	{
		printf("%02hhx ", *p);
	}
	putchar('\n');

	printf("literal(right)  = %s\n", s8.c_str());
	printf("literal(decode) = %s\n", buffer);

	s = std::mbstate_t();
//	assert(cvt.length(s, s8.data(), s8.data() + s8.size(), 40) == (int)s16.length());
	for (int i = 0; i < 21; ++i)
		printf("%d: %d\n", i, cvt.length(s, s8.data(),
		       s8.data() + s8.size(), i) );

}

int main()
{
	u32cvt cvt;
	const char32_t * doneptr = nullptr;;

	char outbuf[64];

	char * end = outbuf + 64, * ptr2 = nullptr;

	auto state = std::mbstate_t();
	for (char32_t i = 0; i < 0x10ffff; ++i)
	{
		auto res = cvt.out(state, &i, (&i) + 1, doneptr, outbuf, end, ptr2);
		if ((i % 0x10000) == 0 || ((i < 0x10000) && ((i % 0x100) == 0)))
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

	checku16();
}
