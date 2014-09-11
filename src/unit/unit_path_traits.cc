#include "../filesystem/path_traits.h"

#include "cppunit-header.h"

#include <vector>
#include <deque>
#include <forward_list>
#include <list>
#include <array>

template<class T> using vec_iter   = typename std::vector<T>::iterator;
template<class T> using deq_iter   = typename std::deque<T>::iterator;
template<class T> using list_iter  = typename std::list<T>::iterator;
template<class T> using flist_iter = typename std::forward_list<T>::iterator;
template<class T> using earr_iter  = typename std::array<T, 0>::iterator;
template<class T> using sarr_iter  = typename std::array<T, 10>::iterator;

class Test_path_traits : public CppUnit::TestFixture
{
	class bogus_class { };

	CPPUNIT_TEST_SUITE(Test_path_traits);
	CPPUNIT_TEST(test_encodable_types);
	CPPUNIT_TEST(test_path_data_sources_good<char>);
	CPPUNIT_TEST(test_path_data_sources_good<wchar_t>);
	CPPUNIT_TEST(test_path_data_sources_good<char16_t>);
	CPPUNIT_TEST(test_path_data_sources_good<char32_t>);

	CPPUNIT_TEST(test_path_data_sources_bad<signed char>);
	CPPUNIT_TEST(test_path_data_sources_bad<short>);
	CPPUNIT_TEST(test_path_data_sources_bad<int>);
	CPPUNIT_TEST(test_path_data_sources_bad<long>);
	CPPUNIT_TEST(test_path_data_sources_bad<long long>);

	CPPUNIT_TEST(test_path_data_sources_bad<unsigned char>);
	CPPUNIT_TEST(test_path_data_sources_bad<unsigned short>);
	CPPUNIT_TEST(test_path_data_sources_bad<unsigned int>);
	CPPUNIT_TEST(test_path_data_sources_bad<unsigned long>);
	CPPUNIT_TEST(test_path_data_sources_bad<unsigned long long>);

	CPPUNIT_TEST(test_path_data_sources_bad<bogus_class>);
	CPPUNIT_TEST_SUITE_END();

 protected:

	// Start of tests
	void test_encodable_types()
	{
		using namespace filesystem::path_traits;
		CPPUNIT_ASSERT(is_path_char_t_encodable<char>::value);
		CPPUNIT_ASSERT(is_path_char_t_encodable<wchar_t>::value);
		CPPUNIT_ASSERT(is_path_char_t_encodable<char16_t>::value);
		CPPUNIT_ASSERT(is_path_char_t_encodable<char32_t>::value);
	}

	template <typename T>
	void test_path_data_sources_helper(bool flipResult)
	{
		using namespace filesystem::path_traits;

		CPPUNIT_ASSERT(! is_path_initializer<T>::value);

		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<               T *>::value);
		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<const          T *>::value);
		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<      volatile T *>::value);
		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<const volatile T *>::value);

		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<               T[]>::value);
		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<const          T[]>::value);
		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<      volatile T[]>::value);
		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<const volatile T[]>::value);

		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<vec_iter<T>>::value );
		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<deq_iter<T>>::value );
		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<list_iter<T>>::value );
		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<flist_iter<T>>::value );
		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<earr_iter<T>>::value );
		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<sarr_iter<T>>::value );

		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<vec_iter<volatile T>>::value );
		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<deq_iter<volatile T>>::value );
		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<list_iter<volatile T>>::value );
		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<flist_iter<volatile T>>::value );
		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<earr_iter<volatile T>>::value );
		CPPUNIT_ASSERT(
			flipResult ^ is_path_initializer<sarr_iter<volatile T>>::value );
	}

	template <typename T>
	void test_path_data_sources_good()
		{ test_path_data_sources_helper<T>(false); }

	template <typename T>
	void test_path_data_sources_bad()
		{ test_path_data_sources_helper<T>(true); }

};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_path_traits);
