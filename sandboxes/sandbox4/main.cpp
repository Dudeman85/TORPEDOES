#include <engine/Application.h>
#include <thread>

//Sanity saver
using namespace engine;


struct Foo
{
	int a;
};

struct Bar : public Foo
{
	float b;
	float* c;

};

void MakeStuff()
{
	for (size_t i = 0; i < 100; i++)
	{
		ecs::Entity e = ecs::NewEntity();
		ecs::AddComponent(e, Transform{ .position = Vector3(0, 0, -2), .rotation = Vector3(90, 0, 0), .scale = Vector3(0.5) });
	}
}

int main()
{
	//Create the window and OpenGL context before creating EngineLib
	//Parameters define window size(x,y) and name
	GLFWwindow* window = CreateGLWindow(800, 600, "Window");

	//Initialize the engine, this must be called after creating a window
	EngineInit();

	//Create the camera
	Camera cam = Camera(800, 600);
	cam.perspective = true;

	//Move the camera back a bit
	cam.SetPosition(Vector3(0, 0, 1000));

	//Set the bacground color
	SpriteRenderSystem::SetBackgroundColor(0, 150, 0);

	//Load some sprites
	Texture strawberry("strawberry.png");

	//Load some models
	Model ship("Achelous.obj");

	//Create a new entity
	ecs::Entity entity = ecs::NewEntity();
	//Add the transform and SpriteRenderer components required for rendering a sprite
	ecs::AddComponent(entity, Transform{ .position = Vector3(1, 0, 0), .rotation = Vector3(0, 0, 0), .scale = Vector3(30) });
	ecs::AddComponent(entity, SpriteRenderer{ .texture = &strawberry });
	ecs::AddComponent(entity, ModelRenderer{ .model = &ship });

	//Create a new entity
	ecs::Entity entity2 = ecs::NewEntity();
	//Add the transform and SpriteRenderer components required for rendering a sprite
	ecs::AddComponent(entity2, Transform{ .position = Vector3(0, 0, -2), .rotation = Vector3(90, 0, 0), .scale = Vector3(0.5) });
	ecs::AddComponent(entity2, SpriteRenderer{ .texture = &strawberry });
	ecs::AddComponent(entity2, ModelRenderer{ .model = &ship });

	MakeStuff();

	TransformSystem::AddParent(entity2, entity);

	//Game Loop
	while (!glfwWindowShouldClose(window))
	{
		Transform& t = ecs::GetComponent<Transform>(entity);

		//Close window when Esc is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		//Getting basic input and moving
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			TransformSystem::Translate(entity, Vector3(0, 2, 0));
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			TransformSystem::Translate(entity, Vector3(-2, 0, 0));
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			TransformSystem::Translate(entity, Vector3(0, -2, 0));
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			TransformSystem::Translate(entity, Vector3(2, 0, 0));
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			TransformSystem::Rotate(entity, Vector3(0, 0, -1));
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			TransformSystem::Rotate(entity, Vector3(0, 0, 1));
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
			TransformSystem::Rotate(entity, Vector3(1, 0, 0));
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
			TransformSystem::Rotate(entity, Vector3(-1, 0, 0));
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
			TransformSystem::Rotate(entity, Vector3(0, 1, 0));
		if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
			TransformSystem::Rotate(entity, Vector3(0, -1, 0));
				

		//Update all engine systems, this usually should go last in the game loop
		Update(&cam);
				
		Primitive right = Primitive::Line(t.position, t.position + TransformSystem::RightVector(entity) * 200);
		right.Draw(&cam, Vector3(0, 0, 255));
		Primitive up = Primitive::Line(t.position, t.position + TransformSystem::UpVector(entity) * 200);
		up.Draw(&cam, Vector3(255, 0, 0));
		Primitive forward = Primitive::Line(t.position, t.position + TransformSystem::ForwardVector(entity) * 200);
		forward.Draw(&cam, Vector3(0, 0, 0));
		
		//OpenGL stuff, goes very last
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}