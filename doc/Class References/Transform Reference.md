# Transform

### ***The default Transform system is a member of the EngineLib class! All related components and functions are within the engine namespace!***

---
## Transform Component

The transform component stores the position, scale, and rotation in degrees, of an entity in Vector3 form.
```cpp
//Transform component
	struct Transform
	{
	//The local position, relative to parent
	Vector3 position;
	//The local rotation, relative to parent
	Vector3 rotation;
	//The local scale, relative to parent
	Vector3 scale = Vector3(1.0f);

	//The parent entity of this entity
	//If set this transform will be relative to the parent
	ecs::Entity parent = 0;
	//All the children of this entity
	std::set<ecs::Entity> children;

	//If true updates all transform based caches, reverts to false next frame
	//WARNING: This will not update if transform is manually changed
	bool staleCache = true;
};
```
This component includes a cache which is used by the collision system to update the bounding box when the transform has changed. ***Currently there is no automatic staling, so manually modifying transform values will not automatically stale the cache! This will break collision!***

## Transform System

The Transform System is currently only comprised of static functions, which you should use to move, rotate and scale the entity:
```cpp
//Add the Transform component
ecs:AddComponent(entity, Transform{ .x = 0, .y = 25, .xScale = 20, .yScale = 20 });
//Get the Transform component
Transform& transform = ecs::GetComponent<Transform>(entity);

//Current Transform system static methods
//These also accept Vector3's
TransformSystem::Translate(entity, x, y, z);
TransformSystem::SetPosition(entity, x, y, z);
TransformSystem::Rotate(entity, x, y, z);
TransformSystem::SetRotation(entity, x, y, z);
TransformSystem::Scale(entity, x, y, z);
TransformSystem::SetScale(entity, x, y, z);

//You can read from members directly, but beware that writing to them won't stale the cache and could break things
float x = transform.position.x;
float yScale = transform.scale.y;

//Other helpful functions:
//Get the right (x) vector of an entity
Vector3 right = TransformSystem::RightVector(entity);
//Get the up (y) vector of an entity
Vector3 up = TransformSystem::UpVector(entity);
//Get the forward (z) vector of an entity
Vector3 forward = TransformSystem::ForwardVector(entity);

//Set the parent of an entity, it's transform will then be relative to its parent
TransformSystem::AddParent(childEntity, parentEntity);
//Remove the parent of an entity, it's transform will then be relative to origin
TransformSystem::RemoveParent(childEntity, parentEntity)
```