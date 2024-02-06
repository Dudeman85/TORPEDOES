//Author Mika Koivuranta: Input system test 06.02.2024 
#include <engine/Application.h>
#include <functional>

#include "engine/Input.h"

// Function to set up GLFW and create a window with a key callback
GLFWwindow* initializeGLFW(int width, int height, const char* title) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    // Set GLFW to not create an OpenGL context (we'll do it later)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    return window;
}


int main()
{
    // Set the window dimensions and title
    int width = 800;
    int height = 600;
    const char* title = "GLFW Key Callback Example";

    // Initialize GLFW and create a window with a key callback
    GLFWwindow* window = initializeGLFW(width, height, title);
    if (!window) {
        return -1;
    }

    input::initialize(window);

    // Main loop
    while (!glfwWindowShouldClose(window)) 
    {
        // Process events
        glfwPollEvents();
        input::update(window);

        // DONE: Add & test InputButtons
        // DONE: Add InputEvents
        
        // DOING: Test inputEvents
        
        // TODO: Add & test InputAxis
        // TODO: Add & test InputAxisEvents
        // TODO: Add & test InputCustom
        // TODO: Add & test API functions for easy creation, modification & access of all above
    }

    input::uninitialize();

    // Clean up GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
