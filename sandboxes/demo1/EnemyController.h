#pragma once
#include <engine/Application.h>
#include "Physics.h"

extern ECS ecs;

using namespace engine;

struct Enemy
{
	int health;
};

//Pickup controller system requires sprite, transform, rigidbody, box collider, animator and Enemy
class EnemyController : public System
{
public:
	EnemyController()
	{
		//defaultAnimations = AnimationsFromSpritesheet();
	}

	void Update()
	{
		for (auto const& entity : entities)
		{


		}
	}

	Entity CreateEnemy(float x, float y)
	{
		Entity enemy = ecs.newEntity();
		ecs.addComponent(enemy, Transform{ .position = Vector3(x, y, 0), .scale = Vector3(20, 20, 0) });
		ecs.addComponent(enemy, SpriteRenderer{});
		ecs.addComponent(enemy, oldPhysics::Rigidbody{});
		ecs.addComponent(enemy, oldPhysics::BoxCollider{});
		ecs.addComponent(enemy, Enemy{});

		return enemy;
	}

	vector<Animation> defaultAnimations;
	Texture* defaultTexture;
	int collected = 0;
};