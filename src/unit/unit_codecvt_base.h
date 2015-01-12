#ifndef GUARD_UNIT_CODECVT_BASE_H
#define GUARD_UNIT_CODECVT_BASE_H 1

#include "codecvt/codecvt"

#include <cstring>
#include <locale>

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
	typedef typename CVT::intern_type intern_char_type;
	typedef typename CVT::extern_type extern_char_type;
	typedef std::codecvt<intern_char_type,
	                     extern_char_type,
	                     std::mbstate_t> basecvt_t;

 public:
	virtual ~Test_codecvt_base()
	{ }

	void construction()
	{
		deletable_facet<cvt_t> cvt;
		deletable_facet<cvt_t> cvt2(1);
		deletable_facet<cvt_t> cvt3(2);

		std::unique_ptr<cvt_t> cvt_p(new deletable_facet<cvt_t>);
		std::unique_ptr<cvt_t> cvt_p2(new deletable_facet<cvt_t>(1));
		std::unique_ptr<cvt_t> cvt_p3(new deletable_facet<cvt_t>(2));

		std::locale(std::locale(), static_cast<basecvt_t *>(new cvt_t));

		static_assert(std::is_same<char, typename cvt_t::extern_type>::value,
		              "codecvt<> extern_type is invalid");
		CPPUNIT_ASSERT(cvt.encoding() == 0);

		Test_codecvt_base dummy;
		dummy.max_length();
		dummy.encode_decode_char_range();
		dummy.unshift();
		dummy.unshift_errors();
		std::unique_ptr<TestFixture> p_dummy(new Test_codecvt_base);
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

	virtual void max_length() { CPPUNIT_ASSERT(true); }

	virtual void encode_decode_char_range() { CPPUNIT_ASSERT(true); }

	virtual void unshift() { CPPUNIT_ASSERT(true); }

	virtual void unshift_errors() { CPPUNIT_ASSERT(true); }
};

typedef Test_codecvt_base<std::codecvt<char16_t, char, std::mbstate_t>> c16;
typedef Test_codecvt_base<std::codecvt<char32_t, char, std::mbstate_t>> c32;

#define REGISTER_UTF_TEST(CVT_TEST, CHAR_T, MAX, MODE) \
	static CppUnit::AutoRegisterSuite< \
		CVT_TEST<CHAR_T, MAX, MODE>> \
	autoRegisterRegistry__ ## CVT_TEST ## _ ## CHAR_T ## _ ## MAX ## _ ## MODE;

#define EXTERN_DECLS(CVT, CHAR_T, MAX, MODE) \
	extern template class \
	deletable_facet<CVT<CHAR_T, MAX, cvt_mode_constants::cvt_ ## MODE>>; \
	extern template class \
	Test_codecvt_base<CVT<CHAR_T, MAX, cvt_mode_constants::cvt_ ## MODE>>;

#define INSTANTIATE_DECLS(CVT, CHAR_T, MAX, MODE) \
	template class \
	deletable_facet<CVT<CHAR_T, MAX, cvt_mode_constants::cvt_ ## MODE>>; \
	template class \
	Test_codecvt_base<CVT<CHAR_T, MAX, cvt_mode_constants::cvt_ ## MODE>>;

FOR_ALL_CVT_MODES(EXTERN_DECLS, std::codecvt_utf8, wchar_t, 0x7f);
FOR_ALL_CVT_MODES(EXTERN_DECLS, std::codecvt_utf8, wchar_t, 0xff);
FOR_ALL_CVT_MODES(EXTERN_DECLS, std::codecvt_utf8, wchar_t, 0xffff);
FOR_ALL_CVT_MODES(EXTERN_DECLS, std::codecvt_utf8, wchar_t, 0x10ffff);

FOR_ALL_CVT_MODES(EXTERN_DECLS, std::codecvt_utf8, char16_t, 0x7f);
FOR_ALL_CVT_MODES(EXTERN_DECLS, std::codecvt_utf8, char16_t, 0xff);
FOR_ALL_CVT_MODES(EXTERN_DECLS, std::codecvt_utf8, char16_t, 0xffff);
FOR_ALL_CVT_MODES(EXTERN_DECLS, std::codecvt_utf8, char16_t, 0x10ffff);

FOR_ALL_CVT_MODES(EXTERN_DECLS, std::codecvt_utf8, char32_t, 0x7f);
FOR_ALL_CVT_MODES(EXTERN_DECLS, std::codecvt_utf8, char32_t, 0xff);
FOR_ALL_CVT_MODES(EXTERN_DECLS, std::codecvt_utf8, char32_t, 0xffff);
FOR_ALL_CVT_MODES(EXTERN_DECLS, std::codecvt_utf8, char32_t, 0x10ffff);
FOR_ALL_CVT_MODES(EXTERN_DECLS, std::codecvt_utf8, char32_t, 0x3ffffff);
FOR_ALL_CVT_MODES(EXTERN_DECLS, std::codecvt_utf8, char32_t, 0x7fffffff);

#endif // GUARD_UNIT_CODECVT_BASE_H
