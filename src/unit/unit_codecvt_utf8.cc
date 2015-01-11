#include "codecvt/codecvt"
#include "unit_codecvt_base.h"

//
// Seperate impl of this calculation here for the sake of testing
//
static constexpr int utf8_chars_needed(unsigned long char_value)
{
	return ( (char_value < 0x80) ? 1 :
	       ( (char_value < 0x800) ?  2 :
	       ( (char_value < 0x10000) ?  3 :
	       ( (char_value < 0x200000) ?  4 :
	       ( (char_value < 0x4000000) ?  5 :
	       ( (char_value < 0x80000000) ?  6 :
	         -1 ) ) ) ) ) );
}

namespace {
  // std::min isn't constexpr until c++14
  template<class T> constexpr const T& min(const T & a, const T & b )
    { return ( (a < b) ? a : b ); }
}

static constexpr int bom_length = utf8_chars_needed(bom_value());

template <typename C, unsigned long N, int M>
class Test_codecvt_utf8
  : public Test_codecvt_base<std::codecvt_utf8<C, N, (std::codecvt_mode)M>>
{
	typedef Test_codecvt_base<
	          std::codecvt_utf8<C, N, (std::codecvt_mode)M>> base;

	using typename base::cvt_t;

	typedef C ictype;

	CPPUNIT_TEST_SUITE(Test_codecvt_utf8);
	CPPUNIT_TEST(construction);
	CPPUNIT_TEST(encoding);
	CPPUNIT_TEST(always_noconv);
	CPPUNIT_TEST(max_length);
	CPPUNIT_TEST(encode_decode_char_range);
	CPPUNIT_TEST(unshift);
	CPPUNIT_TEST(unshift_errors);
	CPPUNIT_TEST_SUITE_END();

	static constexpr std::codecvt_mode cvtMode =
		static_cast<std::codecvt_mode>(M);

	static constexpr bool has_bom = (cvtMode & std::generate_header);

	static constexpr bool eats_bom = (cvtMode & std::consume_header);

	static constexpr unsigned long max_value =
		min(N, static_cast<unsigned long>(std::numeric_limits<C>::max()));

	static constexpr unsigned long max_value_length =
		utf8_chars_needed(max_value);

 public:
	virtual ~Test_codecvt_utf8() { }

	void max_length() override
	{
		typename base::cvt_t cvt;
		if (cvtMode & std::consume_header)
		{
			CPPUNIT_ASSERT( (max_value_length + bom_length) ==
			                  static_cast<unsigned long>(cvt.max_length()));
		} else
		{
			CPPUNIT_ASSERT(max_value_length ==
			                 static_cast<unsigned long>(cvt.max_length()));
		}
	}

	void encode_decode_char_range() override
	{
		cvt_t cvt;
		const int obufsz = 10;
		char outbuffer[obufsz];
		
		std::codecvt_base::result rc = std::codecvt_base::ok;
		char * end = nullptr;

		unsigned long cval = 0;
		for (; cval <= std::min(0xd7fful, max_value); ++cval)
		{
			end = outbuffer + obufsz;
			rc = out_for_ictype(cval, outbuffer, end);

			ictype x = 0;
			const char * end2 = end;
			in_for_ictype(x, outbuffer, end2);

			CPPUNIT_ASSERT(static_cast<unsigned long>(x) == cval);
		}

		for (cval = 0xe000ul;
		     cval <= std::min(0x10fffful, max_value);
		     cval += 0x100ul)
		{
			// N
			end = outbuffer + obufsz;
			rc = out_for_ictype(cval, outbuffer, end);
			CPPUNIT_ASSERT(rc == std::codecvt_base::ok);

			// N + 1
			end = outbuffer + obufsz;
			rc = out_for_ictype(cval + 1, outbuffer, end);
			CPPUNIT_ASSERT(rc == std::codecvt_base::ok);

			// N - 1
			end = outbuffer + obufsz;
			rc = out_for_ictype(cval + 0x100ul - 1, outbuffer, end);
			CPPUNIT_ASSERT(rc == std::codecvt_base::ok);

		}

		for (cval = 0x110000ul; cval <= max_value; cval += 0x10000ul)
		{
			// N - 1
			end = outbuffer + obufsz;
			rc = out_for_ictype(cval - 1, outbuffer, end);
			CPPUNIT_ASSERT(rc == std::codecvt_base::ok);

			// N
			end = outbuffer + obufsz;
			rc = out_for_ictype(cval, outbuffer, end);
			CPPUNIT_ASSERT(rc == std::codecvt_base::ok);

			// N + 1
			end = outbuffer + obufsz;
			rc = out_for_ictype(cval + 1, outbuffer, end);
			CPPUNIT_ASSERT(rc == std::codecvt_base::ok);
		}
	}

	void unshift()
	{
//		std::mbstate_t state = std::mbstate_t();
	}

	void unshift_errors()
	{
		cvt_t cvt;
		const int bufsz = 10;
		char buffer[bufsz];
		char * nextptr = nullptr;
		std::mbstate_t state = std::mbstate_t();
		state.__value.__wch = 0;
		state.__count = -1;
		std::codecvt_base::result rc;
		rc = cvt.unshift(state, buffer, buffer + bufsz, nextptr);
		CPPUNIT_ASSERT(rc == std::codecvt_base::error);
	}

 private:
	std::codecvt_base::result
	out_for_ictype(ictype c, char * buffer, char *& buffer_end)
	{
		cvt_t cvt;
		std::codecvt_base::result rc = std::codecvt_base::ok;
		std::mbstate_t state = {0, {0}};
		ictype internarray[2] = { c , 0 };
		const ictype * from_next = nullptr;
		char * to_next = nullptr;
		rc = cvt.out(state, internarray, internarray + 1, from_next,
		             buffer, buffer_end, to_next);

		CPPUNIT_ASSERT(rc == std::codecvt_base::ok);

		const int encoded_length = has_bom ?
		                             utf8_chars_needed(c) + bom_length :
		                             utf8_chars_needed(c);

		CPPUNIT_ASSERT((to_next - buffer) == encoded_length);

		state = std::mbstate_t();
		int len = cvt.length(state, buffer, to_next, 4);

		CPPUNIT_ASSERT(  len == encoded_length
		              || (  has_bom
		                 && !eats_bom && N < 0xfffful
		                 && len == bom_length) );

		buffer_end = to_next;

		return rc;
	}

	std::codecvt_base::result
	in_for_ictype(ictype & c, const char * buffer, const char *& end)
	{
		cvt_t cvt;
		std::mbstate_t state = std::mbstate_t();
		std::codecvt_base::result rc = std::codecvt_base::ok;

		ictype conv_buffer[10];
		ictype * last = nullptr;
		const char * saved_end = end;

		if ( (cvtMode & std::generate_header)
		   && ! (cvtMode & std::consume_header) )
			buffer += bom_length;

		c = 0;
		rc = cvt.in(state, buffer, saved_end, end,
		            conv_buffer, conv_buffer + 10, last);

		CPPUNIT_ASSERT(rc == std::codecvt_base::ok);
		c = conv_buffer[0];
		              
		return rc;
	}
};

//
// These are static constexpr variables, unfortunately the compiler isn't
// smart enough to find a place to generate them on its own
//
template <typename C, unsigned long N, int M>
  constexpr std::codecvt_mode Test_codecvt_utf8<C, N, M>::cvtMode;

template <typename C, unsigned long N, int M>
  constexpr bool Test_codecvt_utf8<C, N, M>::has_bom;

template <typename C, unsigned long N, int M>
  constexpr bool Test_codecvt_utf8<C, N, M>::eats_bom;

template <typename C, unsigned long N, int M>
  constexpr unsigned long Test_codecvt_utf8<C, N, M>::max_value;

template <typename C, unsigned long N, int M>
  constexpr unsigned long Test_codecvt_utf8<C, N, M>::max_value_length;

FOR_ALL_CVT_MODES(REGISTER_UTF_TEST, Test_codecvt_utf8, wchar_t, 0x7f);
FOR_ALL_CVT_MODES(REGISTER_UTF_TEST, Test_codecvt_utf8, wchar_t, 0xff);
FOR_ALL_CVT_MODES(REGISTER_UTF_TEST, Test_codecvt_utf8, wchar_t, 0xffff);
FOR_ALL_CVT_MODES(REGISTER_UTF_TEST, Test_codecvt_utf8, wchar_t, 0x10ffff);

FOR_ALL_CVT_MODES(REGISTER_UTF_TEST, Test_codecvt_utf8, char16_t, 0x7f);
FOR_ALL_CVT_MODES(REGISTER_UTF_TEST, Test_codecvt_utf8, char16_t, 0xff);
FOR_ALL_CVT_MODES(REGISTER_UTF_TEST, Test_codecvt_utf8, char16_t, 0xffff);
FOR_ALL_CVT_MODES(REGISTER_UTF_TEST, Test_codecvt_utf8, char16_t, 0x10ffff);

FOR_ALL_CVT_MODES(REGISTER_UTF_TEST, Test_codecvt_utf8, char32_t, 0x7f);
FOR_ALL_CVT_MODES(REGISTER_UTF_TEST, Test_codecvt_utf8, char32_t, 0xff);
FOR_ALL_CVT_MODES(REGISTER_UTF_TEST, Test_codecvt_utf8, char32_t, 0xffff);
FOR_ALL_CVT_MODES(REGISTER_UTF_TEST, Test_codecvt_utf8, char32_t, 0x10ffff);
FOR_ALL_CVT_MODES(REGISTER_UTF_TEST, Test_codecvt_utf8, char32_t, 0x3ffffff);
FOR_ALL_CVT_MODES(REGISTER_UTF_TEST, Test_codecvt_utf8, char32_t, 0x7fffffff);
