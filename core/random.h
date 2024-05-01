#pragma once

#include <random>

// For opcode CXNN the requires random number between 0-255 (0x00-0xFF) 
class Random {
public:
    template <typename T>
    void seed(T val) {
        rand_engine.seed(val);
    }

    [[nodiscard]] uint8_t operator()() {
        return static_cast<uint8_t>(rand_dist(rand_engine));
    }

private:
    std::default_random_engine rand_engine;
    std::uniform_int_distribution<int> rand_dist{0, 255};
};
