cmake_minimum_required(VERSION 3.9)
project(steckassembler)

include_directories("./")

set(CMAKE_CXX_STANDARD 17)
SET(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic")

# Add compile flags
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

add_executable(steckassembler main.cpp StackCpu.cpp StackCpu.h Instruction.cpp Instruction.h)