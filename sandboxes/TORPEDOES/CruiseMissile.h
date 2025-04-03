#pragma once
#include <engine/ECS.h>
#include <engine/Transform.h>
#include <engine/SoundComponent.h>
#include <engine/Sprite.h>

using namespace engine;

ECS_REGISTER_COMPONENT(CruiseMissile);
struct CruiseMissile
{
	ecs::Entity owner;
	ecs::Entity target;
};

ECS_REGISTER_SYSTEM(CruiseMissileSystem, CruiseMissile);
class CruiseMissileSystem : public ecs::System
{
private:
	float speed = 10;

public:
	void Update()
	{
		//For each entity
		for (ecs::Entity entity : entities)
		{
			CruiseMissile missile = ecs::GetComponent<CruiseMissile>(entity);
			Vector3 missilePos = ecs::GetComponent<Transform>(entity).position;
			Vector3 targetPos = ecs::GetComponent<Transform>(missile.target).position;

			//If far enough from target, advance towards it
			if (missilePos.Distance(targetPos) > 10)
			{
				Vector3 direction = targetPos - missilePos;

				TransformSystem::Translate(entity, direction * speed * deltaTime);
			}
			//If close enough, explode
			else
			{
				CreateExplosion(entity);
				ecs::DestroyEntity(entity);
			}
		}
	}

	//Spawn a missile going for a target
	static void CreateMissile(ecs::Entity owner, ecs::Entity target)
	{
		ecs::Entity missile = ecs::NewEntity();

		ecs::AddComponent(missile, Transform{ .position = ecs::GetComponent<Transform>(owner).position, .scale = 30 });
		ecs::AddComponent(missile, ModelRenderer{ .model = resources::models["Weapon_CruiseMissile.obj"] });
		ecs::AddComponent(missile, CruiseMissile{ .owner = owner, .target = target});
	}

	//Make the explosion animation
	static void CreateExplosion(ecs::Entity projectile)
	{
		Vector3 position = ecs::GetComponent<Transform>(projectile).position;
		position.z += 500 + ((double)rand() / (double)RAND_MAX);

		Audio* explosionSound = engine::AddAudio("Gameplay", "audio/explosion.wav", false, 0.2f, DistanceModel::LINEAR);
		explosionSound->play();

		engine::ecs::Entity explosion = engine::ecs::NewEntity();
		engine::ecs::AddComponent(explosion, engine::Transform{ .position = position, .scale = Vector3(50) });
		engine::ecs::AddComponent(explosion, engine::SpriteRenderer{ });
		engine::ecs::AddComponent(explosion, engine::Animator{ .onAnimationEnd = ecs::DestroyEntity });
		engine::ecs::AddComponent(explosion, engine::SoundComponent{ .Sounds = {{"Explosion", explosionSound}} });
		Vector2 explosionSize = Vector2(0.7, -0.7);
		std::vector<Vector2> explosionverts{ Vector2(explosionSize.x, explosionSize.x), Vector2(explosionSize.x, explosionSize.y), Vector2(explosionSize.y, explosionSize.y), Vector2(explosionSize.y, explosionSize.x) };
		engine::ecs::AddComponent(explosion, engine::PolygonCollider{ .vertices = explosionverts, .trigger = true, .visualise = true });
		engine::ecs::AddComponent(explosion, Projectile{ .owner = ecs::GetComponent<CruiseMissile>(projectile).owner, .hitType = HitStates::Stop, .hitSpeedFactor = 0.5, .hitTime = 1, .canHitSubmerged = true, .deleteAffterHit = false, .hitAnimation = ""});

		//Play explosion animation
		engine::AnimationSystem::AddAnimation(explosion, resources::explosionAnimation, "explosion");
		engine::AnimationSystem::PlayAnimation(explosion, "explosion", false);
	};
};