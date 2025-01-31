#pragma once
#include <engine/ECS.h>
#include <engine/Vector.h>
#include <engine/ModelSystem.h>
#include <engine/Collision.h>
#include <engine/Timing.h>
#include "Resources.h"

ECS_REGISTER_COMPONENT(PickupComponent)
struct PickupComponent
{
	Vector3 basePosition;
	bool respawn = true;
	bool active = true;
	float randomOffset;
};

ECS_REGISTER_SYSTEM(PickupSystem, PickupComponent, Transform, PolygonCollider)
class PickupSystem : public ecs::System
{
	//Seconds to respawn
	static const float respawnTime;

public:
	void Update()
	{
		//For each entity
		for (ecs::Entity entity : entities)
		{
			PickupComponent& pc = ecs::GetComponent<PickupComponent>(entity);
			TransformSystem::SetPosition(entity, pc.basePosition);
			TransformSystem::Translate(entity, { 0, (float)sin(programTime * 2 + pc.randomOffset) * 2.0f, (float)sin(programTime * 2 + pc.randomOffset) * 1.5f });;
			TransformSystem::Rotate(entity, { 0, (float)sin(programTime * 2 + pc.randomOffset) * 0.2f, 0 });
		}
	}

	//Create a pickup at position
	static void SpawnPickup(Vector3 position, bool respawn = true)
	{
		ecs::Entity pickup = ecs::NewEntity();
		ecs::AddComponent(pickup, ModelRenderer{ .model = resources::models["Prop_PowerUpBox2.obj"] });
		ecs::AddComponent(pickup, Transform{ .position = position, .rotation = {60, 35, 0}, .scale = 25 });
		ecs::AddComponent(pickup, PickupComponent{ .basePosition = position, .respawn = respawn, .randomOffset = (float)rand() / RAND_MAX * 3 });
		vector<Vector2> colliderVerts{ Vector2(.5, .5), Vector2(.5, -.5), Vector2(-.5, -.5), Vector2(-.5, .5) };
		ecs::AddComponent(pickup, PolygonCollider{ .vertices = colliderVerts, .callback = OnCollision, .trigger = true, .visualise = false });
	}

	//Disables a pickup, it will re-enable after a while if set
	static void Disable(ecs::Entity pickup)
	{
		ModelRenderer& model = ecs::GetComponent<ModelRenderer>(pickup);
		PickupComponent& pickupComponent = ecs::GetComponent<PickupComponent>(pickup);

		model.enabled = false;
		pickupComponent.active = false;
		TimerSystem::ScheduleFunction(
			[pickup]()
			{
				if (ecs::EntityExists(pickup))
				{
					ecs::GetComponent<ModelRenderer>(pickup).enabled = true;
					ecs::GetComponent<PickupComponent>(pickup).active = true;
				}
			}, respawnTime);
	}

	static void OnCollision(Collision collision)
	{
		//Don't do anything if not active
		if (!ecs::GetComponent<PickupComponent>(collision.a).active)
			return;
		//Ignore tilemap collision to prevent weird false hit issue
		if (collision.type == Collision::Type::tilemapCollision || collision.type == Collision::Type::tilemapTrigger)
			return;

		//Only pickable by player
		if (ecs::HasComponent<Player>(collision.b))
		{
			//Enable the special action if player does not already have it
			Player& player = ecs::GetComponent<Player>(collision.b);
			if (player.specialEnabled)
				return;
			player.specialEnabled = true;

			Disable(collision.a);
		}
		//Destroy if hit by a projectile
		if (ecs::HasComponent<Projectile>(collision.b))
		{
			Disable(collision.a);

			Transform& transform = ecs::GetComponent<Transform>(collision.a);

			//Make an explosion animation
			ecs::Entity explosion = ecs::NewEntity();
			ecs::AddComponent(explosion, Transform{ .position = transform.position + Vector3(0, 0, 50), .scale = Vector3(20)});
			ecs::AddComponent(explosion, SpriteRenderer{ });
			ecs::AddComponent(explosion, Animator{ .onAnimationEnd = ecs::DestroyEntity });
			engine::AnimationSystem::AddAnimation(explosion, resources::explosionAnimation, "hit");
			engine::AnimationSystem::PlayAnimation(explosion, "hit", false);

			ecs::DestroyEntity(collision.b);
		}
	}
};

const float PickupSystem::respawnTime = 5;