cmake_minimum_required(VERSION 3.14)

project(FloatCalculate C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wpedantic -Wall -Wextra -Wvla -Werror")

add_library(FloatCalculateLib STATIC FloatCalculate.c)

add_executable(FloatCalculate test.c)
target_link_libraries(FloatCalculate m FloatCalculateLib)