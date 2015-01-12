#include "utility/crc.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <cstdio>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stack>
#include <random>

#include "cppunit-header.h"


template<class T, T Polynomial>
class Test_Crc : public CppUnit::TestFixture
{
	using calc_type = Crc<T, Polynomial>;

	using crc_type = T;

	CPPUNIT_TEST_SUITE(Test_Crc);
	CPPUNIT_TEST(testThreadedCreation);
	CPPUNIT_TEST(crcCalculation);
	CPPUNIT_TEST(crcFile<uint8_t>);
	CPPUNIT_TEST(crcFile<uint16_t>);
	CPPUNIT_TEST(crcFile<uint32_t>);
	CPPUNIT_TEST(crcFile<uint64_t>);
	CPPUNIT_TEST_SUITE_END();

	static const std::string datum1;
	static const std::string datum2;
	static const T datum1crc;
	static const T datum2crc;
	static const T fileCrc;

 protected:
	int randomData;

 public:
	Test_Crc() : randomData(-1) { }

	~Test_Crc()
	{
		close(randomData);
		unlink("data");
	}

	void setUp()
	{
		const size_t filesize = (1 << 24);
		uint64_t x;
		struct stat st;
		int rc, size = 0;;

		// force deterministic sequence
		std::mt19937_64 generator(0);
		std::uniform_int_distribution<uint64_t> dist(
			std::numeric_limits<uint64_t>::min(),
			std::numeric_limits<uint64_t>::max());

		if ( stat("data", &st) != 0 )
		{
			if ((randomData = open("data", O_CREAT|O_RDWR| O_TRUNC, 0644)) < 0)
				throw std::runtime_error("Fail");

			for (size_t i = 0; i < filesize/sizeof(x); ++i)
			{
				x = dist(generator);
				rc = write(randomData, reinterpret_cast<char*>(&x), sizeof(x));

				if (rc != sizeof(x))
					throw std::runtime_error("Write failed");

				size+=rc;
			}

			// printf("data created with size %d\n", size);
		} else if ((randomData = open("data", O_RDWR )) < 0)
			throw std::runtime_error("Fail");

		lseek(randomData, 0, SEEK_SET);
	}

	void tearDown() {
		close(randomData);
//		unlink("data");
	}

	void testThreadedCreation()
	{
		std::vector<std::thread> threads;
		std::mutex m;
		std::condition_variable c;
		bool ready = false;
//		std::this_thread::sleep_for(std::chrono::seconds(1));
		for (int i = 0; i < 100; ++i)
		{
			threads.emplace_back([&] {
				std::unique_lock<std::mutex> ul(m);
				c.wait(ul, [&] { return ready; });
				Crc<T, Polynomial> c;
			});
		}

//		std::this_thread::sleep_for(std::chrono::seconds(1));

		m.lock();
		ready = true;
		m.unlock();
		c.notify_all();

		for (auto & i : threads) i.join();
	}

	void crcCalculation()
	{
		calc_type crc1;
		crc1(datum1.c_str(), datum1.size());
		CPPUNIT_ASSERT(crc1.get() == datum1crc);

		calc_type crc2;
		crc2(datum2.c_str(), datum2.size());
		CPPUNIT_ASSERT(crc2.get() == datum2crc);

		crc1(datum1.c_str(), datum1.size());
		CPPUNIT_ASSERT(crc2.get() == crc1.get());
	}

	template <class E>
	void crcFile()
	{
		calc_type myCrc;
		const size_t bufSz = 4096;
		int rc = 0;
		std::array<E, bufSz / sizeof(E)> buffer;

		char * ptr = reinterpret_cast<char*>(buffer.data());

		while ((rc = read(randomData, ptr, bufSz)) > 0)
			myCrc(buffer.data(), rc / sizeof(E));

		if (rc < 0) throw std::runtime_error("read failed");

//		printf("\nFILE CRC = %08X -> %08X\n", myCrc.get(), fileCrc);
		CPPUNIT_ASSERT(myCrc.get() == fileCrc);
	}
		
		

};

template<class T, T polynomial>
const std::string Test_Crc<T, polynomial>::datum1 {
	"The quick brown fox jumps over the lazy dog" };

template<class T, T polynomial>
const std::string Test_Crc<T, polynomial>::datum2 { datum1 + datum1 };


//
// CRC32 - specific
//
typedef Test_Crc<uint32_t, Polynomial::Crc32> Test_CRC32;

template<>
const typename Test_CRC32::crc_type Test_CRC32::datum1crc { 0x414fa339 };

template<>
const typename Test_CRC32::crc_type Test_CRC32::datum2crc { 0x53ec60a9 };

template<>
const typename Test_CRC32::crc_type Test_CRC32::fileCrc { 0x2bd8c0d2 };

//
// CRC32-C - specific
//
typedef Test_Crc<uint32_t, Polynomial::Crc32c> Test_CRC32C;

template<>
const typename Test_CRC32C::crc_type Test_CRC32C::datum1crc { 0x22620404 };

template<>
const typename Test_CRC32C::crc_type Test_CRC32C::datum2crc { 0x20fe73e5 };

template<>
const typename Test_CRC32C::crc_type Test_CRC32C::fileCrc { 0x876e9bbc };

CPPUNIT_TEST_SUITE_REGISTRATION(Test_CRC32);
CPPUNIT_TEST_SUITE_REGISTRATION(Test_CRC32C);
