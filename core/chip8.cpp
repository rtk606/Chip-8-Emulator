#include "chip8.h"

Chip8::Chip8() {
	pc = kStartAddress;
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