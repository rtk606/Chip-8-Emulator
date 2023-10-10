#pragma once

#include "Types.hpp"
#include <array>
#include <iostream>

class Chip8 {
public:
	std::array<uint8_t, 4096> memory{};
	std::array<uint8_t, 16> registers{};
	std::array<uint8_t, 16> stack{};
	uint16_t index{};
	uint16_t program_counter{};
	uint16_t stack_pointer{};
	uint8_t delayTimer{};
	uint8_t soundTimer{};
	std::array<uint8_t, 16> keypad{};
	std::array<uint32_t, 64 * 32> video{};
};