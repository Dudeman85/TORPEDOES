#include <engine/Application.h>
#include <engine/Tilemap.h>

int main()
{
	GLFWwindow* window = engine::CreateGLWindow(1600, 900, "Tiled");

	engine::Camera cam = engine::Camera(1120, 630);
	cam.SetPosition(Vector3(0, 0, 1500));
	cam.SetRotation(Vector3(0, 0, 0));

	Tilemap map(&cam);
	map.loadMap("assets/Tiled/tilemaps/TestMap.tmx");
	spriteRenderSystem->SetTilemap(&map);

	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}

		Update(&cam);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}