#pragma once

#include <array>
#include <string_view>
#include <fstream>
#include <iostream>
#include "random.h"
#include "graphics.h"

class Chip8 {
public:
    Chip8(GLFWwindow* window);

    void ResetChip8();
    bool LoadRom(std::string_view filename);
    void Tick();
    void TickTimer();
    bool IsPressed(GLFWwindow* window, uint8_t key);

    static constexpr int kWindowWidth = 1280;
    static constexpr int kWindowHeight = 720;
    static constexpr int kWidth = 64;
    static constexpr int kHeight = 32;
    static constexpr int kStartAddress = 0x200;    
    static constexpr std::array<uint8_t, 80> kSprites {
        // Sprites 0-F
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

    GLFWwindow* window = nullptr;

    std::array<GLuint, 16> keymap = {
        GLFW_KEY_X,                         // |       | X (0) |       |       |
        GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, // | 1 (1) | 2 (2) | 3 (3) |       |
        GLFW_KEY_Q, GLFW_KEY_W, GLFW_KEY_E, // | Q (4) | W (5) | E (6) |       |
        GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, // | A (7) | S (8) | D (9) |       |
        GLFW_KEY_Z, GLFW_KEY_C,             // | Z (A) |       | C (B) |       |
        GLFW_KEY_4,                         // |       |       |       | 4 (C) |
        GLFW_KEY_R,                         // |       |       |       | R (D) |
        GLFW_KEY_F,                         // |       |       |       | F (E) |
        GLFW_KEY_V,                         // |       |       |       | V (F) |
    };

    std::array<uint8_t, 4096> memory = { 0 };
    std::array<uint8_t, 16> registers = { 0 };
    std::array<uint16_t, 16> stack = { 0 };
    std::array<uint16_t, kWidth* kHeight> display = { 0 };
    std::array<uint8_t, 16> keypad = { 0 };

    uint8_t sp = 0;
    uint16_t pc = 0;
    uint16_t index = 0;
    uint8_t delayTimer = 0;
    uint8_t soundTimer = 0;
    uint16_t opcode = 0;

    bool beep = false;
    bool redraw = false;

    // ROM GUI variables
    std::string romTitle = "N/A";
    std::string romPath = "N/A";
    int romSize = 0;

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