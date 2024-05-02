#include <iostream>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "core/chip8.h"
#include "core/graphics.h"

int main(int argc, char** argv) {
    // Ensure correct command-line usage
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << "./chip8 <Rom>" << std::endl;
        return EXIT_FAILURE;
    }

    // Initialize the GLFW library for creating windows
    if (!glfwInit()) {
        std::cerr << "Error: glfw initialisation failed." << std::endl;
        return EXIT_FAILURE;
    }

    // Create a GLFW window for the Chip-8 emulator (1280x760)
    GLFWwindow* window = glfwCreateWindow(Chip8::kWindowWidth, Chip8::kWindowHeight, "Chip-8 Emulator", NULL, NULL);
    if (!window) {
        std::cerr << "Error: window creation failed." << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync


    // Initialize ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Initialize the display pixel buffer with all pixels set to black (0, 0, 0)
    GLubyte displayPixels[Chip8::kWidth * Chip8::kHeight * 3] = { 0 };

    // Loop through each pixel in the display (totaling 2048 pixels for a 64x32 resolution)
    for (int i = 0; i < Chip8::kWidth * Chip8::kHeight; ++i) {
        displayPixels[i * 3] = 255;        
        displayPixels[i * 3 + 1] = 255;     
        displayPixels[i * 3 + 2] = 255;
    }

    // Create and setup the OpenGL texture for display
    GLuint displayTexture;
    glGenTextures(1, &displayTexture);
    glBindTexture(GL_TEXTURE_2D, displayTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Chip8::kWidth, Chip8::kHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, displayPixels);

    // Initialize ImGui GLFW and OpenGL3 renderers
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Setup Chip-8 Interpreter
    Chip8 chip8(window);
    chip8.ResetChip8();

    // Load the specified ROM
    if (!chip8.LoadRom(argv[1])) {
        std::cerr << "Unable to load specified ROM: " << argv[1] << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    GUI gui(&chip8, displayTexture, displayPixels, window);    
    
    // Main rendering loop
    auto clearColor = ImVec4(0.024f, 0.024f, 0.03f, 1.00f);
    while (!glfwWindowShouldClose(window)) {
        // Handle window events
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render GUI elements
        gui.Render();

        ImGui::Render();
        int displayWidth, displayHeight;
        glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
        glViewport(0, 0, displayWidth, displayHeight);
        glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Handle additional platform windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        // Swap the front and back buffers
        glfwSwapBuffers(window);
    }


    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}