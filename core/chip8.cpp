#include "chip8.h"
#include "opcode.h"
#include "parser.h"
#include "instructions.h"

Chip8::Chip8() {
	ResetChip8();
}

void Chip8::ResetChip8() {
	memory.fill(0);
	pc = kStartAddress;
	std::copy(begin(kSprites), end(kSprites), begin(memory) + 0x50);		// Load kSprites into memory starting at 0x50
}

void Chip8::LoadRom(std::string_view filename) {
    std::ifstream file(filename.data(), std::ios::binary | std::ios::ate);

    if (!file.is_open())
        return;

    auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(&memory[kStartAddress]), size);
    file.close();
}

// Fetch-decode-execute cycle (fetch the opcode, decode the operation and execute the instruction)
void Chip8::Tick() {
	// Fetch
	opcode = memory[pc] << 8 | memory[pc + 1];
    auto instruction = parse(opcode);

	// Increment the program counter
	pc += 2;

    switch (instruction) {
        case Instruction::CLS: return CLS(this);
        case Instruction::RET: return RET(this);
        case Instruction::JMP: return JMP(opcode, this);
        case Instruction::CALL: return CALL(opcode, this);
        case Instruction::SE_VX_KK: return SE_VX_KK(opcode, this);
        case Instruction::SNE_VX_KK: return SNE_VX_KK(opcode, this);
        case Instruction::SE_VX_VY: return SE_VX_VY(opcode, this);
        case Instruction::LD_VX_KK: return LD_VX_KK(opcode, this);
        case Instruction::ADD_VX_KK: return ADD_VX_KK(opcode, this);
        case Instruction::LD_VX_VY: return LD_VX_VY(opcode, this);
        case Instruction::OR_VX_VY: return OR_VX_VY(opcode, this);
        case Instruction::AND_VX_VY: return AND_VX_VY(opcode, this);
        case Instruction::XOR_VX_VY: return XOR_VX_VY(opcode, this);
        case Instruction::ADD_VX_VY: return ADD_VX_VY(opcode, this);
        case Instruction::SUB_VX_VY: return SUB_VX_VY(opcode, this);
        case Instruction::SHR_VX: return SHR_VX(opcode, this);
        case Instruction::SUBN_VX_VY: return SUBN_VX_VY(opcode, this);
        case Instruction::SHL_VX: return SHL_VX(opcode, this);
        case Instruction::SNE_VX_VY: return SNE_VX_VY(opcode, this);
        case Instruction::LD_I: return LD_I(opcode, this);
        case Instruction::JMP_V0: return JP_V0(opcode, this);
        case Instruction::RND: return RND(opcode, this, &rand);
        case Instruction::DRW: return DRW(opcode, this);
        case Instruction::SKP: return SKP(opcode, this);
        case Instruction::SKNP: return SKNP(opcode, this);
        case Instruction::LD_VX_DT: return LD_VX_DT(opcode, this);
        case Instruction::LD_VX_K: return LD_VX_K(opcode, this);
        case Instruction::LD_DT: return LD_DT(opcode, this);
        case Instruction::LD_ST: return LD_ST(opcode, this);
        case Instruction::ADD_I_VX: return ADD_I_VX(opcode, this);
        case Instruction::LD_F_VX: return LD_F_VX(opcode, this);
        case Instruction::LD_B_VX: return LD_B_VX(opcode, this);
        case Instruction::LD_I_VX: return LD_I_VX(opcode, this);
        case Instruction::LD_VX_I: return LD_VX_I(opcode, this);

        default:
            throw bad_opcode(opcode);
    }

    if (delayTimer > 0) --delayTimer;
    if (soundTimer > 0) --soundTimer;
}