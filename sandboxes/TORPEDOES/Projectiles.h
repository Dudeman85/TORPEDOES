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
	engine::AnimationSystem::AddAnimation(torpedoAnim, resources::explosionAnimation, projectile.hitAnimation);
	engine::AnimationSystem::PlayAnimation(torpedoAnim, projectile.hitAnimation, false);
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

ECS_REGISTER_SYSTEM(HedgehogSystem, engine::Rigidbody, engine::Transform, Hedgehog)
class HedgehogSystem : public engine::ecs::System
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
			engine::ecs::Entity entity = *itr++;
			Hedgehog& hedgehogComp = engine::ecs::GetComponent<Hedgehog>(entity);

			if (hedgehogComp.distanceTraveled < maxDistance)
			{
				// Calcula la distancia recorrida sumando la distancia del paso actual
				hedgehogComp.distanceTraveled += hedgehogSpeedVo * engine::deltaTime; // deltaTime es el tiempo transcurrido desde el último frame

				// Calcula el coficiente entre la distacia recorida y la distacia maxima 
				float distanceRatio = hedgehogComp.distanceTraveled / maxDistance;

				//// Calcula la rotación en función de la distancia recorrida
				engine::TransformSystem::Rotate(entity, Vector3(0, 0, -1.5f ));

				// Calcula la escala en funcion de la distancia recorrida
				float scale = maxScale - (maxScale - minScale) * (2 * abs(0.5 - distanceRatio));
			
				// Actualiza la escala del objeto
				engine::ecs::GetComponent<engine::Transform>(entity).scale = Vector3(scale);

			}
			else
			{
				// Si se supera la distancia máxima, detén el movimiento del objeto
				engine::ecs::DestroyEntity(entity);
				continue;
			}
		};
	}
};
