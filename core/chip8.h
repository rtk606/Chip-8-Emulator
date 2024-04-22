#pragma once

#include <array>
#include <string_view>
#include <fstream>
#include <iostream>
#include "random.h"


class Chip8 {
public:
	// Constants defining the display dimensions, ROM start address (0x000 to 0x1FF is reserved) and character sprites
	static constexpr int kWidth = 64;
	static constexpr int kHeight = 32;
	static constexpr int kStartAddress = 0x200;	
	static constexpr std::array<uint8_t, 0x50> kSprites = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	Chip8();

	// Chip 8 Methods
	void ResetChip8();
	void LoadRom(std::string_view filename);
	void Tick();
	bool IsPressed(uint8_t key) {
		return keypad.at(keymap.at(key)) == 1;
	}

	// Memory and register arrays
	std::array<uint8_t, 4096> memory = { 0 };				// 4,096 bytes of RAM (0x000 -> 0xFFF)
	std::array<uint8_t, 16> registers = { 0 };				// 16 general purpose registers (0xV0 -> 0xVF)
	std::array<uint16_t, 16> stack = { 0 };					// Stack with 16 levels of nested subroutines
	std::array<uint16_t, kWidth * kHeight> display = { 0 };	// 64x32 pixel monochrome video display

	std::array<uint8_t, 16> keypad = { 0 };					// 16-key hexadecimal keypad
	std::array<uint8_t, 0x10> keymap = {
			0x78, // X (C8: 0)
			0x31, // 1 (C8: 1)
			0x32, // 2 (C8: 2)
			0x33, // 3 (C8: 3)
			0x71, // Q (C8: 4)
			0x77, // W (C8: 5)
			0x65, // E (C8: 6)
			0x61, // A (C8: 7)
			0x73, // S (C8: 8)
			0x64, // D (C8: 9)
			0x7a, // Z (C8: A)
			0x63, // C (C8: B)
			0x34, // 4 (C8: C)
			0x72, // R (C8: D)
			0x66, // F (C8: E)
			0x76, // V (C8: F)
	};
	
	// Special purpose registers and timers
	uint8_t sp = { 0 };			// Stack pointer
	uint16_t pc = { 0 };		// Program counter
	uint16_t index = { 0 };		// Index register
	uint8_t delayTimer = { 0 };	// Delay timer
	uint8_t soundTimer = { 0 };	// Sound timer

	// Opcode storage
	uint16_t opcode = { 0 };

	Random rand;
};

/*
	------------------------------------------------
	|   Chip 8 Keyboard    |     Keyboard Map      |
	------------------------------------------------
	|   1    2    3    C   |    1    2    3    4   |
	|   4    5    6    D   |    Q    W    E    R   |
	|   7    8    9    E   |    A    S    D    F   |
	|   A    0    B    F   |    Z    X    C    V   |
	------------------------------------------------
*/