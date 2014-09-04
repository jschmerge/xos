#include "../filesystem/filesystem"

#include "cppunit-header.h"

#include <vector>
#include <deque>
#include <forward_list>
#include <list>
#include <array>

class Test_path_traits : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_path_traits);
	CPPUNIT_TEST(test_encodable_types);
	CPPUNIT_TEST(test_path_data_sources<char>);
	CPPUNIT_TEST(test_path_data_sources<wchar_t>);
	CPPUNIT_TEST(test_path_data_sources<char16_t>);
	CPPUNIT_TEST(test_path_data_sources<char32_t>);

	CPPUNIT_TEST(test_negative_path_data_sources<signed char>);
	CPPUNIT_TEST(test_negative_path_data_sources<unsigned char>);
	CPPUNIT_TEST(test_negative_path_data_sources<short>);
	CPPUNIT_TEST(test_negative_path_data_sources<int>);
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
	void test_path_data_sources()
	{
		using namespace filesystem::path_traits;

		CPPUNIT_ASSERT(! is_path_initializer<T>::value);

		CPPUNIT_ASSERT(is_path_initializer<               T *>::value);
		CPPUNIT_ASSERT(is_path_initializer<const          T *>::value);
		CPPUNIT_ASSERT(is_path_initializer<      volatile T *>::value);
		CPPUNIT_ASSERT(is_path_initializer<const volatile T *>::value);

		CPPUNIT_ASSERT(is_path_initializer<               T[]>::value);
		CPPUNIT_ASSERT(is_path_initializer<const          T[]>::value);
		CPPUNIT_ASSERT(is_path_initializer<      volatile T[]>::value);
		CPPUNIT_ASSERT(is_path_initializer<const volatile T[]>::value);
	}

	template <typename T>
	void test_negative_path_data_sources()
	{
		using namespace filesystem::path_traits;

		CPPUNIT_ASSERT(! is_path_initializer<               T *>::value);
		CPPUNIT_ASSERT(! is_path_initializer<const          T *>::value);
		CPPUNIT_ASSERT(! is_path_initializer<      volatile T *>::value);
		CPPUNIT_ASSERT(! is_path_initializer<const volatile T *>::value);

		CPPUNIT_ASSERT(! is_path_initializer<               T[]>::value);
		CPPUNIT_ASSERT(! is_path_initializer<const          T[]>::value);
		CPPUNIT_ASSERT(! is_path_initializer<      volatile T[]>::value);
		CPPUNIT_ASSERT(! is_path_initializer<const volatile T[]>::value);
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_path_traits);
