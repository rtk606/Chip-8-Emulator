#include <chrono>
#include <cstdint>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "graphics.h"
#include "parser.h"


GUI::GUI(Chip8* chip8, GLuint texture, GLubyte* pixels, GLFWwindow* window) : chip8(chip8), displayTexture(texture), displayPixels(pixels), window(window) {
    memoryEditor.Cols = 8;
    memoryEditor.GotoAddr = 0x200;
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
    RenderRom();
    RenderDisassembler();
    RenderCPUState();
    RenderDebug();
    RenderMemory();
    RenderKeypadState();
    RenderStack();
}

// Renders the game (reminder: no title bar)
void GUI::RenderDisplay(float framerate) {
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2(FLT_MAX, FLT_MAX)); // Set minimum size and allow maximum expansion
    ImGui::Begin("Display", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);

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

    if (chip8->redraw) {
        chip8->redraw = false;

        // Use user-specified custom coloring
        GLubyte fg[3] = { foregroundColour.x * 255,
                          foregroundColour.y * 255,
                          foregroundColour.z * 255 };
        GLubyte bg[3] = { backgroundColour.x * 255,
                          backgroundColour.y * 255,
                          backgroundColour.z * 255 };

        // Update the displayPixels based on chip8->display
        for (int i = 0; i < Chip8::kWidth * Chip8::kHeight; ++i) {
            auto subpixel = chip8->display[i] ? fg : bg;
            for (int j = 0; j < 3; j++) {
                displayPixels[i * 3 + j] = subpixel[j];
            }
        }

        // Update the OpenGL texture
        glBindTexture(GL_TEXTURE_2D, displayTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Chip8::kWidth, Chip8::kHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, displayPixels);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ImGui::Image((void*)(intptr_t)displayTexture, ImVec2(Chip8::kWidth * kDisplayScale, Chip8::kHeight * kDisplayScale));
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
    ImGui::PushItemWidth(100);
    ImGui::InputInt("Hz", &clockSpeed);
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(150);
    ImGui::ColorEdit3("FG Color", (float*)&foregroundColour);
    ImGui::ColorEdit3("BG Color", (float*)&backgroundColour);


    ImGui::SliderInt("Display Scale", &kDisplayScale, 1, 20);
    ImGui::PopItemWidth();


    ImGui::End();
}

void GUI::RenderRom() {
    float romFactorH = 0.2;
    float romFactorW = 0.25;

    ImGui::SetNextWindowSize(ImVec2(Chip8::kWindowWidth * romFactorW, Chip8::kWindowHeight * romFactorH));
    ImGui::Begin("ROM", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::TextWrapped("ROM Title: %s", chip8->romTitle.c_str());
    ImGui::TextWrapped("ROM Path: %s", chip8->romPath.c_str());
    ImGui::TextWrapped("Rom Size (Bytes): %d", chip8->romSize);
    ImGui::End();
}

void GUI::RenderDisassembler() {
    float disassemblerH = 1.0f;
    float disassemblerW = 0.2f;
    bool followProgramCounter = true;

    ImGui::Begin("Disassembler", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Checkbox("Follow PC", &followProgramCounter);
    ImGui::Separator();

    // Display the current instruction at the program counter
    uint16_t current_instruction = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
    ImGui::Text("PC: %04X | Current Instruction: %04X", chip8->pc, current_instruction);
    ImGui::Separator();

    // Start displaying memory content
    ImGui::BeginChild("Instructions", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);

    int endAddress = std::min(int(chip8->memory.size()), chip8->pc + 20 * 2); 
    for (int address = std::max(0, chip8->pc - 20 * 2); address < endAddress; address += 2) {
        uint16_t opcode = (chip8->memory[address] << 8) | chip8->memory[address + 1];
        auto disassembly = DisassembleOpcode(opcode, address);
        char lineBuffer[256];

        // Format the display line
        snprintf(lineBuffer, sizeof(lineBuffer), "0x%04X | %04X | %s", address, opcode, disassembly.c_str());

        // Highlight the current PC
        if (address == chip8->pc) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.8f, 0.0f, 1.0f)); 
            ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, ImVec4(0.5f, 0.7f, 1.0f, 0.5f)); 
            ImGui::Selectable(lineBuffer, true, ImGuiSelectableFlags_AllowDoubleClick);
            ImGui::PopStyleColor(2);
            if (followProgramCounter)
                ImGui::SetScrollHereY();
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.8f, 0.0f, 1.0f)); 
            ImGui::Text("%s", lineBuffer);
            ImGui::PopStyleColor(); 
        }
    }

    ImGui::EndChild();
    ImGui::End();
}


std::string GUI::DisassembleOpcode(uint16_t opcode, int address) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    Instruction instr = parse(Opcode(opcode));

    switch (instr) {
    case Instruction::CLS:
        ss << "CLS";
        break;
    case Instruction::RET:
        ss << "RET";
        break;
    case Instruction::JMP:
        ss << "JP " << std::setw(4) << (opcode & 0x0FFF);
        break;
    case Instruction::CALL:
        ss << "CALL " << std::setw(4) << (opcode & 0x0FFF);
        break;
    case Instruction::SE_VX_KK:
        ss << "SE V" << ((opcode & 0x0F00) >> 8) << ", " << std::setw(2) << std::setfill('0') << (opcode & 0x00FF);
        break;
    case Instruction::SNE_VX_KK:
        ss << "SNE V" << ((opcode & 0x0F00) >> 8) << ", " << std::setw(2) << std::setfill('0') << (opcode & 0x00FF);
        break;
    case Instruction::SE_VX_VY:
        ss << "SE V" << ((opcode & 0x0F00) >> 8) << ", V" << ((opcode & 0x00F0) >> 4);
        break;
    case Instruction::LD_VX_KK:
        ss << "LD V" << ((opcode & 0x0F00) >> 8) << ", " << std::setw(2) << std::setfill('0') << (opcode & 0x00FF);
        break;
    case Instruction::ADD_VX_KK:
        ss << "ADD V" << ((opcode & 0x0F00) >> 8) << ", " << std::setw(2) << std::setfill('0') << (opcode & 0x00FF);
        break;
    case Instruction::LD_VX_VY:
        ss << "LD V" << ((opcode & 0x0F00) >> 8) << ", V" << ((opcode & 0x00F0) >> 4);
        break;
    case Instruction::OR_VX_VY:
        ss << "OR V" << ((opcode & 0x0F00) >> 8) << ", V" << ((opcode & 0x00F0) >> 4);
        break;
    case Instruction::AND_VX_VY:
        ss << "AND V" << ((opcode & 0x0F00) >> 8) << ", V" << ((opcode & 0x00F0) >> 4);
        break;
    case Instruction::XOR_VX_VY:
        ss << "XOR V" << ((opcode & 0x0F00) >> 8) << ", V" << ((opcode & 0x00F0) >> 4);
        break;
    case Instruction::ADD_VX_VY:
        ss << "ADD V" << ((opcode & 0x0F00) >> 8) << ", V" << ((opcode & 0x00F0) >> 4);
        break;
    case Instruction::SUB_VX_VY:
        ss << "SUB V" << ((opcode & 0x0F00) >> 8) << ", V" << ((opcode & 0x00F0) >> 4);
        break;
    case Instruction::SHR_VX:
        ss << "SHR V" << ((opcode & 0x0F00) >> 8);
        break;
    case Instruction::SUBN_VX_VY:
        ss << "SUBN V" << ((opcode & 0x0F00) >> 8) << ", V" << ((opcode & 0x00F0) >> 4);
        break;
    case Instruction::SHL_VX:
        ss << "SHL V" << ((opcode & 0x0F00) >> 8);
        break;
    case Instruction::SNE_VX_VY:
        ss << "SNE V" << ((opcode & 0x0F00) >> 8) << ", V" << ((opcode & 0x00F0) >> 4);
        break;
    case Instruction::LD_I:
        ss << "LD I, " << std::setw(4) << (opcode & 0x0FFF);
        break;
    case Instruction::JMP_V0:
        ss << "JP V0, " << std::setw(4) << (opcode & 0x0FFF);
        break;
    case Instruction::RND:
        ss << "RND V" << ((opcode & 0x0F00) >> 8) << ", " << std::setw(2) << std::setfill('0') << (opcode & 0x00FF);
        break;
    case Instruction::DRW:
        ss << "DRW V" << ((opcode & 0x0F00) >> 8) << ", V" << ((opcode & 0x00F0) >> 4) << ", " << (opcode & 0x000F);
        break;
    case Instruction::SKP:
        ss << "SKP V" << ((opcode & 0x0F00) >> 8);
        break;
    case Instruction::SKNP:
        ss << "SKNP V" << ((opcode & 0x0F00) >> 8);
        break;
    case Instruction::LD_VX_DT:
        ss << "LD V" << ((opcode & 0x0F00) >> 8) << ", DT";
        break;
    case Instruction::LD_VX_K:
        ss << "LD V" << ((opcode & 0x0F00) >> 8) << ", K";
        break;
    case Instruction::LD_DT:
        ss << "LD DT, V" << ((opcode & 0x0F00) >> 8);
        break;
    case Instruction::LD_ST:
        ss << "LD ST, V" << ((opcode & 0x0F00) >> 8);
        break;
    case Instruction::ADD_I_VX:
        ss << "ADD I, V" << ((opcode & 0x0F00) >> 8);
        break;
    case Instruction::LD_F_VX:
        ss << "LD F, V" << ((opcode & 0x0F00) >> 8);
        break;
    case Instruction::LD_B_VX:
        ss << "LD B, V" << ((opcode & 0x0F00) >> 8);
        break;
    case Instruction::LD_I_VX:
        ss << "LD [I], V" << ((opcode & 0x0F00) >> 8);
        break;
    case Instruction::LD_VX_I:
        ss << "LD V" << ((opcode & 0x0F00) >> 8) << ", [I]";
        break;
    default:
        ss << "Unknown Opcode: " << std::setw(4) << opcode;
        break;
    }
    return ss.str();
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

    ImGui::TextColored(labelColor, "Status: ");
    ImGui::SameLine();
    ImGui::Text(clockSpeed == 0 ? "paused" : "running");

    ImGui::TextColored(labelColor, "Clock: ");
    ImGui::SameLine();
    if (ImGui::Button(clockSpeed == 0 ? "resume" : "pause")) {
        if (clockSpeed == 0) {
            clockSpeed = prevClockSpeed;
        }
        else {
            prevClockSpeed = clockSpeed;
            clockSpeed = 0;
        }
    }

    if (ImGui::Button("tick")) {
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

    ImGui::Columns(2, NULL, true);

    // First column: Display the first 8 entries
    for (int i = 0; i < 8; ++i) {
        ImGui::TextColored(chip8->sp == i ? successColor : labelColor, "%X:", i);
        ImGui::SameLine();
        ImGui::Text("%04X", chip8->stack[i]);
    }

    ImGui::NextColumn();

    // Second column: Display the next 8 entries
    for (int i = 8; i < 16; ++i) {
        ImGui::TextColored(chip8->sp == i ? successColor : labelColor, "%X:", i);
        ImGui::SameLine();
        ImGui::Text("%04X", chip8->stack[i]);
    }

    ImGui::Columns(1);
    ImGui::End();
}
