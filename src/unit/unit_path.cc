#include "../filesystem/filesystem"

#include "cppunit-header.h"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <tuple>

template <typename T>
struct operands_and_result
{
	operands_and_result(const T & _o1, const T & _o2, const T & _result)
	  : operand1(_o1), operand2(_o2), result(_result) { }

	const T operand1;
	const T operand2;
	const T result;
};

class Test_Path : public CppUnit::TestFixture
{
	typedef filesystem::path path;

	CPPUNIT_TEST_SUITE(Test_Path);
	CPPUNIT_TEST(defaultConstructor);
	CPPUNIT_TEST(valueConstructors);
	CPPUNIT_TEST(assignmentOperators);
	CPPUNIT_TEST(slashEqualOperator);
	CPPUNIT_TEST(plusEqualOperators);
	CPPUNIT_TEST(compareFunctions);
	CPPUNIT_TEST_SUITE_END();

 protected:
	void defaultConstructor()
	{
		path p;
		CPPUNIT_ASSERT(p.empty());
		CPPUNIT_ASSERT(p.native() == "");
		CPPUNIT_ASSERT(*p.c_str() == '\0');
		std::string s = p;

		CPPUNIT_ASSERT(s.empty());
		CPPUNIT_ASSERT(s == "");
		CPPUNIT_ASSERT(*s.c_str() == '\0');
	}

	void valueConstructor()
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
