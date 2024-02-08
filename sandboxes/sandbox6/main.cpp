//Author Sauli Hanell 2.1.2024 
#include <engine/Application.h>
#include <functional>

#include "BulletSystem.h"
#include "MenuSystem.h"

#include "engine/Input.h"

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

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Process events
        glfwPollEvents();
    }

    // Clean up GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
