#pragma once 
#include <engine/Application.h>
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

ECS_REGISTER_COMPONENT(Hedgehog)
struct  Hedgehog : public Projectile
{
	float distanceTraveled = 0.0f;

};

//Temporary function for testing
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

				// Calcula el coficiente entre la distacia recorida y la distacia maxima 
				float distanceRatio = hedgehogComp.distanceTraveled / maxDistance;

				//// Calcula la rotación en función de la distancia recorrida
				TransformSystem::Rotate(entity, Vector3(0, 0, -1.5f ));

				// Calcula la escala en funcion de la distancia recorrida
				float scale = maxScale - (maxScale - minScale) * (2 * abs(0.5 - distanceRatio));
			
				// Actualiza la escala del objeto
				ecs::GetComponent<Transform>(entity).scale = Vector3(scale);

				
			}
			else
			{
				// Si se supera la distancia máxima, detén el movimiento del objeto
				ecs::DestroyEntity(entity);
				continue;
			}
			
			
			
			
		};
	}
};
