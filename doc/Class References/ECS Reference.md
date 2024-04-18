# Entity Component System

This reference is meant to explain how to use the ECS architecture on a surface level. For more information on how it actually works, see *ECS Report.docx (outdated)*.

## ECS Basics
The ECS architectures consists of entities, components, and systems.<br>
- **Entities are IDs and can be though of similarly to game objects.** They don't contain any data themselves.<br>
- **Components store data attached to entities.** Each entity can have one instance of a component type attached to it.<br>
- **Systems operate upon the data in components.** Each system has required components it needs to operate; if an entity has every required component it will be operated upon by the system.

All of the ECS functionality is in **engine/ECS.h** and under the namespace **engine::ecs**

---
## Entity
Entities are IDs. They have no data or methods themselves and can be though of as gaining the attributes of the components attached to them. **Note: 0 will never be a valid entity.**
```cpp
//Example entity
//This is just an id with no actual data
ecs::Entity player = ecs::NewEntity();

//Destroys an entity along with all of its components
ecs::DestroyEntity(player);
```

---
## Component
Components are aggregate structs or classes, meaning no user defined constructors, and no private or virtual members (among other things)! They are initialized with a designated initializer {}, just like arrays. Since components should not have any methods, it is good practice to keep them as structs.

Every component type also must be registered with the ECS_REGISTER_COMPONENT macro before it can be used. This macro takes the component type as a parameter.
```cpp
//Example custom component
ECS_REGISTER_COMPONENT(Position)
struct Position
{
	float x, y, z;
};
```

Adding and getting components to and from entities can be done as such:
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
Systems are esentially collections of functions that operate upon data in components. Each system has a list of required components it needs to operate, known as a signature, which needs to be given manually when registering the system. The ECS implementation then gives the system a list of entities with those required components. For example, a render system could require Sprite and Transform components. It would then automatically operate upon every entity with at least those components.

Every system must be a class that inherits from the System class. This class contains the list of all entities, called entities, the system can operate on.

Systems should have an Update and an Init function, although these are not strictly required. The update function should have a loop which iterates through each entity in the entities set. The loop should be done exactly like below. Avoid using constructors and instead do setup in an Init function.

Every system must also be registered with the ECS_REGISTER_SYSTEM macro before it can be used. This macro takes the system class type and every required component as parameters. **Note: Any components which are used by the system should be set as required!**
```cpp
//Example Gravity System
//Requires GravitySystem system
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

All you need to do now is get a reference to the system and call Update and Init in your game loop:

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

There is a tagging system where you can add string tags to specific entities. The only tag with predefined functionality is "persistent", which prevents the entity from being deleted by DestroyAllEntities.
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
//Check if entity has a specific tag
bool isPersistent = ecs::HasTag(entity, "persistent")
```

You can also destroy all entities. This is useful when unloading a level.
```cpp
//Destroys all entities without the "persistent" tag along with all their components.
//If ignorePersistent is set to true, will also delete "persistent" entities.
ecs::DestroyAllEntities(false);
```