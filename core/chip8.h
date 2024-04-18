#pragma once

#include <array>
#include <string_view>
#include <fstream>
#include <iostream>


class Chip8 {
public:
	// Constants defining the display dimensions and ROM start address
	static constexpr int kWidth = 64;
	static constexpr int kHeight = 32;
	static constexpr int kStartAddress = 0x200;

	Chip8();

	// Chip 8 Methods
	void LoadRom(std::string_view filename);

	// Memory and register arrays
	std::array<uint8_t, 4096> memory = { 0 };				// 4,096 bytes of RAM (0x000 -> 0xFFF)
	std::array<uint8_t, 16> registers = { 0 };				// 16 general purpose registers (0xV0 -> 0xVF)
	std::array<uint16_t, 16> stack = { 0 };					// Stack with 16 levels of nested subroutines
	std::array<uint16_t, kWidth * kHeight> display = { 0 };	// 64x32 pixel monochrome video display
	std::array<uint8_t, 16> keypad = { 0 };					// 16-key hexadecimal keypad
	
	// Special purpose registers and timers
	uint8_t sp = { 0 };			// Stack pointer
	uint16_t pc = { 0 };		// Program counter
	uint16_t index = { 0 };		// Index register
	uint8_t delayTimer = { 0 };	// Delay timer
	uint8_t soundTimer = { 0 };	// Sound timer

	// Opcode storage
	uint16_t opcode = { 0 };

	// REMINDER: Set the PC to kStartAddress  
};