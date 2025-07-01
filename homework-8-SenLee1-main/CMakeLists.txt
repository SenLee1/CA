cmake_minimum_required(VERSION 3.14)
project(24f_CS110_HW8 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wextra -Werror -Wpedantic -Wvla")

add_executable(vm_tlb main.c src/simulator.c src/memory.c src/process.c src/TLB.c)
