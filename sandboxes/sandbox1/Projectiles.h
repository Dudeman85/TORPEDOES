#pragma once 
#include <engine/Application.h>
#include "PlayerController.h"
#include "Resources.h"

using namespace engine;
enum HitStates
{
	None,			// Does nothing when hitting player
	Additive,		// Adds hitSpeedFactor additively:			100% + 50% + 50% = 200%
	Multiplicative,	// Adds hitSpeedFactor multiplicatively:	100% + 50% + 50% = 225%		Decreases are weaker, increases are stronger 
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

	/* Files */

	std::string hitAnimation = "explosion";
	std::string model = "torpedo.obj";
	
};


//Temporary functin for testing
void SpawnProjectile(ecs::Entity p, int playerID)
{
	std::cout << ecs::GetComponent<Transform>(p).position.ToString();
}

static void CreateAnimation(ecs::Entity entity)
{
	Projectile& projectile = ecs::GetComponent<Projectile>(entity);
	Transform& transform = ecs::GetComponent<Transform>(entity);
	Vector3 animPosition = transform.position;
	animPosition.z += 100;

	ecs::Entity torpedoAnim = ecs::NewEntity();
	ecs::AddComponent(torpedoAnim, Transform{ .position = animPosition + Vector3(0, 0, (double)rand() / ((double)RAND_MAX + 1)),  .scale = Vector3(20) });
	ecs::AddComponent(torpedoAnim, SpriteRenderer{ });
	ecs::AddComponent(torpedoAnim, Animator{ .onAnimationEnd = ecs::DestroyEntity });
	AnimationSystem::AddAnimation(torpedoAnim, resources::explosionAnimation, projectile.hitAnimation);
	AnimationSystem::PlayAnimation(torpedoAnim, projectile.hitAnimation, false);

};

static void OnProjectileCollision(Collision collision)
{
	if (collision.type == Collision::Type::tilemapTrigger)
	{
		if (collision.b != 1)
		{
			// Do animation projectile impact animation
			CreateAnimation(collision.a);
			ecs::DestroyEntity(collision.a);
		}
	}
}
