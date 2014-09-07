#include <cstdint>
#include <cstdio>
#include <limits>
#include <type_traits>

#define USE_INLINE_ASM 1

#include "../utility/bithacks.h"

#include <iostream>
#include <stack>
#include <random>

#include "cppunit-header.h"

template <class T>
constexpr size_t  bitsIn()
{
	return std::numeric_limits<T>::digits;
}

template <class T>
void printBits(T x)
{
	for (int i = ((sizeof(x) * 8) - 1); i >= 0; --i)
	{
		if (x & (static_cast<T>(1) << i))
			putchar('1');
		else
			putchar('0');

		if ((i % 16) == 0)
			putchar(' ');
		else if ((i % 8) == 0)
			putchar('_');
	}

	putchar('\n');
}

class Test_bitReverse : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_bitReverse);
	CPPUNIT_TEST(singleBit<uint8_t>);
	CPPUNIT_TEST(singleBit<uint16_t>);
	CPPUNIT_TEST(singleBit<uint32_t>);
	CPPUNIT_TEST(singleBit<uint64_t>);

	CPPUNIT_TEST(randomPattern<uint8_t>);
	CPPUNIT_TEST(randomPattern<uint16_t>);
	CPPUNIT_TEST(randomPattern<uint32_t>);
	CPPUNIT_TEST(randomPattern<uint64_t>);
	CPPUNIT_TEST_SUITE_END();

 protected:
	template<class T> void singleBit()
	{
		T x = 1, y = 1;
		y <<= ((sizeof(x) * 8) - 1);

		for (size_t i = 0; i < bitsIn<T>(); ++i)
		{
			CPPUNIT_ASSERT(bitReverse(x) == y);
			CPPUNIT_ASSERT(bitReverse(bitReverse(x)) == x);
			x <<= 1;
			y >>= 1;
		}
	}

	template<class T> void randomPattern()
	{
		std::stack<bool> s;
		std::default_random_engine engine;
		std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(),
		                                      std::numeric_limits<T>::max());

		for (int num = 0; num < 256; ++num)
		{
			T x = dist(engine);
			const T one = 1;

			for (size_t i = 0; i < bitsIn<T>(); ++i)
			{
				if (x & (one << i))
					s.push(true);
				else
					s.push(false);
			}

			T rx = bitReverse(x);

			CPPUNIT_ASSERT(x == bitReverse(rx));

			for (size_t i = 0; i < bitsIn<T>(); ++i)
			{
				if (rx & (one << i))
					CPPUNIT_ASSERT(s.top());
				else
					CPPUNIT_ASSERT(! s.top());
				s.pop();
			}
		}
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_bitReverse);

class Test_byteReverse : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_byteReverse);
	CPPUNIT_TEST(singleBit<uint8_t>);
	CPPUNIT_TEST(singleBit<uint16_t>);
	CPPUNIT_TEST(singleBit<uint32_t>);
	CPPUNIT_TEST(singleBit<uint64_t>);

	CPPUNIT_TEST(randomPattern<uint8_t>);
	CPPUNIT_TEST(randomPattern<uint16_t>);
	CPPUNIT_TEST(randomPattern<uint32_t>);
	CPPUNIT_TEST(randomPattern<uint64_t>);
	CPPUNIT_TEST_SUITE_END();

 protected:
	template <class T>
	static T getByte (T v, size_t i)
	{
		const T mask = 0xFF;
//		std::cout << "-> " << i << ": "<< std::hex << v << ' ';
		T ret = (v & (mask << (bitsIn<uint8_t>() * i)));
//		std::cout << std::hex << ret << ' ';
		ret >>= (bitsIn<uint8_t>() * i);
//		std::cout << ret << std::endl;
		return ret;
	}

	template<class T> void singleBit()
	{
		for (size_t i = 0; i < (bitsIn<uint8_t>()); ++i)
		{
			T x = 1;
			x <<= i;

			for (size_t j = 0; j < sizeof(T); ++j)
			{
				T rx = byteReverse(x);

//				std::cout << '\n' << std::hex
//				          << static_cast<uint64_t>(rx) << ' '
//				          << static_cast<uint64_t>(x) << std::dec;

				CPPUNIT_ASSERT(getByte(x, j) == getByte(rx, sizeof(T) - 1 - j));

				if (sizeof(T) > 1)
					x <<= 8;
			}
		}
	}

	template<class T> void randomPattern()
	{
		std::stack<uint8_t> s;
		std::default_random_engine engine;
		std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(),
		                                      std::numeric_limits<T>::max());

		for (int num = 0; num < 256; ++num)
		{
			T x = dist(engine);

//			std::cout << "Testing with " << std::hex << x << std::endl;

			for (size_t i = 0; i < sizeof(T); ++i)
			{
				uint8_t b = getByte(x, i);
//				std::cout << i << ": " << std::hex << static_cast<uint32_t>(b)
//				          << std::endl;
				s.push(b);
			}

			T rx = byteReverse(x);

			CPPUNIT_ASSERT(x == byteReverse(rx));

			for (size_t i = 0; i < sizeof(T); ++i)
			{
				if (s.top() != getByte(rx, i))
				{
					std::cout << std::hex << static_cast<uint32_t>(s.top())
					          << " and " << std::hex
					          << static_cast<uint32_t>(getByte(rx, i))
					          << " do not match" << std::endl;
					std::cout << std::dec;
					CPPUNIT_ASSERT(false);
				}
				s.pop();
			}
		}

	}

};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_byteReverse);

class Test_numLeadingZeros : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_numLeadingZeros);
	CPPUNIT_TEST(numLeadingZerosTest<uint8_t>);
	CPPUNIT_TEST(numLeadingZerosTest<uint16_t>);
	CPPUNIT_TEST(numLeadingZerosTest<uint32_t>);
	CPPUNIT_TEST(numLeadingZerosTest<uint64_t>);

	CPPUNIT_TEST(nlzTest<uint8_t>);
	CPPUNIT_TEST(nlzTest<uint16_t>);
	CPPUNIT_TEST(nlzTest<uint32_t>);
	CPPUNIT_TEST(nlzTest<uint64_t>);

	CPPUNIT_TEST(testLog<uint8_t>);
	CPPUNIT_TEST(testLog<uint16_t>);
	CPPUNIT_TEST(testLog<uint32_t>);
	CPPUNIT_TEST(testLog<uint64_t>);
	CPPUNIT_TEST_SUITE_END();

 protected:
	template<class T>
	void nlzTest()
	{
		T value = 1;

		for (size_t i = 0; i < bitsIn<T>(); ++i)
		{
			size_t correctValue = bitsIn<T>() - i - 1;

			CPPUNIT_ASSERT(nlz<T>(value << i) == correctValue);
		}
	}

	template<class T>
	void numLeadingZerosTest()
	{
		T value = 1;

		for (size_t i = 0; i < bitsIn<T>(); ++i)
		{
			size_t correctValue = bitsIn<T>() - i - 1;

			CPPUNIT_ASSERT(numLeadingZeros<T>(value << i) == correctValue);
		}
	}

	template<class T>
	void testLog()
	{
		T value = 1;
		CPPUNIT_ASSERT( intLog2Floor(value) == 0);
		CPPUNIT_ASSERT( intLog2Ceiling(value) == 0);

		value = 2;
		CPPUNIT_ASSERT( intLog2Floor(value) == 1);
		CPPUNIT_ASSERT( intLog2Ceiling(value) == 1);

		value = 3;
		CPPUNIT_ASSERT( intLog2Floor(value) == 1);
		CPPUNIT_ASSERT( intLog2Ceiling(value) == 2);

		for (T i = 4; i != 0; i <<= 1)
		{
			T prev = i - 1;
			T next = i + 1;

			CPPUNIT_ASSERT( intLog2Ceiling(prev) == intLog2Floor(i) );
			CPPUNIT_ASSERT( intLog2Ceiling(i) == intLog2Floor(i) );
			CPPUNIT_ASSERT( intLog2Ceiling(i) == intLog2Floor(next) );
		}
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_numLeadingZeros);

