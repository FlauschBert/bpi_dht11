#include <wiringPi.h>

#include "../dht11/dht11.h"
using namespace dht11;

int main ()
{
	// Use GPIO:0, Physical pin 11
	int constexpr pin = 0;

	if (-1 == setup (0))
		return -1;

	// Wait for sensor
	delay (2000 /*ms*/);

	// Trigger sending data on serial line
	sendStartSignal (pin);

	// Wait for data of client
	if (!waitForResponseSignal (pin))
	{
		std::cout << "Waiting failed.\n";
		return 1;
	}
	// Read and evaluate data from client
	auto const data = getDataFromBits (pin);

	// Consider parity bit check
	if (-1 == data.humidity &&
		  -1 == data.temperature)
	{
		std::cout << "Parity invalid/Failed read.\n";
		return 1;
	}

	print (data);
	return 0;
}
