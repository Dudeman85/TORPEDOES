#pragma once
#include <engine/ECS.h>
#include <engine/Vector.h>
#include <engine/ModelSystem.h>
#include <engine/Collision.h>
#include <engine/Timing.h>
#include "Resources.h"

using namespace engine;

ECS_REGISTER_COMPONENT(PickupComponent)
struct PickupComponent
{
	bool respawn = true;
	bool active = true;
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
			TransformSystem::Translate(entity, {0, (float)sin(programTime) * 0.2f, 0 });
			TransformSystem::Rotate(entity, {0, 1, 0});
		}
	}

	//Create a pickup at position
	void SpawnPickup(Vector3 position, bool respawn = true)
	{
		ecs::Entity pickup = ecs::NewEntity();
		ecs::AddComponent(pickup, ModelRenderer{ .model = resources::models["Prop_PowerUpBox2.obj"] });
		ecs::AddComponent(pickup, Transform{ .position = position, .rotation = {45, 0, 0}, .scale = 30 });
		ecs::AddComponent(pickup, PickupComponent{.respawn = respawn});
		vector<Vector2> colliderVerts{ Vector2(.5, .5), Vector2(.5, -.5), Vector2(-.5, -.5), Vector2(-.5, .5) };
		ecs::AddComponent(pickup, PolygonCollider{.vertices = colliderVerts, .callback = OnCollision, .trigger = true, .visualise = true});
	}

	static void OnCollision(Collision collision) 
	{
		ModelRenderer& model = ecs::GetComponent<ModelRenderer>(collision.a);
		PickupComponent& pickupComponent = ecs::GetComponent<PickupComponent>(collision.a);

		//Don't do anything if not active
		if (!pickupComponent.active)
			return;

		//Only pickable by player
		if (ecs::HasComponent<Player>(collision.b)) 
		{
			model.enabled = false;
			pickupComponent.active = false;
			TimerSystem::ScheduleFunction([collision]() 
				{
					ecs::GetComponent<ModelRenderer>(collision.a).enabled = true;
					ecs::GetComponent<PickupComponent>(collision.a).active = true;
				}, respawnTime);
		}
		//Destroy if hit by a projectile
		if (ecs::HasComponent<Projectile>(collision.b)) 
		{
			model.enabled = false;
			pickupComponent.active = false;
			TimerSystem::ScheduleFunction([collision]()
				{
					ecs::GetComponent<ModelRenderer>(collision.a).enabled = true;
					ecs::GetComponent<PickupComponent>(collision.a).active = true;
				}, respawnTime);

			Transform& transform = ecs::GetComponent<Transform>(collision.a);

			//Make an explosion animation
			ecs::Entity explosion = ecs::NewEntity();
			ecs::AddComponent(explosion, Transform{ .position = transform.position, .scale = Vector3(20) });
			ecs::AddComponent(explosion, SpriteRenderer{ });
			ecs::AddComponent(explosion, Animator{ .onAnimationEnd = ecs::DestroyEntity });
			engine::AnimationSystem::AddAnimation(explosion, resources::explosionAnimation, "hit");
			engine::AnimationSystem::PlayAnimation(explosion, "hit", false);

			ecs::DestroyEntity(collision.b);
		}
	}
};

const float PickupSystem::respawnTime = 5;