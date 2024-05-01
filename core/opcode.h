#pragma once

#include <cinttypes>

struct Opcode {
    uint16_t in;

    Opcode(uint16_t opcode) : in(opcode) {}

    [[nodiscard]] uint8_t x() const { return (in & 0x0F00) >> 8; }
    [[nodiscard]] uint8_t y() const { return (in & 0x00F0) >> 4; }
    [[nodiscard]] uint8_t byte() const { return in & 0x00FF; }
    [[nodiscard]] uint16_t address() const { return in & 0x0FFF; }
    [[nodiscard]] uint16_t high() const { return in >> 12; }
    [[nodiscard]] uint16_t low() const { return in & 0x000F; }
};