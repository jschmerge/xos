#include "utility/spinlock.h"

#include <iostream>
#include <vector>
#include <thread>

#include "cppunit-header.h"

class Test_spinlock : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_spinlock);
	CPPUNIT_TEST(testLock);
	CPPUNIT_TEST(testConcurrency);
	CPPUNIT_TEST_SUITE_END();

 public:
	void setUp() {
		lock = new SpinLock;
		sharedValue = 0;
	}

	void tearDown() {
		delete lock;
		lock = nullptr;
	}

 protected:

	void testLock()
	{
		lock->lock();
		CPPUNIT_ASSERT(!lock->try_lock());
		lock->unlock();
	}

	void testConcurrency()
	{
		const size_t numThreads = 16384;

		sharedValue = 0;
		std::vector<std::thread> threads;
		std::set<size_t> v;

		try {

		for (size_t i = 0; i < numThreads; ++i)
		{
			threads.emplace_back(std::thread([&] (size_t x) {
				lock->lock();
				CPPUNIT_ASSERT(sharedValue == 0);
				sharedValue += x;
//				printf("%d\n", sharedValue);
				v.insert(sharedValue);
				sharedValue = 0;
				lock->unlock();
			}, i));
		}
		} catch (std::exception & e)
		{
			printf("Caught exception: %s\n", e.what());
		}

		for (auto & t : threads)
			t.join();

		CPPUNIT_ASSERT(v.size() == numThreads);
	}

	int sharedValue;
	SpinLock * lock;
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_spinlock);
