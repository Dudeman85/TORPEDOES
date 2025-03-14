#pragma once 
#include <bitset>

#include <engine/Application.h>
#include "Resources.h"
#include "Projectiles.h"
#include "engine/Input.h"
#include "engine/Timing.h"

#include "engine/SoundComponent.h"

using namespace engine;

enum ShipType { torpedoBoat, submarine, cannonBoat, hedgehogBoat, pirateShip };

struct indicatorStruct
{
	engine::ecs::Entity entity;
	engine::Texture* texture1;
	engine::Texture* texture2;

	indicatorStruct(engine::ecs::Entity e, engine::Texture* t1, engine::Texture* t2) : entity(e), texture1(t1), texture2(t2)
	{

	}
};

ECS_REGISTER_COMPONENT(Player)
struct Player
{
	int id = 0;

	// Movement stats
	float forwardSpeed = 400;
	float reverseSpeed = 133;

	float rotationSpeed = 120;
	float minSpeedWhileTurning = 60;

	float offtrackSpeedScale = 0.6;
	Vector2 forwardDirection;

	float _speedScale = 1;
	float _boostScale = 1;
	bool _offroadThisFrame = false;

	bool submerged = false;
	bool reloading = false;
	int secondaryAmmo = 0;

	// Action cooldowns
	float shootCooldown = 0.2f;			// Time between shots
	float specialCooldown = 0.8f;		// Time between special uses
	float ammoRechargeCooldown = 1.f;	// Time between gaining ammo

	bool holdShoot = false;				// Whether we can hold to shoot
	int ammo = 0;
	int maxAmmo = 2;

	// Action timers
	float _ammoRechargeTimer = 0.0f;
	float _shootTimer = 0.0f;
	float _specialTimer = 0.0f;

	// Action functions
	std::function<void(engine::ecs::Entity)> mainAction;
	std::function<void(engine::ecs::Entity)> specialAction;

	// Checkpoint
	int previousCheckpoint = -1;
	int lap = 1;

	// Hit
	std::vector<std::pair<Projectile, float>> hitProjectiles;

	// Rendered child entities
	std::vector<indicatorStruct> shootIndicators;
	std::vector<indicatorStruct> specialIndicators;

	engine::ecs::Entity renderedEntity;
	engine::ecs::Entity nameText;
	engine::ecs::Entity animationEntity;
};

ECS_REGISTER_COMPONENT(CheckPoint)
struct CheckPoint
{
	int checkPointID = 0;
	bool Finish_line = false;
};

/* TORPEDO */

void CreateTorpedo(engine::ecs::Entity entity)
{
	Player& player = ecs::GetComponent<Player>(entity);
	Transform& transform = ecs::GetComponent<Transform>(entity);
	Transform& modelTransform = ecs::GetComponent<Transform>(player.renderedEntity);

	std::cout << modelTransform.rotation.ToString() << "\n";

	float speed = 500;

	ecs::Entity torpedo = ecs::NewEntity();
	ecs::AddComponent(torpedo, Projectile{ .ownerID = player.id, .speed = 500 });

	Projectile& torpedoProjectile = ecs::GetComponent<Projectile>(torpedo);

	ecs::AddComponent(torpedo, Transform{ .position = transform.position, .rotation = modelTransform.rotation, .scale = Vector3(10) });
	ecs::AddComponent(torpedo, Rigidbody{ .velocity = player.forwardDirection * torpedoProjectile.speed });
	std::vector<Vector2> Torpedoverts{ Vector2(2, 0.5), Vector2(2, -0.5), Vector2(-2, -0.5), Vector2(-2, 0.5) };
	ecs::AddComponent(torpedo, PolygonCollider{ .vertices = Torpedoverts, .callback = OnProjectileCollision, .trigger = true, .visualise = true,  .rotationOverride = std::abs(modelTransform.rotation.y) });

	ecs::AddComponent(torpedo, ModelRenderer{ .model = resources::models[torpedoProjectile.model] });
}

/* SHELL */

void CreateShell(engine::ecs::Entity entity)
{
	Player& player = ecs::GetComponent<Player>(entity);
	Transform& transform = ecs::GetComponent<Transform>(entity);
	Transform& modelTransform = ecs::GetComponent<Transform>(player.renderedEntity);

	float speed = 400;

	ecs::Entity shell = ecs::NewEntity();
	ecs::AddComponent(shell, Projectile{ .ownerID = player.id, .speed = 500, .hitType = HitStates::Additive, .hitSpeedFactor = -0.15f, .hitTime = 2.f });

	Projectile& shellProjectile = ecs::GetComponent<Projectile>(shell);

	ecs::AddComponent(shell, Transform{ .position = transform.position, .rotation = modelTransform.rotation, .scale = Vector3(40) });
	ecs::AddComponent(shell, Rigidbody{ .velocity = player.forwardDirection * shellProjectile.speed });
	ecs::AddComponent(shell, ModelRenderer{ .model = resources::models[shellProjectile.model = "Weapon_HedgehogAmmo.obj"] });
	float shellSize = 0.1;

	std::vector<Vector2> shellverts{ Vector2(shellSize, shellSize), Vector2(shellSize, -shellSize), Vector2(-shellSize, -shellSize), Vector2(-shellSize, shellSize) };
	ecs::AddComponent(shell, PolygonCollider{ .vertices = shellverts, .callback = OnProjectileCollision, .trigger = true, .visualise = true,  .rotationOverride = std::abs(modelTransform.rotation.y) });
}

void ShootShell(engine::ecs::Entity entity)
{
	Player& player = ecs::GetComponent<Player>(entity);

	if (player.reloading)
	{
		// Reloading
		if (player.ammo < player.maxAmmo)
		{
			// Not reloaded
			player.ammo++; // We didn't shoot
			return;
		}
		// Fully reloaded
		player.reloading = false;
		player.secondaryAmmo = player.maxAmmo;
	}

	if (player.secondaryAmmo <= 1)
	{
		// Last ammo, start reload
		player.reloading = true;
	}

	player.secondaryAmmo--;
	player.ammo = 0;

	CreateShell(entity);
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
	engine::ecs::AddComponent(hedgehog, Transform{ .position = transform.position, .rotation = modelTransform.rotation });

	Vector3 finalVelocity = Vector3(direction.x, direction.y, 0.0f) * ecs::GetSystem<HedgehogSystem>()->hedgehogSpeedVo;

	engine::ecs::AddComponent(hedgehog, engine::Rigidbody{.velocity = finalVelocity });

	engine::ecs::AddComponent(hedgehog, ModelRenderer{ .model = resources::models["Weapon_HedgehogAmmo.obj"] });
	std::vector<Vector2> Hedgehogverts{ Vector2(0.4, 0.5), Vector2(0.4, -0.5), Vector2(-0.4, -0.5), Vector2(-0.4, 0.5) };
	ecs::AddComponent(hedgehog, Projectile{ .ownerID = player.id });
	ecs::AddComponent(hedgehog, Hedgehog{ .targetDistance = std::clamp(input::map_value(timeHeld, 0, _HedgehogChargeTime, _HedgehogMinDistance, _HedgehogMaxDistance), _HedgehogMinDistance, _HedgehogMaxDistance), .aimingGuide = aimingGuide });
}

struct aimingGuideStruct
{
	std::vector<engine::ecs::Entity> entities;
	float* totalTime;
	engine::ScheduledFunction* timerFunction;
};

//const float HedgehogAngleOffset = Radians(5.0f); // Ajuste de �ngulo para las direcciones de los proyectiles
static std::map<int, aimingGuideStruct> playerIdToAimGuides;

void AimHedgehog(engine::ecs::Entity entity, std::vector<engine::ecs::Entity> aimingGuides, float guideSpeed, float shootAngle, int shootAmount)
{
	Player& player = ecs::GetComponent<Player>(entity);

	*playerIdToAimGuides[player.id].totalTime += engine::deltaTime;

	if (input::GetPressed("Shoot" + std::to_string(player.id)))
	{
		// When button is pressed, move aim guide forward
		Transform& transform = ecs::GetComponent<Transform>(entity);
		Transform& modelTransform = ecs::GetComponent<Transform>(player.renderedEntity);


		Player& player = ecs::GetComponent<Player>(entity);
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
				+ (modifiedDirection * _HedgehogMinDistance)
				+ ((modifiedDirection * guideSpeed * std::min(*playerIdToAimGuides[player.id].totalTime, _HedgehogChargeTime)));
			guideTransform.scale = 100;
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
				+ (modifiedDirection * _HedgehogMinDistance)
				+ ((modifiedDirection * guideSpeed * std::min(*playerIdToAimGuides[player.id].totalTime, _HedgehogChargeTime)));
			guideTransform.scale = 100;
		}
		// Guide negative:
		for (int negative_i = 1; negative_i < (shootAmount / 2) + 1; ++negative_i)
		{
			float angle = playerAngle + evenOffset + (negative_i * -shootAngle);

			modifiedDirection = Vector2(cos(angle), sin(angle));

			Transform& guideTransform = ecs::GetComponent<Transform>(aimingGuides[(negative_i)+(positive_i - 1)]);
			guideTransform.position = playerTransform.position
				+ (modifiedDirection * _HedgehogMinDistance)
				+ ((modifiedDirection * guideSpeed * std::min(*playerIdToAimGuides[player.id].totalTime, _HedgehogChargeTime)));
			guideTransform.scale = 100;
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
			// TODO: Change guide animation
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

		engine::ecs::AddComponent(newGuide, engine::Transform{.position = transform.position, .rotation = transform.rotation, .scale = transform.scale });
		engine::ecs::AddComponent(newGuide, engine::ModelRenderer{.model = resources::models["Weapon_HedgehogAmmo.obj"] });

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

	float guideSpeed = 500;
	float shootAngle = Radians(5.0f);
	//float shootAmount = player.ammo;
	float shootAmount = 4;
	//player.ammo++; // We don't use up ammo until we shoot

	CreateAimingGuides(entity, 500, shootAngle, shootAmount);
}

/* BOOST */

static void BoostEnd(engine::ecs::Entity entity, float boostStrenght)
{
	Player& player = engine::ecs::GetComponent<Player>(entity);
	player._boostScale -= boostStrenght;
}

// Increases player speed for a short while
void Boost(engine::ecs::Entity entity)
{
	double boostTime = 1;
	float boostStrenght = 0.4f;

	Player& player = engine::ecs::GetComponent<Player>(entity);

	player._boostScale += boostStrenght;

	engine::timerSystem->ScheduleFunction(&BoostEnd, boostTime, false, engine::ScheduledFunction::Type::seconds, entity, boostStrenght);
}

//Submerge function for submarine, slightly slower and invincible for the duration
void ToggleSubmerge(engine::ecs::Entity playerEntity)
{
	Player& playerComponent = ecs::GetComponent<Player>(playerEntity);
	Transform& transformComponent = ecs::GetComponent<Transform>(playerEntity);
	Transform& modelTransform = ecs::GetComponent<Transform>(playerComponent.renderedEntity);

	//Make the diving bubbles animation
	ecs::Entity divingEntity = ecs::NewEntity();
	ecs::AddComponent(divingEntity, Transform{ .position = {0, 0, 10}, .scale = {5, 2, 1} });
	ecs::AddComponent(divingEntity, SpriteRenderer{});
	ecs::AddComponent(divingEntity, Animator{ .onAnimationEnd = ecs::DestroyEntity });
	TransformSystem::SetRotation(divingEntity, { 0, 0, modelTransform.rotation.y });
	AnimationSystem::AddAnimation(divingEntity, resources::divingAnim, "dive");
	AnimationSystem::PlayAnimation(divingEntity, "dive");
	TransformSystem::AddParent(divingEntity, playerEntity);

	//Submerge if surfaced
	if (!playerComponent.submerged)
	{
		//Make the continuous diving animation
		playerComponent.animationEntity = ecs::NewEntity();
		ecs::AddComponent(playerComponent.animationEntity, Transform{ .position = {0, 0, 5}, .scale = {5, 2, 1} });
		ecs::AddComponent(playerComponent.animationEntity, SpriteRenderer{});
		ecs::AddComponent(playerComponent.animationEntity, Animator{ });
		TransformSystem::SetRotation(playerComponent.animationEntity, { 0, 0, modelTransform.rotation.y });
		AnimationSystem::AddAnimation(playerComponent.animationEntity, resources::continuousDivingAnim, "diving");
		AnimationSystem::PlayAnimation(playerComponent.animationEntity, "diving", true);
		TransformSystem::AddParent(playerComponent.animationEntity, playerEntity);

		//Start submerging and slow down
		playerComponent.forwardSpeed /= 1.5f;

		//Cannot surface for 5 seconds

		//Finished submerging after 1 second
		TimerSystem::ScheduleFunction(
			[playerEntity]()
			{
				Player& playerComponent = ecs::GetComponent<Player>(playerEntity);
				playerComponent.submerged = true;
				ecs::GetComponent<ModelRenderer>(playerComponent.renderedEntity).textures.push_back(resources::modelTextures["Player_Black.png"]);
			}, 0.3);
	}
	//Surface if submerged
	else
	{
		ecs::DestroyEntity(playerComponent.animationEntity);
		playerComponent.animationEntity = 0;

		//Start surfacing and speed up
		playerComponent.forwardSpeed *= 1.5f;

		//Finished surfacing after 1 second
		TimerSystem::ScheduleFunction(
			[playerEntity]()
			{
				Player& playerComponent = ecs::GetComponent<Player>(playerEntity);
				playerComponent.submerged = false;
				ecs::GetComponent<ModelRenderer>(playerComponent.renderedEntity).textures.clear();
			}, 0.3);
	}
}

// Player controller System. Requires Player , Tranform , Rigidbody , PolygonCollider
ECS_REGISTER_SYSTEM(PlayerController, Player, engine::Transform, engine::Rigidbody, engine::PolygonCollider)
class PlayerController : public engine::ecs::System
{
	static engine::ecs::Entity winScreen;
	static bool hasWon;


	//A map from a ship type to a pre-initialized Player component with the proper stats
	std::unordered_map<ShipType, Player> shipComponents;
	//A map from a ship type to its 3D model
	std::unordered_map<ShipType, engine::Model*> shipModels;

public:
	float countdownTimer = 0;
	static int lapCount; // How many laps to race through
	void Init()
	{
		//Create the entity to be shown at a win
		winScreen = engine::ecs::NewEntity();
		engine::ecs::AddComponent(winScreen, engine::TextRenderer{.font = resources::niagaraFont, .text = "", .offset = Vector3(-1.5, 2, 1), .scale = Vector3(0.03f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
		engine::ecs::AddComponent(winScreen, engine::SpriteRenderer{.texture = resources::uiTextures["winner.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(winScreen, engine::Transform{.position = Vector3(0, 0, 0.5f), .scale = Vector3(0.3f) });

		//Initialize each ship type's stats
		shipComponents.insert(
			{
				ShipType::torpedoBoat, Player
				{
					.forwardSpeed = 400, .rotationSpeed = 75,
					.shootCooldown = 0.2, .specialCooldown = 0.8,
					.holdShoot = false, .maxAmmo = 2,
					.mainAction = CreateTorpedo, .specialAction = Boost
				}
			});
		shipComponents.insert(
			{
				ShipType::submarine, Player
				{
					.forwardSpeed = 400, .rotationSpeed = 75,
					.shootCooldown = 0.2, .specialCooldown = 4,
					.holdShoot = false, .maxAmmo = 2,
					.mainAction = CreateTorpedo, .specialAction = ToggleSubmerge
				}
			});
		shipComponents.insert(
			{
				ShipType::cannonBoat, Player
				{
					.forwardSpeed = 400, .rotationSpeed = 75, .reloading = true,
					.shootCooldown = 0.1, .specialCooldown = 0.8, .ammoRechargeCooldown = 0.16,
					.holdShoot = true, .maxAmmo = 10,
					.mainAction = ShootShell, .specialAction = Boost
				}
			});
		shipComponents.insert(
			{
				ShipType::hedgehogBoat, Player
				{
					.forwardSpeed = 400, .rotationSpeed = 75,
					.shootCooldown = 0.4, .specialCooldown = 0.8, .ammoRechargeCooldown = 0.8,
					.holdShoot = false, .maxAmmo = 8,
					.mainAction = ShootHedgehog, .specialAction = Boost
				}
			});

		//Initialize ship type models
		shipModels.insert({ ShipType::torpedoBoat, resources::models["Ship_PT_109_Torpedo.obj"] });
		shipModels.insert({ ShipType::submarine, resources::models["Ship_U_99_Submarine.obj"] });
		shipModels.insert({ ShipType::cannonBoat, resources::models["Ship_Yamato_Cannon.obj"] });
		shipModels.insert({ ShipType::hedgehogBoat, resources::models["Ship_HMCS_Sackville_Hedgehog.obj"] });
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
			//engine::ecs::GetComponent<engine::Rigidbody>(collision.a).velocity *= player.offtrackSpeedScale;
		}

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
						if (!hasWon)
						{
							//Display the win screen
							hasWon = true;
							engine::ecs::GetComponent<engine::TextRenderer>(winScreen).text = "Player " + std::to_string(player.id + 1);
							engine::ecs::GetComponent<engine::SpriteRenderer>(winScreen).enabled = true;
						}
					}
					else
					{
						player.previousCheckpoint = -1;
						player.lap++;
					}
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

			if (player.id != projectile.ownerID)
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

				CreateAnimation(collision.b);

				//engine::SoundComponent& soundComponent = engine::ecs::GetComponent<engine::SoundComponent>(collision.a);
				//soundComponent.Sounds["Dink"]->play();

				//Destroy projectile at end of frame
				engine::ecs::DestroyEntity(collision.b);
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
					engine::TransformSystem::Rotate(player.renderedEntity, 0, (720.0f / hitProjectile.first.hitTime) * engine::deltaTime, 0);

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
				// Slow rotation based on throttle setting
				// TODO: this function could be improved by testing
				float rotationScalar = 1 - log10(2.0f * std::max(0.5f, accelerationInput));

				float trueRotateInput = -rotateInput * player.rotationSpeed * rotationScalar;

				// Apply forward impulse if rotating or receiving a rotation command
				engine::TransformSystem::Rotate(player.renderedEntity, 0, trueRotateInput * engine::deltaTime, 0);

				// Set min speed while turning

				Vector2 minRotateImpulse = player.forwardDirection * std::abs(trueRotateInput) * player.minSpeedWhileTurning * engine::deltaTime;

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
				if (!newSpecialInput)
				{
					// We haven't pressed the special button, keep _specialTimer at max value
					player._specialTimer = player.specialCooldown;
					break;
				}

				// Do special action
				player.specialAction(entity);

				// Decrease the action time 
				player._specialTimer -= player.specialCooldown;
			}

			float finalBoostScale = player._boostScale;
			if (player._offroadThisFrame)
			{
				if (player._boostScale <= 1)
				{
					// Ignore slower speed due to boost when already offtrack
					finalBoostScale = player.offtrackSpeedScale;
				}
				// Ignore offtrack while boosting
			}

			// Apply the final impulse to the object
			engine::PhysicsSystem::AddForce(entity, (forwardImpulse * player._speedScale * finalBoostScale));

			// Reset offroad status for this frame
			player._offroadThisFrame = false;

			/* Shooting */

			bool reachedMaxAmmoThisFrame = false;
			bool shotThisFrame = false;

			// If not max ammo
			if (player.ammo < player.maxAmmo)
			{
				player._ammoRechargeTimer += engine::deltaTime;

				while (player._ammoRechargeTimer >= player.ammoRechargeCooldown)
				{
					// Add ammo
					player.ammo++;

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
				player.mainAction(entity);

				player.ammo--;

				// Decrease the projectile shoot time 
				player._shootTimer -= player.shootCooldown;
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

			// Shoot indicators

			std::cout << player.shootIndicators.size() << " " << player.maxAmmo << "\n";

			if (player.shootIndicators.size() >= player.maxAmmo)
			{
				for (int i = 0; i < player.maxAmmo; i++)
				{
					indicatorStruct it = player.shootIndicators[i];

					engine::SpriteRenderer& sprite = engine::ecs::GetComponent<engine::SpriteRenderer>(it.entity);
					if (player.ammo > i)
					{
						sprite.texture = it.texture1;
						std::cout << "yes!";
					}
					else
					{
						sprite.texture = it.texture2;
						std::cout << "no!";
					}
				}
				std::cout << "hello!!! \n";
			}

			if (player.animationEntity != 0)
			{
				TransformSystem::SetRotation(player.animationEntity, { 0, 0, modelTransform.rotation.y });
			}
		}
	}

	//Spawn 1-4 players, all in a line from top to bottom
	void CreatePlayers(std::unordered_map<int, ShipType> playerShips, Vector2 startPos)
	{
		Vector2 offset(0, 60);
		for (const auto& playerShip : playerShips)
		{
			//Make all the necessary entities
			engine::ecs::Entity playerEntity = engine::ecs::NewEntity();
			engine::ecs::Entity playerNameText = engine::ecs::NewEntity();
			engine::ecs::Entity playerRender = engine::ecs::NewEntity();

			//Create the player entity which contains everything but rendering
			//Player component is a bit special
			engine::ecs::AddComponent(playerEntity, shipComponents[playerShip.second]);
			Player& playerComponent = engine::ecs::GetComponent<Player>(playerEntity);
			playerComponent.id = playerShip.first;
			playerComponent.renderedEntity = playerRender;
			playerComponent.nameText = playerNameText;

			engine::ecs::AddComponent(playerEntity, engine::Transform{.position = Vector3(startPos - offset * playerShip.first, 150), .rotation = Vector3(0, 0, 0), .scale = Vector3(7) });
			engine::ecs::AddComponent(playerEntity, engine::Rigidbody{.drag = 1.5 });
			vector<Vector2> colliderVerts{ Vector2(3, 1), Vector2(3, -1), Vector2(-3, -1), Vector2(-3, 1) };
			engine::ecs::AddComponent(playerEntity, engine::PolygonCollider{.vertices = colliderVerts, .callback = PlayerController::OnCollision, .visualise = true });

			//Create the player's name tag
			engine::ecs::AddComponent(playerNameText, engine::TextRenderer{.font = resources::niagaraFont, .text = "P" + to_string(playerShip.first + 1), .color = Vector3(0.5, 0.8, 0.2) });
			engine::ecs::AddComponent(playerNameText, engine::Transform{.position = Vector3(-2, 2, 20), .scale = Vector3(0.1) });
			engine::TransformSystem::AddParent(playerNameText, playerEntity);

			//Create the player's rendered entity
			engine::ecs::AddComponent(playerRender, engine::Transform{.rotation = Vector3(45, 0, 0), .scale = Vector3(1.5) });
			engine::ecs::AddComponent(playerRender, engine::ModelRenderer{.model = shipModels[playerShip.second] });
			engine::TransformSystem::AddParent(playerRender, playerEntity);

			Player& player = engine::ecs::GetComponent<Player>(playerEntity);

			//Create the players's shoot indicators
			float totalWidth = 0;
			float indicatorDistance = 0;

			switch (playerShip.second)
			{
			case ShipType::torpedoBoat:
			case ShipType::submarine:
				totalWidth = -2;
				indicatorDistance = 2;

				for (int i = 0; i < player.maxAmmo; i++)
				{
					engine::ecs::Entity shootIndicator = engine::ecs::NewEntity();

					engine::ecs::AddComponent(shootIndicator, engine::SpriteRenderer{  });
					engine::ecs::AddComponent(shootIndicator, engine::Transform{.position = Vector3(totalWidth + (i * indicatorDistance), -2, 10), .scale = Vector3(2, .5, 1) });
					engine::TransformSystem::AddParent(shootIndicator, playerEntity);

					auto a = indicatorStruct(shootIndicator, resources::uiTextures["UI_Green_Torpedo_Icon.png"], resources::uiTextures["UI_Red_Torpedo_Icon.png"]);

					player.shootIndicators.push_back(a);
				}
				break;
			case ShipType::hedgehogBoat:
				break;
			case ShipType::cannonBoat:
			case ShipType::pirateShip:
				break;
			default:
				break;
			}
			//Create the players's special indicators
			switch (playerShip.second)
			{
			case ShipType::torpedoBoat:
			case ShipType::submarine:
				break;
			case ShipType::hedgehogBoat:
				break;
			case ShipType::cannonBoat:
			case ShipType::pirateShip:
				break;
			default:
				break;
			}

			/*
			engine::ecs::AddComponent(torpIndicator2, engine::SpriteRenderer{ .texture = resources::uiTextures["UI_Green_Torpedo_Icon.png"] });
			engine::ecs::AddComponent(torpIndicator2, engine::Transform{ .position = Vector3(2, -2, 10), .scale = Vector3(2, .5, 1) });
			engine::TransformSystem::AddParent(torpIndicator2, player);*/

			// Works
			Audio* audio = engine::AddAudio("Gameplay", "audio/dink.wav", false, 100000);
			audio->pause();
		}
	}
};

//Static member definitions
engine::ecs::Entity PlayerController::winScreen = winScreen;
bool PlayerController::hasWon = false;
int PlayerController::lapCount = lapCount;
