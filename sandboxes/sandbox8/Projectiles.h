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
	float speed = 500.0f;
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
	//int ownerID = 0;
};

//Temporary functin for testing
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

static void CreateAniHedgehog(Vector3 animPosition)
{
	engine::ecs::Entity hedgehogAnim = engine::ecs::NewEntity();
	animPosition.z += 100;
	engine::ecs::AddComponent(hedgehogAnim, engine::Transform{ .position = animPosition + Vector3(0, 0, (double)rand() / ((double)RAND_MAX + 1)),  .scale = Vector3(20) });
	engine::ecs::AddComponent(hedgehogAnim, engine::SpriteRenderer{ });
	engine::ecs::AddComponent(hedgehogAnim, engine::Animator{ .onAnimationEnd = engine::ecs::DestroyEntity });
	std::vector<Vector2> explosionverts{ Vector2(0.2, 0.25), Vector2(0.2, -0.25), Vector2(-0.2, -0.25), Vector2(-0.2, 0.25) };
	engine::ecs::AddComponent(hedgehogAnim, engine::PolygonCollider{ .vertices = explosionverts, .trigger = true, .visualise = true });
	engine::ecs::AddComponent(hedgehogAnim, Projectile{.ownerID = -1 , .hitAnimation = ""});
	engine::AnimationSystem::AddAnimation(hedgehogAnim, resources::explosionAnimation, "explosion");
	engine::AnimationSystem::PlayAnimation(hedgehogAnim, "explosion", false);
};

// check if projectil collision tilemap Trigger
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
			engine::Transform& transformComp = engine::ecs::GetComponent<engine::Transform>(entity);

			if (hedgehogComp.distanceTraveled < maxDistance)
			{
				// Calcula la distancia recorrida sumando la distancia del paso actual
				hedgehogComp.distanceTraveled += hedgehogSpeedVo * engine::deltaTime; // deltaTime es el tiempo transcurrido desde el último frame
				
				// Calcula el coficiente entre la distacia recorida y la distacia maxima 
				float distanceRatio = hedgehogComp.distanceTraveled / maxDistance;

				//// Calcula la rotación en función de la distancia recorrida
				engine::TransformSystem::Rotate(entity, Vector3(0, 0,-1.65f));

				// Calcula la escala en funcion de la distancia recorrida
				float scale = maxScale - (maxScale - minScale) * (2 * abs(0.5 - distanceRatio));

				// Actualiza la escala del objeto
				transformComp.scale = Vector3(scale);

				
			}
			else
			{
				// Create the animation when the projectile reaches the end of the trajectory."
				CreateAniHedgehog(transformComp.position );

				// Si se supera la distancia máxima, detén el movimiento del objeto
				engine::ecs::DestroyEntity(entity);
				break;
			}

		};
	}
};

//void CreateHedgehog(Vector2 direction, Vector3 spanwPposition, Vector3 sapawnRotation, int owerID)
{
	float hedgehogSpeedVo = 500.0f;
	float distanceTraveled = 0.0f;

	engine::ecs::Entity hedgehog = engine::ecs::NewEntity();
	engine::ecs::AddComponent(hedgehog, engine::Transform{ .position = spanwPposition, .rotation = sapawnRotation });

	Vector3 finalVelocity = Vector3(direction.x, direction.y, 0.0f) * hedgehogSpeedVo;

	engine::ecs::AddComponent(hedgehog, engine::Rigidbody{ .velocity = finalVelocity });

	engine::ecs::AddComponent(hedgehog, engine::ModelRenderer{ .model = resources::models["hedgehog.obj"] });
	std::vector<Vector2> Hedgehogverts{ Vector2(0.2, 0.25), Vector2(0.2, -0.25), Vector2(-0.2, -0.25), Vector2(-0.2, 0.25) };
	//engine::ecs::AddComponent(hedgehog, engine::PolygonCollider{ .vertices = Hedgehogverts, .callback = OnTopedoCollision, .trigger = true, .visualise = true,  .rotationOverride = sapawnRotation.y });
	engine::ecs::AddComponent(hedgehog, Hedgehog{ .ownerID = owerID });
}

void CreateTridentHedgehogs(Vector2 direction, Vector3 spawnPosition, Vector3 spawnRotation, int ownerID)
{
	const float angleOffset = Radians(10.0f); // Ajuste de ángulo para las direcciones de los proyectiles
	float playerAngle = atan2(direction.y, direction.x);

	// Creamos los tres proyectiles (hedgehogs)
	for (int i = 0; i < 3; ++i)
	{
		// Calculamos el ángulo para cada proyectil
		float angle = playerAngle + i * angleOffset;

		// Calculamos la dirección para cada proyectil
		Vector2 modifiedDirection = Vector2(cos(angle ), sin(angle )) ;

		// Creamos el hedgehog
		CreateHedgehog(modifiedDirection, spawnPosition, spawnRotation, ownerID);
	}
}


