cmake_minimum_required(VERSION 3.9)
project(steckassembler)

include_directories("./")

set(CMAKE_CXX_STANDARD 17)

add_executable(steckassembler main.cpp StackCpu.cpp StackCpu.h Instruction.cpp Instruction.h)