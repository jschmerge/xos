#include "time/timeutil.h"

#include <iostream>

#include "cppunit-header.h"
/*
	realtime = CLOCK_REALTIME,
	course_realtime = CLOCK_REALTIME_COARSE,
	monotonic = CLOCK_MONOTONIC,
	course_monotonic = CLOCK_MONOTONIC_COARSE,
	raw_monotonic = CLOCK_MONOTONIC_RAW,
	bootime = CLOCK_BOOTTIME,
	process_cputime = CLOCK_PROCESS_CPUTIME_ID,
	thread_cputime = CLOCK_THREAD_CPUTIME_ID,
*/

class Test_timeutil : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_timeutil);
	CPPUNIT_TEST(clocks);
	CPPUNIT_TEST_SUITE_END();

	void clocks()
	{
		auto d = posix_clock<clock_source::realtime>::resolution();
		std::cout << d.count() << std::endl;
		d = posix_clock<clock_source::course_realtime>::resolution();
		std::cout << d.count() << std::endl;
		d = posix_clock<clock_source::monotonic>::resolution();
		std::cout << d.count() << std::endl;
		d = posix_clock<clock_source::course_monotonic>::resolution();
		std::cout << d.count() << std::endl;
		d = posix_clock<clock_source::raw_monotonic>::resolution();
		std::cout << d.count() << std::endl;
		d = posix_clock<clock_source::boot_time>::resolution();
		std::cout << d.count() << std::endl;
		d = posix_clock<clock_source::process_cpu_time>::resolution();
		std::cout << d.count() << std::endl;
		d = posix_clock<clock_source::thread_cpu_time>::resolution();
		std::cout << d.count() << std::endl;
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_timeutil);
