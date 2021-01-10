cmake_minimum_required(VERSION 3.7)

project(bpi_dht11 VERSION 0.1)

add_subdirectory(wiringPi)

add_executable(bpi_dht11 main.cpp)
target_link_libraries(bpi_dht11 wiringPi)

set(CMAKE_CXX_STANDARD 17)
