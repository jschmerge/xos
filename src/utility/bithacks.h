#ifndef GUARD_BITHACKS_H
#define GUARD_BITHACKS_H 1

#include <cstdio>
#include <cstdint>
#include <limits>
#include <type_traits>


/// pop_count gives the number of one-bits in an integral value
template<typename T> size_t pop_count(T value)
	{ return __builtin_popcount(static_cast<unsigned int>(value)); }

template <> inline size_t pop_count(uint32_t value)
	{ return __builtin_popcountl(value); }

template <> inline size_t pop_count(uint64_t value)
	{ return __builtin_popcountl(value); }

template <> inline size_t pop_count(unsigned long long value)
	{ return __builtin_popcountll(value); }

template<class T> size_t nlz(T value)
{
	for (unsigned i = 1; i < (sizeof(value) << 3); i <<= 1)
		value = value | (value >> i);

	return pop_count(static_cast<T>(~value));
}

template <class T>
constexpr size_t
numLeadingZeros(T value, size_t count = std::numeric_limits<T>::digits)
{
	return (value == 0) ? count : numLeadingZeros<T>(value >> 1, count - 1);
}

template <class T>
constexpr size_t intLog2Floor(T value)
{
	return std::numeric_limits<T>::digits - 1 - numLeadingZeros<T>(value);
}

template <class T>
constexpr size_t intLog2Ceiling(T value)
{
	return std::numeric_limits<T>::digits - numLeadingZeros<T>(value - 1);
}

template <class T>
T byteReverse(T value)
{
	// paranoia
	static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value,
	              "parameter must be an unsigned integral type");
	static_assert(std::numeric_limits<T>::radix == 2,
	              "parameter must have binary radix");

	int shift = std::numeric_limits<T>::digits; // number of bits
	T mask = ~ static_cast<T>(0);

	for (size_t i = 0;
	     i < intLog2Floor<T>(std::numeric_limits<T>::digits) - 3; ++i)
	{
		shift >>= 1;
		mask ^= (mask << shift);
		value = (((value >> shift) & mask) | ((value & mask) << shift));
	}

	return value;
}

template<>
inline int8_t byteReverse(int8_t value)
	{ return value; }

template<>
inline uint8_t byteReverse(uint8_t value)
	{ return value; }

#if defined(USE_COMPILER_BUILTINS) || defined(USE_INLINE_ASM)
template <class T>
T bitReverse(T value)
{
	// paranoia
	static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value,
	              "parameter must be an unsigned integral type");
	static_assert(std::numeric_limits<T>::radix == 2,
	              "parameter must have binary radix");

	int shift = std::numeric_limits<T>::digits; // number of bits
	T mask = ~static_cast<T>(0);

	for (size_t i = intLog2Floor<T>(std::numeric_limits<uint8_t>::digits);
	     i < intLog2Floor<T>(std::numeric_limits<T>::digits);
	     ++i)
	{
		shift >>= 1;
		mask ^= (mask << shift);
	}

	value = byteReverse<T>(value);

	for (size_t i = 0;
	     i < intLog2Floor<T>(std::numeric_limits<uint8_t>::digits);
	     ++i)
	{
		shift >>= 1;
		mask ^= (mask << shift);
		value = (((value >> shift) & mask) | ((value & mask) << shift));
	}

	return value;
}
#else
template <class T>
T bitReverse(T value)
{
	// paranoia
	static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value,
	              "parameter must be an unsigned integral type");
	static_assert(std::numeric_limits<T>::radix == 2,
	              "parameter must have binary radix");

	int shift = std::numeric_limits<T>::digits; // number of bits
	T mask = ~static_cast<T>(0);

	for (size_t i = 0; i < intLog2Floor<T>(std::numeric_limits<T>::digits); ++i)
	{
		shift >>= 1;
		mask ^= (mask << shift);
		value = (((value >> shift) & mask) | ((value & mask) << shift));
	}

	return value;
}
#endif
#if defined(USE_COMPILER_BUILTINS)

#if 0
// XXX - these are missing in gcc 4.7.2 on x86 - there's some info about a
// patch for them available, but for now we're disabling these for now
template<>
inline int16_t byteReverse(int16_t value)
{ return __builtin_bswap16(value); }

template<>
inline uint16_t byteReverse(uint16_t value)
{ return __builtin_bswap16(value); }
#endif
template<>
inline int16_t byteReverse(int16_t value)
{ return __builtin_bswap16(value); }

template<>
inline uint16_t byteReverse(uint16_t value)
{ return __builtin_bswap16(value); }

template<>
int32_t byteReverse(int32_t value)
	{ return __builtin_bswap32(value); }

template<>
uint32_t byteReverse(uint32_t value)
	{ return __builtin_bswap32(value); }

template<>
int64_t byteReverse(int64_t value)
	{ return __builtin_bswap64(value); }

template<>
uint64_t byteReverse(uint64_t value)
	{ return __builtin_bswap64(value); }

#elif defined(USE_INLINE_ASM)
template<>
inline int16_t byteReverse(int16_t value)
{
	asm ( "xchg %h0, %b0" : "+a" (value) );
	return value;
}

template<>
inline uint16_t byteReverse(uint16_t value)
{
	asm ( "xchg %h0, %b0" : "+a" (value) );
	return value;
}

template<>
inline int32_t byteReverse(int32_t value)
{
	asm ( "bswap %0" : "+r" (value) );
	return value;
}

template<>
inline uint32_t byteReverse(uint32_t value)
{
	asm ( "bswap %0" : "+r" (value) );
	return value;
}

template<>
inline int64_t byteReverse(int64_t value)
{
	asm ( "bswap %0" : "+r" (value) );
	return value;
}

template<>
inline uint64_t byteReverse(uint64_t value)
{
	asm ( "bswap %0" : "+r" (value) );
	return value;
}


#endif

#endif // GUARD_BITHACKS_H 1
