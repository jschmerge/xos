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

		if (std::is_same<char16_t, char_type>::value)
			CPPUNIT_ASSERT(cvt.max_length() <= 4);
		else if (std::is_same<char32_t, char_type>::value)
			CPPUNIT_ASSERT(cvt.max_length() <= 6);
	}
};

typedef Test_codecvt_base<std::codecvt<char16_t, char, std::mbstate_t>> a;
typedef Test_codecvt_base<std::codecvt<char32_t, char, std::mbstate_t>> b;
typedef Test_codecvt_base<std::codecvt_utf8<char16_t, 0xff, std::codecvt_mode(0)>> c;
typedef Test_codecvt_base<std::codecvt_utf8<char32_t>> d;

CPPUNIT_TEST_SUITE_REGISTRATION(a);
CPPUNIT_TEST_SUITE_REGISTRATION(b);
CPPUNIT_TEST_SUITE_REGISTRATION(c);
CPPUNIT_TEST_SUITE_REGISTRATION(d);
