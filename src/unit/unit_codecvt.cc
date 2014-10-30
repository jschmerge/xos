#include "../codecvt/codecvt_specializations.h"
#include "../codecvt/codecvt_utf8.h"
#include "../codecvt/utf_conversion_helpers.h"

#include <cstring>

#include "cppunit-header.h"
#include "multistring.h"

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template<class Facet>
struct deletable_facet : Facet
{
	template<class ...Args>
	deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}

	~deletable_facet() {}
};

static const DEF_MULTISTRING(nul_char, "\x0");
static const DEF_MULTISTRING(ascii_max, "\x7f");

// XXX - add test for this
static const DEF_MULTISTRING(bom, "\ufeff");

// Bug in gcc converts u"\uffff" into {0xd7ff, 0xdffff } - workaround for that
static const multistring bmp_max(u8"\uffff", L"\xffff",
                                 u"\xffff", U"\xffff");

static const DEF_MULTISTRING(max_unicode, "\U0010ffff");

class Test_codecvt : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_codecvt);
	CPPUNIT_TEST(construction<char16_t>);
	CPPUNIT_TEST(encoding<char16_t>);
	CPPUNIT_TEST(always_noconv<char16_t>);
	CPPUNIT_TEST(max_length<char16_t>);
	CPPUNIT_TEST(out<char16_t>);
	CPPUNIT_TEST(in<char16_t>);
	CPPUNIT_TEST(length<char16_t>);

	CPPUNIT_TEST(construction<char32_t>);
	CPPUNIT_TEST(encoding<char32_t>);
	CPPUNIT_TEST(always_noconv<char32_t>);
	CPPUNIT_TEST(max_length<char32_t>);
	CPPUNIT_TEST(out<char32_t>);
	CPPUNIT_TEST(in<char32_t>);
	CPPUNIT_TEST(length<char32_t>);
	CPPUNIT_TEST_SUITE_END();

	typedef std::codecvt_base::result result;

	// FIXME: we're using deletable_facet here instead of the proper machinery
	// until we get a change into gcc's libstdc++-v3/src/c++98/locale_init.cc
	// to register these facets... without that, the use_facet/has_facet
	// functions will continue to be broken
#if 0
	template<typename T>
	void construction()
	{
		typedef std::codecvt<T, char, std::mbstate_t> cvt_t;
		std::locale mylocale;
		const cvt_t & cvt = std::use_facet<cvt_t>(mylocale);

		CPPUNIT_ASSERT(cvt.encoding() == 0);
	}
#else
	template<typename T>
	void construction()
	{
		typedef std::codecvt<T, char, std::mbstate_t> cvt_t;
		deletable_facet<cvt_t> cvt;
		deletable_facet<cvt_t> cvt2(1);
		deletable_facet<cvt_t> cvt3(2);

		static_assert(std::is_same<T, typename cvt_t::intern_type>::value,
		              "codecvt<> intern_type is invalid");
		static_assert(std::is_same<char, typename cvt_t::extern_type>::value,
		              "codecvt<> extern_type is invalid");
		CPPUNIT_ASSERT(cvt.encoding() == 0);

	}
#endif

	template<typename T>
	void encoding()
	{
		typedef std::codecvt<T, char, std::mbstate_t> cvt_t;
		deletable_facet<cvt_t> cvt;
		CPPUNIT_ASSERT(cvt.encoding() == 0);
	}

	template<typename T>
	void always_noconv()
	{
		typedef std::codecvt<T, char, std::mbstate_t> cvt_t;
		deletable_facet<cvt_t> cvt;
		CPPUNIT_ASSERT(cvt.always_noconv() == false);
	}

	template<typename T>
	void max_length()
	{
		typedef std::codecvt<T, char, std::mbstate_t> cvt_t;
		deletable_facet<cvt_t> cvt;
		if (std::is_same<char16_t, T>::value)
			CPPUNIT_ASSERT(cvt.max_length() == 4);
		else if (std::is_same<char32_t, T>::value)
			CPPUNIT_ASSERT(cvt.max_length() == 6);
	}

	template<typename T>
	void out_type_specific();

	template<typename T>
	void out()
	{
		typedef std::codecvt<T, char, std::mbstate_t> cvt_t;
		deletable_facet<cvt_t> cvt;
		T input_buffer[20];
		std::mbstate_t state;
		result r = std::codecvt_base::ok;

		const int bufSz = 2 * cvt.max_length();
		std::unique_ptr<char[]> output_buffer(new char[bufSz]);
		char * out_start = output_buffer.get();
		char * out_end = out_start + bufSz;
		const T * const_intern_ptr = nullptr;
		char * extern_ptr = nullptr;

		CPPUNIT_ASSERT(nul_char.get<T>().size() != 0);
		for (size_t i = 0; i < nul_char.get<T>().length(); ++i)
			input_buffer[i] = nul_char.get<T>()[i];
		input_buffer[nul_char.get<T>().length()] = 0;
		
#if 0
		input_buffer[0] = 0;
#endif
		memset(&state, 0, sizeof(state));
		r = cvt.out(state, input_buffer,
		            input_buffer + 1, const_intern_ptr,
		            out_start, out_end, extern_ptr);
		                   
		CPPUNIT_ASSERT(r == std::codecvt_base::ok);
		CPPUNIT_ASSERT((extern_ptr - out_start) == 1);
		CPPUNIT_ASSERT(output_buffer[0] == '\0');

		//////

		CPPUNIT_ASSERT(ascii_max.get<T>().size() != 0);
		for (size_t i = 0; i < ascii_max.get<T>().length(); ++i)
			input_buffer[i] = ascii_max.get<T>()[i];
		input_buffer[ascii_max.get<T>().length()] = 0;

		memset(&state, 0, sizeof(state));
		memset(output_buffer.get(), 0, bufSz);
		r = cvt.out(state, input_buffer,
		            input_buffer + 1, const_intern_ptr,
		            out_start, out_end, extern_ptr);
		                   
		CPPUNIT_ASSERT(r == std::codecvt_base::ok);
		CPPUNIT_ASSERT((extern_ptr - out_start) == 1);
		CPPUNIT_ASSERT(output_buffer[0] == 0x7f);

		//////

		CPPUNIT_ASSERT(bmp_max.get<T>().size() != 0);
		for (size_t i = 0; i < bmp_max.get<T>().length(); ++i)
			input_buffer[i] = bmp_max.get<T>()[i];
		input_buffer[bmp_max.get<T>().length()] = 0;

		memset(&state, 0, sizeof(state));
		memset(output_buffer.get(), 0, bufSz);
		r = cvt.out(state, input_buffer,
		            input_buffer + 1, const_intern_ptr,
		            out_start, out_end, extern_ptr);
		                   
		CPPUNIT_ASSERT(r == std::codecvt_base::ok);
		CPPUNIT_ASSERT((extern_ptr - out_start) == 3);
		CPPUNIT_ASSERT(  static_cast<uint8_t>(output_buffer[0]) == 0xefu
		              && static_cast<uint8_t>(output_buffer[1]) == 0xbfu
		              && static_cast<uint8_t>(output_buffer[2]) == 0xbfu);

		out_type_specific<T>();
	}

	template<typename T>
	void in()
	{
		typedef std::codecvt<T, char, std::mbstate_t> cvt_t;
		deletable_facet<cvt_t> cvt;
		result r = std::codecvt_base::ok;
		const char * input_buffer = nul_char.get<char>().c_str();
		const char * input_end = input_buffer + nul_char.get<char>().length();
		const char * input_last = nullptr;
		std::mbstate_t state = std::mbstate_t();

		const size_t outSz = 3;
		T out_buffer[outSz];
		T * out_end = nullptr;

		memset(out_buffer, 0, outSz * sizeof(T));
		r = cvt.in(state, input_buffer, input_end, input_last,
		           out_buffer, out_buffer + outSz, out_end);

		CPPUNIT_ASSERT(r == std::codecvt_base::ok);
		for (size_t i = 0; i < nul_char.get<T>().length(); ++i)
			printf("%06x %06x\n", nul_char.get<T>()[i], out_buffer[i]);
		CPPUNIT_ASSERT(nul_char.get<T>()[0] == out_buffer[0]);

		/////

		input_buffer = ascii_max.get<char>().c_str();
		input_end = input_buffer + ascii_max.get<char>().length();
		input_last = nullptr;
		state = std::mbstate_t();

		memset(out_buffer, 0, outSz * sizeof(T));
		r = cvt.in(state, input_buffer, input_end, input_last,
		           out_buffer, out_buffer + outSz, out_end);

		CPPUNIT_ASSERT(r == std::codecvt_base::ok);
		for (size_t i = 0; i < ascii_max.get<T>().length(); ++i)
			printf("%06x %06x\n", ascii_max.get<T>()[i], out_buffer[i]);
		CPPUNIT_ASSERT(ascii_max.get<T>() == out_buffer);

		/////

		input_buffer = bmp_max.get<char>().c_str();
		input_end = input_buffer + bmp_max.get<char>().length();
		input_last = nullptr;
		state = std::mbstate_t();

		memset(out_buffer, 0, outSz * sizeof(T));
		r = cvt.in(state, input_buffer, input_end, input_last,
		           out_buffer, out_buffer + outSz, out_end);

		CPPUNIT_ASSERT(r == std::codecvt_base::ok);
		for (size_t i = 0; i < bmp_max.get<T>().length(); ++i)
			printf("%06x %06x\n", bmp_max.get<T>()[i], out_buffer[i]);
		CPPUNIT_ASSERT(bmp_max.get<T>() == out_buffer);

		/////

		input_buffer = max_unicode.get<char>().c_str();
		input_end = input_buffer + max_unicode.get<char>().length();
		input_last = nullptr;
		state = std::mbstate_t();

		memset(out_buffer, 0, outSz * sizeof(T));
		r = cvt.in(state, input_buffer, input_end, input_last,
		           out_buffer, out_buffer + outSz, out_end);

		CPPUNIT_ASSERT(r == std::codecvt_base::ok);
		for (size_t i = 0; i < max_unicode.get<T>().length(); ++i)
			printf("%06x %06x\n", max_unicode.get<T>()[i], out_buffer[i]);
		CPPUNIT_ASSERT(max_unicode.get<T>() == out_buffer);
	}

	template<typename T>
	void length()
	{
		typedef std::codecvt<T, char, std::mbstate_t> cvt_t;
		deletable_facet<cvt_t> cvt;

		const char * input_buffer = nul_char.get<char>().c_str();
		const char * input_end = input_buffer + nul_char.get<char>().length();
		std::mbstate_t state = std::mbstate_t();
		size_t len = cvt.length(state, input_buffer, input_end, 1);
		CPPUNIT_ASSERT(len == nul_char.get<char>().length());

		input_buffer = ascii_max.get<char>().c_str();
		input_end = input_buffer + ascii_max.get<char>().length();
		state = std::mbstate_t();
		len = cvt.length(state, input_buffer, input_end, 1);
		CPPUNIT_ASSERT(len == ascii_max.get<char>().length());

		input_buffer = bmp_max.get<char>().c_str();
		input_end = input_buffer + bmp_max.get<char>().length();
		state = std::mbstate_t();
		len = cvt.length(state, input_buffer, input_end, 1);
		CPPUNIT_ASSERT(len == bmp_max.get<char>().length());

		input_buffer = max_unicode.get<char>().c_str();
		input_end = input_buffer + max_unicode.get<char>().length();
		state = std::mbstate_t();
		len = cvt.length(state, input_buffer, input_end, 1);
		CPPUNIT_ASSERT(len == max_unicode.get<char>().length());

	}
};

template<>
void Test_codecvt::out_type_specific<char16_t>()
{
	typedef std::codecvt<char16_t, char, std::mbstate_t> cvt_t;
	deletable_facet<cvt_t> cvt;
	char16_t input_buffer[3];
	std::mbstate_t state;
	result r = std::codecvt_base::ok;

	const int bufSz = 2 * cvt.max_length();
	std::unique_ptr<char[]> output_buffer(new char[bufSz]);
	char * out_start = output_buffer.get();
	char * out_end = out_start + bufSz;
	const char16_t * const_intern_ptr = nullptr;
	char * extern_ptr = nullptr;

	input_buffer[0] = 0xdbff;
	input_buffer[1] = 0xdfff;
	input_buffer[2] = 0;
	memset(&state, 0, sizeof(state));
	memset(output_buffer.get(), 0, bufSz);
	r = cvt.out(state, input_buffer,
	            input_buffer + 2, const_intern_ptr,
	            out_start, out_end, extern_ptr);

	CPPUNIT_ASSERT(r == std::codecvt_base::ok);
	CPPUNIT_ASSERT(static_cast<uint8_t>(output_buffer[0]) == 0xf4);
	CPPUNIT_ASSERT(static_cast<uint8_t>(output_buffer[1]) == 0x8f);
	CPPUNIT_ASSERT(static_cast<uint8_t>(output_buffer[2]) == 0xbf);
	CPPUNIT_ASSERT(static_cast<uint8_t>(output_buffer[3]) == 0xbf);
}

template<>
void Test_codecvt::out_type_specific<char32_t>()
{
	typedef std::codecvt<char32_t, char, std::mbstate_t> cvt_t;
	deletable_facet<cvt_t> cvt;
	char32_t input_buffer[2];
	std::mbstate_t state;
	result r = std::codecvt_base::ok;

	const int bufSz = 2 * cvt.max_length();
	std::unique_ptr<char[]> output_buffer(new char[bufSz]);
	char * out_start = output_buffer.get();
	char * out_end = out_start + bufSz;
	const char32_t * const_intern_ptr = nullptr;
	char * extern_ptr = nullptr;

	input_buffer[0] = 0x0010ffffu;
	input_buffer[1] = 0;
	memset(&state, 0, sizeof(state));
	memset(output_buffer.get(), 0, bufSz);
	r = cvt.out(state, input_buffer,
	            input_buffer + 2, const_intern_ptr,
	            out_start, out_end, extern_ptr);

	CPPUNIT_ASSERT(r == std::codecvt_base::ok);
	CPPUNIT_ASSERT(static_cast<uint8_t>(output_buffer[0]) == 0xf4);
	CPPUNIT_ASSERT(static_cast<uint8_t>(output_buffer[1]) == 0x8f);
	CPPUNIT_ASSERT(static_cast<uint8_t>(output_buffer[2]) == 0xbf);
	CPPUNIT_ASSERT(static_cast<uint8_t>(output_buffer[3]) == 0xbf);

	memcpy(input_buffer, U"\U0010ffff", 4);
	input_buffer[0] = 0x7fffffffu;
	input_buffer[1] = 0;
	memset(&state, 0, sizeof(state));
	memset(output_buffer.get(), 0, bufSz);
	r = cvt.out(state, input_buffer,
	            input_buffer + 2, const_intern_ptr,
	            out_start, out_end, extern_ptr);

	CPPUNIT_ASSERT(r == std::codecvt_base::ok);
	CPPUNIT_ASSERT(static_cast<uint8_t>(output_buffer[0]) == 0xfd);
	CPPUNIT_ASSERT(static_cast<uint8_t>(output_buffer[1]) == 0xbf);
	CPPUNIT_ASSERT(static_cast<uint8_t>(output_buffer[2]) == 0xbf);
	CPPUNIT_ASSERT(static_cast<uint8_t>(output_buffer[3]) == 0xbf);
	CPPUNIT_ASSERT(static_cast<uint8_t>(output_buffer[4]) == 0xbf);
	CPPUNIT_ASSERT(static_cast<uint8_t>(output_buffer[5]) == 0xbf);
}

CPPUNIT_TEST_SUITE_REGISTRATION(Test_codecvt);
