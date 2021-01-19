#pragma once
#include <wiringPi.h>

#include <bitset>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>

namespace {

int expectRead (int const pin, int const level, int const maximumCount)
{
	int count = 0;
	while (digitalRead (pin) == level)
		if (count++ >= maximumCount)
			return 0;
	return count;
}

int constexpr sMaxCount = 1000;

}

namespace dht11 {

inline int
setup ()
{
	// Try to get high process priority
	// (don't know whether this does help anything)
	piHiPri (99);

	return wiringPiSetup ();
}

inline void
sendStartSignal (int const pin)
{
	// Robotics 5.2 Figure 3

	// Step one (AOSONG):
	//
	// MCU/Host sends start signal for >18ms:
	// 	This means setting from HIGH to LOW
	//
	pinMode (pin, OUTPUT);
	digitalWrite (pin, LOW);
	delay (18 /*ms*/);
	
	// Step two (AOSONG):
	//
	// Host pulls up the voltage
	//
	pullUpDnControl (pin, PUD_UP);
}

inline bool
waitForResponseSignal (int const pin)
{
	// Robotics 5.2 Figure 3

	// Step three (AOSONG):
	//
	// first HIGH is still pull up of the MCU/Host
	// (from sendStartSignal())
	// after following LOW and HIGH of DHT/Client data
	// is sent
	//
	pinMode (pin, INPUT);
	if (0 == expectRead (pin, HIGH, sMaxCount))
		return false;
	if (0 == expectRead (pin, LOW, sMaxCount))
		return false;
	if (0 == expectRead (pin, HIGH, sMaxCount))
		return false;
	return true;
}

size_t constexpr cCounts = 40;
using Counts = std::vector<int>;

inline Counts
getCounts (int const pin)
{
	// Robotics 5.3 Figure 4 & 5
	//
	// LOW and HIGH edge make one bit
	// Count number of digitalReads of HIGH edges:
	// * LOW value: 0 bit
	// * HIGH value: 1 bit
	//
	Counts counts;
	counts.resize (cCounts);
	for (auto& count : counts)
	{
		if (0 == expectRead (pin, LOW, sMaxCount))
			return {};
		int const counter = expectRead (pin, HIGH, sMaxCount);
		if (0 == counter)
			return {};
		count = counter;
	}

	return counts;
}

/* Get threshold to distinguish between 0 and 1 bit

	 Unfortunately the polling method (by counting HIGH states) has the
	 drawback that the counts can be very different because we do not have
	 the alone priority over all processes.

	 To overcome this we calculate a threshold from the lowest and highest counts.
 */
inline size_t
getThreshold (Counts const& counts)
{
	auto tmp = counts;
	std::sort (tmp.begin (), tmp.end ());

	// We assume at least 4x 0 bits
	size_t const low = (tmp [0] + tmp [1] + tmp [2] + tmp [3]) / 4;
	// We assume at least 2x 1 bits
	size_t const high = (tmp [tmp.size () - 2] + tmp [tmp.size () - 1]) / 2;
	
	return (low + high) / 2;
}

inline int
getIntFromCounts (size_t const pos, size_t const bits, Counts const& counts, size_t const threshold)
{
	// counts [pos; pos+7] -> intValue [7;0]
	// counts [pos; pos+15] -> intValue [15;0]
	std::bitset<16> intValue;
	intValue.reset ();

	auto cIt = counts.begin () + pos;
	for (int p = std::min (bits, static_cast<size_t> (16)) - 1; p > -1; --p, ++cIt)
		if (*cIt > threshold)
			intValue.set (p);

	return static_cast<int> (intValue.to_ulong ());
}

inline bool
parityValid (Counts const& counts, size_t const threshold)
{
	int constexpr bits = 8;
	auto parity = getIntFromCounts (0 * bits, bits, counts, threshold);
	parity += getIntFromCounts (1 * bits, bits, counts, threshold);
	parity += getIntFromCounts (2 * bits, bits, counts, threshold);
	parity += getIntFromCounts (3 * bits, bits, counts, threshold);
	// last 8 bits are parity bits
	return parity == getIntFromCounts (4 * bits, bits, counts, threshold);
}

struct Data {
	int humidity = -1;
	int temperature = -1;
};

inline Data
getDataFromBits (int const pin)
{
	auto const counts = getCounts (pin);
	if (counts.empty ())
		return {};
	auto const threshold = getThreshold (counts);

	if (!parityValid (counts, threshold))
		return {};

	int constexpr bits = 8;
	return {
		// humidity int is bits 0 - 7 from highest to lowest bit
		getIntFromCounts (0 * bits, bits, counts, threshold) +
		// humidity int is bits 8 - 15 decimal from highest to lowest bit
		getIntFromCounts (1 * bits, bits, counts, threshold),
		// temperature int is bits 16 - 23 from highest to lowest bit
		getIntFromCounts (2 * bits, bits, counts, threshold) +
		// temperature int is bits 24 - 31 decimal from highest to lowest bit
		getIntFromCounts (3 * bits, bits, counts, threshold)
	};
}

// PRINTERS

inline void
print (Counts const& counts)
{
	for (auto const& count : counts)
		std::cout << count << std::endl;
}

inline void
print (Data const& data)
{
	std::cout << "Humidity: " << data.humidity << "\n";
	std::cout << "Temperature: " << data.temperature << "\n";
}

} // dht11

