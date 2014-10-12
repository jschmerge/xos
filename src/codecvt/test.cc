#include "codecvt_specializations.h"
#include "codecvt_utf8.h"

#include <cxxabi.h>

#include <cstring>
#include <cassert>
#include <memory>
#include <limits>
#include <typeinfo>

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
	return nullptr;
}

void checku16()
{
	u16cvt cvt;
	std::u32string s32 = U"a\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068"
	                      "\u6559\u6388\U0001f0df \U0010FFFF";
	std::u16string s16 = u"a\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068"
	                      "\u6559\u6388\U0001f0df \U0010FFFF";
	std::string s8 =    u8"a\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068"
	                      "\u6559\u6388\U0001f0df \U0010FFFF";
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
	assert(cvt.length(s, s8.data(), s8.data() + s8.length(), s16.length())
	          == static_cast<int>(s8.length()));

	char16_t buffer16[40];
	memset(buffer16, 0, sizeof(buffer16));
	const char * cto_end = nullptr;
	char16_t * end16;

	s = std::mbstate_t();
	r = cvt.in(s,s8.data(),s8.data() + s8.size(), cto_end,
	           buffer16, buffer16 + 40, end16);

	printf("-> in returned %s\n", code2str(r));
	assert(r == std::codecvt_base::ok);

	assert(static_cast<size_t>(end16 - buffer16) == s16.length());
	assert(memcmp(buffer16, s16.data(), end16 - buffer16) == 0);
}

const char * mode2str(std::codecvt_mode m)
{
	switch (static_cast<int>(m))
	{
	 case 0: return "none";
	 case 1: return "le";
	 case 2: return "generate";
	 case 3: return "generate|le";
	 case 4: return "consume";
	 case 5: return "consume|le";
	 case 6: return "consume|generate";
	 case 7: return "consume|generate|le";
	}

	return "unknown";
}

std::string demangle(const char * s)
{
	std::string ret;
	int status = 0;
	
	char * val = abi::__cxa_demangle(s, 0, 0, &status);
	if (val != nullptr)
	{
		ret = val;
		free(val);
	}

	return ret;
}

template <typename T, unsigned long MAX, std::codecvt_mode MODE>
void check_codecvt_utf8()
{
	std::codecvt_utf8<T, MAX, MODE> wcvt;
	std::string type_name = demangle(typeid(T).name());

	printf("<%s, %08lx, %s>::max_length() = %d\n",
	       type_name.c_str(), MAX, mode2str(MODE), wcvt.max_length());
}

template <typename T, std::codecvt_mode MODE>
void check_all_enums()
{
	check_codecvt_utf8<T, 0xff, MODE>();
	check_codecvt_utf8<T, 0xffff, MODE>();
	check_codecvt_utf8<T, 0x10fff, MODE>();
	check_codecvt_utf8<T, std::numeric_limits<T>::max(), MODE>();
}

template <typename T>
void check_all()
{
	check_all_enums<T, std::codecvt_mode(0)>();
	check_all_enums<T, std::codecvt_mode(1)>();
	check_all_enums<T, std::codecvt_mode(2)>();
	check_all_enums<T, std::codecvt_mode(3)>();
	check_all_enums<T, std::codecvt_mode(4)>();
	check_all_enums<T, std::codecvt_mode(5)>();
	check_all_enums<T, std::codecvt_mode(6)>();
	check_all_enums<T, std::codecvt_mode(7)>();
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

	check_all<wchar_t>();
	check_all<char16_t>();

}
