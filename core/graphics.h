#pragma once

#include <chrono>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_memory_editor/imgui_memory_editor.h>
#include "chip8.h"

using high_resolution_time_point = std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<long long, std::ratio<1, 1000000000>>>;
class Chip8;

class GUI {
public:
    GUI(Chip8* chip8, GLuint displayTexture, GLubyte* displayPixels, GLFWwindow* window);
    void Render();
    int kDisplayScale = 15;

private:
    Chip8* chip8;
    GLuint displayTexture;
    GLubyte* displayPixels;
    GLFWwindow* window;

    // RGBA
    ImVec4 foregroundColour = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 backgroundColour = ImVec4(0.047f, 0.047f, 0.047f, 1.0f);
    ImVec4 labelColor = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
    ImVec4 successColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

    high_resolution_time_point lastTimer = std::chrono::high_resolution_clock::now();

    int ticks = 0;
    bool tickRequested = false;
    int clockSpeed = 960;
    int prevClockSpeed = clockSpeed;

    void Tick(GLFWwindow* window);
    void RenderDisplay(float framerate);
    void RenderRom();
    void RenderDisassembler();
    void RenderGeneral(float);
    void RenderCPUState();
    void RenderDebug();
    void RenderMemory();
    void RenderKeypadState();
    void RenderStack();

    std::string DisassembleOpcode(uint16_t opcode, int address);


    MemoryEditor memoryEditor;
};
