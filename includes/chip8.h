#pragma once

#include "cpu.h"
#include "memory.h"
#include "random.h"
#include "types.h"

namespace rtk {

	class Chip8 {
	public:
		void Reset();
		void LoadROM(std::string_view filename);
		void Cycle();

		void OP_SKP(Opcode opcode);
		void OP_SKNP(Opcode opcode);
		void OP_LD_VX_K(Opcode opcode);

		std::array<uint8_t, 16> keypad{};
		std::array<uint32_t, 64 * 32> video{};

	private:
		Cpu cpu;
		Memory ram;
		Random rand;
	};
}