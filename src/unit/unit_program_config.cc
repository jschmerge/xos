#include "environment/program_config.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <random>

#include "cppunit-header.h"

class test_config : public program_config
{
 public:
	test_config()
	  : program_config({
		{ argument_type::none, "help", 'h', "Prints this message" },
		{ argument_type::required, "required", 'r', "Required argument" },
		{ argument_type::optional, "optional", 'o', "Optional argument" },
		{ argument_type::none, "longfakeout",'l', "Long opt without param" },
		{ argument_type::optional, "long-only", -1, "No short option" },
	    })
	{
	}

	bool process_option(const config_option & opt,
	                    const std::string & param) override
	{
		if (config::verbose)
			printf("Got option %s with param %s\n",
			       opt.option_synopsis().c_str(), param.c_str());
		return true;
	}

	bool process_option(const config_option & opt)
	{
		if (config::verbose)
			printf("Got option %s\n", opt.option_synopsis().c_str());
		return true;
	}

};

class Test_program_config : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_program_config);
	CPPUNIT_TEST(construction);
	CPPUNIT_TEST(short_options);
	CPPUNIT_TEST(long_options);
	CPPUNIT_TEST(bad_options);
	CPPUNIT_TEST_SUITE_END();

 protected:
	void construction()
	{
		test_config tc;
	}

	void short_options()
	{
		test_config tc;

		const char * command_line[3] = { "prog_name", "-h", "param" };
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(2, command_line));
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(3, command_line));

		command_line[1] = "-rfoo";
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(2, command_line));
		command_line[1] = "-r";
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(3, command_line));

		command_line[1] = "-o";
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(2, command_line));
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(3, command_line));

		command_line[1] = "-l";
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(2, command_line));
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(3, command_line));

	}

	void long_options()
	{
		test_config tc;

		const char * command_line[3] = { "prog_name", "--help", "param" };
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(2, command_line));
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(3, command_line));

		command_line[1] = "--required=foo";
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(2, command_line));
		command_line[1] = "--required";
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(3, command_line));

		command_line[1] = "--optional";
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(2, command_line));
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(3, command_line));

		command_line[1] = "--long-only";
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(2, command_line));
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(3, command_line));

		command_line[1] = "--long-";
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(2, command_line));
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(3, command_line));

		command_line[1] = "--longfakeout";
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(2, command_line));
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(3, command_line));

		command_line[1] = "--longf";
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(2, command_line));
		CPPUNIT_ASSERT_NO_THROW(tc.parse_command_line(3, command_line));
	}

	void bad_options()
	{
		test_config tc;

		const char * bad_opts[3] = { "prog_name", "-q", nullptr };

		CPPUNIT_ASSERT_THROW(tc.parse_command_line(2, bad_opts),
		                     std::runtime_error);

		bad_opts[1] = "--foo";
		CPPUNIT_ASSERT_THROW(tc.parse_command_line(2, bad_opts),
		                     std::runtime_error);

		bad_opts[1] = "--long";
		CPPUNIT_ASSERT_THROW(tc.parse_command_line(2, bad_opts),
		                     std::runtime_error);

		bad_opts[1] = "--required";
		CPPUNIT_ASSERT_THROW(tc.parse_command_line(2, bad_opts),
		                     std::runtime_error);
#if 0
		const char * bad_opts2[3] = { "prog_name", "--foo", nullptr };
		CPPUNIT_ASSERT_THROW(tc.parse_command_line(2, bad_opts2),
		                     std::runtime_error);

		const char * bad_opts3[3] = { "prog_name", "--long", nullptr };
		CPPUNIT_ASSERT_THROW(tc.parse_command_line(2, bad_opts3),
		                     std::runtime_error);
#endif
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_program_config);
