#pragma once

#include "chip8.h"
#include "random.h"
#include "opcode.h"

// 00E0 - Clear the display.
void CLS(Chip8* chip8) {
    chip8->display.fill(0);
}

// 00EE - Return from a subroutine.
void RET(Chip8* chip8) {
    if (chip8->sp > 0) {
        chip8->pc = chip8->stack[--chip8->sp];
    }
    else {
        std::cerr << "Stack underflow error!" << std::endl;
        abort();
    }
}

// 1nnn - Jump to location nnn.
void JMP(Opcode in, Chip8* chip8) {
    chip8->pc = in.address();
}

// 2nnn - Call subroutine at nnn.
void CALL(Opcode in, Chip8* chip8) {
    if (chip8->sp < 16) {
        chip8->stack[chip8->sp++] = chip8->pc;
        chip8->pc = in.address();
    }
    else {
        std::cerr << "Stack overflow error!" << std::endl;
        abort();
    }
}

// 3xkk - Skip next instruction if Vx = kk.
void SE_VX_KK(Opcode in, Chip8* chip8) {
    if (chip8->registers[in.x()] == in.byte()) {
        chip8->pc += 2;
    }
}

// 4xkk - Skip next instruction if Vx != kk.
void SNE_VX_KK(Opcode in, Chip8* chip8) {
    if (chip8->registers[in.x()] != in.byte()) {
        chip8->pc += 2;
    }
}

// 5xy0 - Skip next instruction if Vx = Vy.
void SE_VX_VY(Opcode in, Chip8* chip8) {
    if (chip8->registers[in.x()] == chip8->registers[in.y()]) {
        chip8->pc += 2;
    }
}

// 6xkk - The interpreter puts the value kk into register Vx.
void LD_VX_KK(Opcode in, Chip8* chip8) {
    chip8->registers[in.x()] = in.byte();
}

// 7xkk - Adds the value kk to the value of register Vx.
void ADD_VX_KK(Opcode in, Chip8* chip8) {
    chip8->registers[in.x()] += in.byte();
}

// 8xy0 - Stores the value of register Vy in register Vx.
void LD_VX_VY(Opcode in, Chip8* chip8) {
    chip8->registers[in.x()] = chip8->registers[in.y()];
}

// 8xy1 - Performs a bitwise OR on the values of Vx and Vy.
void OR_VX_VY(Opcode in, Chip8* chip8) {
    chip8->registers[in.x()] |= chip8->registers[in.y()];
}

// 8xy2 - Performs a bitwise AND on the values of Vx and Vy.
void AND_VX_VY(Opcode in, Chip8* chip8) {
    chip8->registers[in.x()] &= chip8->registers[in.y()];
}

// 8xy3 - Performs a bitwise exclusive OR on the values of Vx and Vy.
void XOR_VX_VY(Opcode in, Chip8* chip8) {
    chip8->registers[in.x()] ^= chip8->registers[in.y()];
}

// 8xy4 - Set Vx = Vx + Vy, set VF = carry.
void ADD_VX_VY(Opcode in, Chip8* chip8) {
    uint16_t sum = chip8->registers[in.x()] + chip8->registers[in.y()];
    chip8->registers[0x0F] = sum > 0xFF ? 1 : 0;
    chip8->registers[in.x()] = sum & 0xFF;
}

// 8xy5 - Set Vx = Vx - Vy, set VF = NOT borrow.
void SUB_VX_VY(Opcode in, Chip8* chip8) {
    chip8->registers[0x0F] = chip8->registers[in.x()] > chip8->registers[in.y()] ? 1 : 0;
    chip8->registers[in.x()] -= chip8->registers[in.y()];
}

// 8xy6 - Set Vx = Vx SHR 1.
void SHR_VX(Opcode in, Chip8* chip8) {
    chip8->registers[0x0F] = chip8->registers[in.x()] & 0x01;
    chip8->registers[in.x()] >>= 1;
}

// 8xy7 - Set Vx = Vy - Vx, set VF = NOT borrow.
void SUBN_VX_VY(Opcode in, Chip8* chip8) {
    chip8->registers[0x0F] = chip8->registers[in.y()] > chip8->registers[in.x()] ? 1 : 0;
    chip8->registers[in.x()] = chip8->registers[in.y()] - chip8->registers[in.x()];
}

// 8xyE - Set Vx = Vx SHL 1.
void SHL_VX(Opcode in, Chip8* chip8) {
    chip8->registers[0x0F] = chip8->registers[in.x()] >> 7;
    chip8->registers[in.x()] <<= 1;
}

// 9xy0 - Skip next instruction if Vx != Vy.
void SNE_VX_VY(Opcode in, Chip8* chip8) {
    if (chip8->registers[in.x()] != chip8->registers[in.y()]) {
        chip8->pc += 2;
    }
}

// Annn - Register I is set to nnn.
void LD_I(Opcode in, Chip8* chip8) {
    chip8->index = in.address();
}

// Bnnn - Program counter is set to nnn plus the value of V0.
void JP_V0(Opcode in, Chip8* chip8) {
    chip8->pc = in.address() + chip8->registers[0x00];
}

// Cxkk - Set Vx = random byte AND kk
void RND(Opcode in, Chip8* chip8, Random* rand) {
    chip8->registers[in.x()] = (*rand)() & in.byte();
}

// Dxyn - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
void DRW(Opcode in, Chip8* chip8) {
    chip8->registers[0x0F] = 0;
    for (int y = 0; y < in.low(); ++y) {
        uint8_t sprite_byte = chip8->memory[chip8->index + y];
        for (int x = 0; x < 8; ++x) {
            if (sprite_byte & (0x80 >> x)) {
                uint16_t px = (chip8->registers[in.x()] + x) % Chip8::kWidth;
                uint16_t py = (chip8->registers[in.y()] + y) % Chip8::kHeight;
                uint16_t pixel = px + py * Chip8::kWidth;
                if (chip8->display[pixel]) chip8->registers[0x0F] = 1;
                chip8->display[pixel] ^= 1;
            }
        }
    }
    chip8->redraw = true;
}


// Ex9E - Skip instruction if key with the value of Vx is pressed.
void SKP(Opcode in, Chip8* chip8, GLFWwindow* window) {
    if (chip8->IsPressed(window, chip8->registers[in.x()])) {
        chip8->pc += 2;
    }
}

// ExA1 - Skip instruction if key with the value of Vx is not pressed.
void SKNP(Opcode in, Chip8* chip8, GLFWwindow* window) {
    if (!chip8->IsPressed(window, chip8->registers[in.x()])) {
        chip8->pc += 2;
    }
}

// Fx07 - Set Vx = delay timer value.
void LD_VX_DT(Opcode in, Chip8* chip8) {
    chip8->registers[in.x()] = chip8->delayTimer;
}

// Fx0A - Wait for a key press, store the value of the key in Vx.
void LD_VX_K(Opcode in, Chip8* chip8, GLFWwindow* window) {
    for (auto i = 0; i < 0x0F; ++i) {
        if (chip8->IsPressed(window, i)) {
            chip8->registers[in.x()] = i;
        }
    }
}

// Fx15 - Set delay timer = Vx.
void LD_DT(Opcode in, Chip8* chip8) {
    chip8->delayTimer = chip8->registers[in.x()];
}

// Fx18 - Set sound timer = Vx.
void LD_ST(Opcode in, Chip8* chip8) {
    chip8->soundTimer = chip8->registers[in.x()];
}

// Fx1E - Set I = I + Vx.
void ADD_I_VX(Opcode in, Chip8* chip8) {
    chip8->index += chip8->registers[in.x()];
}

// Fx29 - Set I = location of sprite for digit Vx.
void LD_F_VX(Opcode in, Chip8* chip8) {
    // Sprites start at 0x00 and the size of each sprite is 5 bytes
    chip8->index = chip8->registers[in.x()] * 0x05;
}

// Fx33 - Store BCD representation of Vx in memory locations I, I+1, and I+2.
void LD_B_VX(Opcode in, Chip8* chip8) {
    chip8->memory[chip8->index] = chip8->registers[in.x()] / 100;
    chip8->memory[chip8->index + 1] = (chip8->registers[in.x()] / 10) % 10;
    chip8->memory[chip8->index + 2] = chip8->registers[in.x()] % 10;
}

// Fx55 - Store regs V0 through Vx in memory starting at location I.
void LD_I_VX(Opcode in, Chip8* chip8) {
    for (uint8_t i = 0; i <= in.x(); ++i) {
        chip8->memory[chip8->index + i] = chip8->registers[i];
    }
}

// Fx65 - Read regs V0 through Vx from memory starting at location I.
void LD_VX_I(Opcode in, Chip8* chip8) {
    for (uint8_t i = 0; i <= in.x(); ++i) {
        chip8->registers[i] = chip8->memory[chip8->index + i];
    }
}