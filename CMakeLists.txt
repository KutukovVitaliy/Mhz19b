cmake_minimum_required(VERSION 3.20)
project(Mhz19b)

set(CMAKE_CXX_STANDARD 14)

add_executable(Mhz19b main.cpp Mhz19B.cpp Mhz19B.h)
target_link_libraries(Mhz19b pthread boost_thread)