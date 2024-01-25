#pragma once
#include <engine/Application.h>
#include <cmath>
#include <algorithm>
#include "Physics.h"

extern ECS ecs;

using namespace engine;

struct Pickup
{
	bool floating = false;
	int sound = 0;
};

//Pickup controller system requires sprite, transform, rigidbody, box collider and Pickup
class PickupController : public System
{
public:
	PickupController()
	{
		defaultTexture = new Texture("assets/ammo box.png");
		winner = new Texture("assets/winner.png");

		winScreen = ecs.newEntity();
		ecs.addComponent(winScreen, Transform{ .position = Vector3(0, 0, 20), .scale = Vector3(200, 200, 0) });
		ecs.addComponent(winScreen, SpriteRenderer{ .texture = winner, .enabled = false });
	}

	void Update(Entity player)
	{
		for (auto const& entity : entities)
		{
			Pickup& pickup = ecs.getComponent<Pickup>(entity);
			Transform& transform = ecs.getComponent<Transform>(entity);
			oldPhysics::BoxCollider& collider = ecs.getComponent<oldPhysics::BoxCollider>(entity);

			//If collided with the player start the collection animation
			if (collider.collisions.end() != find_if(collider.collisions.begin(), collider.collisions.end(), [player](const oldPhysics::Collision& collision) { return collision.a == player; }))
			{
				collected++;
				ecs.destroyEntity(entity);
				break;
			}
		}

		if (collected >= total)
		{
			ecs.getComponent<SpriteRenderer>(winScreen).enabled = true;
		}
		TransformSystem::SetPosition(winScreen, Vector3(ecs.getComponent<Transform>(player).position.x, ecs.getComponent<Transform>(player).position.y, 20));
	}

	Entity CreatePickup(float x, float y)
	{
		Entity pickup = ecs.newEntity();
		ecs.addComponent(pickup, Transform{ .position = Vector3(x, y, 1.5), .scale = Vector3(35, 25, 0) });
		ecs.addComponent(pickup, SpriteRenderer{ .texture = defaultTexture });
		ecs.addComponent(pickup, oldPhysics::Rigidbody{ .kinematic = true });
		ecs.addComponent(pickup, oldPhysics::BoxCollider{ .isTrigger = true });
		ecs.addComponent(pickup, Pickup{});
		total++;

		return pickup;
	}

	Entity winScreen;
	Texture* winner;
	Texture* defaultTexture;
	int collected = 0;
	int total = 0;
};