#include "../filesystem/filesystem"

#include "cppunit-header.h"

#include <cstdio>
#include <cstring>
#include <iostream>

template <typename T>
struct operands_and_result
{
	operands_and_result(const T & _o1, const T & _o2, const T & _result)
	  : operand1(_o1), operand2(_o2), result(_result) { }

	const T operand1;
	const T operand2;
	const T result;
};

template <class C>
struct string_constant;

template <> struct string_constant<char>
{ static constexpr char array_value[] = "/foo/bar"; };

#if 0
template <> struct string_constant<wchar_t>
{ static constexpr wchar_t array_value[] = L"/foo/bar"; };

template <> struct string_constant<char16_t>
{ static constexpr char16_t array_value[] = u"/foo/bar"; };

template <> struct string_constant<char32_t>
{ static constexpr char32_t array_value[] = U"/foo/bar"; };
#endif

class Test_Path : public CppUnit::TestFixture
{
	typedef filesystem::path path;

	CPPUNIT_TEST_SUITE(Test_Path);
	CPPUNIT_TEST(constructors<char>);
#if 0
	CPPUNIT_TEST(constructors<wchar_t>);
	CPPUNIT_TEST(constructors<char16_t>);
	CPPUNIT_TEST(constructors<char32_t>);
#endif
	CPPUNIT_TEST(assignmentOperators);
	CPPUNIT_TEST(slashEqualOperator);
	CPPUNIT_TEST(plusEqualOperators);
	CPPUNIT_TEST(compareFunctions);
	CPPUNIT_TEST_SUITE_END();

 protected:
	template <class ECT>
	void constructors()
	{
		// path::path()
		path path_default;
		CPPUNIT_ASSERT(path_default.empty());
		CPPUNIT_ASSERT(path_default.native() == "");
		CPPUNIT_ASSERT(*path_default.c_str() == '\0');

		// path::path<const ECT[]>(const ECT [] &)
		{
			path path_source_array( string_constant<ECT>::array_value);
			CPPUNIT_ASSERT(! path_source_array.empty());
		}

		// path::path<const ECT*>(const ECT * &)
		{
			const ECT * value = string_constant<ECT>::array_value;
			path path_source_pointer(value);
			CPPUNIT_ASSERT(! path_source_pointer.empty());
		}

		// path::path<basic_string<ECT>>(const basic_string<ECT> &)
		{
			const std::basic_string<ECT> value(
				string_constant<ECT>::array_value);

			path path_source_string(value);
			CPPUNIT_ASSERT(! path_source_string.empty());
		}

	}

	void valueConstructors()
	{
		{
			const char value[] = "/foo/bar";
			path p1(value);
			CPPUNIT_ASSERT(! p1.empty());
			CPPUNIT_ASSERT(p1.native() == value);
			CPPUNIT_ASSERT(strcmp(p1.c_str(), value) == 0);

			std::string val2(value);
			path p2(val2);
			CPPUNIT_ASSERT(! p2.empty());
			CPPUNIT_ASSERT(p2.native() == value);
			CPPUNIT_ASSERT(strcmp(p2.c_str(), value) == 0);
		}

		{
			const char * value = "/foo/bar";
			path p1(value);
			CPPUNIT_ASSERT(! p1.empty());
			CPPUNIT_ASSERT(p1.native() == value);
			CPPUNIT_ASSERT(strcmp(p1.c_str(), value) == 0);

			std::string val2(value);
			path p2(val2);
			CPPUNIT_ASSERT(! p2.empty());
			CPPUNIT_ASSERT(p2.native() == value);
			CPPUNIT_ASSERT(strcmp(p2.c_str(), value) == 0);
		}
	}

	void slashEqualOperator()
	{
		std::vector<operands_and_result<std::string>> path_set {
			{ ""      , ""      , "" },
			{ ""      , "/"     , "/" },
			{ ""      , "/bar"  , "/bar" },
			{ ""      , "bar"   , "bar" },
			{ ""      , "/bar/" , "/bar/" },
			{ ""      , "bar/"  , "bar/" },

			{ "/"     , ""      , "/" },
			{ "/"     , "/"     , "/" },
			{ "/"     , "/bar"  , "/bar" },
			{ "/"     , "bar"   , "/bar" },
			{ "/"     , "/bar/" , "/bar/" },
			{ "/"     , "bar/"  , "/bar/" },

			{ "/foo"  , ""      , "/foo" },
			{ "/foo"  , "/"     , "/foo/" },
			{ "/foo"  , "/bar"  , "/foo/bar" },
			{ "/foo"  , "bar"   , "/foo/bar" },
			{ "/foo"  , "/bar/" , "/foo/bar/" },
			{ "/foo"  , "bar/"  , "/foo/bar/" },

			{ "foo"   , ""      , "foo" },
			{ "foo"   , "/"     , "foo/" },
			{ "foo"   , "/bar"  , "foo/bar" },
			{ "foo"   , "bar"   , "foo/bar" },
			{ "foo"   , "/bar/" , "foo/bar/" },
			{ "foo"   , "bar/"  , "foo/bar/" },

			{ "/foo/" , ""      , "/foo/" },
			{ "/foo/" , "/"     , "/foo/" },
			{ "/foo/" , "/bar"  , "/foo/bar" },
			{ "/foo/" , "bar"   , "/foo/bar" },
			{ "/foo/" , "/bar/" , "/foo/bar/" },
			{ "/foo/" , "bar/"  , "/foo/bar/" },

			{ "foo/"  , ""      , "foo/" },
			{ "foo/"  , "/"     , "foo/" },
			{ "foo/"  , "/bar"  , "foo/bar" },
			{ "foo/"  , "bar"   , "foo/bar" },
			{ "foo/"  , "/bar/" , "foo/bar/" },
			{ "foo/"  , "bar/"  , "foo/bar/" },
		};

		for (const auto & i : path_set)
		{
			path p1(i.operand1);
			path p2(i.operand2);

			p1 /= p2;

//			printf("'%s' + '%s' = '%s' (expected '%s')\n", i.operand1.c_str(),
//			       i.operand2.c_str(), p1.native().c_str(), i.result.c_str());

			CPPUNIT_ASSERT(p1.native() == i.result);

		}
	}

	void assignmentOperators()
	{
		path p1("/foo");
		path p2("/bar");

		p1 = p2;

		CPPUNIT_ASSERT(p1.compare(p2) == 0);
	}

	void plusEqualOperators()
	{
		// path
		// string_type
		// value_type *
		// value_type
	}

	void compareFunctions()
	{
		CPPUNIT_ASSERT(path("a").compare(path("a")) == 0);
		CPPUNIT_ASSERT(path("a").compare(path("b")) < 0);
		CPPUNIT_ASSERT(path("b").compare(path("a")) > 0);

		CPPUNIT_ASSERT(path("a").compare(std::string("a")) == 0);
		CPPUNIT_ASSERT(path("a").compare(std::string("b")) < 0);
		CPPUNIT_ASSERT(path("b").compare(std::string("a")) > 0);

		CPPUNIT_ASSERT(path("a").compare("a") == 0);
		CPPUNIT_ASSERT(path("a").compare("b") < 0);
		CPPUNIT_ASSERT(path("b").compare("a") > 0);
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_Path);
