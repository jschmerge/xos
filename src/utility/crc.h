#ifndef GUARD_CRC_H
#define GUARD_CRC_H 1

#include <cstdint>
#include <mutex>
#include <limits>
#include <atomic>
#include <numeric>
#include <algorithm>
#include <array>
#include <type_traits>

#include <iostream>

#include "spinlock.h"
#include "bithacks.h"

namespace Polynomial {

constexpr uint32_t Crc32 = 0x04c11db7;
constexpr uint32_t Crc32c = 0x1edc6f41;

} // namespace Polynomial


uint32_t crc32(const char * data, size_t length, uint32_t seed = 0);

//////////////////////////////////////////////////////////////////////
template <class T, T polynomial>
class Crc
{
 public:
	using crc_type = T;
	using table_type = std::array<crc_type,
	                              std::numeric_limits<uint8_t>::max() + 1>;

	Crc(T seed = 0) noexcept
	  : crcValue(~seed)
	{
		std::lock_guard<SpinLock> lg(tableLock);

		if ( ! tableInitialized )
		{
			generateTable();
			tableInitialized = true;
		}
	}

	Crc(const Crc &) = default;

	Crc & operator = (const Crc &) = default;

	~Crc() = default;

	template <class VALUE>
	Crc & operator () (const VALUE * data,
	                   std::size_t length,
	                   // XXX - default this to something keying off little
	                   // vs big endian
	                   bool byteSwap = false) noexcept
	{
		auto calc = [byteSwap, this] (crc_type seed, VALUE v) noexcept
		{
			typename std::make_unsigned<VALUE>::type val = v;

			if (byteSwap) val = byteReverse(val);

			for (std::size_t i = 0; i < sizeof(v); ++i)
			{
				seed = (seed >> 8) ^ crcTable[(seed & 0xFF) ^ (val & 0xFF)];
				val >>= 8;
			}
			return seed;
		};

		crcValue = std::accumulate(data, data + length, crcValue, calc);
		
		return *this;
	}

	crc_type get() noexcept { return ~crcValue; }

 private:

	T crcValue;

	void static generateTable() noexcept;

	static SpinLock tableLock;
	static bool tableInitialized;
	static table_type crcTable;
};

template<class T, T P> SpinLock Crc<T, P>::tableLock{};
template<class T, T P> bool Crc<T, P>::tableInitialized{false};
template <class T, T P> typename Crc<T, P>::table_type Crc<T, P>::crcTable;

//////////////////////////////////////////////////////////////////////
template <class T, T Polynomial>
void Crc<T, Polynomial>::generateTable() noexcept
{
	// fill the table with 0..255
	int count = 0;
	std::generate(crcTable.begin(), crcTable.end(),
	              [&count] { return count++; } );

	auto calculation = [&] (T val) {
		return (val >> 1) ^ ((val & 1) * bitReverse(Polynomial));
	};

	// calculate the crc table
	for (int j = 0; j < std::numeric_limits<uint8_t>::digits; j++) 
		std::transform(crcTable.begin(), crcTable.end(),
		               crcTable.begin(), calculation);
}

#endif // GUARD_CRC_H 1
