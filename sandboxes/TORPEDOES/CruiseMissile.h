#pragma once
#include <engine/ECS.h>
#include <engine/Transform.h>
#include <engine/SoundComponent.h>
#include <engine/Sprite.h>

using namespace engine;

//Change the cruise missiles time before selecting a target here
constexpr float targetDelayTime = 0.5;

ECS_REGISTER_COMPONENT(CruiseMissile);
struct CruiseMissile
{
	ecs::Entity owner;
	ecs::Entity target;
	float targetDelay = targetDelayTime;
};

ECS_REGISTER_SYSTEM(CruiseMissileSystem, CruiseMissile);
class CruiseMissileSystem : public ecs::System
{
private:
	float speed = 800;
	float rotationSpeed = 10;

public:
	void Update()
	{
		//For each entity
		for (ecs::Entity entity : entities)
		{
			CruiseMissile& missile = ecs::GetComponent<CruiseMissile>(entity);
			Transform& missileTransform = ecs::GetComponent<Transform>(entity);
			Vector3 targetPos = ecs::GetComponent<Transform>(missile.target).position;

			//Move only forward for a time
			if (missile.targetDelay > 0)
			{
				Vector3 dir = (TransformSystem::RightVector(entity) * Vector3(1, 1, 0)) * speed * 1 * cappedDeltaTime;
				TransformSystem::Translate(entity, dir);
				TransformSystem::Rotate(entity, { 0, 90 * (float)cappedDeltaTime / targetDelayTime, 0 });
				TransformSystem::Scale(entity, Vector3(20) * cappedDeltaTime / targetDelayTime);
				missile.targetDelay -= cappedDeltaTime;
			}
			else
			{
				//If far enough from target
				if (missileTransform.position.Distance(targetPos) > 30)
				{
					//Move towards it
					Vector3 direction = (targetPos - missileTransform.position).Normalize();

					//Rotate the checkpoint indicator
					float targetAngle = Degrees(atan2(direction.y, direction.x));
					if (targetAngle < 0)
						targetAngle += 360;
					float currentAngle = missileTransform.rotation.z;
					//This is so shit
					while (currentAngle < 0)
						currentAngle += 360;
					while (currentAngle > 360)
						currentAngle -= 360;

					//Rotate ccw if it is closer
					float rotationDirection = abs(targetAngle - currentAngle) > 180 ? -1 : 1;
					//Apply rotation with a maximum speed
					TransformSystem::Rotate(entity, Vector3(0, 0, rotationDirection * clamp(targetAngle - currentAngle, -rotationSpeed, rotationSpeed)));

					//Move forward
					Vector3 dir = (TransformSystem::RightVector(entity) * Vector3(1, 1, 0)).Normalize() * speed * cappedDeltaTime;
					TransformSystem::Translate(entity, dir);
				}
				//If close enough, explode
				else
				{
					CreateExplosion(entity);
					ecs::DestroyEntity(entity);
				}
			}
		}
	}

	//Spawn a missile going for a target
	static void CreateMissile(ecs::Entity owner, ecs::Entity target, float rotation)
	{
		ecs::Entity missile = ecs::NewEntity();

		ecs::AddComponent(missile, Transform{ .position = ecs::GetComponent<Transform>(owner).position, .rotation = {0, -90, rotation}, .scale = {10, 10, 10} , .rotationOrder = ZYX});
		ecs::AddComponent(missile, ModelRenderer{ .model = resources::models["Weapon_CruiseMissile.obj"] });
		ecs::AddComponent(missile, CruiseMissile{ .owner = owner, .target = target });
	}

	//Make the cruie missile explosion animation and hitbox
	static void CreateExplosion(ecs::Entity projectile)
	{
		Vector3 position = ecs::GetComponent<Transform>(projectile).position;
		position.z += 500 + ((double)rand() / (double)RAND_MAX);

		Audio* explosionSound = engine::AddAudio("Gameplay", "audio/explosion.wav", false, 0.2f, DistanceModel::LINEAR);
		//explosionSound->play();

		engine::ecs::Entity explosion = engine::ecs::NewEntity();
		engine::ecs::AddComponent(explosion, engine::Transform{ .position = position, .scale = Vector3(70) });
		engine::ecs::AddComponent(explosion, engine::SpriteRenderer{ });
		engine::ecs::AddComponent(explosion, engine::Animator{ .onAnimationEnd = ecs::DestroyEntity });
		engine::ecs::AddComponent(explosion, engine::SoundComponent{ .Sounds = {{"Explosion", explosionSound}} });
		Vector2 explosionSize = Vector2(0.7, -0.7);
		std::vector<Vector2> explosionverts{ Vector2(explosionSize.x, explosionSize.x), Vector2(explosionSize.x, explosionSize.y), Vector2(explosionSize.y, explosionSize.y), Vector2(explosionSize.y, explosionSize.x) };
		engine::ecs::AddComponent(explosion, engine::PolygonCollider{ .vertices = explosionverts, .trigger = true, .visualise = true });
		engine::ecs::AddComponent(explosion, Projectile{ .owner = 0/*ecs::GetComponent<CruiseMissile>(projectile).owner*/, .hitType = HitStates::Stop, .hitSpeedFactor = 0.5, .hitTime = 1, .canHitSubmerged = true, .deleteAffterHit = false, .hitAnimation = "" });

		//Play explosion animation
		engine::AnimationSystem::AddAnimation(explosion, resources::explosionAnimation, "explosion");
		engine::AnimationSystem::PlayAnimation(explosion, "explosion", false);
	};
};