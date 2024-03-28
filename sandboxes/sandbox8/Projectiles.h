#pragma once 
#include <engine/Application.h>
#include "PlayerController.h"
#include "Resources.h"

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
struct Hedgehog: public Projectile 
{
	float distanceTraveled = 0.0f;
	int ownerID = 0;
};

//Temporary functin for testing
void SpawnProjectile(engine::ecs::Entity p, int playerID)
{
	std::cout << engine::ecs::GetComponent<engine::Transform>(p).position.ToString();
}

static void CreateAnimation(Vector3 animPosition)
{
	engine::ecs::Entity torpedoAnim = engine::ecs::NewEntity();
	animPosition.z += 100;
	engine::ecs::AddComponent(torpedoAnim, engine::Transform{ .position = animPosition + Vector3(0, 0, (double)rand() / ((double)RAND_MAX + 1)),  .scale = Vector3(20) });
	engine::ecs::AddComponent(torpedoAnim, engine::SpriteRenderer{ });
	engine::ecs::AddComponent(torpedoAnim, engine::Animator{ .onAnimationEnd = engine::ecs::DestroyEntity });
	engine::AnimationSystem::AddAnimation(torpedoAnim, resources::explosionAnimation, "explosion");
	engine::AnimationSystem::PlayAnimation(torpedoAnim, "explosion", false);
};

// check if projectil collision tilemap Trigger
static void OnTopedoCollision(engine::Collision collision)
{
	engine::Transform& torpedotransfor = engine::ecs::GetComponent<engine::Transform>(collision.a);
	if (collision.type == engine::Collision::Type::tilemapTrigger)
	{
		if (collision.b != 1)
		{   // Do animation where projectile impact 
			CreateAnimation(torpedotransfor.position);
			engine::ecs::DestroyEntity(collision.a);
		}
	}
}
void CreateProjectile(Vector2 direction, float projectileSpeed, Vector3 spawnPosition, Vector3 sapawnRotation, int owerID)
{
	engine::ecs::Entity torpedo = engine::ecs::NewEntity();
	engine::ecs::AddComponent(torpedo, engine::Transform{ .position = spawnPosition, .rotation = sapawnRotation, .scale = Vector3(10) });
	engine::ecs::AddComponent(torpedo, engine::Rigidbody{ .velocity = direction * projectileSpeed });
	engine::ecs::AddComponent(torpedo, engine::ModelRenderer{ .model = resources::models["torpedo.obj"] });
	std::vector<Vector2> Torpedoverts{ Vector2(2, 0.5), Vector2(2, -0.5), Vector2(-2, -0.5), Vector2(-2, 0.5) };
	engine::ecs::AddComponent(torpedo, engine::PolygonCollider{ .vertices = Torpedoverts, .callback = OnTopedoCollision, .trigger = true, .visualise = false,  .rotationOverride = sapawnRotation.y });
	engine::ecs::AddComponent(torpedo, Projectile{ .ownerID = owerID });
}

ECS_REGISTER_SYSTEM(HeggehogSynten, engine::Rigidbody, engine::Transform, Hedgehog)
class HeggehogSynten : public engine::ecs::System
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
				engine::TransformSystem::Rotate(entity, Vector3(0, 0,-1.5f));

				// Calcula la escala en funcion de la distancia recorrida
				float scale = maxScale - (maxScale - minScale) * (2 * abs(0.5 - distanceRatio));

				// Actualiza la escala del objeto
				engine::ecs::GetComponent<engine::Transform>(entity).scale = Vector3(scale);

				engine::Transform& transformComp = engine::ecs::GetComponent<engine::Transform>(entity);
			}
			else
			{
				// Si se supera la distancia máxima, detén el movimiento del objeto
				engine::ecs::DestroyEntity(entity);
				break;
			}

		};
	}
};

void CreateHedgehog(Vector2 direction, Vector3 spanwPposition, Vector3 sapawnRotation, int owerID)
{
	float hedgehogSpeedVo = 500.0f;
	float distanceTraveled = 0.0f;

	engine::ecs::Entity hedgehog = engine::ecs::NewEntity();
	engine::ecs::AddComponent(hedgehog, engine::Transform{ .position = spanwPposition, .rotation = sapawnRotation });

	Vector3 finalVelocity = Vector3(direction.x, direction.y, 0.0f) * hedgehogSpeedVo;

	engine::ecs::AddComponent(hedgehog, engine::Rigidbody{ .velocity = finalVelocity });

	engine::ecs::AddComponent(hedgehog, engine::ModelRenderer{ .model = resources::models["hedgehog.obj"] });
	std::vector<Vector2> Hedgehogverts{ Vector2(0.2, 0.25), Vector2(0.2, -0.25), Vector2(-0.2, -0.25), Vector2(-0.2, 0.25) };
	engine::ecs::AddComponent(hedgehog, engine::PolygonCollider{ .vertices = Hedgehogverts, .callback = OnTopedoCollision, .trigger = true, .visualise = true,  .rotationOverride = sapawnRotation.y });
	engine::ecs::AddComponent(hedgehog, Hedgehog{ .ownerID = owerID });
}
