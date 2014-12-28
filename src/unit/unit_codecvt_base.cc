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

		printf("-------> Max encodable = %d\n", get_max_encodable());
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

	char32_t get_max_encodable() { return 0xefffffff; }

	virtual void max_length()
	{
		deletable_facet<cvt_t> cvt;

		if (std::is_same<char16_t, char_type>::value)
			CPPUNIT_ASSERT(cvt.max_length() <= 4);
		else if (std::is_same<char32_t, char_type>::value)
			CPPUNIT_ASSERT(cvt.max_length() <= 6);
	}
};

template<>
char32_t
Test_codecvt_base<std::codecvt<char16_t, char, std::mbstate_t>>
  ::get_max_encodable()
{
	return 0x10ffff;
}

typedef Test_codecvt_base<std::codecvt<char16_t, char, std::mbstate_t>> c16;
typedef Test_codecvt_base<std::codecvt<char32_t, char, std::mbstate_t>> c32;

typedef Test_codecvt_base<std::codecvt_utf8<char16_t>> c_utf8_16;
typedef Test_codecvt_base<std::codecvt_utf8<char32_t>> c_utf8_32;

typedef Test_codecvt_base<std::codecvt_utf16<char16_t>> c_utf16_16;
typedef Test_codecvt_base<std::codecvt_utf16<char32_t>> c_utf16_32;

typedef Test_codecvt_base<std::codecvt_utf8_utf16<char16_t>> c_utf8_utf16_16;
typedef Test_codecvt_base<std::codecvt_utf8_utf16<char32_t>> c_utf8_utf16_32;

CPPUNIT_TEST_SUITE_REGISTRATION(c16);
CPPUNIT_TEST_SUITE_REGISTRATION(c32);

CPPUNIT_TEST_SUITE_REGISTRATION(c_utf8_16);
CPPUNIT_TEST_SUITE_REGISTRATION(c_utf8_32);

CPPUNIT_TEST_SUITE_REGISTRATION(c_utf16_16);
CPPUNIT_TEST_SUITE_REGISTRATION(c_utf16_32);

CPPUNIT_TEST_SUITE_REGISTRATION(c_utf8_utf16_16);
CPPUNIT_TEST_SUITE_REGISTRATION(c_utf8_utf16_32);
