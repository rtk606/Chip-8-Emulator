#include <chrono>
#include <cstdint>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "graphics.h"


GUI::GUI(Chip8* chip8, GLuint texture, GLubyte* pixels, GLFWwindow* window) : chip8(chip8), displayTexture(texture), displayPixels(pixels), window(window) {
    memoryEditor.Cols = 8;
}

void GUI::Tick(GLFWwindow* window) {
    if (ImGui::IsWindowFocused()) {
        for (int i = 0; i < 16; ++i) {
            chip8->keypad[i] = (glfwGetKey(window, chip8->keymap[i]) == GLFW_PRESS);
        }
    }

    ticks++;
    chip8->Tick();
}


void GUI::Render() {
    auto framerate = ImGui::GetIO().Framerate;

    RenderDisplay(framerate);
    RenderGeneral(framerate);
    RenderCPUState();
    RenderDebug();
    RenderMemory();
    RenderKeypadState();
    RenderStack();
}

void GUI::RenderDisplay(float framerate) {
    ImGui::Begin("Display", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::SetWindowSize(ImVec2(32 + (64 * kDisplayScale), 48 + (32 * kDisplayScale)));

    if (tickRequested) {
        tickRequested = false;
        Tick(window);
    }

    for (int i = 0; i < (clockSpeed / framerate); ++i) {
        Tick(window);
    }

    auto currentTime = std::chrono::high_resolution_clock::now();
    if ((currentTime - lastTimer).count() >= 16666666) {
        chip8->TickTimer();
        lastTimer = currentTime;
    }

    if (chip8->beep) {
        chip8->beep = false;
    }

    // Check if there's a need to update the display
    if (chip8->redraw) {        
        chip8->redraw = false;  

        GLubyte fg[3] = { 255, 255, 255 };  // White color for foreground
        GLubyte bg[3] = { 0, 0, 0 };        // Black color for background

        // Update the displayPixels based on chip8->display
        for (int i = 0; i < 64 * 32; ++i) {
            auto subpixel = chip8->display[i] ? fg : bg;
            for (int j = 0; j < 3; j++) {
                displayPixels[i * 3 + j] = subpixel[j];
            }
        }

        // Update the OpenGL texture
        glBindTexture(GL_TEXTURE_2D, displayTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, displayPixels);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ImGui::Image((void*)(intptr_t)displayTexture,ImVec2(64 * kDisplayScale, 32 * kDisplayScale));
    ImGui::End();
}

void GUI::RenderGeneral(float framerate) {
    ImGui::Begin("General", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::TextColored(labelColor, "FPS:");
    ImGui::SameLine();
    ImGui::Text("%f", framerate);

    ImGui::TextColored(labelColor, "Ticks:");
    ImGui::SameLine();
    ImGui::Text("%d", ticks);

    ImGui::TextColored(labelColor, "Display Scale:");
    ImGui::SameLine();
    ImGui::Text("%d", kDisplayScale);

    ImGui::TextColored(labelColor, "Clock:");
    ImGui::SameLine();
    ImGui::InputInt("Hz", &clockSpeed);

    ImGui::ColorEdit3("FG Color", (float*)&fgColor);
    ImGui::ColorEdit3("BG Color", (float*)&bgColor);

    ImGui::End();
}

void GUI::RenderCPUState() {
    ImGui::Begin("CPU State", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::TextColored(labelColor, "PC:");
    ImGui::SameLine();
    ImGui::Text("%04X", chip8->pc);

    ImGui::TextColored(labelColor, "IR:");
    ImGui::SameLine();
    ImGui::Text("%04X", chip8->index);

    ImGui::TextColored(labelColor, "OP:");
    ImGui::SameLine();
    ImGui::Text("%04X", chip8->opcode);

    ImGui::NewLine();
    ImGui::TextColored(labelColor, "Reg (V)");

    for (int i = 0; i < 16; ++i) {
        ImGui::TextColored(labelColor, "%01X:", i);
        ImGui::SameLine();
        ImGui::Text("%02X  ", chip8->registers[i]);
        if (i % 2 == 0) {
            ImGui::SameLine();
        }
    }

    ImGui::NewLine();

    ImGui::TextColored(labelColor, "DT:");
    ImGui::SameLine();
    ImGui::Text("%02X", chip8->delayTimer);

    ImGui::TextColored(labelColor, "ST:");
    ImGui::SameLine();
    ImGui::Text("%02X", chip8->soundTimer);

    ImGui::End();
}

void GUI::RenderDebug() {
    ImGui::Begin("Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::TextColored(labelColor, "Status");
    ImGui::Text(clockSpeed == 0 ? "Paused" : "Running");

    ImGui::TextColored(labelColor, "Clock");
    if (ImGui::Button(clockSpeed == 0 ? "Resume" : "Pause")) {
        if (clockSpeed == 0) {
            clockSpeed = prevClockSpeed;
        }
        else {
            prevClockSpeed = clockSpeed;
            clockSpeed = 0;
        }
    }

    if (ImGui::Button("Tick")) {
        tickRequested = true;
    }

    ImGui::End();
}

void GUI::RenderMemory() {
    ImGui::Begin("Memory Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    memoryEditor.DrawContents(std::data(chip8->memory), 4096);
    ImGui::End();
}

void GUI::RenderKeypadState() {
    ImGui::Begin("Keypad", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::TextColored(chip8->keypad[0x1] ? successColor : labelColor, "1");
    ImGui::SameLine();
    ImGui::TextColored(chip8->keypad[0x2] ? successColor : labelColor, "2");
    ImGui::SameLine();
    ImGui::TextColored(chip8->keypad[0x3] ? successColor : labelColor, "3");
    ImGui::SameLine();
    ImGui::TextColored(chip8->keypad[0xC] ? successColor : labelColor, "C");
    ImGui::Separator();

    ImGui::TextColored(chip8->keypad[0x4] ? successColor : labelColor, "4");
    ImGui::SameLine();
    ImGui::TextColored(chip8->keypad[0x5] ? successColor : labelColor, "5");
    ImGui::SameLine();
    ImGui::TextColored(chip8->keypad[0x6] ? successColor : labelColor, "6");
    ImGui::SameLine();
    ImGui::TextColored(chip8->keypad[0xD] ? successColor : labelColor, "D");
    ImGui::Separator();

    ImGui::TextColored(chip8->keypad[0x7] ? successColor : labelColor, "7");
    ImGui::SameLine();
    ImGui::TextColored(chip8->keypad[0x8] ? successColor : labelColor, "8");
    ImGui::SameLine();
    ImGui::TextColored(chip8->keypad[0x9] ? successColor : labelColor, "9");
    ImGui::SameLine();
    ImGui::TextColored(chip8->keypad[0xE] ? successColor : labelColor, "E");
    ImGui::Separator();

    ImGui::TextColored(chip8->keypad[0xA] ? successColor : labelColor, "A");
    ImGui::SameLine();
    ImGui::TextColored(chip8->keypad[0x0] ? successColor : labelColor, "0");
    ImGui::SameLine();
    ImGui::TextColored(chip8->keypad[0xB] ? successColor : labelColor, "B");
    ImGui::SameLine();
    ImGui::TextColored(chip8->keypad[0xF] ? successColor : labelColor, "F");
    ImGui::Separator();

    ImGui::End();
}

void GUI::RenderStack() {
    ImGui::Begin("Stack", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    for (int i = 0; i < 16; ++i) {
        ImGui::TextColored(chip8->sp == i ? successColor : labelColor, "%X", i);
        ImGui::SameLine();
        ImGui::Text("%04X", chip8->stack[i]);
    }

    ImGui::End();
}