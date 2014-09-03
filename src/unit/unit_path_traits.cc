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


	CPPUNIT_TEST(test_path_container_data_sources<char>);
	CPPUNIT_TEST(test_path_container_data_sources<wchar_t>);
	CPPUNIT_TEST(test_path_container_data_sources<char16_t>);
	CPPUNIT_TEST(test_path_container_data_sources<char32_t>);

	CPPUNIT_TEST(test_path_container_data_sources<volatile char>);
	CPPUNIT_TEST(test_path_container_data_sources<volatile wchar_t>);
	CPPUNIT_TEST(test_path_container_data_sources<volatile char16_t>);
	CPPUNIT_TEST(test_path_container_data_sources<volatile char32_t>);


	CPPUNIT_TEST_SUITE_END();

 protected:
	// specialization for array
	template<template <typename, std::size_t> class C,
	         typename T, std::size_t N>
	void test_path_specific_container_data_sources()
	{
		using namespace filesystem::path_traits;

		typedef C<T, N> Container;

	  static_assert(is_path_initializer<Container>::value, "foo");

		CPPUNIT_ASSERT(
		  is_path_initializer<Container>::value);
		CPPUNIT_ASSERT(
		  is_path_initializer<Container>::value);
	}

	template<template <typename...> class C, typename T, typename ... Args>
	void test_path_specific_container_data_sources()
	{
		using namespace filesystem::path_traits;

		typedef C<T, Args...> Container;

		CPPUNIT_ASSERT(
		  is_path_initializer<Container>::value);
		CPPUNIT_ASSERT(
		  is_path_initializer<Container>::value);
	}

	// Start of tests
	void test_encodable_types()
	{
		using namespace filesystem::path_traits;
		CPPUNIT_ASSERT(is_path_char_t_encodable<char>::value);
		CPPUNIT_ASSERT(is_path_char_t_encodable<wchar_t>::value);
		CPPUNIT_ASSERT(is_path_char_t_encodable<char16_t>::value);
		CPPUNIT_ASSERT(is_path_char_t_encodable<char32_t>::value);
	}

	template<typename T>
	void test_path_container_data_sources()
	{
		test_path_specific_container_data_sources<std::basic_string, T>();

		test_path_specific_container_data_sources<std::basic_string, T,
		                                          std::char_traits<T>,
		                                          std::allocator<T>>();

		test_path_specific_container_data_sources<std::vector, T>();
		test_path_specific_container_data_sources<std::vector, T,
		                                          std::allocator<T>>();

		test_path_specific_container_data_sources<std::deque, T>();
		test_path_specific_container_data_sources<std::deque, T,
		                                          std::allocator<T>>();

		test_path_specific_container_data_sources<std::list, T>();
		test_path_specific_container_data_sources<std::list, T,
		                                          std::allocator<T>>();

		test_path_specific_container_data_sources<std::forward_list, T>();
		test_path_specific_container_data_sources<std::forward_list, T,
		                                          std::allocator<T>>();

		test_path_specific_container_data_sources<std::array, T, 0>();
		test_path_specific_container_data_sources<std::array, T, 1>();
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

		test_path_container_data_sources<T>();
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
