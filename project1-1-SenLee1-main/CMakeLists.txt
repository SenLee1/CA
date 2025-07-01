cmake_minimum_required(VERSION 3.10)

PROJECT(cs110p1-1)

include_directories(inc)

set(CMAKE_C_STANDARD 11)
SET(CMAKE_C_FLAG "-g -Wall -Wextra -Werror -Wpedantic -Wvla")

file(GLOB SRC "src/*.c")

add_executable(main main.c ${SRC})

