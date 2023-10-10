cmake_minimum_required(VERSION 3.21)
project(chip8-Emulator)

set(CMAKE_CXX_STANDARD 20)
set(PROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Add 3rdParty libraries
add_subdirectory(3rdParty/glad)
add_subdirectory(3rdParty/sdl-2.0.20)
add_subdirectory(3rdParty/imgui-1.88)

set(HEADERS
    chip8.h
    cpu.h
    instructions.h
    memory.h
    parser.h
    platform.h
    random.h
    types.h
)

set(SOURCES
    main.cpp
    chip8.cpp
    platform.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${HEADERS} ${SOURCES})

# Link 3rdParty libraries
target_link_libraries(chip8-Emulator glad SDL2 imgui)
