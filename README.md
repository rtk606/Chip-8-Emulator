# 👾 C++ Chip-8 Emulator

A [Chip-8](https://en.wikipedia.org/wiki/Chip-8) interpreter written in modern C++ using GLFW, OpenGL and ImGui.

### Screenshot
![chip8_screenshot](https://github.com/rtk606/Chip-8-Emulator/assets/132792358/828e1f16-5b94-49de-9ac4-5aecd5571d10)

### Video
https://github.com/rtk606/Chip-8-Emulator/assets/132792358/07a951d0-4a2e-44ca-8ba6-6cee22068226

## Dependencies
All the dependencies are included as submodules.

## Building with CMake
```
$ git clone https://github.com/rtk606/Chip-8-Emulator.git
$ cd Chip-8-Emulator
$ git submodule update --init --recursive
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## Usage

```
$ ./chip8 invaders.ch8
```