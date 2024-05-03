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
	std::string model = "Weapon_Torpedo.obj";
	Audio* explosionSound;
};

ECS_REGISTER_COMPONENT(Hedgehog)
struct Hedgehog
{
	float distanceTraveled = 0;	// Distance travelled
	float targetDistance = 0;	// Distance until explosion

	engine::ecs::Entity aimingGuide;
	Audio* explosionSound;
};

static void CreateAnimation(engine::ecs::Entity entity)
{
	Projectile& projectile = engine::ecs::GetComponent<Projectile>(entity);
	engine::Transform& transform = engine::ecs::GetComponent<engine::Transform>(entity);
	Vector3 animPosition = transform.position;
	animPosition.z += 500;

	engine::ecs::Entity torpedoAnim = engine::ecs::NewEntity();

	Audio* audio = engine::AddAudio("Gameplay", "audio/glass-hit.mp3", false, 10000);
	//audio->pause();
	engine::ecs::AddComponent(torpedoAnim, engine::SoundComponent{ .Sounds = {{"GlassHit" ,audio}} });

	engine::ecs::AddComponent(torpedoAnim, engine::Transform{ .position = animPosition + Vector3(0, 0, ((double)rand() / (double)RAND_MAX) + 2), .scale = Vector3(20) });
	engine::ecs::AddComponent(torpedoAnim, engine::SpriteRenderer{ });
	engine::ecs::AddComponent(torpedoAnim, engine::Animator{ .onAnimationEnd = engine::ecs::DestroyEntity });

	// MIKA ME MUUTATIN COSNT STRING MISSÄ LUKKE "HIT" ÄLÄ MUOKATA SIKSI C++ EI OSA LUUKE OIKEA JOS IE CONST STRING
	engine::AnimationSystem::AddAnimation(torpedoAnim, resources::explosionAnimation, "hit");
	engine::AnimationSystem::PlayAnimation(torpedoAnim, "hit", false);

	engine::SoundComponent& soundComponent = engine::ecs::GetComponent<engine::SoundComponent>(torpedoAnim);
	//soundComponent.Sounds["GlassHit"]->play();

};

void CreateHedgehogExplosion(engine::ecs::Entity entity)
{
	engine::Transform& transform = engine::ecs::GetComponent<engine::Transform>(entity);
	Hedgehog& hedgehog = engine::ecs::GetComponent<Hedgehog>(entity);
	Projectile& projectile = engine::ecs::GetComponent<Projectile>(entity);

	engine::ecs::DestroyEntity(hedgehog.aimingGuide);

	Vector2 explosionSize = Vector2(0.7, -0.7);
	Vector2 explosionScale = Vector3(20);

	engine::ecs::Entity hedgehogExplosion = engine::ecs::NewEntity();
	engine::ecs::AddComponent(hedgehogExplosion, engine::Transform{ .position = transform.position + Vector3(0, 0, 100 + (double)rand() / ((double)RAND_MAX + 1)), .scale = explosionScale });
	engine::ecs::AddComponent(hedgehogExplosion, engine::SpriteRenderer{ });
	engine::ecs::AddComponent(hedgehogExplosion, engine::Animator{ .onAnimationEnd = engine::ecs::DestroyEntity });
	std::vector<Vector2> explosionverts{ Vector2(explosionSize.x, explosionSize.x), Vector2(explosionSize.x, explosionSize.y), Vector2(explosionSize.y, explosionSize.y), Vector2(explosionSize.y, explosionSize.x) };
	engine::ecs::AddComponent(hedgehogExplosion, engine::PolygonCollider{ .vertices = explosionverts, .trigger = true, .visualise = true });
	engine::ecs::AddComponent(hedgehogExplosion, Projectile{ .ownerID = projectile.ownerID, .hitType = HitStates::Stop, .hitSpeedFactor = 0.5, .hitTime = 1, .canHitSubmerged = true, .hitAnimation = "" });

	//create audio Explosion tilemap ID
	Audio* audio = engine::AddAudio("Gameplay", "audio/explosion.wav", false, 100);
	Audio* audio1 = engine::AddAudio("Gameplay", "audio/dink.wav", false, 100);
	audio->pause();
	audio1->pause();
	engine::ecs::AddComponent(hedgehogExplosion, engine::SoundComponent{ .Sounds = {{"Explosion" ,audio},{"Dink" , audio1}}});


	//Disable the hedgehog collider after .5 seconds
	engine::TimerSystem::ScheduleFunction([hedgehogExplosion]()
		{
			if(engine::ecs::EntityExists(hedgehogExplosion))
				if(engine::ecs::HasComponent<engine::PolygonCollider>(hedgehogExplosion))
					engine::ecs::RemoveComponent<engine::PolygonCollider>(hedgehogExplosion);
		}, 0.5);

	// aqui verifica si el id del tilecolare y activa la otra animacion 
	if (engine::collisionSystem->tilemap->checkCollision(transform.position.x, transform.position.y) > 1)
	{
		engine::AnimationSystem::AddAnimation(hedgehogExplosion, resources::explosionAnimation, "explosion");
		engine::AnimationSystem::PlayAnimation(hedgehogExplosion, "explosion", false);
		
		// Wall collision effect suond nime Explosion
		engine::SoundComponent& soundComponent = engine::ecs::GetComponent<engine::SoundComponent>(hedgehogExplosion);
		soundComponent.Sounds["Explosion"]->play();

	}
	else
	{
		engine::AnimationSystem::AddAnimation(hedgehogExplosion, resources::WaterexplosionAnimation, "Hedgehog_Explosion.png");
		engine::AnimationSystem::PlayAnimation(hedgehogExplosion, "Hedgehog_Explosion.png", false);

		// water collison tile sound effect nime Dink
		engine::SoundComponent& soundComponent = engine::ecs::GetComponent<engine::SoundComponent>(hedgehogExplosion);
		soundComponent.Sounds["Dink"]->play();
		
	}
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
public:
	const float maxScale = 100.0f;
	const float minScale = 50.0f;
	const float minRotation = -50.0f;
	const float maxRotation = +50.0f;

	const float maxDistance = 700.0f;	// Full charge distance
	const float minDistance = 100.0f;	// No charge distance 
	const float chargeTime = 1.0f;		// Time until full charge

	const float speed = 500.0f;

	void Update()
	{
		// Iterate through entities in the system
		for (engine::ecs::Entity entity : entities)
		{
			// Get the entity and increment the iterator
			Hedgehog& hedgehogComp = engine::ecs::GetComponent<Hedgehog>(entity);
			engine::Transform& transformComp = engine::ecs::GetComponent<engine::Transform>(entity);

			if (hedgehogComp.distanceTraveled <= hedgehogComp.targetDistance)
			{
				// Projectile is still travelling to it's target distance

				// Increment distance travelled
				hedgehogComp.distanceTraveled += engine::ecs::GetSystem<HedgehogSystem>()->speed * engine::deltaTime;

				// Ratio of distance travelled to the target distance
				float distanceRatio = hedgehogComp.distanceTraveled / hedgehogComp.targetDistance;

				// TODO: Calculate based on distance travelled
				engine::TransformSystem::Rotate(entity, Vector3(0, 0, -105.5f * engine::deltaTime));

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
