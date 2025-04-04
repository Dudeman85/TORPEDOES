#pragma once
#include <engine/ECS.h>
#include <engine/Transform.h>
#include <engine/Collision.h>
#include <engine/Tilemap.h>
#include <engine/Timing.h>
#include <vector>
#include <array>

namespace engine
{
	enum Direction { up = 0, right = 1, down = 2, left = 3 };

	///Rigidbody component
	ECS_REGISTER_COMPONENT(Rigidbody)
		struct Rigidbody
	{
		///Velocity of the rigidbody
		Vector3 velocity;
		///Mass of the rigidbody in Danny's
		float mass = 1;
		///How much gravity affects the rigidbody
		float gravityScale = 1;
		///How much linear drag should be applied 
		float drag = 0;
		///Aka bounciness how much velocity will be preserved after a collision between 0-1
		float restitution = 1;
		///If true, this will not be effected by outside forces calculations
		bool kinematic = false;
	};

	///Tilemap tile properties, not a component
	struct TileProperty
	{
		bool trigger;
	};

	///Physics System, Requires Rigidbody and Transform components
	ECS_REGISTER_SYSTEM(PhysicsSystem, Transform, Rigidbody)
		class PhysicsSystem : public ecs::System
	{
	public:
		///Update the physics system, call this every frame
		void Update()
		{
			//For each entity
			for (ecs::Entity entity : entities)
			{
				//Get required components
				Transform& transform = ecs::GetComponent<Transform>(entity);
				Rigidbody& rigidbody = ecs::GetComponent<Rigidbody>(entity);

				//Don't apply outside forces to kinematic rigidbodies
				if (!rigidbody.kinematic)
				{
					//Add gravity
					rigidbody.velocity += gravity * rigidbody.mass * rigidbody.gravityScale * cappedDeltaTime;
					//Apply drag
					rigidbody.velocity -= rigidbody.velocity * rigidbody.drag * cappedDeltaTime;
				}
				if ((rigidbody.velocity * cappedDeltaTime).Length() != 0)
				{
					//Integrate position
					Move(entity, rigidbody.velocity * cappedDeltaTime, step);
				}
			}
		}

		//COLLISION RESOLUTION:

		///Temporary (permanent) function to solve a collision does not affect rotation, Returns 0 on success, >0 on trigger, and <0 on failure 
		static int SimpleSolveCollision(Collision collision)
		{
			//One of the entities does not have a rigidbody. Return <0 on failure;
			if (!ecs::HasComponent<Rigidbody>(collision.a) || !ecs::HasComponent<Rigidbody>(collision.b))
				return -1;
			//Nothing needs to be done. Return >0 on trigger
			if (collision.type == Collision::Type::trigger || collision.type == Collision::Type::tilemapTrigger)
				return 1;
			//Nothing needs to be done. Return 0 on success
			if (collision.type == Collision::Type::miss)
				return 0;

			Rigidbody& rba = ecs::GetComponent<Rigidbody>(collision.a);

			//Entity collision
			if (collision.type == Collision::Type::collision)
			{
				Rigidbody& rbb = ecs::GetComponent<Rigidbody>(collision.b);

				//Calculate new velocities
				float j = -(1 + (rba.restitution + rbb.restitution) / 2) * (rba.velocity - rbb.velocity).Dot(collision.normal) / collision.normal.Dot(collision.normal * (1 / rba.mass + 1 / rbb.mass));
				rba.velocity = rba.velocity + collision.normal * (j / rba.mass);
				rbb.velocity = rbb.velocity - collision.normal * (j / rbb.mass);

				//Move the entity by the minimum translation vector to make sure it is out of the collision
				TransformSystem::Translate(collision.a, collision.mtv);

				return 0;
			}

			return -1;
		}

		///Solve a collision between an entity and a tilemap, Returns 0 on success, >0 on trigger, and <0 on failure
		static int SolveTilemapCollision(std::vector<Collision> collisions)
		{
			//No collision, nothing needs to be done
			if (collisions.empty())
				return 0;

			//Trigger, nothing needs to be done. Return >0 on trigger
			if (collisions.front().type == Collision::Type::trigger || collisions.front().type == Collision::Type::tilemapTrigger)
				return 1;
			//Nothing needs to be done. Return 0 on success
			if (collisions.front().type == Collision::Type::miss)
				return 0;

			ecs::Entity a = collisions.front().a;
			Collision maxIntersect = Collision{ .mtv = Vector2(0) };

			//Find the largest collision
			for (Collision& collision : collisions)
			{
				if (collision.mtv.Length() > maxIntersect.mtv.Length())
				{
					//If the tile is set to be a trigger
					if (tileProperties.count(collision.b) != 0)
					{
						if (!tileProperties[collision.b].trigger)
							maxIntersect = collision;
					}
					else
					{
						maxIntersect = collision;
					}
				}
			}
			//Solve the largest collision
			if (maxIntersect.type == Collision::Type::tilemapCollision)
			{
				TransformSystem::Translate(a, maxIntersect.mtv);
			}

			//Check collision again
			collisions = ecs::GetSystem<CollisionSystem>()->CheckTilemapCollision(a);

			//Calculate new velocities
			Rigidbody& rba = ecs::GetComponent<Rigidbody>(a);
			Vector3 newAVeclocity = rba.velocity - maxIntersect.normal * 2 * rba.velocity.Dot(maxIntersect.normal);
			rba.velocity = newAVeclocity * rba.restitution;

			return 0;
		}


		//UTILITY:

		///Move an entity while checking for collision, assuming entity has collider
		static void Move(ecs::Entity entity, Vector3 amount, int steps = 1)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);
			transform.staleCache = true;

			//Split the movement into steps
			for (int i = 0; i < steps; i++)
			{
				TransformSystem::Translate(entity, amount / steps);

				//Check collision if entity has collider
				if (ecs::HasComponent<PolygonCollider>(entity))
				{
					CollisionSystem::UpdateAABB(entity);
					//Check entity and tilemap collision
					std::vector<Collision> collisions = ecs::GetSystem<CollisionSystem>()->CheckCollision(entity);
					std::vector<Collision> tilemapCollisions;

					//Solve each entity collision
					for (Collision& collision : collisions)
					{
						if (collision.type == Collision::Type::collision || collision.type == Collision::Type::trigger)
							//Solve entity collisions
							SimpleSolveCollision(collision);
						else
							//Store all the tilemap collision for later
							tilemapCollisions.push_back(collision);
					}

					//Solve tilemap collisions
					SolveTilemapCollision(tilemapCollisions);
				}
			}
		}

		///Add an impulse to entity, does not include deltaTime
		static inline void Impulse(ecs::Entity entity, Vector3 velocity)
		{
			Rigidbody& rigidbody = ecs::GetComponent<Rigidbody>(entity);
			rigidbody.velocity += velocity * rigidbody.mass;
		}

		///Add force to entity
		static inline void AddForce(ecs::Entity entity, Vector3 velocity)
		{
			//Physics dt is capped at 20 fps, less than that will slow down physics to stop impercision
			float cappedDt = std::min(engine::deltaTime, 1.0 / 20.0);

			Rigidbody& rigidbody = ecs::GetComponent<Rigidbody>(entity);
			rigidbody.velocity += velocity * cappedDt * rigidbody.mass;
		}

		///Sets the rigidbody properties of a tile type
		static void SetTileProperty(unsigned int tileID, TileProperty properties)
		{
			tileProperties.insert({ tileID, properties });
		}

		///How many steps should be used for movements, bigger is more accurate but slower
		int step = 1;
		///gravity yay
		Vector3 gravity;
	private:
		//Currently a bad system to assign different physics properties to tile IDs
		//TODO: Probably move this to tilemap or make better in some other way
		static std::map<unsigned int, TileProperty> tileProperties;
	};

	//I don't really get static members
	std::map<unsigned int, TileProperty> PhysicsSystem::tileProperties = tileProperties;
}