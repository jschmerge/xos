#include "../unicode/utfconv.h"

#include <cstring>

#include "cppunit-header.h"

const char * string_utf8
  = u8"a\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068\u6559\u6388"
      "\U0010FF00  \U00011111 \U0010FFFF";

const char16_t * string_utf16
  = u"a\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068\u6559\u6388"
     "\U0010FF00  \U00011111 \U0010FFFF";

const char32_t * string_utf32
  = U"a\u5916\u56FD\u8A9E\u306E\u5B66\u7FD2\u3068\u6559\u6388"
     "\U0010FF00  \U00011111 \U0010FFFF";

class Test_Unicode : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_Unicode);

	CPPUNIT_TEST(utf8_error_sequences);

	CPPUNIT_TEST(trivialTests<char>);
	CPPUNIT_TEST(trivialTests<wchar_t>);
	CPPUNIT_TEST(trivialTests<char16_t>);
	CPPUNIT_TEST(trivialTests<char32_t>);

	CPPUNIT_TEST(test_utf8_utf32_conversions<char>);
	CPPUNIT_TEST(test_utf8_utf32_conversions<wchar_t>);
	CPPUNIT_TEST(test_utf8_utf32_conversions<char16_t>);
	CPPUNIT_TEST(test_utf8_utf32_conversions<char32_t>);

	CPPUNIT_TEST(test_utf8_ucs4_conversions<char>);
	CPPUNIT_TEST(test_utf8_ucs4_conversions<wchar_t>);
	CPPUNIT_TEST(test_utf8_ucs4_conversions<char16_t>);
	CPPUNIT_TEST(test_utf8_ucs4_conversions<char32_t>);
	CPPUNIT_TEST_SUITE_END();

	typedef std::codecvt_base::result result;

	template <class CHAR_T = char32_t, unsigned int limit = 0x10FFFF>
	void trivialTests()
	{
		codecvt_utf8<CHAR_T, limit> cnv;
		if (std::is_same<CHAR_T, char>::value)
		{
			CPPUNIT_ASSERT(cnv.encoding() == 1);
			CPPUNIT_ASSERT(cnv.max_length() == 1);

			char buf[] = "xxxxx";
			char * end = buf + sizeof(buf), * last = nullptr;
			std::mbstate_t mbs = std::mbstate_t();
			CPPUNIT_ASSERT(  cnv.unshift(mbs, buf, end, last)
			              == std::codecvt_base::noconv);
			CPPUNIT_ASSERT(last == buf);
		} else
		{
			CPPUNIT_ASSERT(cnv.encoding() == 0);
		}
	}

	void utf8_error_sequences()
	{
		std::mbstate_t mbs = std::mbstate_t();

		unsigned char c = 0;
		do {
			if (is_utf8_codepoint_start(c))
				CPPUNIT_ASSERT(utf8_codepoint_length(c) != 0);
			++c;
		} while (c != 0);

		mbs.__count = 1;
		CPPUNIT_ASSERT(utf8_update_mbstate(mbs, '\xC0') == false);

		mbs.__count = -1;
		CPPUNIT_ASSERT(utf8_update_mbstate(mbs, '\xFF') == false);
	}

	template <class CHAR_T = char32_t, unsigned int limit = 0x10FFFF>
	void test_utf8_utf32_conversions()
	{
		CHAR_T wc[4];
		char nc[8];

		const CHAR_T * last_in = nullptr;
		char * last_out = nullptr;
		codecvt_utf8<CHAR_T, limit> cnv;

		trivialTests<CHAR_T, limit>();

		if (config::verbose) putchar('\n');

		for (wc[0] = 0;
		     static_cast<unsigned int>(wc[0]) < (limit + 1); ++wc[0])
		{
			if (config::verbose && ((wc[0] & 0xFFFF) == 0))
				printf("Processing plane 0x%08x\n", wc[0]);

			std::mbstate_t mbs;
			
			memset(&mbs, 0, sizeof(mbs));
			memset(nc, 0, 8);
			result r = cnv.out(mbs, wc, wc + 1, last_in, nc, nc + 8, last_out);


			CPPUNIT_ASSERT(  r == std::codecvt_base::ok
			              || (r == std::codecvt_base::noconv
			                 && std::is_same<CHAR_T, char>::value));
			CPPUNIT_ASSERT(mbs.__count == 0);

			const char * last_in2 = nullptr;
			CHAR_T * last_out2 = nullptr;
			memset(&mbs, 0, sizeof(mbs));
			int len = cnv.length(mbs, nc, last_out, 20);

			memset(&mbs, 0, sizeof(mbs));

			r = cnv.in(mbs, nc, last_out, last_in2, wc + 1, wc + 2, last_out2);

			CPPUNIT_ASSERT(len == (last_in2 - nc));

			CPPUNIT_ASSERT(  r == std::codecvt_base::ok
			              || (r == std::codecvt_base::noconv
			                 && std::is_same<CHAR_T, char>::value));

			if (r != std::codecvt_base::noconv)
				CPPUNIT_ASSERT(wc[0] == wc[1]);

			CPPUNIT_ASSERT(mbs.__count == 0);

			if (wc[0] == std::numeric_limits<CHAR_T>::max())
				break;
		}
	}

	template <class CHAR_T = char32_t, unsigned int limit = 0x7FFFFFFF>
	void test_utf8_ucs4_conversions()
	{
		CHAR_T wc[4] = { 1, 0, 0, 0 };
		char nc[8];

		const CHAR_T * last_in = nullptr;
		char * last_out = nullptr;
		codecvt_utf8<CHAR_T, limit> cnv;

		trivialTests<CHAR_T, limit>();

		//printf("----> %d\n", cnv.max_length());
		CPPUNIT_ASSERT(cnv.always_noconv() || cnv.max_length() > 1);
		while (static_cast<unsigned int>(wc[0]) < (limit + 1))
		{
			std::mbstate_t mbs;
			memset(&mbs, 0, sizeof(mbs));
			memset(nc, 0, 8);

			result r = cnv.out(mbs, wc, wc + 1, last_in, nc, nc + 8, last_out);

			CPPUNIT_ASSERT(  r == std::codecvt_base::ok
			              || (r == std::codecvt_base::noconv
			                 && std::is_same<CHAR_T, char>::value));
			CPPUNIT_ASSERT(mbs.__count == 0);

			const char * last_in2 = nullptr;
			CHAR_T * last_out2 = nullptr;
			memset(&mbs, 0, sizeof(mbs));
			int len = cnv.length(mbs, nc, last_out, 20);

			memset(&mbs, 0, sizeof(mbs));

			r = cnv.in(mbs, nc, last_out, last_in2, wc + 1, wc + 2, last_out2);

			CPPUNIT_ASSERT(len == (last_in2 - nc));

			CPPUNIT_ASSERT(  r == std::codecvt_base::ok
			              || (r == std::codecvt_base::noconv
			                 && std::is_same<CHAR_T, char>::value));

			if (r != std::codecvt_base::noconv)
				CPPUNIT_ASSERT(wc[0] == wc[1]);

			CPPUNIT_ASSERT(mbs.__count == 0);


			wc[0] <<= 1;
			if ((wc[0] == 0) || wc[0] == std::numeric_limits<CHAR_T>::max())
				break;
		}
	}
};
CPPUNIT_TEST_SUITE_REGISTRATION(Test_Unicode);
