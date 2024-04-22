#pragma once

#include <random>

// For opcode CXNN the requires random number between 0-255 (0x00-0xFF) 
class Random {
public:
	template <typename T>
	void seed(T val) {
		rand_engine.seed(val);
	}

	// Dont discard the random number
	[[nodiscard]] virtual uint8_t operator()() {
		return rand_dist(rand_engine);
	}

private:
	std::default_random_engine rand_engine;
	std::uniform_int_distribution<uint8_t> rand_dist{ 0x00, 0xFF };
};