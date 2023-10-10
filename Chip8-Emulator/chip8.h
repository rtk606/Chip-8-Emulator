#pragma once

#include "cpu.h"
#include "memory.h"

namespace rtk {

	class Chip8 {
	public:
		void Reset();
		void LoadROM(std::string_view filename);



	private:
		Cpu cpu;
		Memory ram;
		//Display display;
		//Input input;
	};

}