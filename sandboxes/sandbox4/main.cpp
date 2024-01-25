#include <engine/Application.h>

using namespace std;
using namespace engine;

ECS ecs;

int main()
{
	NO_OPENAL = true;
	//Create the window and OpenGL context before creating EngineLib
	GLFWwindow* window = CreateGLWindow(1250, 1000, "Window");
	//Create the camera
	Camera cam = Camera(1250, 1000);

	//Initialize the default engine library
	EngineLib engine;

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

	return 0;
}
