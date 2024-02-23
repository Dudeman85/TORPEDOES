#include <engine/Application.h>
//#include <engine/OldTilemap.h>
// tmxlite debugging
#include <engine/Tilemap.h>

int main()
{
	GLFWwindow* window = engine::CreateGLWindow(1600, 900, "Tiled Test");

	engine::EngineInit();

	engine::Camera cam = engine::Camera(1120, 630);
	cam.SetPosition(Vector3(0, 0, 1500));
	cam.SetRotation(Vector3(0, 0, 0));

	engine::SpriteRenderSystem::SetBackgroundColor(0, 150, 0);


	TMap map;
	map.setup("Sandbox2/Tiled/tilemaps/TestMap.tmx");
	/*Tilemap map(&cam);
	map.loadMap("Sandbox2/Tiled/tilemaps/TestMap.tmx");
	engine::spriteRenderSystem->SetTilemap(&map);*/

	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}

		engine::Update(&cam);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}