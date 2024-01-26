# Getting Started

## Downloading game-engine

Engine can be found at https://repo.kamit.fi/spring2024_technology_project_2/team5

You can download source files as archive or clone with git.

You need to build the project with CMake before you can use the engine.
First add filepath to source, and then define path to build folder, for example C:/....filepath
It is recommended to create build folder into your source folder, for example C:/....filepath.../build
After defining filepaths, you can first configure the project. CMake asks you to specify the generator for the project. Select Visual Studio compiler you are using. Leave other fields empty or default.
Once configuration is completed, generate build files and you can open project.

***This engine uses OpenAL. You might need to download OpenAL 1.1 Windows Installer from their website***


## Creating window and game loop

Create new .cpp file for your game and add following lines at the beginning:
```cpp
#include <engine/Application.h>

//Creates a global instance of the ECS manager
ECS ecs;
```

In your main function:
```cpp
//Create the window and OpenGL context before creating EngineLib
//Parameters define window size(x,y) and name
GLFWwindow* window = engine::CreateWindow(800, 600, "Window");
//Initialize the default engine library
engine::EngineLib engine;
//Create the camera
engine::Camera cam = engine::Camera(800, 600);

//changes window backround color, values 0-1, RGB
engine::RenderSystem::SetBackgroundColor(0, .5, .1);
```

Once the window is created, you can create the game loop

```cpp
//Game Loop
while (!glfwWindowShouldClose(window))
{
	//Close window when Esc is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
		
    //Update all engine systems, this usually should go last in the game loop
	//For greater control of system execution, you can update each one manually
	engine.Update(&cam);

	//OpenGL stuff, goes very last
	glfwSwapBuffers(window);
	glfwPollEvents();
}
glfwTerminate();

```
