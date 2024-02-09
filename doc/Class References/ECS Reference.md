# Entity Component System

This reference is meant to explain how to use the ECS architecture on a surface level. For more information on how it actually works, see *ECS Report.docx (outdated)*.

## ECS Basics
The ECS architectures consists of entities, components, and systems.<br>
**Entities are IDs and can be though of similarly to game objects.**<br>
**Components store data attached to entities.**<br>
**Systems operate upon the data in components.** Each system has required components it needs to operate; if an entity has every required component it will be operated upon by the system.

All of the ECS functionality is in **engine/ECS.h** and under the namespace **engine::ecs**
```cpp
//The ECS implementation, this is unnecessary if you include engine/Application.h
#include <engine/ECS.h>	

//Probably want to use this
using namespace engine;
```

---
## Entity
Entities are IDs. They have no data or methods themselves and can be though of as gaining the attributes of the components attached to them. **0 will never be a valid entity.**
```cpp
//Example entity
//This is just an id with no actual data
ecs::Entity player = ecs::NewEntity();

//Destroys an entity along with all of its components
ecs::DestroyEntity(player);
```

---
## Component
Components are aggregate structs or classes, meaning no user defined constructors, and no private or virtual members! They are initialized with a designated initializer {}, just like arrays. Since components should not have any methods, it is good to keep them as structs.

Every component type also must be registered before it can be used.
```cpp
//Example custom component
ECS_REGISTER_COMPONENT(Position)
struct Position
{
	float x, y, z;
};
```

Adding and getting components to entities can be done as such:
```cpp
//Add the component to an entity
ecs::AddComponent(player, Position{ .x = 10.0f, .y = 25.25f, .z = 0.0f});

//Make sure the entity has the component, this is usually not necessary if your system is designed properly
if(ecs::HasComponent<Position>(player))
{
	//Get the player entity's position component
	Position& position = ecs::GetComponent<Position>(player);

	//You can read/write to the members of a component directly
	position.x = 5.1f;
}
```

You can also add a destructor function to a component type. It will be called whenever a component is removed or an entity with that component is destroyed. The function should return void and take an Entity and Component as parameters.
```cpp
//Destructor function for Position component
void OnPositionRemoved(Entity e, Position p)
{
	std::cout << "Position component removed" << std::endl;
}

//Set the destrucor callback
ecs::SetComponentDestructor<Position>(OnPositionRemoved);
```

---
## System
Systems are esentially collections of functions that operate upon data in components. Each system has a list of required components it needs to operate which needs to be given manually, as well as a list of entities with those required components which is automaticaly set by the ECS implementation.
For example, a render system could require Sprite and Transform components. It would then automatically operate upon every entity with at least those components.

Every system must be a class that inherits from the System class which contains the list of all entities with the required components.

Systems should have an Update and an Init function, although these are not strictly required. The update function should have a loop which iterates through each entity in the entities set. The loop should be done exactly like below. Avoid using constructors and instead do setup in Init.
```cpp
//Example Gravity System
//Requires Position component
ECS_REGISTER_SYSTEM(GravitySystem, Position)
class GravitySystem : public ecs::System
{
public:
	//The Init function of the Gravity system should be called once before the game loop
	void Init(Vector3 g)
	{
		gravity = g;
	}

	//The Update function of the Gravity system should be called every frame
	void Update(float deltaTime)
	{
		//Loop through each entity available to this system
        //The loop should be done like this to avoid invalid iterators when deleting entities
        for (auto itr = entities.begin(); itr != entities.end();)
        {
            //Get the entity and increment the iterator
            ecs::Entity entity = *itr++;
		
			//Get the Position component of the entity
			//There is no need to make sure it has one since the ECS manager takes care of that
			Position& position = ecs::GetComponent<Position>(entity);

			//Operate upon the data in the entity's Position component
			position.x += gravity.x;
			position.y += gravity.y;
			position.z += gravity.z;
		}
	}
private:
	Vector3 gravity = Vector3(0, -9.81, 0);
}
```

Most ECS system functions that operate upon only one entity don't usually need to be members of the system class. However, they should be static members for the sake of organization.
```cpp
//These are equivalent
TranformSystem::SetPosition(entity, Vector3(100, 100, 100));
engine::tranformSystem->SetPosition(entity, Vector3(100, 100, 100));
```

All you need to do now is get a reference to the system and call Update in your game loop:

```cpp
//Get the gravity system
std::shared_ptr<GravitySystem> gravitySystem = ecs::GetSystem<GravitySystem>();

//Call init before game loop
gravitySystem->Init();

//Game loop
while(true)
{
	//Now you can use the gravity system
	//It will automatically operate upon every entity with the Position component
	gravitySystem->Update();
}
```

## Other Features

There is a tagging system where you can add string tags to specific entities. The only predefined tag is "persistent", which prevents the entity from being deleted by DestroyAllEntities.
```cpp
//Set the tags of an entity, overrites any previous tags
ecs::SetTags(entity, { "a", "b" });
//Add a tag to an entity without affecting others
ecs::AddTag(entity, "persistent");
//Removes a specific tag from an entity
ecs::RemoveTag(entity, "b");
//Removes all tags from an entity
ecs::RemoveAllTags(entity);
//Gets the list of tags of an entity
std::vector<std::string> tags = ecs::GetTags(entity);
```

You can also destroy all entities. This is useful when unloading a level
```cpp

```