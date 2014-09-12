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
	CPPUNIT_TEST(testStuff<char>);
	CPPUNIT_TEST(testStuff<wchar_t>);
	CPPUNIT_TEST(testStuff<char32_t>);
	CPPUNIT_TEST_SUITE_END();

	typedef std::codecvt_base::result result;

	template <class CHAR_T = char32_t, unsigned int limit = 0x10FFFF>
	void testStuff()
	{
#if 0
		static_assert(std::numeric_limits<CHAR_T>::max() >= limit,
		              "Upper limit for char values must be less "
		              "than charater type max");
#endif
		CHAR_T wc[4];
		char nc[8];

		const CHAR_T * last_in = nullptr;
		char * last_out = nullptr;
		codecvt_utf8<CHAR_T, limit> cnv;
	
		for (wc[0] = 0;
		     static_cast<unsigned int>(wc[0]) < (limit + 1); ++wc[0])
		{
			if (config::verbose && ((wc[0] & 0xFFFF) == 0))
				printf("Processing plane 0x%08x\n", wc[0]);

			std::mbstate_t mbs = std::mbstate_t();
			memset(nc, 0, 8);
			result r = cnv.out(mbs, wc, wc + 1, last_in, nc, nc + 8, last_out);

			CPPUNIT_ASSERT(  r == std::codecvt_base::ok
			              || (r == std::codecvt_base::noconv
			                 && std::is_same<CHAR_T, char>::value));
			CPPUNIT_ASSERT(mbs.__count == 0);

			const char * last_in2 = nullptr;
			CHAR_T * last_out2 = nullptr;
			memset(&mbs, 0, sizeof(mbs));

			r = cnv.in(mbs, nc, last_out, last_in2, wc + 1, wc + 2, last_out2);

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
};
CPPUNIT_TEST_SUITE_REGISTRATION(Test_Unicode);
