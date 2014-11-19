#include "filesystem/file_status.h"
#include "filesystem/path.h"
#include "filesystem/directory_entry.h"

#include <iostream>

#include "cppunit-header.h"
#include "args-result.h"

namespace fs = filesystem::v1;
class Test_directory_entry : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_directory_entry);
	CPPUNIT_TEST(constructors);
	CPPUNIT_TEST(assign);
	CPPUNIT_TEST(cast);
	CPPUNIT_TEST(replace_filename);
	CPPUNIT_TEST(status);
	CPPUNIT_TEST(symlink_status);
	CPPUNIT_TEST(comparisons);
	CPPUNIT_TEST_SUITE_END();

 protected:
	void constructors()
	{
		fs::directory_entry e1;
		CPPUNIT_ASSERT(e1.path().empty());

		fs::directory_entry e2(fs::path("/tmp"));
		CPPUNIT_ASSERT(e2.path().string() == "/tmp");

		fs::directory_entry e3(e2);
		CPPUNIT_ASSERT(e2 == e3);
	}

	void assign()
	{
		const fs::path p("foo");
		fs::directory_entry e1(p);
		fs::directory_entry e2("bar");
		e2.assign(p);
		CPPUNIT_ASSERT(e1 == e2);
	}

	void cast()
	{
		fs::path p("foo");
		fs::directory_entry d(p);
		fs::path p2 = d;

		CPPUNIT_ASSERT(p == p2);
	}

	void replace_filename()
	{
		if (config::verbose) putchar('\n');

		for (const char * s : { "", "/", "/a", "a/", "/a/b", "/a/b/" })
		{
			fs::path p(s);
			fs::directory_entry e(p);

			p.replace_filename("foo");

			e.replace_filename("foo");

			CPPUNIT_ASSERT(e.path() == p);
		}
	}

	static fs::file_type regStatus(const char * s)
	{
		fs::directory_entry de{fs::path(s)};
		if (config::verbose) std::cout << s << std::endl;
		return de.status().type();
	}

	static fs::file_type ecStatus(const char * s)
	{
		std::error_code ec;
		fs::directory_entry de{fs::path(s)};
		auto rv = de.status(ec).type();
		CPPUNIT_ASSERT(!ec);
		return rv;
	};

	static fs::file_type symStat(const char * s)
	{
		fs::directory_entry de{fs::path(s)};
		if (config::verbose)
			std::cout << s << std::endl;
		return de.symlink_status().type();
	};

	static fs::file_type ecSymStat(const char * s)
	{
		std::error_code ec;
		fs::directory_entry de{fs::path(s)};
		auto rv = de.symlink_status(ec).type();
		CPPUNIT_ASSERT(!ec);
		return rv;
	};

	void status()
	{
		std::vector<args_and_result<fs::file_type, const char *>> names = {
			{ "/etc/passwd", fs::file_type::regular },
			{ "/tmp", fs::file_type::directory },
			{ "/proc/self", fs::file_type::directory },
			{ "/dev/sda", fs::file_type::block },
			{ "/dev/null", fs::file_type::character },
			{ "/dev/log", fs::file_type::socket },
		};

		if (config::verbose) putchar('\n');

		for (auto & e : names)
		{
			CPPUNIT_ASSERT(e.result == apply_function(regStatus, e.operands));
			CPPUNIT_ASSERT(e.result == apply_function(ecStatus, e.operands));
		}
	}

	void symlink_status()
	{
		std::vector<args_and_result<fs::file_type, const char *>> names = {
			{ "/etc/passwd", fs::file_type::regular },
			{ "/tmp", fs::file_type::directory },
			{ "/proc/self", fs::file_type::symlink },
			{ "/dev/sda", fs::file_type::block },
			{ "/dev/null", fs::file_type::character },
			{ "/dev/log", fs::file_type::socket },
		};

		if (config::verbose) putchar('\n');

		for (auto & e : names)
		{
			CPPUNIT_ASSERT(e.result == apply_function(symStat, e.operands));
			CPPUNIT_ASSERT(e.result == apply_function(ecSymStat, e.operands));
		}
	}

	static bool eq(const fs::directory_entry & a,
	               const fs::directory_entry & b) { return (a == b); };
	static bool ne(const fs::directory_entry & a,
	               const fs::directory_entry & b) { return (a != b); };
	static bool lt(const fs::directory_entry & a,
	               const fs::directory_entry & b) { return (a < b); };
	static bool ge(const fs::directory_entry & a,
	               const fs::directory_entry & b) { return (a >= b); };
	static bool gt(const fs::directory_entry & a,
	               const fs::directory_entry & b) { return (a > b); };
	static bool le(const fs::directory_entry & a,
	               const fs::directory_entry & b) { return (a <= b); };

	void comparisons()
	{
		typedef fs::directory_entry DE;

		std::vector<args_and_result<bool, const fs::directory_entry,
		                            const fs::directory_entry>> names = {
			{ DE("a"), DE("a"), true },
			{ DE("a"), DE("b"), false },
			{ DE("b"), DE("a"), false },
			{ DE("b"), DE("b"), true },
		};

		std::vector<args_and_result<bool, const fs::directory_entry,
		                            const fs::directory_entry>> lt_names = {
			{ DE("a"), DE("a"), false },
			{ DE("a"), DE("b"), true },
			{ DE("b"), DE("a"), false },
			{ DE("b"), DE("b"), false },
		};

		std::vector<args_and_result<bool, const fs::directory_entry,
		                            const fs::directory_entry>> gt_names = {
			{ DE("a"), DE("a"), false },
			{ DE("a"), DE("b"), false },
			{ DE("b"), DE("a"), true },
			{ DE("b"), DE("b"), false },
		};

		for (auto & e : names)
		{
			CPPUNIT_ASSERT(e.result == apply_function(eq, e.operands));
			CPPUNIT_ASSERT(e.result != apply_function(ne, e.operands));
		}

		for (auto & e : lt_names)
		{
			CPPUNIT_ASSERT(e.result == apply_function(lt, e.operands));
			CPPUNIT_ASSERT(e.result != apply_function(ge, e.operands));
		}

		for (auto & e : gt_names)
		{
			CPPUNIT_ASSERT(e.result == apply_function(gt, e.operands));
			CPPUNIT_ASSERT(e.result != apply_function(le, e.operands));
		}
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_directory_entry);
