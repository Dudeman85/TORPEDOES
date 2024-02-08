#include <engine/Application.h>
#include <thread>

//Sanity saver
using namespace engine;

//Add a deleteAllEntities function to ecs
//Add an overridable exception to deleteAllEntities in the for of a "persistent" tag which prevents deletion
//Game programmer defines functions with everything needed to load a level, which they can call after deleteAllEntities

ecs::Entity entity;
ecs::Entity entity2;

int level = 0;

	//Load some sprites
Texture* strawberry;

//Load some models
Model* ship;
Tilemap* tilemap;

void CreateLevel1(Camera* cam)
{
	level = 1;

	delete tilemap;
	tilemap = new Tilemap(cam);
	tilemap->loadMap("level1.tmx");
	spriteRenderSystem->SetTilemap(tilemap);
	collisionSystem->SetTilemap(tilemap);

	//Create a new entity
	entity = ecs::NewEntity();
	//Add the transform and SpriteRenderer components required for rendering a sprite
	ecs::AddComponent(entity, Transform{ .position = Vector3(10, 10, 0), .rotation = Vector3(54, 45, 0), .scale = Vector3(30) });
	ecs::AddComponent(entity, SpriteRenderer{ .texture = strawberry });
	ecs::AddComponent(entity, ModelRenderer{ .model = ship });
	ecs::AddComponent(entity, Rigidbody{});

	//Create a new entity
	entity2 = ecs::NewEntity();
	//Add the transform and SpriteRenderer components required for rendering a sprite
	ecs::AddComponent(entity2, Transform{ .position = Vector3(0, 0, -10), .rotation = Vector3(90, 0, 0), .scale = Vector3(0.5) });
	ecs::AddComponent(entity2, SpriteRenderer{ .texture = strawberry });
	ecs::AddComponent(entity2, ModelRenderer{ .model = ship });

	TransformSystem::AddParent(entity2, entity);
}
void DeleteLevel1()
{
	level = 0;
	ecs::DestroyEntity(entity);
	if (ecs::EntityExists(entity2))
		ecs::DestroyEntity(entity2);
	spriteRenderSystem->RemoveTilemap();
	collisionSystem->RemoveTilemap();
}
void DeleteLevel2()
{
	level = 0;
	ecs::DestroyEntity(entity);
	spriteRenderSystem->RemoveTilemap();
	collisionSystem->RemoveTilemap();
}

void CreateLevel2(Camera* cam)
{
	level = 2;

	delete tilemap;
	tilemap = new Tilemap(cam);
	tilemap->loadMap("torptest.tmx");
	spriteRenderSystem->SetTilemap(tilemap);
	collisionSystem->SetTilemap(tilemap);

	//Create a new entity
	entity = ecs::NewEntity();
	//Add the transform and SpriteRenderer components required for rendering a sprite
	ecs::AddComponent(entity, Transform{ .position = Vector3(1, 0, 0), .rotation = Vector3(0, 0, 0), .scale = Vector3(30) });
	ecs::AddComponent(entity, SpriteRenderer{ .texture = strawberry });
	ecs::AddComponent(entity, Rigidbody{});
}

void LoadModels()
{
	//Load some sprites
	strawberry = new Texture("sandbox4/strawberry.png");
	//Load some models
	ship = new Model("sandbox4/Achelous.obj");
}

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

	LoadModels();

	CreateLevel1(&cam);

	//Game Loop
	while (!glfwWindowShouldClose(window))
	{
		//Close window when Esc is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		//Getting basic input and moving
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		{
			DeleteLevel2();
			CreateLevel1(&cam);
		}
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		{
			DeleteLevel1();
			CreateLevel2(&cam);
		}
		if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
		{
			if(level == 1)
				DeleteLevel1();
			if(level == 2)
				DeleteLevel2();
		}

		//Update all engine systems, this usually should go last in the game loop
		Update(&cam);

		//OpenGL stuff, goes very last
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}