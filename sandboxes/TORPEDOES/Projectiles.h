#pragma once 
#include <engine/Application.h>
#include "Resources.h"

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
	int ownerID = 0;

	float speed = 500;

	/* Hit */

	HitStates hitType = HitStates::Stop;
	float hitSpeedFactor = -0.05f;		// If hitType is not Stop, scale of speed change when hit
	float hitTime = 2.0f;				// Time the hit lasts
	bool canHitSubmerged = false;

	/* Files */

	std::string hitAnimation = "explosion";
	std::string model = "torpedo.obj";
};

ECS_REGISTER_COMPONENT(Hedgehog)
struct  Hedgehog : public Projectile
{
	float distanceTraveled = 0;	// Distance travelled
	float targetDistance = 0;	// Distance until explosion
};

//Temporary function for testing
 void SpawnProjectile(engine::ecs::Entity p, int playerID)
 {
	std::cout << engine::ecs::GetComponent<engine::Transform>(p).position.ToString();
 }

static void CreateAnimation(engine::ecs::Entity entity)
{
	Projectile& projectile = engine::ecs::GetComponent<Projectile>(entity);
	engine::Transform& transform = engine::ecs::GetComponent<engine::Transform>(entity);
	Vector3 animPosition = transform.position;
	animPosition.z += 100;

	engine::ecs::Entity torpedoAnim = engine::ecs::NewEntity();
	engine::ecs::AddComponent(torpedoAnim, engine::Transform{ .position = animPosition + Vector3(0, 0, (double)rand() / ((double)RAND_MAX + 1)),  .scale = Vector3(20) });
	engine::ecs::AddComponent(torpedoAnim, engine::SpriteRenderer{ });
	engine::ecs::AddComponent(torpedoAnim, engine::Animator{ .onAnimationEnd = engine::ecs::DestroyEntity });

	// MIKA ME MUUTATIN COSNT STRING MISSÄ LUKKE "HIT" ÄLÄ MUOKATA SIKSI C++ EI OSA LUUKE OIKEA JOS IE CONST STRING
	engine::AnimationSystem::AddAnimation(torpedoAnim, resources::explosionAnimation, "hit");
	engine::AnimationSystem::PlayAnimation(torpedoAnim, "hit", false);
};

void CreateHedgehogExplosion(engine::ecs::Entity entity)
{
	engine::Transform& transform = engine::ecs::GetComponent<engine::Transform>(entity);

	engine::ecs::Entity hedgehogExplosion = engine::ecs::NewEntity();
	engine::ecs::AddComponent(hedgehogExplosion, engine::Transform{ .position = transform.position + Vector3(0, 0, 100 +(double)rand() / ((double)RAND_MAX + 1)),  .scale = Vector3(20) });
	engine::ecs::AddComponent(hedgehogExplosion, engine::SpriteRenderer{ });
	engine::ecs::AddComponent(hedgehogExplosion, engine::Animator{ .onAnimationEnd = engine::ecs::DestroyEntity });
	std::vector<Vector2> explosionverts{ Vector2(0.5, 0.55), Vector2(0.5, -0.55), Vector2(-0.5, -0.55), Vector2(-0.5, 0.55) };
	engine::ecs::AddComponent(hedgehogExplosion, engine::PolygonCollider{ .vertices = explosionverts, .trigger = true, .visualise = true });
	engine::ecs::AddComponent(hedgehogExplosion, Projectile{ .ownerID = -1, .hitType = HitStates::Stop, .hitSpeedFactor = 0.5, .hitTime = 1, .canHitSubmerged = true, .hitAnimation = "" });

	engine::AnimationSystem::AddAnimation(hedgehogExplosion, resources::explosionAnimation, "explosion");
	engine::AnimationSystem::PlayAnimation(hedgehogExplosion, "explosion", false);
};

static void OnProjectileCollision(engine::Collision collision)
{
	if (collision.type == engine::Collision::Type::tilemapTrigger)
	{
		if (collision.b != 1)
		{   
			// Do animation projectile impact animation
			CreateAnimation(collision.a);
			engine::ecs::DestroyEntity(collision.a);
		}
	}
}

static const float _HedgehogMaxDistance = 700.0f;	// Full charge distance
static const float _HedgehogMinDistance = 200.0f;	// No charge distance
static const float _HedgehogChargeTime = 1.0f;		// Time until full charge

ECS_REGISTER_SYSTEM(HedgehogSystem, engine::Rigidbody, engine::Transform, Hedgehog)
class HedgehogSystem : public engine::ecs::System
{
	const float hedgehogSpeed = 500.0f;
	const float maxScale = 200.0f;
	const float minScale = 100.0f;

	// TODO: these must be calculated
	const float minRotation = -50.0f;
	const float maxRotation = +50.0f;

public:
	void Update()
	{
		// Iterate through entities in the system
		for (engine::ecs::Entity entity: entities)
		{			
			// Get the entity and increment the iterator
			Hedgehog& hedgehogComp = engine::ecs::GetComponent<Hedgehog>(entity);
			engine::Transform& transformComp = engine::ecs::GetComponent<engine::Transform>(entity);

			if (hedgehogComp.distanceTraveled < hedgehogComp.targetDistance)
			{
				// Projectile is still travelling to it's target distance

				// Increment distance travelled
				hedgehogComp.distanceTraveled += hedgehogSpeed * engine::deltaTime;

				// Ratio of distance travelled to the target distance
				float distanceRatio = hedgehogComp.distanceTraveled / hedgehogComp.targetDistance;

				// TODO: Calculate based on distance travelled
				engine::TransformSystem::Rotate(entity, Vector3(0, 0, -130.5f * engine::deltaTime));

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
