#include "../codecvt/codecvt_specializations.h"
#include "../codecvt/utf_conversion_helpers.h"

#include <cstring>

#include "cppunit-header.h"
#include "multistring.h"

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template<class Facet>
class deletable_facet : public Facet
{
 public:
	template<class ...Args>
	deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}

	~deletable_facet() {}
};

// Primary declaration
template <typename> class Test_codecvt_base;

template <template<class, class...> class CVT,
          typename CHAR_T,
          typename... OTHER> 
class Test_codecvt_base<CVT<CHAR_T, OTHER...>> : public CppUnit::TestFixture
{
 public:
	typedef CVT<CHAR_T, OTHER...> cvt_t;

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

		static_assert(std::is_same<CHAR_T, typename cvt_t::intern_type>::value,
		              "codecvt<> intern_type is invalid");
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

		if (std::is_same<char16_t, CHAR_T>::value)
			CPPUNIT_ASSERT(cvt.max_length() == 4);
		else if (std::is_same<char32_t, CHAR_T>::value)
			CPPUNIT_ASSERT(cvt.max_length() == 6);
	}
};

template <typename> class Test_codecvtx;

template <template<class, class...> class CVT, typename CHAR_T, typename... OTHER> 
class Test_codecvtx<CVT<CHAR_T, OTHER...>>
  : public Test_codecvt_base<CVT<CHAR_T, OTHER...>>
{
 public:
	typedef Test_codecvt_base<CVT<CHAR_T, OTHER...>> parent;
	CPPUNIT_TEST_SUB_SUITE(Test_codecvtx, parent);
	CPPUNIT_TEST(foo);
	CPPUNIT_TEST_SUITE_END();

	void foo()
	{
	}
};

typedef std::codecvt<char16_t, char, std::mbstate_t> foo;
CPPUNIT_TEST_SUITE_REGISTRATION(Test_codecvtx<foo>);
