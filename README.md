# DHT11 on the Banana Pi

Humidity and temperate sensor DHT11 on the Banana Pi or Raspberry Pi (not tested).

# Motivation

Read out the values from the sensor and print to the console.

# Results

The approach of polling the values is improvable unfortunately. Because we are running on a system with many concurrent processes the results vary with every run. With an heuristic approach it works very good however.  
It would be perfect if we could rely on interrupts but this is not possible in user-space.

# Compile and install wiringBP library and tools

Clone the [repository](https://github.com/LeMaker/WiringBP) and run `./build`.  
This installs includes and library in */usr/local/include* and */usr/local/lib*. Binaries in the */usr/local/bin* folder.

# Compile and install

Go to the *source* directory and use the mighty force of shining CMake :D to compile the pico project.  
The *wiringPi* sub folder contains the internal interface library for the installed wiringBP library.  
Do `make install` to install the library in */usr/local*.

(I used Armbian stretch but also more recent versions should work).

# References

* [Banana Pi LED test](https://github.com/FlauschBert/bpi_led_test/blob/master/README.md) for initial setup
* [Documentation of DHT11 from AOSONG](https://lastminuteengineers.com/datasheets/dht11-datasheet.pdf)
* [Documentation of DHT11 from Robotics](https://robojax.com/learn/arduino/robojax-DHT11_manual.pdf)

