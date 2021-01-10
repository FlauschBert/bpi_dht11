#include <wiringPi.h>

#include "dht11.h"
using namespace dht11;

int main ()
{
	if (-1 == setup ())
		return -1;

	// Use GPIO:0, Physical pin 11
	int constexpr pin = 0;

	// Trigger sending data on serial line
	sendStartSignal (pin);
	// Wait for data of client
	waitForResponseSignal (pin);
	// Read and evaluate data from client
	auto const data = getDataFromBits (pin);

	// Consider parity bit check
	if (-1 == data.humidity &&
		  -1 == data.temperature)
	{
		std::cout << "Parity invalid. Run again.\n";
		return 1;
	}

	print (data);
	return 0;
}
