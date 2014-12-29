#include "codecvt/codecvt"

#include <cstring>

#include "cppunit-header.h"
#include "multistring.h"

// utility wrapper to adapt locale facets for use outside of locale framework
template<class Facet>
class deletable_facet : public Facet
{
 public:
	template<class ...Args>
	deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}

	~deletable_facet() {}
};

template <typename T>
struct cvt_info
{
	static const unsigned long max_intern = 0x7ffffffful;
	static const bool consumes_bom = false;
	static const bool creates_bom = false;
};

template <>
struct cvt_info<std::codecvt<char16_t, char, std::mbstate_t>>
{
	static const unsigned long max_intern =
		utf16_conversion::max_encodable_value();
	static const bool consumes_bom = false;
	static const bool creates_bom = false;
};

template <typename C,
          unsigned long N,
          std::codecvt_mode M,
          template <class, unsigned long, std::codecvt_mode> class CVT>
struct cvt_info<CVT<C, N, M>>
{
	static const unsigned long max_intern = N;
	static const bool consumes_bom = (M & std::consume_header);
	static const bool creates_bom = (M & std::generate_header);
};

// Primary declaration
template <typename CVT>
class Test_codecvt_base : public CppUnit::TestFixture
{
 public:
	typedef CVT cvt_t;
	typedef typename CVT::intern_type char_type;

	CPPUNIT_TEST_SUITE(Test_codecvt_base);
	CPPUNIT_TEST(construction);
	CPPUNIT_TEST(encoding);
	CPPUNIT_TEST(always_noconv);
	CPPUNIT_TEST(max_length);
	CPPUNIT_TEST_SUITE_END();

 public:
	virtual ~Test_codecvt_base()
	{ }

	virtual void construction()
	{
		deletable_facet<cvt_t> cvt;
		deletable_facet<cvt_t> cvt2(1);
		deletable_facet<cvt_t> cvt3(2);

		static_assert(std::is_same<char, typename cvt_t::extern_type>::value,
		              "codecvt<> extern_type is invalid");
		CPPUNIT_ASSERT(cvt.encoding() == 0);
	}

	virtual void encoding()
	{
		deletable_facet<cvt_t> cvt;
		CPPUNIT_ASSERT(cvt.encoding() == 0);
	}

	virtual void always_noconv()
	{
		deletable_facet<cvt_t> cvt;
		CPPUNIT_ASSERT(cvt.always_noconv() == false);
	}


	virtual void max_length()
	{
		deletable_facet<cvt_t> cvt;

#if 0
		if (std::is_same<char16_t, char_type>::value)
			CPPUNIT_ASSERT(cvt.max_length() <= 4);
		else if (std::is_same<char32_t, char_type>::value)
			CPPUNIT_ASSERT(cvt.max_length() <= 6);
#endif
	}
};

typedef Test_codecvt_base<std::codecvt<char16_t, char, std::mbstate_t>> c16;
typedef Test_codecvt_base<std::codecvt<char32_t, char, std::mbstate_t>> c32;

#define REGISTER_CVT_UTF_CLASS_TEST(CVT, C, MAX, MODE) \
	static CppUnit::AutoRegisterSuite< Test_codecvt_base<std::CVT<C, MAX, static_cast<std::codecvt_mode>(MODE)>>>  \
	autoRegisterRegistry__ ## CVT ## _ ## C ## _ ## MAX ## _ ## MODE;


REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0xfful, 0);
#if 0
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0xfful, 1);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0xfful, 2);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0xfful, 3);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0xfful, 4);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0xfful, 5);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0xfful, 6);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0xfful, 7);

REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0xfffful, 0);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0xfffful, 1);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0xfffful, 2);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0xfffful, 3);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0xfffful, 4);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0xfffful, 5);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0xfffful, 6);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0xfffful, 7);

REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0x10fffful, 0);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0x10fffful, 1);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0x10fffful, 2);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0x10fffful, 3);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0x10fffful, 4);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0x10fffful, 5);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0x10fffful, 6);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char16_t, 0x10fffful, 7);

REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0xfful, 0);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0xfful, 1);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0xfful, 2);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0xfful, 3);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0xfful, 4);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0xfful, 5);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0xfful, 6);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0xfful, 7);

REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0xfffful, 0);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0xfffful, 1);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0xfffful, 2);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0xfffful, 3);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0xfffful, 4);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0xfffful, 5);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0xfffful, 6);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0xfffful, 7);

REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0x10fffful, 0);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0x10fffful, 1);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0x10fffful, 2);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0x10fffful, 3);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0x10fffful, 4);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0x10fffful, 5);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0x10fffful, 6);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0x10fffful, 7);

REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0x7ffffffful, 0);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0x7ffffffful, 1);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0x7ffffffful, 2);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0x7ffffffful, 3);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0x7ffffffful, 4);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0x7ffffffful, 5);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0x7ffffffful, 6);
REGISTER_CVT_UTF_CLASS_TEST(codecvt_utf8, char32_t, 0x7ffffffful, 7);
#endif

CPPUNIT_TEST_SUITE_REGISTRATION(c16);
CPPUNIT_TEST_SUITE_REGISTRATION(c32);

/*
CPPUNIT_TEST_SUITE_REGISTRATION(c_utf8_16_ff_0);
CPPUNIT_TEST_SUITE_REGISTRATION(c_utf8_16_10ffff_0);
CPPUNIT_TEST_SUITE_REGISTRATION(c_utf8_32_10ffff_0);

CPPUNIT_TEST_SUITE_REGISTRATION(c_utf16_16_10ffff_0);
CPPUNIT_TEST_SUITE_REGISTRATION(c_utf16_32_10ffff_0);

CPPUNIT_TEST_SUITE_REGISTRATION(c_utf8_utf16_16_10ffff_0);
CPPUNIT_TEST_SUITE_REGISTRATION(c_utf8_utf16_32_10ffff_0);
*/
