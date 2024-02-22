//Author Mika Koivuranta: Input system test 06.02.2024 
#include <engine/Application.h>
#include <functional>

#include "engine/Input.h"

// Function to set up GLFW and create a window with a key callback
GLFWwindow* initializeGLFW(int width, int height, const char* title) 
{
    // Create a GLFW window

    GLFWwindow* window = CreateGLWindow(width, height, title);

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
    if (!window) 
    {
        return -1;
    }

    input::initialize(window);

    for (size_t i = 0; i < 4; i++)
    {
        std::string eventName = "Boost" + to_string(i);

        input::ConstructDigitalEvent(eventName);
        input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_A, { eventName });
    }

    for (size_t i = 0; i < 4; i++)
    {
        std::string eventName = "Move" + to_string(i);

        input::ConstructAnalogEvent(eventName);
        input::bindAnalogControllerInput(GLFW_JOYSTICK_4, { GLFW_GAMEPAD_AXIS_LEFT_X, GLFW_GAMEPAD_AXIS_LEFT_Y }, { eventName });

        std::cout << eventName << "\n";
    }
    
    input::ConstructDigitalEvent("shootEvent");

    input::bindDigitalInput(GLFW_KEY_SPACE, {"shootEvent"});
    input::bindDigitalInput(GLFW_GAMEPAD_BUTTON_A, {"shootEvent"});

    // Main loop
    while (!glfwWindowShouldClose(window)) 
    {
        // Process events
        //glfwPollEvents();
        input::update();

        std::string message = "";
        if (input::GetNewPress("Boost0"))
        {
            message = "Boost pressed";
        }
        if (input::GetNewRelease("Boost0"))
        {
            std::cout << "Boost release";
        }
        if (input::GetNewPress("Boost1"))
        {
            message = "Boost2 pressed";
        }
        if (input::GetNewRelease("Boost1"))
        {
            std::cout << "Boost2 release";
        }
        if (message.length() > 3)
        {
            std::cout << message << "\n";
        }

        //std::cout << moveforward1->getValue(0) << " " << moveforward1->getValue(1) << "\n";

        // TODO: Add & test API functions for easy creation, modification & access of all above
    }

    input::uninitialize();

    // Clean up GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
