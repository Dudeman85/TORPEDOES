#pragma once 
#include <engine/Application.h>
#include "Resources.h"

using namespace engine;

struct Player;

enum HitStates
{
	None,			// Does nothing when hitting player
	Additive,		// Adds hitSpeedFactor additively:			100% + 50% + 50% = 200%		Flat increase/decrease
	Multiplicative,	// Adds hitSpeedFactor multiplicatively:	100% + 50% + 50% = 225%		Stacked decreases become constantly weaker, stacked increases become constantly stronger 
	Stop			// Stops and forces target to spin in place
};

ECS_REGISTER_COMPONENT(Projectile)
struct Projectile
{
	engine::ecs::Entity owner;

	float speed = 500;

	/* Hit */

	HitStates hitType = HitStates::Stop;
	float hitSpeedFactor = -0.05f;		// If hitType is not Stop, scale of speed change when hit
	float hitTime = 2.0f;				// Time the hit lasts
	bool canHitSubmerged = false;
	bool deleteAffterHit = true;

	/* Files */

	std::string hitAnimation = "explosion";
	std::string model = "Weapon_Torpedo.obj";
};

ECS_REGISTER_COMPONENT(Hedgehog)
struct Hedgehog
{
	float distanceTraveled = 0;	// Distance travelled
	float targetDistance = 0;	// Distance until explosion

	engine::ecs::Entity aimingGuide;
};

static void CreateExplosionAnimation(Vector3 animPosition)
{
	animPosition.z += 500;

	engine::ecs::Entity torpedoAnim = engine::ecs::NewEntity();
	Audio* explosion = engine::AddAudio("Gameplay", "audio/explosion.wav", false, 0.2f, DistanceModel::LINEAR);

	explosion->play();
	auto remover = [explosion, torpedoAnim](engine::ecs::Entity) { engine::ecs::DestroyEntity(torpedoAnim); };
	engine::ecs::AddComponent(torpedoAnim, engine::Animator{ .onAnimationEnd = remover });
	engine::ecs::AddComponent(torpedoAnim, engine::Transform{ .position = animPosition + Vector3(0, 0, ((double)rand() / (double)RAND_MAX) + 2), .scale = Vector3(20) });
	engine::ecs::AddComponent(torpedoAnim, engine::SpriteRenderer{ });
	engine::ecs::AddComponent(torpedoAnim, engine::SoundComponent{ .Sounds = {{"Explosion", explosion}} });

	//Play explosion animation
	engine::AnimationSystem::AddAnimation(torpedoAnim, resources::explosionAnimation, "hit");
	engine::AnimationSystem::PlayAnimation(torpedoAnim, "hit", false);
};

void CreateHedgehogExplosion(engine::ecs::Entity entity)
{
	engine::Transform& transform = engine::ecs::GetComponent<engine::Transform>(entity);
	Hedgehog& hedgehog = engine::ecs::GetComponent<Hedgehog>(entity);
	Projectile& projectile = engine::ecs::GetComponent<Projectile>(entity);

	engine::ecs::DestroyEntity(hedgehog.aimingGuide);

	Vector2 explosionSize = Vector2(0.7, -0.7);
	Vector2 explosionScale = Vector3(30);

	engine::ecs::Entity hedgehogExplosion = engine::ecs::NewEntity();
	engine::ecs::AddComponent(hedgehogExplosion, engine::Transform{ .position = Vector3(transform.position.x, transform.position.y, 200 + (double)rand() / ((double)RAND_MAX + 1)), .scale = explosionScale });
	engine::ecs::AddComponent(hedgehogExplosion, engine::SpriteRenderer{ });
	engine::ecs::AddComponent(hedgehogExplosion, engine::Animator{ .onAnimationEnd = engine::ecs::DestroyEntity });
	std::vector<Vector2> explosionverts{ Vector2(explosionSize.x, explosionSize.x), Vector2(explosionSize.x, explosionSize.y), Vector2(explosionSize.y, explosionSize.y), Vector2(explosionSize.y, explosionSize.x) };
	engine::ecs::AddComponent(hedgehogExplosion, engine::PolygonCollider{ .vertices = explosionverts, .trigger = true, .visualise = true });
	engine::ecs::AddComponent(hedgehogExplosion, Projectile{ .owner = projectile.owner, .hitType = HitStates::Stop, .hitSpeedFactor = 0.5, .hitTime = 1, .canHitSubmerged = true, .deleteAffterHit = false, .hitAnimation = "" });

	//Disable the hedgehog collider after .4 seconds
	engine::TimerSystem::ScheduleFunction([hedgehogExplosion]()
		{
			if (engine::ecs::EntityExists(hedgehogExplosion))
			{
				if (engine::ecs::HasComponent<engine::PolygonCollider>(hedgehogExplosion))
				{
					engine::ecs::RemoveComponent<engine::PolygonCollider>(hedgehogExplosion);
				}
			}
		}, 0.4);

	// check tilemap collision and activate explosion animation
	engine::collisionSystem->UpdateAABB(hedgehogExplosion);
	bool tileMapCollision = engine::collisionSystem->tilemap->checkCollision(transform.position.x, transform.position.y) > 1;
	bool entityCollision = false;
	for (engine::Collision& c : engine::collisionSystem->CheckCollision(hedgehogExplosion))
	{
		if (c.type == engine::Collision::Type::trigger)
		{
			if (engine::ecs::HasComponent<Player>(c.b))
			{
				entityCollision = true;
			}
		}
	}
	if (tileMapCollision || entityCollision)
	{
		engine::AnimationSystem::AddAnimation(hedgehogExplosion, resources::explosionAnimation, "explosion");
		engine::AnimationSystem::PlayAnimation(hedgehogExplosion, "explosion", false);
		Audio* explosionSound = engine::AddAudio("Gameplay", "audio/explosion.wav", false, 0.15f, DistanceModel::LINEAR);
		explosionSound->play();
		ecs::AddComponent(hedgehogExplosion, SoundComponent{ .Sounds = {{"Explosion", explosionSound}} });
	}
	else
	{
		engine::AnimationSystem::AddAnimation(hedgehogExplosion, resources::WaterexplosionAnimation, "Hedgehog_Explosion.png");
		engine::AnimationSystem::PlayAnimation(hedgehogExplosion, "Hedgehog_Explosion.png", false);
		Audio* explosionSound = engine::AddAudio("Gameplay", "audio/submerge_01.wav", false, 0.15f, DistanceModel::LINEAR);
		explosionSound->play();
		ecs::AddComponent(hedgehogExplosion, SoundComponent{ .Sounds = {{"Explosion", explosionSound}} });
	}
};

static void OnProjectileCollision(engine::Collision collision)
{
	if (collision.type == engine::Collision::Type::tilemapTrigger)
	{
		if (collision.b != 1)
		{
			// Do animation projectile impact animation
			Vector3 pos = engine::ecs::GetComponent<engine::Transform>(collision.a).position;
			if (ecs::GetComponent<Projectile>(collision.a).hitType != HitStates::Additive)
				pos += engine::ecs::GetComponent<Rigidbody>(collision.a).velocity.Normalize() * 20;

			CreateExplosionAnimation(pos);
			engine::ecs::DestroyEntity(collision.a);
		}
	}
}

ECS_REGISTER_SYSTEM(HedgehogSystem, engine::Rigidbody, engine::Transform, Hedgehog)
class HedgehogSystem : public engine::ecs::System
{
public:
	const float maxScale = 100.0f;
	const float minScale = 50.0f;
	const float minRotation = -35.0f;
	const float maxRotation = -130.0f;

	const float maxDistance = 700.0f;	// Full charge distance
	const float minDistance = 100.0f;	// No charge distance 
	const float chargeTime = 1.0f;		// Time until full charge

	const float speed = 1000.0f;
	const float aimSpeed = 700.0f;

	void Update()
	{
		// Iterate through entities in the system
		for (engine::ecs::Entity entity : entities)
		{
			// Get the entity and increment the iterator
			Hedgehog& hedgehogComp = engine::ecs::GetComponent<Hedgehog>(entity);
			engine::Transform& transformComp = engine::ecs::GetComponent<engine::Transform>(entity);

			if (hedgehogComp.distanceTraveled <= hedgehogComp.targetDistance)
			{
				// Projectile is still travelling to it's target distance

				// Increment distance travelled
				hedgehogComp.distanceTraveled += engine::ecs::GetSystem<HedgehogSystem>()->speed * std::min(engine::deltaTime, 1.0 / 20.0);;

				// Ratio of distance travelled to the target distance
				float distanceRatio = hedgehogComp.distanceTraveled / hedgehogComp.targetDistance;
				float rotation = minRotation + (maxRotation - minRotation) * distanceRatio;
				transformComp.rotation.z = rotation;
				transformComp.staleCache = true;

				// Map scale to distance ratio
				float scale = maxScale - (maxScale - minScale) * (2 * abs(0.5 - distanceRatio));

				transformComp.scale = Vector3(scale);
			}
			else
			{
				// Projectile has reached the end of it's trajectory
				CreateHedgehogExplosion(entity);

				engine::ecs::DestroyEntity(entity);
			}
		};
	}
};
