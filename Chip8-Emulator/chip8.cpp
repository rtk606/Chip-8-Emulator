#include <iostream>
#include <fstream>
#include <vector>

#include "chip8.h"

using namespace rtk;

void Chip8::Reset() {
    ram.Reset();
    // Input
    // Display

    cpu.regs  = { 0 };
    cpu.stack = { 0 };
    
    cpu.I  = { 0 };
    cpu.pc = { 0 };
    cpu.sp = { 0 };

    cpu.t_delay = { 0 };
    cpu.t_sound = { 0 };
}

void Chip8::LoadROM(std::string_view filename)
{
    std::ifstream file{ filename.data(), std::ios::binary };
    if (!file) {
        std::cerr << "Failed to open ROM file: " << filename << std::endl;
        return;
    }

    // Read the entire file into a std::vector
    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>{file}, {});

    // Ensure the ROM isn't too big to fit into memory
    if (buffer.size() > 0xDFF)
    {
        std::cerr << "ROM is too large to fit into memory!" << std::endl;
        return;
    }

    // Copy the buffer contents into the Chip8's memory starting at START_ADDRESS
    ram.SaveProgram(buffer);
}