#include <iostream>
#include "core/chip8.h"

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <Scale><Delay><Rom>\n";
        return EXIT_FAILURE;
    }

    Chip8 chip8;
    chip8.LoadRom(argv[3]);

    //todo gfx layer (imgui & opengl or sdl)


    return 0;
}


/* 
    Usage example: ./chip8 10 3 Tetris.ch8 
*/
