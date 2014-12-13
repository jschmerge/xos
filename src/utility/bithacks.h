#ifndef GUARD_BITHACKS_H
#define GUARD_BITHACKS_H 1

#include <cstdio>
#include <cstdint>
#include <limits>
#include <type_traits>

///
/// bit_width returns the number of bits in a given integral type - this
/// unfortunately isn't defined in std::numeric_limits for some reason
///
template <typename T>         
  constexpr size_t bit_width(const T & = 0)
{    
    return (sizeof(T) << 3);
}

///
/// pop_count gives the number of one-bits in an integral value
/// the integer is cast to an unsigned value, then possibly promoted to
/// an integer
///
/// This function maps to one of three compiler intrinsic functions:
///   int __builtin_popcount (unsigned int x)
///   int __builtin_popcountl (unsigned long)
///   int __builtin_popcountll (unsigned long long)
///
template <typename T>
  inline typename std::enable_if<std::is_integral<T>::value, size_t>::type
    pop_count(const T & value)
{
	// need to first cast to unsigned type with the same width
	typedef typename std::make_unsigned<T>::type utype;
	utype uval = static_cast<utype>(value);
	// ... then do a possible extension
	return pop_count(
	  static_cast<typename std::common_type<utype, unsigned int>::type>(uval));
}

/// Specialization to call __builtin_popcount
template <>
  inline size_t pop_count(const unsigned int & value)
	{ return __builtin_popcount(value); }

/// Specialization to call __builtin_popcountl
template <>
  inline size_t pop_count(const unsigned long & value)
	{ return __builtin_popcountl(value); }

/// Specialization to call __builtin_popcountll
template <>
  inline size_t pop_count(const unsigned long long & value)
	{ return __builtin_popcountll(value); }

#include <cassert>
#include <iostream>

template<class T>
constexpr T fill_trailing_bits(const T & value, size_t shift = 1)
{
	return ( (shift < bit_width(value)) ?
	         fill_trailing_bits(static_cast<T>(value | (value >> shift)),
	                            shift << 1) :
	         value);
}

template<class T>
constexpr size_t nlz(T value)
{
#if 0
	T tmp = fill_trailing_bits(value);
	for (unsigned i = 1; i < (sizeof(value) << 3); i <<= 1)
		value = value | (value >> i);

	assert(tmp == value);
#endif

	//return pop_count(static_cast<T>(~value));
	return pop_count(static_cast<T>(~fill_trailing_bits(value)));
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
