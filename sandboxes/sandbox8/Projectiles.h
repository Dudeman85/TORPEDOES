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

ECS_REGISTER_COMPONENT(Hedgehog)
struct Hedgehog
{
	float distanceTraveled = 0.0f;
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
	ecs::AddComponent(torpedo, ModelRenderer{ .model = resources::models["torpedo.obj"] });
	std::vector<Vector2> Torpedoverts{ Vector2(2, 0.5), Vector2(2, -0.5), Vector2(-2, -0.5), Vector2(-2, 0.5) };
	ecs::AddComponent(torpedo, PolygonCollider{ .vertices = Torpedoverts, .callback = OnTopedoCollision, .trigger = true, .visualise = false,  .rotationOverride = sapawnRotation.y });
	ecs::AddComponent(torpedo, Projectile{ .ownerID = owerID });
}

ECS_REGISTER_SYSTEM(HeggehogSynten, Rigidbody, Transform, Hedgehog)


class HeggehogSynten : public ecs::System
{
	const float hedgehogSpeedVo = 500.0f;
	const float maxDistance = 700.0f;
	const float maxScale = 200.0f;
	const float minScale = 100.0f;
	const float minRotation = -50.0f;
	const float maxRotation = +50.0f;
public:
	void Update()
	{
		// Iterate through entities in the system
		for (auto itr = entities.begin(); itr != entities.end();)
		{			// Get the entity and increment the iterator
			ecs::Entity entity = *itr++;
			Hedgehog& hedgehogComp = ecs::GetComponent<Hedgehog>(entity);

			if (hedgehogComp.distanceTraveled < maxDistance)
			{
				// Calcula la distancia recorrida sumando la distancia del paso actual
				hedgehogComp.distanceTraveled += hedgehogSpeedVo * deltaTime; // deltaTime es el tiempo transcurrido desde el último frame

				float distanceRatio = hedgehogComp.distanceTraveled / maxDistance;
				float scale = maxScale - (maxScale - minScale) * (2 * abs(0.5 - distanceRatio));
				float hedgehogRotation = maxRotation - (maxRotation - minRotation) * (2 * abs(0.5 - distanceRatio));

				// Actualiza la escala del objeto
				ecs::GetComponent<Transform>(entity).scale = Vector3(scale);
				Transform& transformComp = ecs::GetComponent<Transform>(entity);
				transformComp.rotation.z = hedgehogRotation;
			}
			// Verifica si se ha superado la distancia máxima permitida
			else
			{
				// Si se supera la distancia máxima, detén el movimiento del objeto
				ecs::DestroyEntity(entity);
				break;
			}

		};
	}
};

void CreateHedgehog(Vector2 direction, Vector3 spanwPposition, Vector3 sapawnRotation, int owerID)
{
	float hedgehogSpeedVo = 500.0f;
	float distanceTraveled = 0.0f;


	ecs::Entity hedgehog = ecs::NewEntity();
	ecs::AddComponent(hedgehog, Transform{ .position = spanwPposition, .rotation = sapawnRotation });

	Vector3 finalVelocity = Vector3(direction.x, direction.y, 0.0f) * hedgehogSpeedVo;


	ecs::AddComponent(hedgehog, Rigidbody{ .velocity = finalVelocity });

	ecs::AddComponent(hedgehog, ModelRenderer{ .model = resources::models["hedgehog.obj"] });
	std::vector<Vector2> Hedgehogverts{ Vector2(0.2, 0.25), Vector2(0.2, -0.25), Vector2(-0.2, -0.25), Vector2(-0.2, 0.25) };
	ecs::AddComponent(hedgehog, PolygonCollider{ .vertices = Hedgehogverts, .callback = OnTopedoCollision, .trigger = true, .visualise = true,  .rotationOverride = sapawnRotation.y });
	ecs::AddComponent(hedgehog, Hedgehog{ .ownerID = owerID });



}


