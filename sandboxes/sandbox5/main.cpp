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
    input::DigitalInputEvent* shootEvent = new input::DigitalInputEvent("shootEvent");
    // TODO: Make an API for this, so no need to call "new"

    input::DigitalInputEvent* Boost = new input::DigitalInputEvent("Boost");

    input::AnalogInputEvent* moveforward1 = new input::AnalogInputEvent("movePlayer1");
    input::AnalogInputEvent* moveforward2 = new input::AnalogInputEvent("movePlayer2");
    input::AnalogInputEvent* moveforward3 = new input::AnalogInputEvent("movePlayer3");
    input::AnalogInputEvent* moveforward4 = new input::AnalogInputEvent("movePlayer4");
    // TODO: Make an API for this, so no need to call "new"

    // TODO: Add GLFW_GAMEPAD axis to analog binding
    input::bindAnalogInput(GLFW_JOYSTICK_1, {"movePlayer1"}, { GLFW_GAMEPAD_AXIS_LEFT_X, GLFW_GAMEPAD_AXIS_LEFT_Y });
    input::bindAnalogInput(GLFW_JOYSTICK_2, {"movePlayer2"});
    input::bindAnalogInput(GLFW_JOYSTICK_3, {"movePlayer3"});
    input::bindAnalogInput(GLFW_JOYSTICK_4, {"movePlayer3"});

    input::bindDigitalControllerInput(GLFW_JOYSTICK_1, GLFW_GAMEPAD_BUTTON_A, { "movePlayer3" });

    input::bindDigitalInput(GLFW_KEY_SPACE, {"shootEvent"});
    input::bindDigitalInput(GLFW_GAMEPAD_BUTTON_A, {"shootEvent"});

    // Main loop
    while (!glfwWindowShouldClose(window)) 
    {
        // Process events
        //glfwPollEvents();
        input::update();

        std::string message = "";
        if (Boost->isNewPress())
        {
            message = "Boost pressed";
        }
        if (Boost->isNewRelease())
        {
            std::cout << "Boost release";
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
