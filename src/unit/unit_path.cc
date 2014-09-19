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
	operands_and_result(const T & _o1, const T & _result)
	  : operand1(_o1), operand2(), result(_result) { }

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
	CPPUNIT_TEST(appendFunctions);
	CPPUNIT_TEST(plusEqualOperators);
	CPPUNIT_TEST(modifierFunctions);
	CPPUNIT_TEST(compareFunctions);
	CPPUNIT_TEST(interegatorFunctions);
	CPPUNIT_TEST(extension);
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
		const char32_t * bad_str_ptr
		  = reinterpret_cast<const char32_t*>(bad_str);

		CPPUNIT_ASSERT_THROW({
			path bad{bad_str_ptr};
		}, filesystem::filesystem_error);

		std::basic_string<char32_t> bad_std_str(bad_str_ptr);
		CPPUNIT_ASSERT_THROW({
			path bad{bad_std_str};
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

	void appendFunctions()
	{
		std::vector<operands_and_result<const char *>> path_set {
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
			path p1(std::string(i.operand1));
			path p2(i.operand2);

			p1 /= p2;

			if (config::verbose)
			{
				printf("'%s' + '%s' = '%s' (expected '%s')\n",
				       i.operand1, i.operand2,
				       p1.native().c_str(), i.result);
			}

			CPPUNIT_ASSERT(p1.native() == i.result);

			p1 = i.operand1;
			p1 /= i.operand2;
			CPPUNIT_ASSERT(p1.native() == i.result);

			p1 = i.operand1;
			p1.append(i.operand2);
			CPPUNIT_ASSERT(p1.native() == i.result);
		}

/*
		for (const auto & i : path_set)
		{
			const int limit_len = 30;
			char s1[limit_len]; strncpy(s1, i.operand1, limit_len);
			char s2[limit_len]; strncpy(s2, i.operand2, limit_len);
			char s3[limit_len]; strncpy(s3, i.result, limit_len);

		}
*/
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

		if (config::verbose) putchar('\n');
		const std::vector<operands_and_result<std::string>> paths = {
			{ "", "" },
			{ "/", "" },
			{ "/foo", "/" },
			{ "/foo/", "/foo" },
			{ "/foo/bar", "/foo" },
			{ "/foo/bar/", "/foo/bar" },

			{ "///", "" },
			{ "///foo", "/" },
			{ "///foo///", "///foo" },
			{ "///foo///bar", "///foo" },
			{ "///foo///bar///", "///foo///bar" },
		};

		for (auto & i : paths)
		{
			path tmp(i.operand1);
			tmp.remove_filename();
			CPPUNIT_ASSERT(tmp.compare(i.result) == 0);
		}
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

	void extension()
	{
		path p1("foo/bar.txt");
		path p2("bar.txt/foo");
		path p3("..");
		path p4(".");
		path p5("foo/bar.txt.txt");
		path p6("foo/bar..txt");

		CPPUNIT_ASSERT(p1.extension().string() == ".txt");
		CPPUNIT_ASSERT(p2.extension().empty());
		CPPUNIT_ASSERT(p3.extension().empty());
		CPPUNIT_ASSERT(p4.extension().empty());
		CPPUNIT_ASSERT(p5.extension().string() == ".txt");
		CPPUNIT_ASSERT(p6.extension().string() == ".txt");
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_Path);
