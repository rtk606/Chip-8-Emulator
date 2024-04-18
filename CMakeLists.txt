cmake_minimum_required(VERSION 3.21)
project(chip8)

set(CMAKE_CXX_STANDARD 23)

add_executable(chip8 main.cpp "core/chip8.h" "core/chip8.cpp")