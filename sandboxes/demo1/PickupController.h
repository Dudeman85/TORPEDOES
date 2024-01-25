#pragma once
#include <engine/Application.h>
#include "Physics.h"
#include <cmath>
#include <algorithm>

extern ECS ecs;

using namespace engine;

struct Pickup
{
	bool floating = true;
	int sound = 0;
};

struct  Board
{
	int vicCondition = 0;
	glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);
};

//Pickup controller system requires sprite, transform, rigidbody, box collider, Animator and Pickup
class PickupController : public System
{
public:
	PickupController()
	{
		defaultTexture = new Texture("assets/strawberry.png");
		winner = new Texture("assets/winner.png");
		animations = AnimationsFromSpritesheet("assets/Strawberry Animation.png", 7, 2, vector<int>(7 * 2, 100));
		//scoreAnims = AnimationsFromSpritesheet("assets/");
		winScreen = ecs.newEntity();
		ecs.addComponent(winScreen, Transform{ .position = Vector3(0, 0, 20), .scale = Vector3(200, 200, 0) });
		ecs.addComponent(winScreen, SpriteRenderer{ .texture = winner, .enabled = false });
	}

	void Update(Entity player, double programTime)
	{
		//Board& board = ecs.getComponent<Board>(entity);
		for (auto const& entity : entities)
		{
			Pickup& pickup = ecs.getComponent<Pickup>(entity);

			Transform& transform = ecs.getComponent<Transform>(entity);
			oldPhysics::BoxCollider& collider = ecs.getComponent<oldPhysics::BoxCollider>(entity);
			Animator& animator = ecs.getComponent<Animator>(entity);

			//If floating animate it
			if (pickup.floating)
				transform.position.y += sin(programTime * 3) * 0.5;

			//If collided with the player start the collection animation
			if (collider.collisions.end() != find_if(collider.collisions.begin(), collider.collisions.end(), [player](const oldPhysics::Collision& collision) { return collision.a == player; }))
			{
				AnimationSystem::PlayAnimation(entity, "collect");
				pickup.sound = 1;
			}

			//When the collection animation is over
			if (!animator.playingAnimation)
			{
				collected++;
				//AnimationSystem::PlayAnimation(board, to_string(collected));
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
		ecs.addComponent(pickup, Transform{ .position = Vector3(x, y, 5), .scale = Vector3(30, 25, 0) });
		ecs.addComponent(pickup, SpriteRenderer{ .texture = defaultTexture });
		ecs.addComponent(pickup, oldPhysics::Rigidbody{ .kinematic = true });
		ecs.addComponent(pickup, oldPhysics::BoxCollider{ .isTrigger = true });
		ecs.addComponent(pickup, Pickup{});
		ecs.addComponent(pickup, Animator{});
		AnimationSystem::AddAnimations(pickup, animations, vector<string>{"default", "collect"});
		AnimationSystem::PlayAnimation(pickup, "default", true);
		total++;

		return pickup;
	}

	Entity CreateScoreboard(float x, float y)
	{
		Entity board = ecs.newEntity();
		ecs.addComponent(board, Transform{ .position = Vector3(x, y, 0) });
		ecs.addComponent(board, Animator{});
		ecs.addComponent(board, Board{});
		AnimationSystem::AddAnimations(board, scoreAnims, vector<string>{"0", "1", "2", "3", "4",
			"5", "6", "7", "8", "9", });
		AnimationSystem::PlayAnimation(board, "0", true);
	}

	Entity winScreen;
	Texture* winner;
	vector<Animation> scoreAnims;
	vector<Animation> animations;
	Texture* defaultTexture;
	int collected = 0;
	int total = 0;
};