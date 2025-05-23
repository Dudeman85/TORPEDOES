#pragma once 
#include <bitset>
#include <engine/Application.h>
#include "engine/Input.h"
#include "engine/Timing.h"
#include "engine/Random.h"
#include "engine/SoundComponent.h"
#include "Globals.h"
#include "Resources.h"
#include "Projectiles.h"
#include "CruiseMissile.h"

using namespace engine;


enum ShipType { torpedoBoat, submarine, cannonBoat, hedgehogBoat, pirateShip };

static std::vector<engine::ecs::Entity> checkpointEntities;

static void ReturnToMainMenu();

struct indicatorStruct
{
	engine::ecs::Entity entity;
	std::vector<engine::Texture*> textures;

	indicatorStruct(engine::ecs::Entity e, std::vector<engine::Texture*> t) : entity(e), textures(t)
	{

	}
};

ECS_REGISTER_COMPONENT(Player)
struct Player
{
	int id = 0;

	// Movement stats
	float forwardSpeed = 400;
	float reverseSpeed = 233;

	float rotationSpeed = 120;
	float minSpeedWhileTurning = 1;

	float offtrackSpeedScale = 0.6;
	Vector2 forwardDirection;

	float _speedScale = 1;
	float _boostScale = 1;
	bool _offroadThisFrame = false;

	// State variables
	bool specialEnabled = false;
	bool submerged = false;

	// Action cooldowns
	float shootCooldown = 0.2f;			// Time between shots
	float specialCooldown = 0.8f;		// Time between special uses
	float ammoRechargeCooldown = 1.f;	// Time between gaining ammo

	bool holdShoot = false;				// Whether we can hold to shoot
	int ammo = 0;
	int maxAmmo = 2;
	int ammoPerReload = 1;

	// Action timers
	float _ammoRechargeTimer = 0.0f;
	float _shootTimer = 0.0f;
	float _specialTimer = 0.0f;

	// Action functions
	std::function<void(engine::ecs::Entity)> shootAction;
	std::function<void(engine::ecs::Entity)> specialAction;

	// Checkpoint
	int previousCheckpoint = -1;
	int lap = 1;

	// Hit
	std::vector<std::pair<Projectile, float>> hitProjectiles;

	// Rendered child entities
	std::vector<indicatorStruct> shootIndicators;
	std::vector<indicatorStruct> specialIndicators;

	std::function<void(engine::ecs::Entity)> shootIndicatorUpdate;
	std::function<void(engine::ecs::Entity)> specialIndicatorUpdate;

	//Children
	engine::ecs::Entity renderedEntity;
	engine::ecs::Entity nameText;
	engine::ecs::Entity animationEntity;
	engine::ecs::Entity wakeAnimationEntity;
	engine::ecs::Entity checkpointIndicatorEntity;

	//Next checkpoint object
	engine::ecs::Entity nextCheckpoint;
};

ECS_REGISTER_COMPONENT(CheckPoint)
struct CheckPoint
{
	int checkPointID = 0;
	bool Finish_line = false;
};

ECS_REGISTER_COMPONENT(SubmarineComponent)
struct SubmarineComponent
{
	float maxSubmergeTime = 3;
	float timeSubmerged = 0;
	bool submerged = false;
};

/* TORPEDO */

void CreateTorpedo(engine::ecs::Entity entity)
{
	Player& player = ecs::GetComponent<Player>(entity);
	Transform& transform = ecs::GetComponent<Transform>(entity);
	Transform& modelTransform = ecs::GetComponent<Transform>(player.renderedEntity);
	PolygonCollider& playerCollider = ecs::GetComponent<PolygonCollider>(entity);

	float speed = 800;

	ecs::Entity torpedo = ecs::NewEntity();

	engine::SoundComponent& sound = ecs::GetComponent<engine::SoundComponent>(entity);
	if(!sound.Sounds["ShootTorpedo"]->getPlaying())
		sound.Sounds["ShootTorpedo"]->play();
	else
		sound.Sounds["ShootTorpedo2"]->play();

	// Torpedo shoot sound 

	ecs::AddComponent(torpedo, Projectile{ .owner = entity, .speed = speed, .hitType = HitStates::Stop, .hitTime = 0.8 });
	Projectile& torpedoProjectile = ecs::GetComponent<Projectile>(torpedo);
	ecs::AddComponent(torpedo, Transform{ .position = transform.position, .rotation = modelTransform.rotation, .scale = Vector3(15) });
	ecs::AddComponent(torpedo, Rigidbody{ .velocity = player.forwardDirection * torpedoProjectile.speed });
	std::vector<Vector2> Torpedoverts{ Vector2(2, 0.5), Vector2(2, -0.5), Vector2(-2, -0.5), Vector2(-2, 0.5) };
	ecs::AddComponent(torpedo, PolygonCollider{ .vertices = Torpedoverts, .callback = OnProjectileCollision, .trigger = true, .layer = 4, .visualise = true,  .rotationOverride = playerCollider.rotationOverride });
	ecs::AddComponent(torpedo, ModelRenderer{ .model = resources::models[torpedoProjectile.model] });
}

/* SHELL */

void CreateShell(engine::ecs::Entity entity)
{
	Player& player = ecs::GetComponent<Player>(entity);
	Transform& transform = ecs::GetComponent<Transform>(entity);
	Transform& modelTransform = ecs::GetComponent<Transform>(player.renderedEntity);
	player._ammoRechargeTimer = 0;

	float speed = 1200;

	ecs::Entity shell = ecs::NewEntity();

	// Play shell shoot
	engine::SoundComponent& sound = ecs::GetComponent<engine::SoundComponent>(entity);
	sound.Sounds["ShootShell"]->play();

	ecs::AddComponent(shell, Projectile{ .owner = entity, .speed = speed, .hitType = HitStates::Additive, .hitSpeedFactor = -0.15f, .hitTime = 2.f });
	Projectile& shellProjectile = ecs::GetComponent<Projectile>(shell);
	ecs::AddComponent(shell, Transform{ .position = transform.position, .rotation = modelTransform.rotation, .scale = Vector3(30) });
	ecs::AddComponent(shell, Rigidbody{ .velocity = player.forwardDirection * shellProjectile.speed });
	ecs::AddComponent(shell, ModelRenderer{ .model = resources::models[shellProjectile.model = "Weapon_CannonAmmo.obj"] });
	float shellSize = 0.1;
	std::vector<Vector2> shellverts{ Vector2(shellSize, shellSize), Vector2(shellSize, -shellSize), Vector2(-shellSize, -shellSize), Vector2(-shellSize, shellSize) };
	ecs::AddComponent(shell, PolygonCollider{ .vertices = shellverts, .callback = OnProjectileCollision, .trigger = true, .layer = 4, .visualise = true,  .rotationOverride = std::abs(modelTransform.rotation.y) });
}

/* MULTISHOT */

using ShootFunc = std::function<void(engine::ecs::Entity, engine::ecs::Entity, Vector2, float)>;

void Multishot(ShootFunc callfunc, engine::ecs::Entity entity, std::vector<engine::ecs::Entity> entities, float angleOffset, int amount, float timeHeld)
{
	using namespace engine;

	Player& player = ecs::GetComponent<Player>(entity);

	const float playerAngle = atan2(player.forwardDirection.y, player.forwardDirection.x);

	// If amount is even, we offset by this amount at the start
	float evenOffset = 0;

	int positive_i = 0;
	int negative_i = 1;
	int extra = 0;

	Vector2 modifiedDirection;

	if (amount % 2)
	{
		// Amount is odd: Shoot to the center with 0 angle offset
		modifiedDirection = Vector2(cos(playerAngle), sin(playerAngle));
		--amount;
		++positive_i;
		extra = 1;

		callfunc(entity, entities[0], modifiedDirection, timeHeld);
	}
	else
	{
		// Amount is even: Add half the offset
		evenOffset = angleOffset / 2;
	}

	// Shoot positive:
	for (; positive_i < (amount / 2) + extra; ++positive_i)
	{
		float angle = playerAngle + evenOffset + (positive_i * angleOffset);

		modifiedDirection = Vector2(cos(angle), sin(angle));

		callfunc(entity, entities[positive_i], modifiedDirection, timeHeld);
	}

	// Shoot negative:
	for (int negative_i = 1; negative_i < (amount / 2) + 1; ++negative_i)
	{
		float angle = playerAngle + evenOffset + (negative_i * -angleOffset);

		modifiedDirection = Vector2(cos(angle), sin(angle));

		callfunc(entity, entities[positive_i + negative_i - 1], modifiedDirection, timeHeld);
	}
}

/* HEDGEHOG */

void CreateHedgehog(engine::ecs::Entity entity, engine::ecs::Entity aimingGuide, Vector2 direction, float timeHeld)
{
	using namespace engine;
	Player& player = ecs::GetComponent<Player>(entity);
	Transform& transform = ecs::GetComponent<Transform>(entity);
	Transform& modelTransform = ecs::GetComponent<Transform>(player.renderedEntity);

	engine::ecs::Entity hedgehog = engine::ecs::NewEntity();

	engine::ecs::AddComponent(hedgehog, Transform{ .position = transform.position + Vector3(0, 0, 100), .rotation = modelTransform.rotation });
	Vector3 finalVelocity = Vector3(direction.x, direction.y, 0.0f) * ecs::GetSystem<HedgehogSystem>()->speed;
	engine::ecs::AddComponent(hedgehog, engine::Rigidbody{ .velocity = finalVelocity });
	engine::ecs::AddComponent(hedgehog, ModelRenderer{ .model = resources::models["Weapon_HedgehogAmmo.obj"] });
	std::vector<Vector2> Hedgehogverts{ Vector2(0.4, 0.5), Vector2(0.4, -0.5), Vector2(-0.4, -0.5), Vector2(-0.4, 0.5) };
	ecs::AddComponent(hedgehog, Projectile{ .owner = entity });

	// Play shell shoot
	engine::SoundComponent& sound = ecs::GetComponent<engine::SoundComponent>(entity);
	sound.Sounds["ShootShell"]->play();

	auto& hedgehogTransform = engine::ecs::GetComponent<engine::Transform>(hedgehog);
	auto& aimingGuideTransform = engine::ecs::GetComponent<engine::Transform>(aimingGuide);
	ecs::AddComponent(hedgehog,
		Hedgehog
		{
			.targetDistance = (hedgehogTransform.position - aimingGuideTransform.position).Length(),
			.aimingGuide = aimingGuide
		});
}

struct aimingGuideStruct
{
	std::vector<engine::ecs::Entity> entities;
	float* totalTime;
	engine::ScheduledFunction* timerFunction;
};

static std::map<int, aimingGuideStruct> playerIdToAimGuides;

void AimHedgehog(engine::ecs::Entity entity, std::vector<engine::ecs::Entity> aimingGuides, float guideSpeed, float shootAngle, int shootAmount)
{
	Player& player = ecs::GetComponent<Player>(entity);

	*playerIdToAimGuides[player.id].totalTime += engine::deltaTime;

	if (input::GetPressed("Shoot" + std::to_string(player.id)))
	{
		player._ammoRechargeTimer = 0;
		// When button is pressed, move aim guide forward
		Transform& transform = ecs::GetComponent<Transform>(entity);
		Transform& modelTransform = ecs::GetComponent<Transform>(player.renderedEntity);

		Transform& playerTransform = ecs::GetComponent<Transform>(entity);

		const float playerAngle = atan2(player.forwardDirection.y, player.forwardDirection.x);

		// If amount is even, we offset by this amount at the start
		float evenOffset = 0;

		int positive_i = 0;
		int negative_i = 1;
		int extra = 0;

		Vector2 modifiedDirection;

		if (shootAmount % 2)
		{
			// Amount is odd: Move guide to the center with 0 angle offset
			modifiedDirection = Vector2(cos(playerAngle), sin(playerAngle));
			--shootAmount;
			++positive_i;
			extra = 1;

			Transform& guideTransform = ecs::GetComponent<Transform>(aimingGuides[0]);
			guideTransform.position = playerTransform.position
				+ (modifiedDirection * ecs::GetSystem<HedgehogSystem>()->minDistance)
				+ ((modifiedDirection * guideSpeed * std::min(*playerIdToAimGuides[player.id].totalTime, ecs::GetSystem<HedgehogSystem>()->chargeTime)));
			guideTransform.scale = 30;
		}
		else
		{
			// Amount is even: Add half the offset
			evenOffset = shootAngle / 2;
		}

		// Guide positive:
		for (; positive_i < (shootAmount / 2) + extra; ++positive_i)
		{
			float angle = playerAngle + evenOffset + (positive_i * shootAngle);

			modifiedDirection = Vector2(cos(angle), sin(angle));

			Transform& guideTransform = ecs::GetComponent<Transform>(aimingGuides[positive_i]);
			guideTransform.position = playerTransform.position
				+ (modifiedDirection * ecs::GetSystem<HedgehogSystem>()->minDistance)
				+ ((modifiedDirection * guideSpeed * std::min(*playerIdToAimGuides[player.id].totalTime, ecs::GetSystem<HedgehogSystem>()->chargeTime)));
			guideTransform.scale = 20;

			guideTransform.position.z += 170;
		}
		// Guide negative:
		for (int negative_i = 1; negative_i < (shootAmount / 2) + 1; ++negative_i)
		{
			float angle = playerAngle + evenOffset + (negative_i * -shootAngle);

			modifiedDirection = Vector2(cos(angle), sin(angle));

			Transform& guideTransform = ecs::GetComponent<Transform>(aimingGuides[(negative_i)+(positive_i - 1)]);
			guideTransform.position = playerTransform.position
				+ (modifiedDirection * ecs::GetSystem<HedgehogSystem>()->minDistance)
				+ ((modifiedDirection * guideSpeed * std::min(*playerIdToAimGuides[player.id].totalTime, ecs::GetSystem<HedgehogSystem>()->chargeTime)));
			guideTransform.scale = 20;

			guideTransform.position.z += 170;
		}
	}
	else
	{
		// When button is released, shoot
		Multishot(CreateHedgehog, entity, aimingGuides, shootAngle, shootAmount, *playerIdToAimGuides[player.id].totalTime);
		player.ammo -= aimingGuides.size();
		player.ammo = std::clamp(player.ammo, 0, player.maxAmmo);

		playerIdToAimGuides[player.id].entities.clear();

		delete playerIdToAimGuides[player.id].totalTime;
		playerIdToAimGuides[player.id].totalTime = nullptr;

		playerIdToAimGuides[player.id].timerFunction->repeat = false;

		playerIdToAimGuides.erase(player.id);

		for (auto& aimingGuide : aimingGuides)
		{
			auto& spriteRenderer = engine::ecs::GetComponent<engine::SpriteRenderer>(aimingGuide);

			spriteRenderer.texture = resources::uiTextures["crosshair.png"];
		}
	}
}

void CreateAimingGuides(engine::ecs::Entity entity, float guideSpeed, float shootAngle, int shootAmount)
{
	Transform& transform = ecs::GetComponent<Transform>(entity);

	Player& player = ecs::GetComponent<Player>(entity);

	// Create aiming guides
	std::vector<engine::ecs::Entity> aimingGuides;
	for (int i = 0; i < shootAmount; ++i)
	{
		engine::ecs::Entity newGuide = engine::ecs::NewEntity();

		engine::ecs::AddComponent(newGuide, engine::Transform{ .position = transform.position, .rotation = transform.rotation, .scale = transform.scale });
		engine::ecs::AddComponent(newGuide, engine::SpriteRenderer{ .texture = resources::uiTextures["Orange_crosshair.png"] });

		aimingGuides.push_back(newGuide);
	}

	// Deleted when AimHedgehog finishes
	float* time = new float(0);
	playerIdToAimGuides[player.id] =
	{
		aimingGuides, time,
		// Start 1-frame looping timer to check button press
		engine::timerSystem->ScheduleFunction(&AimHedgehog, 1, true, ScheduledFunction::Type::frames, entity, aimingGuides, guideSpeed, shootAngle, shootAmount)
	};
}

void ShootHedgehog(engine::ecs::Entity entity)
{
	Player& player = ecs::GetComponent<Player>(entity);

	// Whether this player id has shot once before
	if (playerIdToAimGuides.find(player.id) != playerIdToAimGuides.end())
	{
		if (!playerIdToAimGuides[player.id].entities.empty())
		{
			// Do not add aiming guides if this player already has them
			return;
		}
	}
	float shootAngle = Radians(8.0f);
	//float shootAmount = player.ammo;
	float shootAmount = 4;
	//player.ammo++; // We don't use up ammo until we shoot

	CreateAimingGuides(entity, ecs::GetSystem<HedgehogSystem>()->aimSpeed, shootAngle, shootAmount);
}

void ShootCruiseMissile(ecs::Entity owner);

/* BOOST */

static void BoostEnd(engine::ecs::Entity entity, float boostStrenght)
{
	engine::SoundComponent& sound = ecs::GetComponent<engine::SoundComponent>(entity);
	sound.Sounds["boostAudio"]->pause();
	Player& player = engine::ecs::GetComponent<Player>(entity);
	player._boostScale -= boostStrenght;

	player._specialTimer = player.specialCooldown;

	AnimationSystem::PlayAnimation(player.wakeAnimationEntity, "normal", true);
}

// Increases player speed for a short while
void Boost(engine::ecs::Entity entity)
{
	engine::SoundComponent& sound = ecs::GetComponent<engine::SoundComponent>(entity);
	sound.Sounds["boostAudio"]->play();
	double boostTime = 1.5f;
	float boostStrenght = 0.4f;

	Player& player = engine::ecs::GetComponent<Player>(entity);
	player.specialEnabled = false;
	player._boostScale += boostStrenght;

	AnimationSystem::PlayAnimation(player.wakeAnimationEntity, "boost", true);

	engine::timerSystem->ScheduleFunction(&BoostEnd, boostTime, false, engine::ScheduledFunction::Type::seconds, entity, boostStrenght);
}

//Submerge function for submarine
void ToggleSubmerge(engine::ecs::Entity playerEntity)
{
	Player& playerComponent = ecs::GetComponent<Player>(playerEntity);
	Transform& transformComponent = ecs::GetComponent<Transform>(playerEntity);
	Transform& modelTransform = ecs::GetComponent<Transform>(playerComponent.renderedEntity);
	engine::SoundComponent& sound = ecs::GetComponent<engine::SoundComponent>(playerEntity);

	//Submerge if surfaced
	if (!playerComponent.submerged)
	{
		//Make the continuous diving animation
		playerComponent.animationEntity = ecs::NewEntity();
		ecs::AddComponent(playerComponent.animationEntity, Transform{ .position = {0, 0, 1}, .scale = {5, 2, 0} });
		ecs::AddComponent(playerComponent.animationEntity, SpriteRenderer{});
		ecs::AddComponent(playerComponent.animationEntity, Animator{ });
		TransformSystem::SetRotation(playerComponent.animationEntity, { 0, 0, modelTransform.rotation.y });
		AnimationSystem::AddAnimation(playerComponent.animationEntity, resources::continuousDivingAnim, "diving");
		AnimationSystem::PlayAnimation(playerComponent.animationEntity, "diving", true);
		TransformSystem::AddParent(playerComponent.animationEntity, playerEntity);
		sound.Sounds["submergeAudio"]->play();

		TransformSystem::Translate(playerEntity, { 0, 0, -20 });

		SubmarineComponent& sc = ecs::GetComponent<SubmarineComponent>(playerEntity);
		sc.submerged = true;
		sc.timeSubmerged = 0;

		//Finished submerging after .3 second
		TimerSystem::ScheduleFunction(
			[playerEntity]()
			{
				Player& playerComponent = ecs::GetComponent<Player>(playerEntity);

				playerComponent.submerged = true;
				ecs::GetComponent<PolygonCollider>(playerEntity).layer = 2;
				ecs::GetComponent<ModelRenderer>(playerComponent.renderedEntity).textures = { resources::subUnderwaterTexture };
			}, 0.0);
	}
	//Surface if submerged
	else
	{
		//Make sure the submarine is not under a bridge
		ecs::GetComponent<PolygonCollider>(playerEntity).layer = 1;
		std::vector<Collision> collisions = collisionSystem->CheckCollision(playerEntity);
		for (const Collision& c : collisions)
		{
			//If any hits were with a player, disallow surfacing
			if (c.type == Collision::Type::collision)
			{
				if (ecs::GetComponent<PolygonCollider>(c.b).layer == 1)
				{
					ecs::GetComponent<PolygonCollider>(playerEntity).layer = 2;
					return;
				}
			}
			else
			{
				//If any hits were with a bridge tile, disallow surfacing
				if (collisionSystem->GetTileCollisionLayer(c.b) == 3)
				{
					ecs::GetComponent<PolygonCollider>(playerEntity).layer = 2;
					return;
				}
			}
		}

		SubmarineComponent& sc = ecs::GetComponent<SubmarineComponent>(playerEntity);
		sc.submerged = false;
		sc.timeSubmerged = 0;

		//Get rid of the submerged bubbles animation
		ecs::DestroyEntity(playerComponent.animationEntity);
		playerComponent.animationEntity = 0;

		//Start surfacing and speed up
		playerComponent.specialEnabled = false;
		TransformSystem::Translate(playerEntity, { 0, 0, 20 });

		//Finished surfacing after .3 second
		TimerSystem::ScheduleFunction(
			[playerEntity]()
			{
				Player& playerComponent = ecs::GetComponent<Player>(playerEntity);
				playerComponent.submerged = false;
				ecs::GetComponent<ModelRenderer>(playerComponent.renderedEntity).textures = { resources::playerIdToTexture[playerComponent.id] };
			}, 0.3);
	}

	//Make the diving bubbles animation
	ecs::Entity divingEntity = ecs::NewEntity();
	ecs::AddComponent(divingEntity, Transform{ .position = {0, 0, 4}, .scale = {5, 2, 0} });
	ecs::AddComponent(divingEntity, SpriteRenderer{});
	ecs::AddComponent(divingEntity, Animator{ .onAnimationEnd = ecs::DestroyEntity });
	TransformSystem::SetRotation(divingEntity, { 0, 0, modelTransform.rotation.y });
	AnimationSystem::AddAnimation(divingEntity, resources::divingAnim, "dive");
	AnimationSystem::PlayAnimation(divingEntity, "dive");
	TransformSystem::AddParent(divingEntity, playerEntity);
}

/* INDICATOR UPDATES */

void CannonIndicatorUpdate(ecs::Entity entity)
{
	Player& player = ecs::GetComponent<Player>(entity);

	if (player.shootIndicators.empty())
	{
		return;
	}

	indicatorStruct& it0 = player.shootIndicators.front();
	indicatorStruct& it1 = player.shootIndicators.back();
	Transform& tf0 = ecs::GetComponent<Transform>(it0.entity);
	Transform& tf1 = ecs::GetComponent<Transform>(it1.entity);

	tf1.position.z = tf0.position.z - 0.001 + ((float)player.ammo - ((float)player.maxAmmo / 2)) / 50.f;
}

void HedgehogIndicatorUpdate(engine::ecs::Entity entity)
{
	Player& player = engine::ecs::GetComponent<Player>(entity);

	if (player.shootIndicators.empty())
	{
		return;
	}

	indicatorStruct& it = player.shootIndicators.back();

	engine::SpriteRenderer& sprite = engine::ecs::GetComponent<engine::SpriteRenderer>(it.entity);
	if (player.ammo > 0)
	{
		sprite.texture = it.textures[0];
	}
	else
	{
		sprite.texture = it.textures[1];
	}
}

void TorpedoIndicatorUpdate(engine::ecs::Entity entity)
{
	Player& player = engine::ecs::GetComponent<Player>(entity);

	if (player.shootIndicators.size() < player.maxAmmo)
	{
		return;
	}

	for (int i = 0; i < player.maxAmmo; i++)
	{
		indicatorStruct& it = player.shootIndicators[i];

		engine::SpriteRenderer& sprite = engine::ecs::GetComponent<engine::SpriteRenderer>(it.entity);
		if (player.ammo > i)
		{
			sprite.texture = it.textures[0];
		}
		else
		{
			sprite.texture = it.textures[1];
		}
	}
}

void BoostIndicatorUpdate(engine::ecs::Entity entity)
{
	Player& player = engine::ecs::GetComponent<Player>(entity);

	if (player.specialIndicators.empty())
	{
		return;
	}

	indicatorStruct& it = player.specialIndicators[0];
	engine::SpriteRenderer& sprite = engine::ecs::GetComponent<engine::SpriteRenderer>(it.entity);

	if (player.specialCooldown <= player._specialTimer && player.specialEnabled)
	{
		// Available to use
		sprite.texture = it.textures[0];
	}
	else if (player.specialCooldown <= player._specialTimer && !player.specialEnabled)
	{
		// Not available
		sprite.texture = it.textures[2];
	}
	else
	{
		// In use
		sprite.texture = it.textures[1];
	}
}

void SubmergeIndicatorUpdate(engine::ecs::Entity entity)
{
	Player& player = engine::ecs::GetComponent<Player>(entity);

	indicatorStruct& it = player.specialIndicators[0];
	engine::SpriteRenderer& sprite = engine::ecs::GetComponent<engine::SpriteRenderer>(it.entity);

	if (player.submerged)
	{
		if (player.specialEnabled)
		{
			sprite.texture = it.textures[0];
		}
		else
		{
			sprite.texture = it.textures[1];
		}
	}
	else
	{
		if (player.specialEnabled)
		{

			sprite.texture = it.textures[2];
		}
		else
		{
			sprite.texture = it.textures[3];
		}
	}
}

// Player controller System. Requires Player , Tranform , Rigidbody , PolygonCollider
ECS_REGISTER_SYSTEM(PlayerController, Player, engine::Transform, engine::Rigidbody, engine::PolygonCollider)
class PlayerController : public engine::ecs::System
{
	static engine::ecs::Entity winScreen;
	static engine::ecs::Entity winTimer;

	//A map from a ship type to a pre-initialized Player component with the proper stats
	std::unordered_map<ShipType, Player> shipComponents;
	//A map from a ship type to its 3D model
	std::unordered_map<ShipType, engine::Model*> shipModels;

public:
	//Timer to return to main menu
	static float hasWon;
	float countdownTimer = 0;
	static int lapCount; // How many laps to race through

	void Init()
	{
		shipComponents.clear();
		//Initialize each ship type's stats
		shipComponents.insert(
			{
				ShipType::torpedoBoat, Player
				{
					.forwardSpeed = 550, .rotationSpeed = 100,
					.shootCooldown = 0.2, .specialCooldown = 5, .ammoRechargeCooldown = 2,
					.holdShoot = false, .maxAmmo = 2,
					.shootAction = CreateTorpedo, .specialAction = Boost,
					.shootIndicatorUpdate = TorpedoIndicatorUpdate, .specialIndicatorUpdate = BoostIndicatorUpdate
				}
			});
		shipComponents.insert(
			{
				ShipType::submarine, Player
				{
					.forwardSpeed = 500, .rotationSpeed = 150,
					.shootCooldown = 0.2, .specialCooldown = 1, .ammoRechargeCooldown = 2,
					.holdShoot = false, .maxAmmo = 2,
					.shootAction = CreateTorpedo, .specialAction = ToggleSubmerge,
					.shootIndicatorUpdate = TorpedoIndicatorUpdate, .specialIndicatorUpdate = SubmergeIndicatorUpdate
				}
			});
		shipComponents.insert(
			{
				ShipType::cannonBoat, Player
				{
					.forwardSpeed = 480, .rotationSpeed = 180,
					.shootCooldown = 0.15, .specialCooldown = 5, .ammoRechargeCooldown = 4,
					.holdShoot = true, .maxAmmo = 20, .ammoPerReload = 20,
					.shootAction = CreateShell, .specialAction = Boost,
					.shootIndicatorUpdate = CannonIndicatorUpdate, .specialIndicatorUpdate = BoostIndicatorUpdate
				}
			});
		shipComponents.insert(
			{
				ShipType::hedgehogBoat, Player
				{
					.forwardSpeed = 500, .rotationSpeed = 150,
					.shootCooldown = 0.4, .specialCooldown = 5, .ammoRechargeCooldown = 5,
					.holdShoot = false, .maxAmmo = 1,
					.shootAction = ShootHedgehog, .specialAction = ShootCruiseMissile,
					.shootIndicatorUpdate = HedgehogIndicatorUpdate, .specialIndicatorUpdate = BoostIndicatorUpdate
				}
			});

		//Initialize ship type models
		shipModels.insert({ ShipType::torpedoBoat, resources::models["Ship_PT_109_Torpedo.obj"] });
		shipModels.insert({ ShipType::submarine, resources::models["Ship_U_99_Submarine.obj"] });
		shipModels.insert({ ShipType::cannonBoat, resources::models["Ship_Yamato_Cannon.obj"] });
		shipModels.insert({ ShipType::hedgehogBoat, resources::models["Ship_HMCS_Sackville_Hedgehog.obj"] });
	}

	static void MakeWinEntity()
	{
		//Create the entity to be shown at a win
		winScreen = engine::ecs::NewEntity();
		engine::ecs::AddComponent(winScreen, engine::TextRenderer{ .font = resources::niagaraFont, .text = "", .offset = Vector3(-1.5, 2, 1), .scale = Vector3(0.03f), .color = Vector3(1.f, 1.f, 1.f), .uiElement = true });
		engine::ecs::AddComponent(winScreen, engine::SpriteRenderer{ .texture = resources::uiTextures["winner.png"], .enabled = true, .uiElement = true });
		engine::ecs::AddComponent(winScreen, engine::Transform{ .position = Vector3(0, 0, 0.5f), .scale = Vector3(0.3f) });

		winTimer = engine::ecs::NewEntity();
		engine::ecs::AddComponent(winTimer, engine::TextRenderer{ .font = resources::niagaraFont, .text = "", .offset = Vector3(-1.5, -2, 1), .scale = Vector3(0.013f), .color = Vector3(1.f, 1.f, 1.f), .uiElement = true });
		engine::ecs::AddComponent(winTimer, engine::Transform{ .position = Vector3(-0.05, -0.15, 0), .scale = Vector3(0.3f) });
	}

	//Kill the players who fell behind on the river map
	void PurgeSlowPlayers()
	{
		ecs::Entity lastPlayer = 0;
		int lastCheckpoint = 100000;
		for (ecs::Entity entity : entities)
		{
			Player& pc = ecs::GetComponent<Player>(entity);

			if (pc.previousCheckpoint <= lastCheckpoint)
			{
				lastPlayer = entity;
				lastCheckpoint = pc.previousCheckpoint;
			}
		}

		if (ecs::EntityExists(lastPlayer))
		{
			engine::ecs::Entity torpedoAnim = engine::ecs::NewEntity();
			engine::ecs::AddComponent(torpedoAnim, engine::Animator{ .onAnimationEnd = ecs::DestroyEntity });
			engine::ecs::AddComponent(torpedoAnim, engine::Transform{ .position = ecs::GetComponent<Transform>(lastPlayer).position + Vector3(0, 0, 100 + ((double)rand() / (double)RAND_MAX) + 2), .scale = Vector3(60)});
			engine::ecs::AddComponent(torpedoAnim, engine::SpriteRenderer{ });
			engine::ecs::AddComponent(torpedoAnim, engine::SoundComponent{ .Sounds = {{"Explosion", resources::explosion.back()}} });
			resources::explosion.back()->play();

			//Play explosion animation
			engine::AnimationSystem::AddAnimation(torpedoAnim, resources::explosionAnimation, "hit");
			engine::AnimationSystem::PlayAnimation(torpedoAnim, "hit", false);

			Player& pc = ecs::GetComponent<Player>(lastPlayer);
			ecs::DestroyEntity(pc.specialIndicators.front().entity);
			ecs::DestroyEntity(pc.shootIndicators.front().entity);
			ecs::DestroyEntity(pc.shootIndicators.back().entity);
			ecs::DestroyEntity(pc.checkpointIndicatorEntity);

			ecs::DestroyEntity(lastPlayer);
		}
	}

	//Get the min and max bounds of every player
	std::array<float, 4> GetPlayerBounds()
	{
		std::array<float, 4> playerBounds{ -INFINITY, -INFINITY, INFINITY, INFINITY };

		for (engine::ecs::Entity entity : entities)
		{
			engine::Transform& transform = engine::ecs::GetComponent<engine::Transform>(entity);

			if (playerBounds[0] < transform.position.y)
				playerBounds[0] = transform.position.y;
			if (playerBounds[1] < transform.position.x)
				playerBounds[1] = transform.position.x;
			if (playerBounds[2] > transform.position.y)
				playerBounds[2] = transform.position.y;
			if (playerBounds[3] > transform.position.x)
				playerBounds[3] = transform.position.x;
		}
		return playerBounds;
	}

	static void OnCollision(engine::Collision collision)
	{
		// collision.a is always a player
		// Get references to the involved components
		Player& player = engine::ecs::GetComponent<Player>(collision.a);

		// Slow player down when off track
		if (collision.type == engine::Collision::Type::tilemapCollision)
		{
			player._offroadThisFrame = true;
		}
		else
		{
			// Check if the collision involves a checkpoint
			if (engine::ecs::HasComponent<CheckPoint>(collision.b))
			{
				CheckPoint& checkpoint = engine::ecs::GetComponent<CheckPoint>(collision.b);	// Get checkpoint component
				if (player.previousCheckpoint + 1 == checkpoint.checkPointID)			// Check whether player collided with next checkpoint
				{
					player.previousCheckpoint = checkpoint.checkPointID;				// Set as previous checkpoint

					if (checkpoint.Finish_line)
					{
						if (player.lap == lapCount)
						{
							if (hasWon <= 0)
							{
								//Display the win screen
								hasWon = 7;
								MakeWinEntity();
								engine::ecs::GetComponent<engine::TextRenderer>(winScreen).text = "Player " + std::to_string(player.id + 1);
								engine::ecs::GetComponent<engine::SpriteRenderer>(winScreen).enabled = true;
								ecs::GetComponent<SpriteRenderer>(player.checkpointIndicatorEntity).enabled = false;

								TimerSystem::ScheduleFunction(ReturnToMainMenu, hasWon);
							}
						}
						else
						{
							player.nextCheckpoint = checkpointEntities[0];
							player.previousCheckpoint = -1;
							player.lap++;
						}
					}
					else
					{
						player.nextCheckpoint = checkpointEntities[player.previousCheckpoint + 1];
					}
				}
			}
			// Check if the collision involves a projectile
			else if (engine::ecs::HasComponent<Projectile>(collision.b))
			{
				Projectile& projectile = engine::ecs::GetComponent<Projectile>(collision.b); // projectile is collision.b 

				if (player.submerged && !projectile.canHitSubmerged)
				{
					// Submerged hit: Ignore
					return;
				}

				if (collision.a != projectile.owner)
				{
					for (auto& hitProjectile : player.hitProjectiles)
					{
						// If player has been hit by stop, do not add current hit
						if (hitProjectile.first.hitType == HitStates::Stop)
						{
							goto SkipAddingHit;
						}
					}
					// If the new hit is stop, clear all other hits
					if (projectile.hitType == HitStates::Stop)
					{
						player.hitProjectiles.clear();
					}
					// Add the new hit
					player.hitProjectiles.push_back({ projectile, 0.f });
				SkipAddingHit:

					// Do animation projectile impact animation
					Vector3 pos = engine::ecs::GetComponent<engine::Transform>(collision.b).position;
					if (ecs::HasComponent<Rigidbody>(collision.b))
						if (ecs::GetComponent<Projectile>(collision.b).hitType != HitStates::Additive)
							pos += engine::ecs::GetComponent<Rigidbody>(collision.b).velocity.Normalize() * 20;

					//Destroy projectile at end of frame
					if (projectile.deleteAffterHit == true)
					{
						CreateExplosionAnimation(pos);
						engine::ecs::DestroyEntity(collision.b);
					}
				}
			}
		}
	}

	/// PlayerController Update 
	void Update(GLFWwindow* window)
	{
		//Don't do anything until countdown is done
		if (countdownTimer > 0)
		{
			countdownTimer -= engine::deltaTime;
			return;
		}
		//Countdown to return to menu
		if (hasWon > 0)
		{
			hasWon -= engine::deltaTime;
			engine::ecs::GetComponent<engine::TextRenderer>(winTimer).text = "Returning to menu in " + std::to_string((int)round(hasWon));
		}

		// Iterate through entities in the system
		for (engine::ecs::Entity entity : entities)
		{
			// Get necessary components
			Player& player = engine::ecs::GetComponent<Player>(entity);
			engine::Transform& transform = engine::ecs::GetComponent<engine::Transform>(entity);
			engine::Transform& modelTransform = engine::ecs::GetComponent<engine::Transform>(player.renderedEntity);
			engine::Rigidbody& rigidbody = engine::ecs::GetComponent<engine::Rigidbody>(entity);
			engine::PolygonCollider& collider = engine::ecs::GetComponent<engine::PolygonCollider>(entity);

			// Initialize inputs
			float accelerationInput = input::GetTotalInputValue("Throttle" + std::to_string(player.id));
			float rotateInput = input::GetTotalInputValue("Turn" + std::to_string(player.id));

			bool newShotInput = 0;
			if (player.holdShoot)
			{
				newShotInput = input::GetPressed("Shoot" + std::to_string(player.id));
			}
			else
			{
				newShotInput = input::GetNewPress("Shoot" + std::to_string(player.id));
			}
			bool newSpecialInput = input::GetPressed("Boost" + std::to_string(player.id));

			accelerationInput = std::clamp(accelerationInput, -1.0f, 1.0f);
			rotateInput = std::clamp(rotateInput, -1.0f, 1.0f);

			/* Projectile hit logic  */

			for (auto it = player.hitProjectiles.begin(); it != player.hitProjectiles.end();)
			{
				// Increase the timer value
				it->second += engine::deltaTime;

				if (it->second >= it->first.hitTime)
				{
					it = player.hitProjectiles.erase(it); // Erase the element and move iterator to the next element
					continue;
				}
				++it; // Move to the next element
			}

			player._speedScale = 1.f; // 100%
			for (auto& hitProjectile : player.hitProjectiles)
			{
				switch (hitProjectile.first.hitType)
				{
				case HitStates::Stop:
					// Rotate player
					engine::TransformSystem::Rotate(player.renderedEntity, 0, (360.0f / hitProjectile.first.hitTime) * engine::deltaTime, 0);

					// Ignore all input
					accelerationInput = 0;
					rotateInput = 0;
					newShotInput = false;
					newSpecialInput = false;
					break;
				case HitStates::Additive:
					player._speedScale = std::max(player._speedScale += hitProjectile.first.hitSpeedFactor, 0.f);
					break;
				case HitStates::Multiplicative:
					player._speedScale = std::max(player._speedScale *= hitProjectile.first.hitSpeedFactor, 0.f);
					break;
				default:
					break;
				}
			}

			/* Movement */

			// CALCULATE POSITION AND ROTATION
			// Calculate the forward direction
			player.forwardDirection = Vector2(cos(glm::radians(modelTransform.rotation.y)), sin(glm::radians(modelTransform.rotation.y)));
			player.forwardDirection.Normalize();

			// Initialize the impulse as zero
			Vector2 forwardImpulse(0.0f, 0.0f);

			// Apply acceleration impulse if positive input is received
			if (accelerationInput > 0.0f)
			{
				forwardImpulse = player.forwardDirection * accelerationInput * player.forwardSpeed;
			}
			// Apply deceleration impulse if negative input is received
			if (accelerationInput < 0.0f)
			{
				forwardImpulse = player.forwardDirection * accelerationInput * player.reverseSpeed;
			}
			if (rotateInput != 0.0f)
			{
				// Slow rotation based on speed
				// TODO: this function could be improved by testing
				float rotationScalar = std::clamp(std::abs(log10(rigidbody.velocity.Length() / player.forwardSpeed / 2)), 0.5f, 1.0f);

				float trueRotateInput = -rotateInput * player.rotationSpeed * rotationScalar;

				// Apply forward impulse if rotating or receiving a rotation command
				engine::TransformSystem::Rotate(player.renderedEntity, 0, trueRotateInput * engine::deltaTime, 0);

				// Set min speed while turning

				Vector2 minRotateImpulse = player.forwardDirection * std::abs(trueRotateInput) * player.minSpeedWhileTurning;
				if (accelerationInput < 0.0f)
				{
					minRotateImpulse *= -1;

				}

				if (minRotateImpulse.Length() > forwardImpulse.Length())
				{
					forwardImpulse = minRotateImpulse;
				}


			}

			//Set the collider's rotation
			collider.rotationOverride = (int)std::round(modelTransform.rotation.y) % 360;
			if (modelTransform.rotation.y < 0)
				collider.rotationOverride = 360 - std::abs(collider.rotationOverride);

			// Increase the special timer
			player._specialTimer += engine::deltaTime;

			// If the special cooldown has passed
			while (player._specialTimer >= player.specialCooldown)
			{
				if (!newSpecialInput || !player.specialEnabled)
				{
					// We haven't pressed the special button or we don't have special enabled, keep _specialTimer at max value
					player._specialTimer = player.specialCooldown;
					break;
				}

				// Do special action
				player.specialAction(entity);

				// Decrease the action time 
				player._specialTimer -= player.specialCooldown;
			}

			float finalBoostScale = player._boostScale;
			if (player._offroadThisFrame && !player.submerged)
			{
				finalBoostScale *= player.offtrackSpeedScale;
			}

			// Apply the final impulse to the object
			engine::PhysicsSystem::AddForce(entity, (forwardImpulse * player._speedScale * finalBoostScale));

			// Reset offroad status for this frame
			player._offroadThisFrame = false;

			/* Shooting */

			bool reachedMaxAmmoThisFrame = false;
			bool shotThisFrame = false;

			// Increase the projectile timer
			player._shootTimer += engine::deltaTime;

			// If the projectile cooldown has passed
			while (player._shootTimer >= player.shootCooldown)
			{
				if (!newShotInput)
				{
					// We haven't pressed the shoot button, keep shootTimer at max value
					player._shootTimer = player.shootCooldown;
					break;
				}

				if (player.ammo <= 0)
				{
					player.ammo = 0;
					// We don't have ammo, keep shootTimer at max value
					player._shootTimer = player.shootCooldown;
					break;
				}

				// Create a projectile using the parameters of the player object.
				player.shootAction(entity);

				player.ammo--;

				// Decrease the projectile shoot time 
				player._shootTimer -= player.shootCooldown;
			}

			// If not max ammo
			if (player.ammo < player.maxAmmo)
			{
				player._ammoRechargeTimer += engine::deltaTime;

				while (player._ammoRechargeTimer >= player.ammoRechargeCooldown)
				{
					// Add ammo
					player.ammo += player.ammoPerReload;

					if (player.ammo < player.maxAmmo)
					{
						// Not max ammo, continue recharge
						player._ammoRechargeTimer -= player.ammoRechargeCooldown;
					}
					else
					{
						// Max ammo, handle recharge after shooting
						player.ammo = player.maxAmmo;
						reachedMaxAmmoThisFrame = true;
						break;
					}
				}
			}

			// Max ammo, stop rechage
			if (reachedMaxAmmoThisFrame)
			{
				if (player.ammo < player.maxAmmo)
				{
					// Not max ammo, continue recharge
					player._ammoRechargeTimer -= player.ammoRechargeCooldown;
				}
				else
				{
					// Max ammo, ignore recharge done for next ammo
					player._ammoRechargeTimer = 0;
				}
			}

			player.shootIndicatorUpdate(entity);
			player.specialIndicatorUpdate(entity);

			if (player.animationEntity != 0)
			{
				TransformSystem::SetRotation(player.animationEntity, { 0, 0, modelTransform.rotation.y });
			}
			TransformSystem::SetRotation(player.wakeAnimationEntity, { 0, 0, modelTransform.rotation.y - 90 });

			//Wake animation stuff
			if (rigidbody.velocity.Length() > 100)
			{
				ecs::GetComponent<SpriteRenderer>(player.wakeAnimationEntity).enabled = true;
			}
			else if (rigidbody.velocity.Length() < 50)
			{
				ecs::GetComponent<SpriteRenderer>(player.wakeAnimationEntity).enabled = false;
			}

			ecs::GetComponent<Animator>(player.wakeAnimationEntity).playbackSpeed = std::lerp(0.1f, 2.0f, rigidbody.velocity.Length() / 900.0f);
			Vector3 wakeOffset = player.forwardDirection.Normalize() * 20;
			wakeOffset += Vector3(0, 0, 40);
			TransformSystem::SetPosition(player.wakeAnimationEntity, transform.position - wakeOffset);

			engine::SoundComponent& soundComponent = engine::ecs::GetComponent<engine::SoundComponent>(entity);
			soundComponent.Sounds["Engine"]->setPitch((player.id * 0.001) + rigidbody.velocity.Length() / (166.0f * 4) + player.forwardDirection.Length() + forwardImpulse.Length() / (166.0f * 16));

			if (soundComponent.Sounds["Engine"]->getAtEnd())
			{
				soundComponent.Sounds["Engine"]->play();
			}

			//Rotate the checkpoint indicator
			Vector3 nextCheckpointPosition = ecs::GetComponent<Transform>(player.nextCheckpoint).position - transform.position;
			float nextCheckpointAngle = Degrees(atan2(nextCheckpointPosition.y, nextCheckpointPosition.x)) - 90;
			if (nextCheckpointAngle < 0)
				nextCheckpointAngle += 360;
			float currentAngle = ecs::GetComponent<Transform>(player.checkpointIndicatorEntity).rotation.z;
			//This is so shit
			while (currentAngle < 0)
				currentAngle += 360;
			while (currentAngle > 360)
				currentAngle -= 360;

			//Rotate ccw if it is closer
			float rotationDirection = abs(nextCheckpointAngle - currentAngle) > 180 ? -1 : 1;
			//Apply rotation with a maximum speed
			TransformSystem::SetPosition(player.checkpointIndicatorEntity, transform.position + Vector3(0, 0, (double)rand() / (double)RAND_MAX));
			TransformSystem::Rotate(player.checkpointIndicatorEntity, Vector3(0, 0, rotationDirection * clamp((nextCheckpointAngle - currentAngle) * deltaTime * 60, -3.0, 3.0)));
		}
	}

	indicatorStruct CreateIndicator(engine::ecs::Entity entity, Vector3 pos, Vector3 scale, std::vector<std::string> textureNames)
	{
		Player& player = engine::ecs::GetComponent<Player>(entity);

		engine::ecs::Entity shootIndicator = engine::ecs::NewEntity();

		engine::ecs::AddComponent(shootIndicator, engine::SpriteRenderer{ .texture = resources::uiTextures[textureNames[0]] });
		engine::ecs::AddComponent(shootIndicator, engine::Transform{ .position = pos + Vector3(0, 0, 60 + ((double)rand() / RAND_MAX)), .scale = scale });
		engine::TransformSystem::AddParent(shootIndicator, entity);

		std::vector<engine::Texture*> textures;
		for (auto& textureName : textureNames)
		{
			textures.push_back(resources::uiTextures[textureName]);
		}

		return indicatorStruct(shootIndicator, textures);
	}

	float generateEquidistantPoint(float minRange, float maxRange, int numOfPoints, int iteration)
	{
		double interval = (maxRange - minRange) / (numOfPoints - 1);

		return minRange + (iteration * interval);
	}

	//Get the first player
	ecs::Entity GetLeadingPlayer()
	{
		ecs::Entity leadingPlayer = 0; // 0 is the invalid value according to ecs.h
		int maxLap = -1;
		int maxCheckpoint = -1;
		float minDistanceToNext = std::numeric_limits<float>::max();

		for (ecs::Entity entity : entities)
		{
			Player& player = ecs::GetComponent<Player>(entity);
			float distanceToNext = TransformSystem::Distance(entity, player.nextCheckpoint);

			if (player.lap > maxLap ||
				(player.lap == maxLap && player.previousCheckpoint > maxCheckpoint) ||
				(player.lap == maxLap && player.previousCheckpoint == maxCheckpoint && distanceToNext < minDistanceToNext))
			{
				maxLap = player.lap;
				maxCheckpoint = player.previousCheckpoint;
				minDistanceToNext = distanceToNext;
				leadingPlayer = entity;
			}
		}

		return leadingPlayer;
	}

	//Spawn 1-4 players, all in a line from top to bottom
	void CreatePlayers(std::unordered_map<int, ShipType> playerShips, Vector2 startPos)
	{
		hasWon = 0;
		Vector2 offset(0, 60);
		for (const auto& playerShip : playerShips)
		{
			//Make all the necessary entities
			engine::ecs::Entity playerEntity = engine::ecs::NewEntity();
			engine::ecs::Entity playerNameText = engine::ecs::NewEntity();
			engine::ecs::Entity playerRender = engine::ecs::NewEntity();
			engine::ecs::Entity wakeAnimation = engine::ecs::NewEntity();
			engine::ecs::Entity checkpointIndicator = engine::ecs::NewEntity();

			//Create the player entity which contains everything but rendering
			//Player component is a bit special
			engine::ecs::AddComponent(playerEntity, shipComponents[playerShip.second]);
			Player& playerComponent = engine::ecs::GetComponent<Player>(playerEntity);
			playerComponent.id = playerShip.first;
			playerComponent.renderedEntity = playerRender;
			playerComponent.nameText = playerNameText;
			playerComponent.wakeAnimationEntity = wakeAnimation;
			playerComponent.checkpointIndicatorEntity = checkpointIndicator;
			playerComponent._specialTimer = playerComponent.specialCooldown;

			engine::ecs::AddComponent(playerEntity, engine::Transform{ .position = Vector3(startPos - offset * playerShip.first, 150), .rotation = Vector3(0, 0, 0), .scale = Vector3(7) });
			engine::ecs::AddComponent(playerEntity, engine::Rigidbody{ .drag = 1.5 });
			vector<Vector2> colliderVerts{ Vector2(3, 1), Vector2(3, -1), Vector2(-3, -1), Vector2(-3, 1) };
			engine::ecs::AddComponent(playerEntity, engine::PolygonCollider{ .vertices = colliderVerts, .callback = PlayerController::OnCollision, .trigger = false, .layer = 1, .visualise = true });

			//Create the player's rendered entity
			engine::ecs::AddComponent(playerRender, engine::Transform{ .rotation = Vector3(45, 0, 0), .scale = Vector3(1.5) });
			engine::ecs::AddComponent(playerRender, engine::ModelRenderer{ .model = shipModels[playerShip.second], .textures = {resources::playerIdToTexture[playerShip.first]} });
			engine::TransformSystem::AddParent(playerRender, playerEntity);

			//Create the wake rendered entity
			engine::ecs::AddComponent(wakeAnimation, engine::Transform{ .position = Vector3(0, 0, 0), .rotation = Vector3(0, 0, 0), .scale = Vector3(12, 25, 0) });
			engine::ecs::AddComponent(wakeAnimation, engine::SpriteRenderer{ .enabled = true });
			engine::ecs::AddComponent(wakeAnimation, engine::Animator{  });
			AnimationSystem::AddAnimations(wakeAnimation, resources::wakeAnims, { "boost", "normal" });
			AnimationSystem::PlayAnimation(wakeAnimation, "normal", true);

			engine::ecs::AddComponent(checkpointIndicator, engine::Transform{ .position = ecs::GetComponent<Transform>(playerEntity).position, .rotation = Vector3(0), .scale = Vector3(1.2 * 7, 7 * 7, 0) });
			engine::ecs::AddComponent(checkpointIndicator, engine::SpriteRenderer{ .texture = resources::uiTextures["Checkpoint_Arrow.png"], .enabled = true });
			//engine::TransformSystem::AddParent(checkpointIndicator, playerEntity);
			playerComponent.nextCheckpoint = checkpointEntities[0];
			//Rotate the checkpoint indicator
			Vector3 nextCheckpointPosition = ecs::GetComponent<Transform>(playerComponent.nextCheckpoint).position - ecs::GetComponent<Transform>(playerEntity).position;
			float nextCheckpointAngle = atan2(nextCheckpointPosition.y, nextCheckpointPosition.x);
			TransformSystem::SetRotation(playerComponent.checkpointIndicatorEntity, Vector3(0, 0, Degrees(nextCheckpointAngle) - 90));

			
			// Create shoot indicators
			float rangeEnd = -3.2;
			float rangeStart = -4.5;
			Vector3 offset = Vector3(1, -1, 10);
			Vector3 scale = Vector3(1.5, 1.5, 1);
			auto func = *playerComponent.shootAction.target<void(*)(engine::ecs::Entity)>();

			if (*func == CreateTorpedo)
			{
				scale = Vector3(1.5, 0.45, 1);

				// Add max ammo's number of indicators
				for (int i = 0; i < playerComponent.maxAmmo; i++)
				{
					// Place indicators equidistant along the range
					offset.y = generateEquidistantPoint(rangeStart, rangeEnd, playerComponent.maxAmmo, i);

					playerComponent.shootIndicators.push_back(CreateIndicator(playerEntity, offset, scale, { "UI_Green_Torpedo_Icon.png", "UI_Red_Torpedo_Icon.png" }));
				}
			}
			else if (*func == ShootHedgehog)
			{
				offset = Vector3(1.5, -3.7, 10);
				scale = Vector3(1 * 1.45, 1 * 1.45, 1);

				playerComponent.shootIndicators.push_back(CreateIndicator(playerEntity, offset, scale, { "UI_Green_Hedgehog_Icon.png", "UI_Red_Hedgehog_Icon.png" }));
			}
			else if (*func == CreateShell)
			{
				offset = Vector3(1.5, -3.7, 10);
				scale = Vector3(1 * 1.34, 1 * 1.34, 1);

				playerComponent.shootIndicators.push_back(CreateIndicator(playerEntity, offset, scale, { "UI_Red_Cannon_Icon.png" }));
				playerComponent.shootIndicators.push_back(CreateIndicator(playerEntity, offset, scale, { "UI_Green_Cannon_Icon.png" }));
				TransformSystem::Scale(playerComponent.shootIndicators.back().entity, 0, 0.01, 0);
				TransformSystem::Rotate(playerComponent.shootIndicators.back().entity, -10, 0, 0);
			}

			// Create special indicators
			rangeEnd = 2;
			rangeStart = -5;
			offset = Vector3(-2, -3.8, 10);
			scale = Vector3(1.25, 1.25, 1);
			func = *playerComponent.specialAction.target<void(*)(engine::ecs::Entity)>();

			if (*func == Boost)
			{
				playerComponent.specialIndicators.push_back(CreateIndicator(playerEntity, offset, scale, { "UI_Green_Booster_Icon.png", "UI_Booster_Icon.png", "UI_Red_Booster_Icon.png" }));
			}
			else if (*func == ToggleSubmerge)
			{
				ecs::AddComponent(playerEntity, SubmarineComponent{});
				playerComponent.specialIndicators.push_back(CreateIndicator(playerEntity, offset, scale, { "UI_Green_Surface_Icon.png", "UI_Red_Surface_Icon.png", "UI_Green_Submerge_Icon.png", "UI_Red_Submerge_Icon.png" }));
			}
			else if (*func == ShootCruiseMissile)
			{
				playerComponent.specialIndicators.push_back(CreateIndicator(playerEntity, offset, scale, { "UI_Green_Cruise_Icon.png", "UI_Invalid_Cruise_Icon.png", "UI_Red_Cruise_Icon.png" }));
			}

			playerComponent.shootIndicatorUpdate(playerEntity);
			playerComponent.specialIndicatorUpdate(playerEntity);
			
			Audio* engineAudio = engine::AddAudio("Gameplay", "audio/enginemono.wav", true, 0.1f, DistanceModel::LINEAR);
			engineAudio->play();
			Audio* shootShell = engine::AddAudio("Gameplay", "audio/bang_05.wav", false, 0.3f, DistanceModel::LINEAR);
			Audio* shootTorpedo = engine::AddAudio("Gameplay", "audio/torpedoshoot.wav", false, 0.3f, DistanceModel::LINEAR);
			Audio* shootTorpedo2 = engine::AddAudio("Gameplay", "audio/torpedoshoot.wav", false, 0.3f, DistanceModel::LINEAR);
			Audio* boostAudio = engine::AddAudio("Gameplay", "audio/boost_01.wav", false, 0.3f, DistanceModel::LINEAR);
			Audio* submergeAudio = engine::AddAudio("Gameplay", "audio/submerge_01.wav", false, 0.3f, DistanceModel::LINEAR);
			float startDelay = Random(0.0f, 100.0f); // Start delay between 0-100ms
			engineAudio->setStartTimeMilliseconds(startDelay);

			//Add sounds
			engine::ecs::AddComponent(playerEntity, engine::SoundComponent{ .Sounds =
			{
				{"Engine", engineAudio},
				{"ShootShell", shootShell},
				{"ShootTorpedo", shootTorpedo},
				{"ShootTorpedo2", shootTorpedo2},
				{"boostAudio", boostAudio},
				{"submergeAudio", submergeAudio}
			} , .maxDistance = 1500, .deleteSounds = true });
		}
	}
};


ECS_REGISTER_SYSTEM(SubmarineSystem, SubmarineComponent, Transform, Player, PolygonCollider)
class SubmarineSystem : public ecs::System
{
public:
	void Update()
	{
		for (ecs::Entity entity : entities)
		{
			SubmarineComponent& sc = ecs::GetComponent<SubmarineComponent>(entity);

			//Force surface after some time
			if (sc.timeSubmerged >= sc.maxSubmergeTime)
			{
				ToggleSubmerge(entity);
			}
			else if (sc.submerged)
			{
				sc.timeSubmerged += deltaTime;
			}
		}
	}
};

void ShootCruiseMissile(ecs::Entity owner)
{
	// Get the leading player
	ecs::Entity target = ecs::GetSystem<PlayerController>()->GetLeadingPlayer();

	// Verify that the target is valid and not the same as the trigger
	if (target == 0 || target == owner)
	{
		//Decrease the cooldown time for failed shot
		Player& pc = ecs::GetComponent<Player>(owner);
		pc._specialTimer += pc.specialCooldown / 1.4;
		return;
	}

	// Deactivate special ability and shoot
	Player& pc = ecs::GetComponent<Player>(owner);
	pc.specialEnabled = false;
	CruiseMissileSystem::CreateMissile(owner, target, ecs::GetComponent<Transform>(pc.renderedEntity).rotation.y);
}

//Static member definitions
engine::ecs::Entity PlayerController::winScreen = winScreen;
engine::ecs::Entity PlayerController::winTimer = winTimer;
float PlayerController::hasWon = hasWon;
int PlayerController::lapCount = lapCount;
