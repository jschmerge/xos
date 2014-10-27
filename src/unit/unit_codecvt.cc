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


const char * code2str(std::codecvt_base::result r)
{
	switch (r) {
	 case std::codecvt_base::ok:      return "ok";
	 case std::codecvt_base::error:   return "error";
	 case std::codecvt_base::partial: return "partial";
	 case std::codecvt_base::noconv:  return "noconv";
	}
	return nullptr;
}

class Test_codecvt : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_codecvt);
	CPPUNIT_TEST(construction<char16_t>);
	CPPUNIT_TEST(encoding<char16_t>);
	CPPUNIT_TEST(always_noconv<char16_t>);
	CPPUNIT_TEST(max_length<char16_t>);

	CPPUNIT_TEST(construction<char32_t>);
	CPPUNIT_TEST(encoding<char32_t>);
	CPPUNIT_TEST(always_noconv<char32_t>);
	CPPUNIT_TEST(max_length<char32_t>);
	CPPUNIT_TEST(out_common<char32_t>);
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
	void out_common()
	{
		typedef std::codecvt<T, char, std::mbstate_t> cvt_t;
		deletable_facet<cvt_t> cvt;
		T input_buffer[2] = { 0, 0 };
		std::mbstate_t state;
		result r = std::codecvt_base::ok;

		const int bufSz = 10;
		char output_buffer[bufSz];
		const T * const_intern_ptr = nullptr;
		char * extern_ptr = nullptr;

		input_buffer[0] = 0;
		memset(&state, 0, sizeof(state));
		memset(output_buffer, 0, bufSz);
		r = cvt.out(state, input_buffer,
		            input_buffer + 1, const_intern_ptr,
		            output_buffer, output_buffer + bufSz, extern_ptr);
		                   
		CPPUNIT_ASSERT(r == std::codecvt_base::ok);
		CPPUNIT_ASSERT((extern_ptr - output_buffer) == 1);
		CPPUNIT_ASSERT(output_buffer[0] == '\0');

		//////

		input_buffer[0] = 0x7f;
		memset(&state, 0, sizeof(state));
		memset(output_buffer, 0, bufSz);
		r = cvt.out(state, input_buffer,
		            input_buffer + 1, const_intern_ptr,
		            output_buffer, output_buffer + bufSz, extern_ptr);
		                   
		CPPUNIT_ASSERT(r == std::codecvt_base::ok);
		CPPUNIT_ASSERT((extern_ptr - output_buffer) == 1);
		CPPUNIT_ASSERT(output_buffer[0] == 0x7f);

		//////

		input_buffer[0] = 0xffffu;
		memset(&state, 0, sizeof(state));
		memset(output_buffer, 0, bufSz);
		r = cvt.out(state, input_buffer,
		            input_buffer + 1, const_intern_ptr,
		            output_buffer, output_buffer + bufSz, extern_ptr);
		                   
		CPPUNIT_ASSERT(r == std::codecvt_base::ok);
		CPPUNIT_ASSERT((extern_ptr - output_buffer) == 3);
		CPPUNIT_ASSERT(  static_cast<uint8_t>(output_buffer[0]) == 0xefu
		              && static_cast<uint8_t>(output_buffer[1]) == 0xbfu
		              && static_cast<uint8_t>(output_buffer[2]) == 0xbfu);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_codecvt);
