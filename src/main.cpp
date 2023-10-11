#include <iostream>
#include <chrono>
#include <array>

#include "platform.h"
#include "chip8.h"

namespace Chip8Config {
    constexpr int VIDEO_WIDTH = 64;  // CHIP-8 default video width
    constexpr int VIDEO_HEIGHT = 32; // CHIP-8 default video height
    constexpr int expectedArgCount = 4;
}

int main(int argc, char** argv) {
    using namespace Chip8Config;

    // Check command line arguments
    if (argc != expectedArgCount) {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
        return EXIT_FAILURE;
    }

    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    std::string romFilename = argv[3];

    // Initialize platform and CHIP-8 emulator
    Platform platform{"CHIP-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT};
    rtk::Chip8 chip8; 
    chip8.LoadROM(romFilename.c_str());

    // Calculate video pitch
    auto videoPitch = sizeof(chip8.video[0]) * chip8.video.size();

    using namespace std::chrono;
    auto lastCycleTime = high_resolution_clock::now();
    bool quit = false;

    // Main loop
    while (!quit) {
        quit = platform.ProcessInput(chip8.keypad);
        
        auto currentTime = high_resolution_clock::now();
        auto deltaTime = duration<float, milliseconds::period>(currentTime - lastCycleTime).count();

        if (deltaTime > cycleDelay) {
            lastCycleTime = currentTime;
            chip8.Cycle();
            platform.Update(chip8.video.data(), videoPitch);
        }
    }

    return EXIT_SUCCESS;
}
