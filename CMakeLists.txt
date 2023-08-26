cmake_minimum_required(VERSION 3.10)
project(inOneWeekend)

set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/local/include)

add_executable(inOneWeekend main.cpp Vec3.h Color.h Ray.h)