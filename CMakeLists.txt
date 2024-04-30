# Minimum version of CMake required to build this project
cmake_minimum_required(VERSION 3.21)

# Project name
project(chip-8)

# Create the variable EXEC and sets it to chip8
set(EXEC chip8)

# Set to the C++23 standard
set(CMAKE_CXX_STANDARD 23)

# Set the submodule directory
SET(THIRD_PARTY external)

# Configure GLFW to not build its docs, tests, or examples to save compile time and dependencies
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Add the GLFW subdirectory from the external folder so CMake can find and configure it
add_subdirectory(${THIRD_PARTY}/glfw)

# Include directories for GLFW, ImGui, and ImGui club library
include_directories(${THIRD_PARTY}/glfw/include)
include_directories(${THIRD_PARTY}/imgui)
include_directories(${THIRD_PARTY}/imgui/backends)
include_directories(${THIRD_PARTY}/imgui_club)

# Suppress deprecation warnings for OpenGL on newer platforms
add_compile_definitions(GL_SILENCE_DEPRECATION)

# Look for the OpenGL package on the system
find_package(OpenGL REQUIRED)

# Gather all .cpp source files from the core directory into SOURCES_CHIP8 variable
file(GLOB SOURCES_CHIP8 core/*.cpp)

# Put imgui .cpp files to sources
file(GLOB SOURCES_IMGUI ${THIRD_PARTY}/imgui/*.cpp)
file(GLOB SOURCES_IMGUI ${THIRD_PARTY}/imgui/*.cpp)

# Set SOURCES variable which includes all source files from core and imgui directories, and specific backend implementations for imgui
set(SOURCES
  ${SOURCES_CHIP8}
  ${SOURCES_IMGUI}
  ${THIRD_PARTY}/imgui/backends/imgui_impl_opengl3.cpp
  ${THIRD_PARTY}/imgui/backends/imgui_impl_glfw.cpp
)

# Compile all source files into an executable named as defined by EXEC variable
add_executable(${EXEC} ${SOURCES})

# Link the executable with the GLFW and OpenGL libraries
target_link_libraries(${EXEC} glfw)
target_link_libraries(${EXEC} OpenGL::GL)