//Author Mika Koivuranta: preload textures test 23/02/24
#include "engine/Application.h"
#include "engine/Input.h"
#include "engine/Constants.h"

#include <functional>
#include <filesystem>

std::map<std::string, engine::Texture*> Textures;
std::map<std::string, engine::Model*> Models;

// Function to set up GLFW and create a window with a key callback
GLFWwindow* initializeGLFW(int width, int height, const char* title) 
{
    // Create a GLFW window

    GLFWwindow* window = CreateGLWindow(width, height, title);

    return window;
}

void processDirectory(const std::string& path, void (*processingFunction)(const std::string&, const std::string&, const std::string&, unsigned int, bool),
unsigned int filteringType, bool flip)
{
    using namespace std::filesystem;

    for (const auto& directoryEntry : directory_iterator(path)) 
    {
        std::string PathName = directoryEntry.path().string();

        if (is_directory(directoryEntry.path()))
        {
            // If a directory, call the function recursively
            processDirectory(PathName, processingFunction, filteringType, flip);
            return;
        }
        
        // Name creation
        std::string Name = PathName;
        size_t position = path.find(path);
        if (position != std::string::npos)
        {
            // Erase the original path from name
            Name.erase(position, path.length());
        }

        if (!Name.empty()) 
        {
            // Erase the first character, which is "/"
            Name = Name.substr(1);
        }

        std::string extension = "";
        position = Name.find_last_of('.');
        if (position != std::string::npos)
        {
            extension = Name.substr(position + 1); // Save the extension
            Name = Name.substr(0, position); // Remove extension
        }

        // Process file
        processingFunction(PathName, Name, extension, filteringType, flip);
    }
}

static void addTexture(const std::string& path, const std::string& name, const std::string& extension, unsigned int filteringType, bool flip)
{
    if (extension != "png")
    {
        return; // Wrong file format
    }

    engine::Texture* NewTexture = new engine::Texture(path, filteringType, flip, false);
    
    if (!NewTexture)
    {
        printf("Failed to load a texture %s from %s \n", name.c_str(), path.c_str());
        return;
    }

    // Place to texture list
    Textures.emplace(name, NewTexture);

    // Log
    printf("Loaded a texture %s from %s \n", name.c_str(), path.c_str());
}

static void addModel(const std::string& path, const std::string& name, const std::string& extension, unsigned int filteringType, bool flip)
{
    if (extension != "obj")
    {
        return; // Wrong file format
    }

    // This is stupid: Why does model constructor add asset path?
    engine::Model* newModel = new engine::Model(path, true);

    if (!newModel)
    {
        printf("Failed to load a model %s from %s \n", name.c_str(), path.c_str());
        return;
    }

    // Place to texture list
    Models.emplace(name, newModel);

    // Log
    printf("Loaded a model %s from %s \n", name.c_str(), path.c_str());
}

void preloadTextures(const std::string& path, unsigned int filteringType, bool flip)
{
    processDirectory(assetPath + path, addTexture, filteringType, flip);
}

void preloadModels(const std::string& path)
{
    processDirectory(assetPath + path, addModel, 0, false);
}

engine::Texture* findTexture(const std::string& textureName)
{
    auto it = Textures.find(textureName);
    if (it != Textures.end())
    {
        return (*it).second;
    }

    return nullptr;
}

engine::Model* findModel(const std::string& modelName)
{
    auto it = Models.find(modelName);
    if (it != Models.end())
    {
        return (*it).second;
    }

    return nullptr;
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

    preloadTextures("menuUI", GL_NEAREST, false);
    preloadModels("3dmodels");

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
        input::bindAnalogControllerInput(i, { GLFW_GAMEPAD_AXIS_LEFT_X, GLFW_GAMEPAD_AXIS_LEFT_Y }, { eventName });
    }
    
    input::bindAnalogInput(GLFW_KEY_RIGHT, { "Move0" }, GLFW_GAMEPAD_AXIS_LEFT_X);
    input::bindAnalogInput(GLFW_KEY_LEFT, { "Move0" }, GLFW_GAMEPAD_AXIS_LEFT_X, -1);
    
    input::bindAnalogInput(GLFW_KEY_UP, { "Move0" }, GLFW_GAMEPAD_AXIS_LEFT_Y);
    input::bindAnalogInput(GLFW_KEY_DOWN, { "Move0" }, GLFW_GAMEPAD_AXIS_LEFT_Y, -1);
    
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

        std::cout << input::GetInputValue("Move0", 0) << input::GetInputValue("Move0", 1) << "\n";
    }

    input::uninitialize();

    // Clean up GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
