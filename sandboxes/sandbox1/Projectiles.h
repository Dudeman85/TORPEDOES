#pragma once 
#include <engine/Application.h>
#include "PlayerController.h"
#include "Resources.h"

using namespace engine;

ECS_REGISTER_COMPONENT(Torpedo)
struct Torpedo
{
	int ownerID = 0;
};	
ECS_REGISTER_COMPONENT(Cannon)
struct Cannon
{
	int ownerID = 0;
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
	void CreateProjectile(Vector2 direction, float projectileSpeed, Vector3 spawnPosition, Vector3 sapawnRotation, int owerID)
    {
		ecs::Entity torpedo = ecs::NewEntity();
	    ecs::AddComponent(torpedo, Transform{ .position = spawnPosition, .rotation = sapawnRotation, .scale = Vector3(10) });
	   ecs::AddComponent(torpedo, Rigidbody{ .velocity = direction * projectileSpeed });
	   ecs::AddComponent(torpedo, ModelRenderer{ .model = resources::torpedoModel });
	   std::vector<Vector2> Torpedoverts{ Vector2(2, 0.5), Vector2(2, -0.5), Vector2(-2, -0.5), Vector2(-2, 0.5) };
	   ecs::AddComponent(torpedo, PolygonCollider{ .vertices = Torpedoverts, .callback = OnTopedoCollision, .trigger = true, .visualise = false,  .rotationOverride = sapawnRotation.y });
	  ecs::AddComponent(torpedo, Torpedo{ .ownerID = owerID });
    }


	
	static void CreateCannonAnimation(Vector3 animPosition)
	{


		ecs::Entity cannonAnim = ecs::NewEntity();
		animPosition.z += 100;
		ecs::AddComponent(cannonAnim, Transform{ .position = animPosition + Vector3(0, 0, (double)rand() / ((double)RAND_MAX + 1)),  .scale = Vector3(10) });
		ecs::AddComponent(cannonAnim, SpriteRenderer{ });
		ecs::AddComponent(cannonAnim, Animator{ .onAnimationEnd = ecs::DestroyEntity });
		AnimationSystem::AddAnimation(cannonAnim, resources::explosionAnimation, "explosion");
		AnimationSystem::PlayAnimation(cannonAnim, "explosion", false);

	};
	static void OnCannonCollision(Collision collision)
	{
		Transform& cannontransform = ecs::GetComponent<Transform>(collision.a);
		if (collision.type == Collision::Type::tilemapTrigger)
		{
			//ecs::GetComponent<Rigidbody>(collision.a).velocity *= 0.99f;
			if (collision.b != 1)
			{   // Do animation where projectile impact 
				CreateCannonAnimation(cannontransform.position);
				ecs::DestroyEntity(collision.a);
			}
		}

	}
	void CreateCannonProjectile(Vector2 direction, float projectileSpeed, Vector3 spawnPosition, Vector3 sapawnRotation, int owerID)
	{
		ecs::Entity shell = ecs::NewEntity();
		ecs::AddComponent(shell, Transform{ .position = spawnPosition, .rotation = sapawnRotation, .scale = Vector3(30) });
		ecs::AddComponent(shell, Rigidbody{ .velocity = direction * projectileSpeed });
		ecs::AddComponent(shell, ModelRenderer{ .model = resources::shellModel });
		std::vector<Vector2> Shellverts{ Vector2(2, 0.5), Vector2(2, -0.5), Vector2(-2, -0.5), Vector2(-2, 0.5) };
		ecs::AddComponent(shell, PolygonCollider{ .vertices = Shellverts, .callback = OnCannonCollision, .trigger = true, .visualise = false});
		ecs::AddComponent(shell, Cannon{ .ownerID = owerID });
	}
