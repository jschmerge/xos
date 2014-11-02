#include "codecvt"

#include <cxxabi.h>

#include <cstring>
#include <cassert>
#include <algorithm>
#include <memory>
#include <limits>
#include <typeinfo>

std::string make_msg(const char * fname, size_t line, const char * msg)
{
	std::string ret(fname);
	ret += ": ";
	ret += std::to_string(line);
	ret += msg;

	return ret;
}

#define test_assert_line(x, file, line) \
	if (! (x)) { \
		throw std::runtime_error(make_msg(file, line,  \
	                                      ": Assertion '" #x "' failed")); }

#define test_assert(x) test_assert_line(x, __FILE__, __LINE__)
	
struct u16cvt : public std::codecvt<char16_t, char, std::mbstate_t> {
	using codecvt<char16_t, char, std::mbstate_t>::codecvt;
	virtual ~u16cvt() { };
};

struct u32cvt : public std::codecvt<char32_t, char, std::mbstate_t> {
	using codecvt<char32_t, char, std::mbstate_t>::codecvt;
	virtual ~u32cvt() { };
};

struct multistring {
	multistring(const char * _ns, const wchar_t * _ws,
	            const char16_t * _s16, const char32_t * _s32)
	: ns(_ns), ws(_ws), s16(_s16), s32(_s32)
	{ }

	template <typename T>
	const std::basic_string<T> & get() const;

	template <typename T>
	const std::basic_string<char> byte_oriented_string(bool little_endian);

	std::string    ns;
	std::wstring   ws;
	std::u16string s16;
	std::u32string s32;
};

template <>
const std::basic_string<char>
multistring::byte_oriented_string<char16_t>(bool little_endian)
{
	std::string ret;
	uint8_t tmp;

	for (auto x : s16)
	{
		if (little_endian)
		{
			tmp = (x & 0xff);
			ret += tmp;
			tmp = ((x >> 8) & 0xff);
			ret += tmp;
		} else
		{
			tmp = ((x >> 8) & 0xff);
			ret += tmp;
			tmp = (x & 0xff);
			ret += tmp;
		}
	}
	return ret;
}

#define FULL_RANGE "z\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068" \
	               "\u6559\u6388\U0001f0df \U0010FFFF"

#define TRIPLE_CAT_(a_, b_, c_) a_ ## b_ ## c_
#define TRIPLE_CAT(a_, b_, c_) TRIPLE_CAT_(a_, b_, c_)
#define NARROW(s_) TRIPLE_CAT(, s_, )
#define WIDE(s_) TRIPLE_CAT(L, s_, )
#define UTF8(s_) TRIPLE_CAT(u8, s_, )
#define UTF16(s_) TRIPLE_CAT(u, s_, )
#define UTF32(s_) TRIPLE_CAT(U, s_, )

#define DEF_MULTISTRING(name, literalval) \
	multistring name(NARROW(literalval), WIDE(literalval), \
	                 UTF16(literalval), UTF32(literalval))

DEF_MULTISTRING(multi, FULL_RANGE);

template<>
const std::basic_string<char> & multistring::get<char>() const
{ return ns; }

template<>
const std::basic_string<wchar_t> & multistring::get<wchar_t>() const
{ return ws; }

template<>
const std::basic_string<char16_t> & multistring::get<char16_t>() const
{ return s16; }

template<>
const std::basic_string<char32_t> & multistring::get<char32_t>() const
{ return s32; }

const char * code2str(std::codecvt_base::result r)
{
	switch (r)
	{
	 case std::codecvt_base::ok:      return "ok";
	 case std::codecvt_base::error:   return "error";
	 case std::codecvt_base::partial: return "partial";
	 case std::codecvt_base::noconv:  return "noconv";
	}
	return nullptr;
}

void checku16()
{
	u16cvt cvt;

	char buffer[64];
	const char16_t * from_end = nullptr;
	char * to_end = nullptr;

	memset(buffer, 0, 64);

	for (auto c : multi.get<char32_t>())
	{ printf("%08x ", c); } putchar('\n');
	printf("s32 size = %zu\n", multi.get<char32_t>().length());

	for (auto c : multi.get<char16_t>())
	{ printf("%04x ", c); } putchar('\n');
	printf("s16 size = %zu\n", multi.get<char16_t>().length());

	for (auto c : multi.get<char>())
	{ printf("%02hhx ", c); } putchar('\n');
	printf("s8 size = %zu\n", multi.get<char>().length());


	std::mbstate_t s = std::mbstate_t();
	auto r = cvt.out(s, multi.get<char16_t>().data(),
	                 multi.get<char16_t>().data()
	                   + multi.get<char16_t>().size(),
	                 from_end,
	                 buffer, buffer + 64, to_end);

	printf("Conversion yielded '%s'\n", code2str(r));

	for (char * p = buffer; p != to_end; ++p)
	{ printf("%02hhx ", *p); }
	putchar('\n');

	printf("literal(right)  = %s\n", multi.get<char>().c_str());
	printf("literal(decode) = %s\n", buffer);

	s = std::mbstate_t();
	test_assert(cvt.length(s, multi.get<char>().data(),
	                       multi.get<char>().data()
	                         + multi.get<char>().length(),
	                  multi.get<char16_t>().length())
	          == static_cast<int>(multi.get<char>().length()));

	char16_t buffer16[40];
	memset(buffer16, 0, sizeof(buffer16));
	const char * cto_end = nullptr;
	char16_t * end16;

	s = std::mbstate_t();
	r = cvt.in(s,multi.get<char>().data(),
	           multi.get<char>().data() + multi.get<char>().size(),
	           cto_end,
	           buffer16, buffer16 + 40, end16);

	printf("-> in returned %s\n", code2str(r));
	test_assert(r == std::codecvt_base::ok);

	test_assert(static_cast<size_t>(end16 - buffer16)
	              == multi.get<char16_t>().length());
	test_assert(!memcmp(buffer16, multi.get<char16_t>().data(),
	                    end16 - buffer16));
}

const char * mode2str(std::codecvt_mode m)
{
	switch (static_cast<int>(m))
	{
	 case 0: return "none";
	 case 1: return "little_endian";
	 case 2: return "generate_header";
	 case 3: return "generate_header|little_endian";
	 case 4: return "consume_header";
	 case 5: return "consume_header|little_endian";
	 case 6: return "consume_header|generate_header";
	 case 7: return "consume_header|generate_header|little_endian";
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

namespace bug
{
	template <class T>
	constexpr const T & min(const T & a, const T & b)
	{ return (a < b) ? a : b; }
}

static const std::string sep(80, '-');

template <typename T, unsigned long MAX, std::codecvt_mode MODE>
void check_codecvt_utf16()
{
	std::codecvt_utf16<T, MAX, MODE> cvt;
	std::string type_name = demangle(typeid(T).name());

	printf("%s\ncodecvt_utf16<%s,0x%lx,%s>\n%s\n", sep.c_str(),
	       type_name.c_str(), MAX, mode2str(MODE), sep.c_str());

	printf("always_noconv() = %s\n", cvt.always_noconv() ? "true" : "false");
	test_assert(!cvt.always_noconv());

	printf("encoding() = %d\n", cvt.encoding());
	test_assert(cvt.encoding() == 0);

	printf("max_length() = %d\n", cvt.max_length());

	bool expect_error = false;

	for (auto c : multi.get<char32_t>())
	{
		if (c > MAX)
			expect_error = true;
	}

	for (auto c : multi.get<T>())
	{
		if (utf16_conversion::is_surrogate(c))
			expect_error = true;
	}

	std::codecvt_base::result res;
	auto state = std::mbstate_t();

	{
		const T * begin = multi.get<T>().data();
		const T * end = multi.get<T>().data() + multi.get<T>().size();
		const T * last = nullptr;

		const size_t bufsize = 256;
		char buffer_out[bufsize];
		char * last_out;
		memset(buffer_out, 0, bufsize);

		res = cvt.out(state, begin, end, last,
		              buffer_out, buffer_out + bufsize, last_out);

		printf("out() conversion returned '%s' after writing %ld bytes\n",
		       code2str(res), last_out - buffer_out);

		printf("---> ");
		for (int i = 0; i < (last_out - buffer_out); ++i)
			printf("%02hhx ", buffer_out[i]);
		putchar('\n');

		if (res == std::codecvt_base::ok)
		{
			bool le = ((MODE & std::little_endian) == std::little_endian);
			bool header =
			       ((MODE & std::generate_header) == std::generate_header);

			std::string correct_val( (header) ?
			                         (le ? "\xff\xfe" : "\xfe\xff") :
			                         "");
			correct_val += multi.byte_oriented_string<char16_t>(le);

			std::string converted(buffer_out, last_out - buffer_out);
			test_assert(correct_val == converted);

			{
				std::codecvt_base::result res2;
				const char * cptr = nullptr;
				T buffer_x[bufsize];
				T * last = nullptr;
				state = std::mbstate_t();
				res2 = cvt.in(state, buffer_out, last_out, cptr,
				              buffer_x, buffer_x + bufsize, last);
				printf("in() conversion returned '%s' after writing "
				       "%ld bytes\n",
				       code2str(res2), last - buffer_x);
				printf("in() consumed %ld bytes\n", cptr - buffer_out);
				state = std::mbstate_t();
				int length_ret = cvt.length(state, buffer_out, last_out,
				                            last - buffer_x);
				printf("length() returned %d\n", length_ret);
			}
		}

		test_assert(expect_error ^ (res == std::codecvt_base::ok));
	}
}

template <typename T, unsigned long MAX, std::codecvt_mode MODE>
void check_codecvt_utf8()
{
	std::codecvt_utf8<T, MAX, MODE> cvt;
	std::string type_name = demangle(typeid(T).name());

	printf("%s\ncodecvt_utf8<%s, 0x%lx, %s>\n%s\n", sep.c_str(),
	       type_name.c_str(), MAX, mode2str(MODE), sep.c_str());

	printf("always_noconv() = %s\n", cvt.always_noconv() ? "true" : "false");
	test_assert(!cvt.always_noconv());

	printf("encoding() = %d\n", cvt.encoding());
	test_assert(cvt.encoding() == 0);

	printf("max_length() = %d\n", cvt.max_length());

	bool expect_error = false;

	for (auto c : multi.get<char32_t>())
	{
		if (c > MAX)
			expect_error = true;
	}

	for (auto c : multi.get<T>())
	{
		if (utf16_conversion::is_surrogate(c))
			expect_error = true;
	}

	std::codecvt_base::result res;
	auto state = std::mbstate_t();

	{
		const T * begin = multi.get<T>().data();
		const T * end = multi.get<T>().data() + multi.get<T>().size();
		const T * last = nullptr;

		const size_t bufsize = 256;
		char buffer_out[bufsize];
		char * last_out;
		memset(buffer_out, 0, bufsize);

		res = cvt.out(state, begin, end, last,
		              buffer_out, buffer_out + bufsize, last_out);

		printf("out() conversion returned '%s' after writing %ld bytes\n",
		       code2str(res), last_out - buffer_out);

		if (res == std::codecvt_base::ok)
		{
			std::string out_string(buffer_out, last_out - buffer_out);
			std::string reference = (MODE & std::generate_header) ?
			                        "\xef\xbb\xbf" : "";

			reference += multi.get<char>();

			if (out_string != reference)
			{
				for (size_t x = 0;
				     x < out_string.length() && x < reference.length();
				     ++x)
				{
					printf("%zu: %02hhx %02hhx\n", x, out_string.at(x),
					       reference.at(x));
				}
			}
			test_assert(out_string == reference);

			state = std::mbstate_t();
			const char * length_start = out_string.data();
			const char * length_end = out_string.data() + out_string.length();

			if (  ((MODE & std::generate_header) == std::generate_header)
			   && ((MODE & std::consume_header) == 0))
			{
				length_start += 3;
			}

			int count = cvt.length(state, length_start, length_end,
			                       multi.get<T>().length());

			test_assert(count == (length_end - length_start));
		}

		test_assert(expect_error ^ (res == std::codecvt_base::ok));
	}

	{
		const size_t bufsize = 256;

		std::string encoded = multi.get<char>();
		const char * begin = encoded.data();
		const char * end = encoded.data() + encoded.size();
		const char * last = nullptr;
		T buffer[bufsize];
		T * last_out = nullptr;
		memset(buffer, 0, bufsize);

		res = cvt.in(state, begin, end, last,
		             buffer, buffer + bufsize, last_out);

		printf("in() conversion(no bom) returned '%s' after writing "
		       "%ld bytes\n", code2str(res), last_out - buffer);

		int no_bom_chars_written = (last_out - buffer);

		if (res == std::codecvt_base::ok)
			test_assert(buffer == multi.get<T>());

		encoded = "\xef\xbb\xbf";
		encoded += multi.get<char>();

		begin = encoded.data();
		end = encoded.data() + encoded.size();
		last = nullptr;
		last_out = nullptr;
		memset(buffer, 0, bufsize);

		res = cvt.in(state, begin, end, last,
		             buffer, buffer + bufsize, last_out);

		printf("in() conversion(with bom) returned '%s' after writing "
		       "%ld bytes\n", code2str(res), last_out - buffer);

		int with_bom_chars_written = (last_out - buffer);

		test_assert(expect_error ^ (res == std::codecvt_base::ok));

		if (res == std::codecvt_base::ok)
		{
			if ((MODE & std::consume_header) == std::consume_header)
			{
				test_assert(no_bom_chars_written == with_bom_chars_written);
				test_assert(buffer == multi.get<T>());
			} else
			{
				test_assert(no_bom_chars_written == (with_bom_chars_written-1));
				assert((buffer + 1) == multi.get<T>());
			}
		}
	}
}

template <typename T, std::codecvt_mode MODE>
void check_all_enums()
{
	constexpr unsigned long max =
	  bug::min<unsigned long>(0x7ffffffful, std::numeric_limits<T>::max());

	check_codecvt_utf8<T, 0xff, MODE>();
	check_codecvt_utf8<T, 0xffff, MODE>();
	check_codecvt_utf8<T, 0x10ffff, MODE>();
	check_codecvt_utf8<T, max, MODE>();

	check_codecvt_utf16<T, 0xff, MODE>();
	check_codecvt_utf16<T, 0xffff, MODE>();
	check_codecvt_utf16<T, 0x10ffff, MODE>();
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
		if (utf16_conversion::is_surrogate(i))
			continue;

		auto res = cvt.out(state, &i, (&i) + 1, doneptr, outbuf, end, ptr2);
//		if ((i % 0x10000) == 0 || ((i < 0x10000) && ((i % 0x100) == 0)))
//			printf("processing mega-plane 0x%08x, current len = %ld\n",
//			       i, ptr2 - outbuf);

		try {
		test_assert(res == std::codecvt_base::ok);
		} catch (...)
		{
			printf("failed value = %08x\n", i);
		}

		int length = ptr2 - outbuf;

		state = std::mbstate_t();
		int l2 = cvt.length(state, outbuf, ptr2, 20);

		test_assert(length == l2);

		char32_t redecoded = 0, * lastptr = nullptr;
		const char * ptr1 = nullptr;
		state = std::mbstate_t();
		res = cvt.in(state, outbuf, outbuf + length, ptr1,
		             &redecoded, (&redecoded) + 1, lastptr);

		test_assert(res == std::codecvt_base::ok);
		test_assert(i == redecoded);
	}

	checku16();

	check_all<wchar_t>();
	check_all<char16_t>();
	check_all<char32_t>();
}
