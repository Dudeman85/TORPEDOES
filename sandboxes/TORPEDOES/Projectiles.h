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

static void CreateAnimation(Vector3 animPosition)
{
	ecs::Entity torpedoAnim = ecs::NewEntity();
	animPosition.z += 100;
	ecs::AddComponent(torpedoAnim, Transform{ .position = animPosition + Vector3(0, 0, (double)rand() / ((double)RAND_MAX + 1)),  .scale = Vector3(20) });
	ecs::AddComponent(torpedoAnim, SpriteRenderer{ });
	ecs::AddComponent(torpedoAnim, Animator{ .onAnimationEnd = ecs::DestroyEntity });
	AnimationSystem::AddAnimation(torpedoAnim, resources::explosionAnimation, "explosion");
	AnimationSystem::PlayAnimation(torpedoAnim, "explosion", false);

};

// check if projectil collision tilemap Trigger
static void OnTopedoCollision(Collision collision)
			{
				Transform& torpedotransfor = ecs::GetComponent<Transform>(collision.a);
				if (collision.type == Collision::Type::tilemapTrigger)
				{
					if (collision.b != 1)
					{   // Do animation where projectile impact 
						CreateAnimation(torpedotransfor.position);
						ecs::DestroyEntity(collision.a);
					}
				}
			}
			void CreateProjectile(Vector2 direction, float projectileSpeed, Vector3 spawnPosition, Vector3 spawnRotation, int id)
			{
				ecs::Entity torpedo = ecs::NewEntity();
	ecs::AddComponent(torpedo, Transform{ .position = spawnPosition, .rotation = spawnRotation, .scale = Vector3(10) });
	ecs::AddComponent(torpedo, Rigidbody{ .velocity = direction * projectileSpeed });
	ecs::AddComponent(torpedo, ModelRenderer{ .model = resources::models["torpedo.obj"]});
	std::vector<Vector2> Torpedoverts{ Vector2(2, 0.5), Vector2(2, -0.5), Vector2(-2, -0.5), Vector2(-2, 0.5) };
	ecs::AddComponent(torpedo, PolygonCollider{ .vertices = Torpedoverts, .callback = OnTopedoCollision, .trigger = true, .visualise = false,  .rotationOverride = spawnPosition.y });
	ecs::AddComponent(torpedo, Projectile{ .ownerID = id });
}