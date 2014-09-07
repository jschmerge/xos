#include "../utility/average.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <random>

#include "cppunit-header.h"

class Test_Average : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_Average);
	CPPUNIT_TEST(simpleSequence<float>);
	CPPUNIT_TEST(simpleSequence<double>);
	CPPUNIT_TEST(simpleSequence<long double>);
	CPPUNIT_TEST_SUITE_END();

 protected:
	template<class T> void simpleSequence()
	{
		std::mt19937 generator(0.0);
		std::uniform_real_distribution<T> dist(-1.0, 1.0);

		std::vector<T> values(1000000);
		std::generate(values.begin(), values.end(),
		              [&] () { return dist(generator); });

		Average<T> avg;
		T accum = 0;

		/// \todo - use Kahan Summation technique here
		for (auto i : values)
		{
			accum += i;
			avg(i);
		}

		accum /= values.size();
		if (std::abs(accum - avg.mean()) > std::max(accum, avg.mean()))
		{
			std::cout << "\n" << std::setprecision(9) << accum << " " 
			          << avg.mean() << std::endl
			          << "Epsilon = " << std::abs(accum - avg.mean())
			          << ", " << std::numeric_limits<T>::epsilon()
			          << std::endl;
			CPPUNIT_ASSERT(accum == avg.mean());
		}
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_Average);
