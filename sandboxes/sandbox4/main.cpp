#include <engine/Application.h>
#include <engine/ResourceManagement.h>
#include <thread>

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
	Camera cam = Camera(1600 * 4, 1200 * 4);
	cam.perspective = false;

	//Move the camera back a bit
	cam.SetPosition(Vector3(0, 0, 1000));

	//Set the bacground color
	SpriteRenderSystem::SetBackgroundColor(0, 150, 0);

	ecs::Entity e1 = ecs::NewEntity();
	ecs::Entity e2 = ecs::NewEntity();
	ecs::Entity e3 = ecs::NewEntity();
	ecs::Entity e4 = ecs::NewEntity();

	Texture tex1("../../../assets/GUI/UI_Green_Cannon_Icon.png");
	Texture tex2("../../../assets/GUI/UI_Red_Cannon_Icon.png");

	ecs::AddComponent(e1, Transform{ .rotation = {0, 0, 0}, .scale = {1000} });
	ecs::AddComponent(e1, SpriteRenderer{.texture = &tex1});
	ecs::AddComponent(e2, Transform{ .position = {0, 0, -1}, .rotation = {12, 0, 0}, .scale = {1000} });
	ecs::AddComponent(e2, SpriteRenderer{.texture = &tex2});

	//Game Loop
	while (!glfwWindowShouldClose(window))
	{
		//Close window when Esc is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_0))
			TransformSystem::Translate(e2, 0, 0, 1);
		if (glfwGetKey(window, GLFW_KEY_1))
			TransformSystem::Translate(e2, 0, 0, -1);

		//Update all engine systems, this usually should go last in the game loop
		Update(&cam);

		//OpenGL stuff, goes very last
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}