#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>

#include "chip8.h"
#include "parser.h"
#include "types.h"
#include "instructions.h"

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

    rand.seed(time(nullptr));
}

void Chip8::LoadROM(std::string_view filename) {
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

void Chip8::Cycle() {
    // Fetch opcode
    Opcode opcode = (ram[cpu.pc] << 8u) | ram[cpu.pc + 1];

    // Retrive instruction
    auto instruction = parse(opcode);


    // Increment the PC before execution
    cpu.pc += 2;

    using namespace instruction;
    switch (instruction) {
        case Instruction::CLS: CLS(video);
        case Instruction::RET: return RET(&cpu);
        case Instruction::JMP: return JMP(opcode, &cpu);
        case Instruction::CALL: return CALL(opcode, &cpu);
        case Instruction::SE_VX_KK: return SE_VX_KK(opcode, &cpu);
        case Instruction::SNE_VX_KK: return SNE_VX_KK(opcode, &cpu);
        case Instruction::SE_VX_VY: return SE_VX_VY(opcode, &cpu);
        case Instruction::LD_VX_KK: return LD_VX_KK(opcode, &cpu);
        case Instruction::ADD_VX_KK: return ADD_VX_KK(opcode, &cpu);
        case Instruction::LD_VX_VY: return LD_VX_VY(opcode, &cpu);
        case Instruction::OR_VX_VY: return OR_VX_VY(opcode, &cpu);
        case Instruction::AND_VX_VY: return AND_VX_VY(opcode, &cpu);
        case Instruction::XOR_VX_VY: return XOR_VX_VY(opcode, &cpu);
        case Instruction::ADD_VX_VY: return ADD_VX_VY(opcode, &cpu);
        case Instruction::SUB_VX_VY: return SUB_VX_VY(opcode, &cpu);
        case Instruction::SHR_VX: return SHR_VX(opcode, &cpu);
        case Instruction::SUBN_VX_VY: return SUBN_VX_VY(opcode, &cpu);
        case Instruction::SHL_VX: return SHL_VX(opcode, &cpu);
        case Instruction::SNE_VX_VY: return SNE_VX_VY(opcode, &cpu);
        case Instruction::LD_I: return LD_I(opcode, &cpu);
        case Instruction::JMP_V0: return JP_V0(opcode, &cpu);
        case Instruction::RND: return RND(opcode, &cpu, &rand);
        case Instruction::DRW: return DRW(opcode, &cpu, video, &ram);
        case Instruction::SKP: OP_SKP(opcode); break;
        case Instruction::SKNP: OP_SKNP(opcode); break;
        case Instruction::LD_VX_DT: return LD_VX_DT(opcode, &cpu);
        case Instruction::LD_VX_K: OP_LD_VX_K(opcode); break;
        case Instruction::LD_DT: return LD_DT(opcode, &cpu);
        case Instruction::LD_ST: return LD_ST(opcode, &cpu);
        case Instruction::ADD_I_VX: return ADD_I_VX(opcode, &cpu);
        case Instruction::LD_F_VX: return LD_F_VX(opcode, &cpu);
        case Instruction::LD_B_VX: return LD_B_VX(opcode, &cpu, &ram);
        case Instruction::LD_I_VX: return LD_I_VX(opcode, &cpu, &ram);
        case Instruction::LD_VX_I: return LD_VX_I(opcode, &cpu, &ram);

        default:
            throw std::exception("Bad opcode");
    }

}


void Chip8::OP_SKP(Opcode opcode) {
    uint8_t Vx = opcode.x();
    uint8_t key = cpu.regs[Vx];

    if (keypad[key]) {
        cpu.pc += 2;
    }
}

void Chip8::OP_SKNP(Opcode opcode) {
    uint8_t Vx = opcode.x();
    uint8_t key = cpu.regs[Vx];

    if (!keypad[key]) {
        cpu.pc += 2;
    }
}

void Chip8::OP_LD_VX_K(Opcode opcode) {
    uint8_t Vx = opcode.x();

    for (int i = 0; i <= 0x0F; ++i) {
        if (keypad[i]) {
            cpu.regs[Vx] = i;
            return;
        }
    }

    // If no key is pressed, decrement the program counter
    cpu.pc -= 2;
}