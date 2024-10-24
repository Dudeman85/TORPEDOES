#include <engine/Application.h>

//Sanity saver
using namespace engine;

int main()
{
	//Create the window and OpenGL context before creating EngineLib
	//Parameters define window size(x,y) and name
	GLFWwindow* window = CreateGLWindow(800, 600, "Window");

	//Initialize the engine, this must be called after creating a window
	EngineInit();

	//Create the camera
	Camera cam = Camera(800, 600);

	//Move the camera back a bit
	cam.SetPosition(Vector3(0, 0, 10));

	//Set the bacground color
	SpriteRenderSystem::SetBackgroundColor(0, 150, 0);

	//Load some sprites
	Texture strawberry("strawberry.png");

	//Create a new entity
	ecs::Entity entity = ecs::NewEntity();
	//Add the transform and SpriteRenderer components required for rendering a sprite
	ecs::AddComponent(entity, new Transform{ .position = Vector3(0, 40, 0), .rotation = Vector3(0, 0, 45), .scale = Vector3(50) });
	ecs::AddComponent(entity, new SpriteRenderer{ .texture = &strawberry });

	//Game Loop
	while (!glfwWindowShouldClose(window))
	{
		//Close window when Esc is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		//Getting basic input and moving
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			TransformSystem::Translate(entity, Vector3(0, 1, 0));
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			TransformSystem::Translate(entity, Vector3(-1, 0, 0));
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			TransformSystem::Translate(entity, Vector3(0, -1, 0));
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			TransformSystem::Translate(entity, Vector3(1, 0, 0));
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			TransformSystem::Rotate(entity, Vector3(0, 0, -10));
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			TransformSystem::Rotate(entity, Vector3(0, 0, 10));

		//Update all engine systems, this usually should go last in the game loop
		Update(&cam);

		//OpenGL stuff, goes very last
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}