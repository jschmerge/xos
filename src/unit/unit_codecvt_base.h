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


	virtual void max_length() = 0;

	virtual void encode_decode_char_range() = 0;
};

typedef Test_codecvt_base<std::codecvt<char16_t, char, std::mbstate_t>> c16;
typedef Test_codecvt_base<std::codecvt<char32_t, char, std::mbstate_t>> c32;

#define REGISTER_CVT_UTF_CLASS(CVT, C, MAX, MODE) \
	static CppUnit::AutoRegisterSuite< \
		Test_codecvt_base< \
			std::CVT<C, MAX, static_cast<std::codecvt_mode>(MODE)>>>  \
	autoRegisterRegistry__ ## CVT ## _ ## C ## _ ## MAX ## _ ## MODE;

#define CVT_UTF_CLASS_MODES(CVT, C, MAX) \
	REGISTER_CVT_UTF_CLASS(CVT, C, MAX, 0) \
	REGISTER_CVT_UTF_CLASS(CVT, C, MAX, 1) \
	REGISTER_CVT_UTF_CLASS(CVT, C, MAX, 2) \
	REGISTER_CVT_UTF_CLASS(CVT, C, MAX, 3) \
	REGISTER_CVT_UTF_CLASS(CVT, C, MAX, 4) \
	REGISTER_CVT_UTF_CLASS(CVT, C, MAX, 5) \
	REGISTER_CVT_UTF_CLASS(CVT, C, MAX, 6) \
	REGISTER_CVT_UTF_CLASS(CVT, C, MAX, 7)

#define CVT_UTF_CLASS_TYPES(CVT, MAX) \
	CVT_UTF_CLASS_MODES(CVT, wchar_t, MAX) \
	CVT_UTF_CLASS_MODES(CVT, char16_t, MAX) \
	CVT_UTF_CLASS_MODES(CVT, char32_t, MAX)

#define CVT_UTF_CLASS_MAXES(CVT) \
	CVT_UTF_CLASS_TYPES(CVT, 0x7f) \
	CVT_UTF_CLASS_TYPES(CVT, 0xff) \
	CVT_UTF_CLASS_TYPES(CVT, 0xffff) \
	CVT_UTF_CLASS_TYPES(CVT, 0x10ffff)
