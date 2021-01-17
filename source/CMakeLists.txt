cmake_minimum_required(VERSION 3.7)

project(dht11 VERSION 0.1)
set(CMAKE_CXX_STANDARD 14)

# use interface library for including wiringBP
add_subdirectory(wiringPi)

# add test executable
add_executable(dht11_test test/main.cpp)
target_link_libraries(dht11_test wiringPi)
target_include_directories(dht11_test PRIVATE ${CMAKE_SOURCE_DIR})

# install
SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
install(
	TARGETS dht11_test
	RUNTIME DESTINATION /usr/local/bin
)
install(FILES
	dht11/dht11.h DESTINATION /usr/local/include/dht11
)
