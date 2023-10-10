cmake_minimum_required(VERSION 3.21)
project(chip8-Emulator)

set(CMAKE_CXX_STANDARD 20)
set(PROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Specify the include directory
include_directories(${PROJECT_SRC_DIR}/includes)

# Add 3rdParty libraries
add_subdirectory(3rdParty/glad)
add_subdirectory(3rdParty/sdl-2.0.20)
add_subdirectory(3rdParty/imgui-1.88)

set(HEADERS
    includes/chip8.h
    includes/cpu.h
    includes/instructions.h
    includes/memory.h
    includes/parser.h
    includes/platform.h
    includes/random.h
    includes/types.h
)

set(SOURCES
    src/main.cpp
    src/chip8.cpp
    src/platform.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${HEADERS} ${SOURCES})

# Link 3rdParty libraries
target_link_libraries(chip8-Emulator glad SDL2 imgui)