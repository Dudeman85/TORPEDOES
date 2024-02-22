# Engine Library

### ***All engine related classes and functions are within the engine namespace!***

The engine comes with multiple premade components and systems, these do not need to be manually registered. Their functionality should be self-explanatory, but each is explained more thoroughly in its own reference.

Components:
- [Transform](Transform%20Reference.md)
- [SpriteRenderer](Sprite%20Reference.md)
- [ModelRenderer]()
- [TextRenderer]()
- [PrimitiveRenderer]()
- [Animator](Sprite%20Reference.md)
- [Rigidbody](Physics%20Reference.md)
- [PolygonCollider](Physics%20Reference.md)

Systems:
- [TransformSystem](Physics%20Reference.md): Requires Transform
- [SpriteRenderSystem](Sprite%20Reference.md): Requires Transform and SpriteRenderer
- [ModelRenderSystem](): Requires Transform and ModelRenderer
- [TextRenderSystem](): Requires Transform and TextRenderer
- [PrimitiveRenderSystem](): Requires Transform and PrimitiveRenderer
- [AnimationSystem](Sprite%20Reference.md): Requires Sprite and Animator
- [PhysicsSystem](Physics%20Reference.md): Requires Rigidbody and Transform
- [CollisionSystem](Physics%20Reference.md): Transform and PolygonCollider

To use these, create a window, camera, and initialize the engine. The window must be created first, otherwise OpenGL will fail to load.
```cpp
//Includes engine library and all default ECS functionality
#include <engine/Application.h>

int main()
{
	//Create the window before initializing engine
	GLFWwindow* window = engine::CreateGLWindow(800, 600, "Window");

	//Initialize the engine library
	engine::InitEngine();

	//Create the camera
	engine::Camera cam = engine::Camera(800, 600);

	//You can access each default engine system
	engine.physicsSystem->gravity = engine::Vector2(0, -400);

	//Main game loop
	while (!glfwWindowShouldClose(window))
	{
		//This will update all default engine systems and return deltaTime
		double deltaTime = engine::Update(&cam);
		
		//OpenGL stuff, goes very last
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
```

Some other useful engine features:
```cpp
//A global delta time variable tracking how many seconds the last frame took
double dt = engine::deltaTime;

//A global program time variable tracking how many seconds have passed since calling EngineInit()
double pt = engine::programTime;
```