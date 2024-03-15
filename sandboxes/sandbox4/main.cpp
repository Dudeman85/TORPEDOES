#include <engine/Application.h>
#include <engine/ResourceManagement.h>
#include <thread>

//Sanity saver
using namespace engine;

ecs::Entity entity;
ecs::Entity entity2;

int level = 0;

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
	ecs::AddComponent(entity2, Timer{ .duration = 5, .callback = ecs::DestroyEntity, .running = true });

	TransformSystem::AddParent(entity2, entity);
}
void UnloadLevel(bool everything = false)
{
	level = 0;
	ecs::DestroyAllEntities(everything);
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

	//Create a new persistent entity
	entity2 = ecs::NewEntity();
	//Add the transform and SpriteRenderer components required for rendering a sprite
	ecs::AddComponent(entity2, Transform{ .position = Vector3(500, 500, 0), .rotation = Vector3(0, 0, 90), .scale = Vector3(50) });
	ecs::AddComponent(entity2, SpriteRenderer{ .texture = strawberry });
	ecs::AddTag(entity2, "persistent");
}

void LoadModels()
{
	//Load some sprites
	strawberry = new Texture("testAssets/strawberry.png");
	//Load some models
	ship = new Model("testAssets/Achelous.obj");
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

	std::unordered_map<std::string, Texture*> textures = engine::PreloadTextures("testAssets");

	double timer = 0;

	//Game Loop
	while (!glfwWindowShouldClose(window))
	{
		if (timer < 0)
		{
			std::cout << "second\n";
			timer = 1;
		}
		else
			timer -= engine::deltaTime;

		//Close window when Esc is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		//Getting basic input and moving
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		{
			UnloadLevel();
			CreateLevel1(&cam);
		}
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		{
			UnloadLevel();
			CreateLevel2(&cam);
		}
		if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
		{
			UnloadLevel(true);
		}

		//Update all engine systems, this usually should go last in the game loop
		Update(&cam);

		//OpenGL stuff, goes very last
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}