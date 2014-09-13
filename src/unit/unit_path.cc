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

template <class C> struct string_constant;

template <> struct string_constant<char>
{
	static constexpr char array_value[] = "/foo/bar";
	static constexpr char array_value2[] = "/\u5916\u56FD/\u8A9E\u306E/"
	                                       "\u5B66\u7FD2\u3068/\u6559\u6388";
};

template <> struct string_constant<wchar_t>
{
	static constexpr wchar_t array_value[] = L"/foo/bar";
	static constexpr wchar_t array_value2[] = L"/\u5916\u56FD/\u8A9E\u306E/"
	                                           "\u5B66\u7FD2\u3068/"
	                                           "\u6559\u6388";
};

template <> struct string_constant<char16_t>
{
	static constexpr char16_t array_value[] = u"/foo/bar";
	static constexpr char16_t array_value2[] = u"/\u5916\u56FD/\u8A9E\u306E/"
	                                            "\u5B66\u7FD2\u3068/"
	                                            "\u6559\u6388";
};

template <> struct string_constant<char32_t>
{
	static constexpr char32_t array_value[] = U"/foo/bar";
	static constexpr char32_t array_value2[] = U"/\u5916\u56FD/\u8A9E\u306E/"
	                                            "\u5B66\u7FD2\u3068/"
	                                            "\u6559\u6388";
};

constexpr char string_constant<char>::array_value[];
constexpr wchar_t string_constant<wchar_t>::array_value[];
constexpr char16_t string_constant<char16_t>::array_value[];
constexpr char32_t string_constant<char32_t>::array_value[];

constexpr char string_constant<char>::array_value2[];
constexpr wchar_t string_constant<wchar_t>::array_value2[];
constexpr char16_t string_constant<char16_t>::array_value2[];
constexpr char32_t string_constant<char32_t>::array_value2[];


class Test_Path : public CppUnit::TestFixture
{
	typedef filesystem::path path;

	CPPUNIT_TEST_SUITE(Test_Path);
	CPPUNIT_TEST(constructors<char>);
	CPPUNIT_TEST(constructors<wchar_t>);
	CPPUNIT_TEST(constructors<char16_t>);
	CPPUNIT_TEST(constructors<char32_t>);
	CPPUNIT_TEST(valueConstructors);
	CPPUNIT_TEST(assignmentOperators);
	CPPUNIT_TEST(conversionOperators);
	CPPUNIT_TEST(slashEqualOperator);
	CPPUNIT_TEST(plusEqualOperators);
	CPPUNIT_TEST(modifierFunctions);
	CPPUNIT_TEST(compareFunctions);
	CPPUNIT_TEST(interegatorFunctions);
	CPPUNIT_TEST_SUITE_END();

 protected:
	template <class ECT>
	void constructors()
	{
		// TODO - augment string classes with non-ascii chars and
		// native and narrow versions of the strings

		// path::path()
		path path_default;
		CPPUNIT_ASSERT(path_default.empty());
		CPPUNIT_ASSERT(path_default.native() == "");
		CPPUNIT_ASSERT(*path_default.c_str() == '\0');

		// path::path<const ECT[]>(const ECT [] &)
		{
			if (config::verbose)
			{
				printf("\n---------------------------------------------\n"
				       "Constructing path with '%s'\n",
				       string_constant<char>::array_value);
			}

			path path_source_array( string_constant<ECT>::array_value);

			if (config::verbose)
			{
				printf("                Path = '%s'\n",
				       path_source_array.c_str());
			}
			CPPUNIT_ASSERT(! path_source_array.empty());
			CPPUNIT_ASSERT( strcmp(path_source_array.c_str(),
			                       string_constant<char>::array_value) == 0);
		}

		// path::path<const ECT*>(const ECT * &)
		{
			const ECT * value = string_constant<ECT>::array_value;
			path path_source_pointer(value);
			CPPUNIT_ASSERT(! path_source_pointer.empty());
			CPPUNIT_ASSERT( strcmp(path_source_pointer.c_str(),
			                       string_constant<char>::array_value) == 0);
		}

		// path::path<basic_string<ECT>>(const basic_string<ECT> &)
		{
			const std::basic_string<ECT> value(
				string_constant<ECT>::array_value);

			path path_source_string(value);
			CPPUNIT_ASSERT(! path_source_string.empty());
			CPPUNIT_ASSERT( strcmp(path_source_string.c_str(),
			                       string_constant<char>::array_value) == 0);
		}

		//////////

		// path::path<const ECT[]>(const ECT [] &)
		{
			if (config::verbose)
			{
				std::cout << "-----------------------------------------------\n"
				          << "Constructing path with '"
				          << string_constant<char>::array_value2
				          << "'" << std::endl;
			}
			path path_source_array( string_constant<ECT>::array_value2);
			if (config::verbose)
			{
				std::cout << "                Path = '"
				          << path_source_array.c_str() << "'" << std::endl;
			}

			CPPUNIT_ASSERT(! path_source_array.empty());
			CPPUNIT_ASSERT( strcmp(path_source_array.c_str(),
			                       string_constant<char>::array_value2) == 0);
		}

		// path::path<const ECT*>(const ECT * &)
		{
			const ECT * value = string_constant<ECT>::array_value2;
			path path_source_pointer(value);
			CPPUNIT_ASSERT(! path_source_pointer.empty());
			CPPUNIT_ASSERT( strcmp(path_source_pointer.c_str(),
			                       string_constant<char>::array_value2) == 0);
		}

		// path::path<basic_string<ECT>>(const basic_string<ECT> &)
		{
			const std::basic_string<ECT> value(
				string_constant<ECT>::array_value2);

			path path_source_string(value);
			CPPUNIT_ASSERT(! path_source_string.empty());
			CPPUNIT_ASSERT( strcmp(path_source_string.c_str(),
			                       string_constant<char>::array_value2) == 0);
		}

		const unsigned int bad_str[] = { 0x80000000, 0x80 };
		CPPUNIT_ASSERT_THROW({
			path bad{reinterpret_cast<const char32_t*>(bad_str)};
		}, filesystem::filesystem_error);
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

		{
			path p1("/foo/bar");
			path p2(std::move(p1));

			CPPUNIT_ASSERT(strcmp("/foo/bar", p2.c_str()) == 0);
			CPPUNIT_ASSERT(p1.empty());
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

			if (config::verbose)
			{
				printf("'%s' + '%s' = '%s' (expected '%s')\n",
				       i.operand1.c_str(), i.operand2.c_str(),
				       p1.native().c_str(), i.result.c_str());
			}

			CPPUNIT_ASSERT(p1.native() == i.result);
		}
	}

	void assignmentOperators()
	{
		path p1("/foo");
		path p2("/bar");

		p1 = p2;

		CPPUNIT_ASSERT(p1.compare(p2) == 0);

		path p3;
		p3 = std::move(p2);

		CPPUNIT_ASSERT(p2.empty() && (p3.compare(p1) == 0));
	}

	void plusEqualOperators()
	{
		const std::string s1 = "/foo/bar";
		const std::string s2 = "/another/path";
		const path::value_type ch = 'x';
		
		// path
		{
			path tmp(s1);
			tmp += path(s2);
			CPPUNIT_ASSERT( static_cast<std::string>(tmp) == ( s1 + s2) );
		}

		// string_type
		{
			path tmp(s1);
			tmp += s2;
			CPPUNIT_ASSERT( static_cast<std::string>(tmp) == ( s1 + s2) );
		}

		// value_type *
		{
			path tmp(s1);
			tmp += s2.c_str();
			CPPUNIT_ASSERT( static_cast<std::string>(tmp) == ( s1 + s2) );
		}

		// value_type
		{
			path tmp(s1);
			tmp += ch;
			CPPUNIT_ASSERT( static_cast<std::string>(tmp) == ( s1 + ch ) );
		}
	}

	void conversionOperators()
	{
		std::string s("/foo/bar");
		path p(s);

		path::string_type s2 = p;

		CPPUNIT_ASSERT(s2 == s);
	}

	void modifierFunctions()
	{
		path p("/foo/bar");

		path p2(p);
		CPPUNIT_ASSERT(p2.make_preferred().compare(p) == 0);

		p2.clear();
		CPPUNIT_ASSERT(p2.empty());

		p2 = "/something/different";
		p.swap(p2);

		CPPUNIT_ASSERT(p.compare("/something/different") == 0
		              && p2.compare("/foo/bar") == 0);
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

	void interegatorFunctions()
	{
		path p1("foo/bar");
		path p2("/foo/bar");

		CPPUNIT_ASSERT(p1.is_relative() && !p1.is_absolute());
		CPPUNIT_ASSERT(!p2.is_relative() && p2.is_absolute());
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_Path);
