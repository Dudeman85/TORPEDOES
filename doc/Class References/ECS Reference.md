# Entity Component System

This reference is meant to explain how to use the ECS architecture on a surface level. For more information on how it actually works, see *ECS Report.docx*.

# ***!!IMPORTANT!!***
**This is out of date as the new implementation of ECS had been added.**

Improvements:
- Doesn't use singleton classes
- About 40% faster in release mode, although I'm not entirely sure why (possibly due to better memory allocation)
- Memory and call stack optimizations
- Significantly improved debug mode and error checking
- No need to define max entities
- Numerous obscure bugs have been fixed
- Better Code ledgibility

Existing issues:
- Components must be registered manually during runtime
- Systems and their signatures must be registered manually during runtime
- Max components still needs to be defined

## ECS Basics
The ECS architectures consists of entities, components, and systems.<br>
**Entities are IDs and can be though of similarly to game objects.**<br>
**Components store data attached to entities.**<br>
**Systems operate upon the data in components.** Each system has required components it needs to operate; if an entity has every required component it will be operated upon by the system.

To use the ECS implementation for your own classes you need to create a global instance of ECS before your main function and declare it as external in every file you use ECS in.
```cpp
//The ECS implementation, this is unnecessary if you include <engine/Application.h>
#include <engine/ECSCore.h>	

//Global ECS manager needs to be created once before main
ECS ecs;

//In every file using ECS, declare it as external
extern ECS ecs;
```

---
## Entity
Entities are IDs. They have no data or methods themselves and can be though of as gaining the attributes of the components attached to them.
```cpp
//Example entity
//This is just an id with no actual data
Entity player = ecs.newEntity();

//Destroys an entity along with all of its components
ecs.destroyEntity(player);
```

---
## Component
Components are aggregate structs or classes, meaning no user defined constructors, and no private or virtual members! They are initialized with a designated initializer {}, just like arrays.<br>
Since components usually should not have any methods, it is good to keep them as structs.
```cpp
//Example custom component	
struct Position
{
	float x, y, z;
};
```

Every component type also must be registered with the ECS manager once before it can be used.
```cpp
//Register custom component before game loop
ecs.registerComponent<Position>();

//Add the component to an entity, returns a reference to that component
Position& position = ecs.addComponent(player, Position{ .x = 10.0f, .y = 25.25f, .z = 0.0f});

//You can also get any component of any entity whenever
Position& position = ecs.getComponent<Position>(player);

//You can read/write to the variables of a component directly
position.x = 5.1f;
```

---
## System
Systems are esentially collections of functions that operate upon data in components. Each system has a list of required components it needs to operate which needs to be given manually, as well as a list of entities with those required components which is automaticaly set by the ECS manager.<br>
For example, a render system could require Sprite and Transform components. It would then automatically operate upon every entity with at least those components.

Every system must be a class that inherits from the System class, this class will contain the list of all entities with the required components.<br>
```cpp
//Example Gravity System
//Requires Position component
class GravitySystem : public System
{
public:
	//The Update function of the Gravity system should be called every frame
	void Update(float deltaTime)
	{
		//Every systems Update method should operate upon the entire entities list
		for (auto const& entity : entities)
		{
			//Get the Position component of the entity
			//There is no need to make sure it has one since the ECS manager takes care of that
			Position& position = ecs.getComponent<Position>(entity);

			//Operate upon the data in the entity's Position component
			position.x += gravity.x;
			position.y += gravity.y;
			position.z += gravity.z;
		}
	}
	//A static function to set gravity
	static void SetGravity(Vector3 g)
	{
		gravity = g;
	}

private:
	static Vector3 gravity = Vector3(0, -9.81, 0);
}
```

Most ECS system functions that operate upon only one entity don't usually need to be members of the system class. However, they should be static members for the sake of organization.
```cpp
//These are equivalent
GravitySystem::SetGravity(Vector3(100, 100, 100));
engine.physicsSystem->SetGravity(Vector3(100, 100, 100));
```

Once you create a system, you need to register it with the ECS manager and set its signature of required components. This should only be done once before the game loop.
```cpp
//Register the custom GravitySystem
shared_ptr<GravitySystem> gravitySystem = ecs.registerSystem<GravitySystem>();

//Create a new signature and set it to require the Position component
Signature gravitySystemSignature;
gravitySystemSignature.set(ecs.getComponentId<Position>());

//Assign the signature to the GravitySystem
ecs.setSystemSignature<GravitySystem>(gravitySystemSignature);
```

At this point the GravitySystem entities member will be automatically updated to contain every entity with the Position component. All you need to do now is call Update in your game loop:

```cpp
//Now you can use the gravity system
//It will automatically operate upon every entity with the Position component
gravitySystem->Update();
```

## Engine Library

### ***All engine related classes and functions are within the engine namespace!***

The engine comes with multiple premade components and systems, these do not need to be manually registered. Their functionality should be self-explanatory, but each is explained more thoroughly in its own reference.<br>

Components:
- [Transform](Transform%20Reference.md)
- [SpriteRenderer](Sprite%20Reference.md)
- [ModelRenderer]()
- [Animator](Sprite%20Reference.md)
- [Rigidbody](Physics%20Reference.md)
- [PolygonCollider](Physics%20Reference.md)

Systems:
- [TransformSystem](Physics%20Reference.md): Requires Transform
- [SpriteRenderSystem](Sprite%20Reference.md): Requires Transform and SpriteRenderer
- [ModelRenderSystem](): Requires Transform and ModelRenderer
- [AnimationSystem](Sprite%20Reference.md): Requires Sprite and Animator
- [PhysicsSystem](Physics%20Reference.md): Requires Rigidbody and Transform
- [CollisionSystem](Physics%20Reference.md): Transform and PolygonCollider

To use these create a window, camera, and an instance of EngineLib. The window must be created first, otherwise OpenGL will fail to load.
```cpp
//Includes engine library and all default ECS functionality
#include <engine/Application.h>

//Create a global instance of the ECS manager
ECS ecs;

int main()
{
	//Create the window and OpenGL context before creating EngineLib
	GLFWwindow* window = engine::CreateWindow(800, 600, "Window");

	//Initialize the default engine library
	engine::EngineLib engine;

	//Create the camera
	Camera cam = Camera(800, 600);

	//You can access each default engine system
	engine.physicsSystem->gravity = engine::Vector2(0, -400);

	//Main game loop
	while (!glfwWindowShouldClose(window))
	{
		//This will update all default engine systems and return deltaTime
		double deltaTime = engine.Update(&cam);
		
		//OpenGL stuff, goes very last
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
```