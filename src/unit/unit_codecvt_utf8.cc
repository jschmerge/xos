#include "codecvt/codecvt"
#include "unit_codecvt_base.h"

//
// Seperate impl of this calculation here for the sake of testing
//
static int utf8_chars_needed(unsigned long char_value)
{
	int ret = -1;
	if (char_value < 0x80)
		ret = 1;
	else if (char_value < 0x800)
		ret = 2;
	else if (char_value < 0x10000)
		ret = 3;
	else if (char_value < 0x200000)
		ret = 4;
	else if (char_value < 0x4000000)
		ret = 5;
	else if (char_value < 0x80000000)
		ret = 6;
	return ret;
}

namespace {
  // std::min isn't constexpr until c++14
  template<class T> constexpr const T& min(const T & a, const T & b )
    { return ( (a < b) ? a : b ); }
}

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

#if 0
	static const std::codecvt_mode cvtMode = static_cast<std::codecvt_mode>(M);
	static const bool has_bom  = (cvtMode & std::generate_header);
	static const bool eats_bom = (cvtMode & std::consume_header);
	static constexpr unsigned long max_value =
	  min(N, static_cast<unsigned long>(std::numeric_limits<C>::max()));
#endif

	static const std::codecvt_mode cvtMode;
	static const bool has_bom;
	static const bool eats_bom;
//	static constexpr unsigned long max_value;
	static constexpr unsigned long max_value =
	  min(N, static_cast<unsigned long>(std::numeric_limits<C>::max()));

 public:
	virtual ~Test_codecvt_utf8() { }

	void max_length() override
	{
		typename base::cvt_t cvt;
		if (cvtMode & std::consume_header)
		{
			CPPUNIT_ASSERT(
			  (utf8_chars_needed(max_value) +
			   utf8_chars_needed(bom_value()) == cvt.max_length()));
		} else
		{
			CPPUNIT_ASSERT(utf8_chars_needed(max_value) == cvt.max_length());
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

			CPPUNIT_ASSERT( (rc == std::codecvt_base::ok)
			  || ( (N < static_cast<unsigned long>(bom_value()))
			     && (cvtMode & std::generate_header)
			     && (rc == std::codecvt_base::error)) );

			ictype x = 0;
			const char * end2 = end;
			in_for_ictype(x, outbuffer, end2);
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
		CPPUNIT_ASSERT(rc = std::codecvt_base::error);
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

		const int bom_length = utf8_chars_needed(bom_value());
		const int encoded_length = has_bom ?
		                             utf8_chars_needed(c) + bom_length :
		                             utf8_chars_needed(c);

		CPPUNIT_ASSERT(rc == std::codecvt_base::ok);
		if( (to_next - buffer) != encoded_length)
		{
			printf("\n%08X:, difference = %zd, computed = %d\n",
			       c, to_next - buffer, encoded_length);
		}
		CPPUNIT_ASSERT((to_next - buffer) == encoded_length);

		state = std::mbstate_t();
		int len = cvt.length(state, buffer, to_next, 4);

		CPPUNIT_ASSERT(  len == encoded_length
		              || (has_bom && !eats_bom && N < 0xfffful && len == 3) );

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

//		for (auto p = buffer; p < end; ++p)
//			printf("%02hhX ", *p);

		c = 0;
		rc = cvt.in(state, buffer, saved_end, end,
		            conv_buffer, conv_buffer + 10, last);

//		if (rc) printf("\nRC = %d, yeild = %zd\n", rc, last-conv_buffer);
//		CPPUNIT_ASSERT(rc == std::codecvt_base::ok);
#if 0
		if( (has_bom  &&  eats_bom && ((last - conv_buffer) == 1))
		  || (!has_bom &&  eats_bom && ((last - conv_buffer) == 1))
		  || (has_bom  && !eats_bom && ((last - conv_buffer) == 2))
		  || (!has_bom && !eats_bom && ((last - conv_buffer) == 1))
		  )
		{ } else {
			printf("\n");
			for (auto p = buffer; p < saved_end; ++p)
				printf("%02hhX ", *p);
			printf("\nhas %d eats %d\n", has_bom, eats_bom);
			printf("num eaten = %zd\n", last - conv_buffer);
			CPPUNIT_ASSERT(false);
		}
#endif
		              
		return rc;
	}
};

template <typename C, unsigned long N, int M>
const std::codecvt_mode Test_codecvt_utf8<C, N, M>::cvtMode =
	static_cast<std::codecvt_mode>(M);

template <typename C, unsigned long N, int M>
const bool Test_codecvt_utf8<C, N, M>::has_bom =
	(cvtMode & std::generate_header);

template <typename C, unsigned long N, int M>
const bool Test_codecvt_utf8<C, N, M>::eats_bom =
	(cvtMode & std::consume_header);

template <typename C, unsigned long N, int M>
constexpr unsigned long Test_codecvt_utf8<C, N, M>::max_value;

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

