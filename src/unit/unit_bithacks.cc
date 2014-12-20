#include <cstdint>
#include <cstdio>
#include <limits>
#include <type_traits>
#include <iostream>
#include <stack>
#include <random>


#include "utility/bithacks.h"

#include "cppunit-header.h"

template <typename T>
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

class Test_bit_width : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_bit_width);
	CPPUNIT_TEST(test_width<char>);
	CPPUNIT_TEST(test_width<signed char>);
	CPPUNIT_TEST(test_width<unsigned char>);
	CPPUNIT_TEST(test_width<short>);
	CPPUNIT_TEST(test_width<unsigned short>);
	CPPUNIT_TEST(test_width<int>);
	CPPUNIT_TEST(test_width<unsigned int>);
	CPPUNIT_TEST(test_width<long>);
	CPPUNIT_TEST(test_width<unsigned long>);
	CPPUNIT_TEST(test_width<long long>);
	CPPUNIT_TEST(test_width<unsigned long long>);
	CPPUNIT_TEST(test_width<float>);
	CPPUNIT_TEST(test_width<double>);
	CPPUNIT_TEST(test_width<long double>);
	CPPUNIT_TEST_SUITE_END();
 protected:
	template <typename T>
	void test_width()
	{
		T val = 0;
		CPPUNIT_ASSERT(bit_width<T>() == (sizeof(T) * 8));
		CPPUNIT_ASSERT(bit_width(val) == (sizeof(T) * 8));
	}
};

class Test_pop_count : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_pop_count);
	CPPUNIT_TEST(single_bit<char>);
	CPPUNIT_TEST(single_bit<signed char>);
	CPPUNIT_TEST(single_bit<unsigned char>);
	CPPUNIT_TEST(single_bit<short>);
	CPPUNIT_TEST(single_bit<unsigned short>);
	CPPUNIT_TEST(single_bit<int>);
	CPPUNIT_TEST(single_bit<unsigned int>);
	CPPUNIT_TEST(single_bit<long>);
	CPPUNIT_TEST(single_bit<unsigned long>);
	CPPUNIT_TEST(single_bit<long long>);
	CPPUNIT_TEST(single_bit<unsigned long long>);
	CPPUNIT_TEST_SUITE_END();

 protected:
	template <typename T>
	  void single_bit()
	{
		const T one = 1; // used to avoid unwanted implict conversion

		for (size_t i = 0; i < bit_width<T>(); ++i)
		{
			T value = (one << i);
			size_t res = pop_count<T>(value);
			CPPUNIT_ASSERT(res == 1);
		}
	}
};

class Test_bitReverse : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_bitReverse);
	CPPUNIT_TEST(single_bit<uint8_t>);
	CPPUNIT_TEST(single_bit<uint16_t>);
	CPPUNIT_TEST(single_bit<uint32_t>);
	CPPUNIT_TEST(single_bit<uint64_t>);

	CPPUNIT_TEST(randomPattern<uint8_t>);
	CPPUNIT_TEST(randomPattern<uint16_t>);
	CPPUNIT_TEST(randomPattern<uint32_t>);
	CPPUNIT_TEST(randomPattern<uint64_t>);
	CPPUNIT_TEST_SUITE_END();

 protected:
	template<typename T> void single_bit()
	{
		T x = 1, y = 1;
		y <<= ((sizeof(x) * 8) - 1);

		for (size_t i = 0; i < bit_width<T>(); ++i)
		{
			CPPUNIT_ASSERT(bitReverse(x) == y);
			CPPUNIT_ASSERT(bitReverse(bitReverse(x)) == x);
			x <<= 1;
			y >>= 1;
		}
	}

	template<typename T> void randomPattern()
	{
		std::stack<bool> s;
		std::default_random_engine engine;
		std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(),
		                                      std::numeric_limits<T>::max());

		for (int num = 0; num < 256; ++num)
		{
			T x = dist(engine);
			const T one = 1;

			for (size_t i = 0; i < bit_width<T>(); ++i)
			{
				if (x & (one << i))
					s.push(true);
				else
					s.push(false);
			}

			T rx = bitReverse(x);

			CPPUNIT_ASSERT(x == bitReverse(rx));

			for (size_t i = 0; i < bit_width<T>(); ++i)
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

class Test_byteReverse : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_byteReverse);
	CPPUNIT_TEST(single_bit<uint8_t>);
	CPPUNIT_TEST(single_bit<uint16_t>);
	CPPUNIT_TEST(single_bit<uint32_t>);
	CPPUNIT_TEST(single_bit<uint64_t>);

	CPPUNIT_TEST(randomPattern<uint8_t>);
	CPPUNIT_TEST(randomPattern<uint16_t>);
	CPPUNIT_TEST(randomPattern<uint32_t>);
	CPPUNIT_TEST(randomPattern<uint64_t>);
	CPPUNIT_TEST_SUITE_END();

 protected:
	template <typename T>
	static T getByte (T v, size_t i)
	{
		const T mask = 0xFF;
//		std::cout << "-> " << i << ": "<< std::hex << v << ' ';
		T ret = (v & (mask << (bit_width<uint8_t>() * i)));
//		std::cout << std::hex << ret << ' ';
		ret >>= (bit_width<uint8_t>() * i);
//		std::cout << ret << std::endl;
		return ret;
	}

	template<typename T> void single_bit()
	{
		for (size_t i = 0; i < (bit_width<uint8_t>()); ++i)
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

	template<typename T> void randomPattern()
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
#if 0
				if (s.top() != getByte(rx, i))
				{
					std::cout << std::hex << static_cast<uint32_t>(s.top())
					          << " and " << std::hex
					          << static_cast<uint32_t>(getByte(rx, i))
					          << " do not match" << std::endl;
					std::cout << std::dec;
				}
#else
				CPPUNIT_ASSERT(s.top() == getByte(rx, i));
#endif
				s.pop();
			}
		}

	}

};

class Test_nlz : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_nlz);
	CPPUNIT_TEST(top_bit<char>);
	CPPUNIT_TEST(top_bit<signed char>);
	CPPUNIT_TEST(top_bit<unsigned char>);
	CPPUNIT_TEST(top_bit<short>);
	CPPUNIT_TEST(top_bit<unsigned short>);
	CPPUNIT_TEST(top_bit<int>);
	CPPUNIT_TEST(top_bit<unsigned int>);
	CPPUNIT_TEST(top_bit<long>);
	CPPUNIT_TEST(top_bit<unsigned long>);
	CPPUNIT_TEST(top_bit<long long>);
	CPPUNIT_TEST(top_bit<unsigned long long>);

	CPPUNIT_TEST(pentultimate_bit<char>);
	CPPUNIT_TEST(pentultimate_bit<signed char>);
	CPPUNIT_TEST(pentultimate_bit<unsigned char>);
	CPPUNIT_TEST(pentultimate_bit<short>);
	CPPUNIT_TEST(pentultimate_bit<unsigned short>);
	CPPUNIT_TEST(pentultimate_bit<int>);
	CPPUNIT_TEST(pentultimate_bit<unsigned int>);
	CPPUNIT_TEST(pentultimate_bit<long>);
	CPPUNIT_TEST(pentultimate_bit<unsigned long>);
	CPPUNIT_TEST(pentultimate_bit<long long>);
	CPPUNIT_TEST(pentultimate_bit<unsigned long long>);

	CPPUNIT_TEST(least_bit<char>);
	CPPUNIT_TEST(least_bit<signed char>);
	CPPUNIT_TEST(least_bit<unsigned char>);
	CPPUNIT_TEST(least_bit<short>);
	CPPUNIT_TEST(least_bit<unsigned short>);
	CPPUNIT_TEST(least_bit<int>);
	CPPUNIT_TEST(least_bit<unsigned int>);
	CPPUNIT_TEST(least_bit<long>);
	CPPUNIT_TEST(least_bit<unsigned long>);
	CPPUNIT_TEST(least_bit<long long>);
	CPPUNIT_TEST(least_bit<unsigned long long>);
	CPPUNIT_TEST_SUITE_END();

 protected:
	template<typename T>
	void top_bit()
	{
		size_t type_width = bit_width<T>();

		T value = 1;
		value <<= (type_width - 1);

		CPPUNIT_ASSERT(nlz(value) == 0);
	}

	template<typename T>
	void pentultimate_bit()
	{
		size_t type_width = bit_width<T>();

		T value = 1;
		value <<= (type_width - 2);

		CPPUNIT_ASSERT(nlz(value) == 1);
	}

	template<typename T>
	void least_bit()
	{
		size_t type_width = bit_width<T>();
		T value = 1;
		CPPUNIT_ASSERT(nlz(value) == (type_width - 1));
	}
};

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
	template<typename T>
	void nlzTest()
	{
		T value = 1;

		for (size_t i = 0; i < bit_width<T>(); ++i)
		{
			size_t correctValue = bit_width<T>() - i - 1;

			CPPUNIT_ASSERT(nlz<T>(value << i) == correctValue);
		}
	}

	template<typename T>
	void numLeadingZerosTest()
	{
		T value = 1;

		for (size_t i = 0; i < bit_width<T>(); ++i)
		{
			size_t correctValue = bit_width<T>() - i - 1;

			CPPUNIT_ASSERT(numLeadingZeros<T>(value << i) == correctValue);
		}
	}

	template<typename T>
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

CPPUNIT_TEST_SUITE_REGISTRATION(Test_bit_width);
CPPUNIT_TEST_SUITE_REGISTRATION(Test_pop_count);
CPPUNIT_TEST_SUITE_REGISTRATION(Test_bitReverse);
CPPUNIT_TEST_SUITE_REGISTRATION(Test_byteReverse);
CPPUNIT_TEST_SUITE_REGISTRATION(Test_numLeadingZeros);
CPPUNIT_TEST_SUITE_REGISTRATION(Test_nlz);

