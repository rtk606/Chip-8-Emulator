#pragma once

#include <array>

namespace rtk {

    struct Cpu {
        std::array<uint8_t, 16> regs = { 0 };    
        std::array<uint16_t, 16> stack = { 0 };  

        uint16_t I = { 0 };
        uint16_t pc = { 0 }; // program counter
        uint16_t sp = { 0 }; // stack pointer
        uint8_t t_delay = 0; // delay timer
        uint8_t t_sound = 0; // sound timer
    };

}
