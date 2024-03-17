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


ECS_REGISTER_COMPONENT(Hedgehog)
struct Hedgehog
{
	int ownerID = 0;
};

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



void CreateHedgehog(Vector2 direction,   Vector3 spanwPposition , Vector3 sapawnRotation , int owerID)
{
	float hedgehogSpeedVo = 500.0f;
	float G = 25.0f ; //Gravity
	float angleShot = engine::Radians(70); 

	float vx = cos(angleShot) * hedgehogSpeedVo; // componenti in X
	float vy = sin(angleShot) * hedgehogSpeedVo; // componeti in Y
	
	ecs::Entity hedgehog = ecs::NewEntity();
	ecs::AddComponent(hedgehog, Transform{ .position = spanwPposition, .rotation = sapawnRotation, .scale = Vector3(100) });

    Vector3 finalVelocity = direction * Vector3(vx, vy, 0.0f) - (Vector3(0, G, 0) * deltaTime);

	ecs::AddComponent(hedgehog, Rigidbody{ .velocity = finalVelocity });

	ecs::AddComponent(hedgehog, ModelRenderer{ .model = resources::hedgehogModel });
	std::vector<Vector2> Hedgehogverts{ Vector2(0.2, 0.25), Vector2(0.2, -0.25), Vector2(-0.2, -0.25), Vector2(-0.2, 0.25) };
	ecs::AddComponent(hedgehog, PolygonCollider{ .vertices = Hedgehogverts, .callback = OnTopedoCollision, .trigger = true, .visualise = true,  .rotationOverride = sapawnRotation.y });
	ecs::AddComponent(hedgehog, Torpedo{ .ownerID = owerID });
	std::cout << "vx: " << vx << ", vy: " << vy << std::endl;
	std::cout << "Final Velocity: " << finalVelocity.ToString() << std::endl;
}


