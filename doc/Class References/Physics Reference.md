# Physics Reference

### ***The default Physics system is a member of the EngineLib class! All related components and functions are within the engine namespace!***

---
## Transform
To understand physics, you need to know how [Transform](Transform%20Reference.md) works.

---
## Rigidbody

The Rigidbody component contain all the physics material properties of the entity:
```cpp
//Rigidbody component
struct Rigidbody
	{
	Vector3 velocity;
	float mass = 1;
	float gravityScale = 1;
	//How much linear drag should be applied between 0-1 
	float drag = 0;
	//Aka bounciness how much velocity will be preserved after a collision between 0-1
	float restitution = 1;
	//If true, this will not be effected by outside forces calculations
	bool kinematic = false;
};
```

Using the Rigidbody component:
```cpp
//Add the Rigidbody component
Rigidbody& rigidbody = ecs.addComponent(entity, Rigidbody{ .mass = 100, .drag = 0.05, .bounciness = 0.125 });
//Get the Rigidbody component
Rigidbody& rigidbody = ecs.getComponent<Rigidbody>(entity);

//You can read and write to members directly with no negatives
Vector3 velocity = rigidbody.velocity;
rigidbody.drag = 0.1;
```

---
## PhysicsSystem
<u>Requires the Transform and Rigidbody components.</u>

The Physics System is in charge of applying forces on entities, updating the entity's position based on its velocity, and resolving collisions.

Moving entities can be done directly. The PhysicsSystem::Move() method is used to move an entity while checking collision, as opposed to Transform::Translate() which does not check collision.
Impulse can be used to add velocity to a Rigidbody. If you want to add velocity every frame remember to multiply by deltaTime.
```cpp
//Move the entity while checking collision, most physics calculations should be done with respect to deltaTime
PhysicsSystem::Move(player, Vector3(0, 300, 0) * engine.deltaTime);
//Add an impulse to entity
PhysicsSystem::Impulse(player, Vector3(100, 100, 100) * engine.deltaTime);

//Set the gravity
engine.physicsSystem->gravity = Vector3(0, -9.81, 0);
```

---
## PolygonCollider

The Polygon Collider component is the hitbox of an entity. It can be comprised of as many vertices as desired, but it must be concave. The CollisionSystem automatically updates the bounding box for performance.

```cpp
//Polygon Collider component
struct PolygonCollider
{
	//The vertices of the polygon making up the collider, going clockwise
	//The vertices must form a convex polygon
	std::vector<Vector2> vertices;
    //The axis-aligned bounding box of the collider
	//This is updated automatically and is set in world coordinates
	std::array<float, 4> bounds;
    //Should the collider only act as a trigger, aka not be affected by physics
	bool trigger = false;
	//Draw the collider vertices
	bool visualize = false;
	//Callback function on collision
	std::function<void(Collision)> callback;
};
```

An entity with a PolygonCollider but not a Rigidbody will only check collision if trigger = true. The callback function will be called every collision or trigger and will have a Collision type parameter with information of the collision.

Using the Polygon Collider component:
```cpp
//Define the callback function on collision, it doesn't have to be a lambda and it is not required
std::function<void(Collision)> OnCollision = [](Collision c)
{
	std::cout << c.b;
};
//Define the vertices of the collider, these should be in clockwise order
vector<Vector2> colliderVerts{ Vector2(5, 5), Vector2(5, -5), Vector2(-5, -5), Vector2(-5, 5) };

//Add the PolygonCollider component, set it's vertices and callback function
PolygonCollider& collider = ecs.addComponent(entity, PolygonCollider{ .vertices = colliderVerts, .callback = OnCollision });
//Get the PolygonCollider component
PolygonCollider& collider = ecs.getComponent<PolygonCollider>(entity);

//You can change the vertices or other members whenever, but don't touch the bounds since they are updated automatically.
collider.trigger = true;
collider.vertices = colliderVerts;
```


As with most ECS systems, PhysicsSystem and CollisionSystem functions that operate upon only one entity don't usually need to be members of the system class. However here they are static members for the sake of organization.
```cpp
//These are equivalent
PhysicsSystem::Impulse(player, Vector3(100, 100, 100));
engine.physicsSystem->Impulse(player, Vector3(100, 100, 100));
```