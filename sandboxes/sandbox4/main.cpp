#include <engine/Application.h>
#include <engine/ResourceManagement.h>
#include <thread>

//Sanity saver
using namespace engine;

ECS_REGISTER_COMPONENT(TestComponent)
struct TestComponent
{
	ecs::Entity toDelete;
};

ECS_REGISTER_SYSTEM(TestSystem, TestComponent)
class TestSystem : public ecs::System
{
public:
	void Update()
	{
		int i = 0;

		for (ecs::Entity entity : entities)
		{
			TestComponent& comp = ecs::GetComponent<TestComponent>(entity);

			if (ecs::EntityExists(comp.toDelete))
			{
				//ecs::DestroyEntity(comp.toDelete);
				ecs::DestroyEntity(entity);
			}

			cout << "Completed Iterations: " << i++ << endl;
		}
	}
};

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

	auto testSystem = ecs::GetSystem<TestSystem>();


	ecs::Entity e1 = ecs::NewEntity();
	ecs::Entity e2 = ecs::NewEntity();
	ecs::Entity e3 = ecs::NewEntity();
	ecs::Entity e4 = ecs::NewEntity();


	ecs::AddComponent(e1, TestComponent{ 0 });
	ecs::AddComponent(e2, TestComponent{ 0 });
	ecs::AddComponent(e3, TestComponent{ e4 });
	ecs::AddComponent(e4, TestComponent{ 0 });


	//Game Loop
	while (!glfwWindowShouldClose(window))
	{
		//Close window when Esc is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		testSystem->Update();

		//Update all engine systems, this usually should go last in the game loop
		Update(&cam);

		//OpenGL stuff, goes very last
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}