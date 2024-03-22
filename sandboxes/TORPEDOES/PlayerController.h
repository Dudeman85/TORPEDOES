#pragma once 
#include <bitset>

#include <engine/Application.h>
#include "Resources.h"
#include "Projectiles.h"
#include "engine/Input.h"

using namespace engine;

enum ShipType { torpedoBoat, submarine, cannonBoat, hedgehogBoat, pirateShip };

ECS_REGISTER_COMPONENT(Player)
struct Player
{
	int id = 0;

	// Movement stats
	float forwardSpeed = 400;
	float reverseSpeed = 133;

	float offtrackSpeedScale = 0.991f;

	float rotationSpeed = 100;
	float minSpeedWhileTurning = 60;

	float _speedScale = 1;
	Vector2 _forwardDirection;

	// Action cooldowns
	float shootCooldown = 0.2f;			// Time between shots
	float specialCooldown = 0.2f;		// Time between special uses
	float ammoRechargeCooldown = 1.f;	// Time between gaining ammo

	int ammo = 0;
	int maxAmmo = 2;

	// Action timers
	float _ammoRechargeTimer = 0.0f;
	float _shootTimer = 0.0f;
	float _specialTimer = 0.0f;

	// Action functions
	std::function<void(ecs::Entity)> mainAction;
	std::function<void(ecs::Entity)> specialAction;

	// Checkpoint
	int previousCheckpoint = -1;
	int lap = 1;

	// Hit
	std::vector<std::pair<Projectile, float>> hitProjectiles;

	// Rendered child entities
	ecs::Entity renderedEntity;
	ecs::Entity nameText;
};

ECS_REGISTER_COMPONENT(CheckPoint)
struct CheckPoint
{
	int checkPointID = 0;
	bool Finish_line = false;
};

void CreateTorpedo(ecs::Entity entity)
{
	Player& player = ecs::GetComponent<Player>(entity);
	Transform& transform = ecs::GetComponent<Transform>(entity);
	Transform& modelTransform = ecs::GetComponent<Transform>(player.renderedEntity);
	
	float speed = 500;

	ecs::Entity torpedo = ecs::NewEntity();
	ecs::AddComponent(torpedo, Projectile{ .ownerID = player.id, .speed = 500 });

	Projectile& torpedoProjectile = ecs::GetComponent<Projectile>(torpedo);

	ecs::AddComponent(torpedo, Transform{ .position = transform.position, .rotation = modelTransform.rotation, .scale = Vector3(10) });
	ecs::AddComponent(torpedo, Rigidbody{ .velocity = player._forwardDirection * torpedoProjectile.speed });
	ecs::AddComponent(torpedo, ModelRenderer{ .model = resources::models[torpedoProjectile.model] });
	std::vector<Vector2> Torpedoverts{ Vector2(2, 0.5), Vector2(2, -0.5), Vector2(-2, -0.5), Vector2(-2, 0.5) };
	ecs::AddComponent(torpedo, PolygonCollider{ .vertices = Torpedoverts, .callback = OnProjectileCollision, .trigger = true, .visualise = false,  .rotationOverride = transform.position.y });
}

void CreateHedgehog(ecs::Entity entity)
{
	Player& player = ecs::GetComponent<Player>(entity);
	Transform& transform = ecs::GetComponent<Transform>(entity);
	Transform& modelTransform = ecs::GetComponent<Transform>(player.renderedEntity);

	float hedgehogSpeedVo = 500.0f;
	float distanceTraveled = 0.0f;


	ecs::Entity hedgehog = ecs::NewEntity();
	ecs::AddComponent(hedgehog, Transform{ .position = transform.position, .rotation = modelTransform.rotation });

	Vector3 finalVelocity = Vector3(player._forwardDirection.x, player._forwardDirection.y, 0.0f) * hedgehogSpeedVo;


	ecs::AddComponent(hedgehog, Rigidbody{ .velocity = finalVelocity });

	ecs::AddComponent(hedgehog, ModelRenderer{ .model = resources::models["hedgehog.obj"] });
	std::vector<Vector2> Hedgehogverts{ Vector2(0.2, 0.25), Vector2(0.2, -0.25), Vector2(-0.2, -0.25), Vector2(-0.2, 0.25) };
	ecs::AddComponent(hedgehog, PolygonCollider{ .vertices = Hedgehogverts, .callback = OnProjectileCollision, .trigger = true, .visualise = true,  .rotationOverride = transform.position.y });
	ecs::AddComponent(hedgehog, Projectile{ .ownerID = player.id });
	ecs::AddComponent(hedgehog, Hedgehog{});



}

// Player controller System. Requires Player , Tranform , Rigidbody , PolygonCollider
ECS_REGISTER_SYSTEM(PlayerController, Player, Transform, Rigidbody, PolygonCollider)
class PlayerController : public ecs::System
{
	static ecs::Entity winScreen;
	static bool hasWon;
	static int lapCount; // How many laps to race through

	//A map from a ship type to a pre-initialized Player component with the proper stats
	std::unordered_map<ShipType, Player> shipComponents;
	//A map from a ship type to its 3D model
	std::unordered_map<ShipType, Model*> shipModels;

public:
	float countdownTimer = 0;

	void Init()
	{
		//Create the entity to be shown at a win
		winScreen = ecs::NewEntity();
		ecs::AddComponent(winScreen, TextRenderer{ .font = resources::niagaraFont, .text = "", .offset = Vector3(-1.5, 2, 1), .scale = Vector3(0.03f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
		ecs::AddComponent(winScreen, SpriteRenderer{ .texture = resources::uiTextures["winner.png"], .enabled = false, .uiElement = true });
		ecs::AddComponent(winScreen, Transform{ .position = Vector3(0, 0, 0.5f), .scale = Vector3(0.3f) });

		//Initialize each ship type's stats
		shipComponents.insert({ ShipType::torpedoBoat,
			Player{.forwardSpeed = 400, .rotationSpeed = 75, .shootCooldown = 0.2, .specialCooldown = 10, .mainAction = CreateTorpedo, .specialAction = CreateTorpedo } });
		shipComponents.insert({ ShipType::submarine,
			Player{.forwardSpeed = 800, .rotationSpeed = 75, .shootCooldown = 0.2, .specialCooldown = 10, .mainAction = CreateTorpedo, .specialAction = CreateTorpedo } });
		shipComponents.insert({ ShipType::cannonBoat,
			Player{.forwardSpeed = 400, .rotationSpeed = 75, .shootCooldown = 0.2, .specialCooldown = 10, .mainAction = CreateTorpedo, .specialAction = CreateTorpedo } });
		shipComponents.insert({ ShipType::hedgehogBoat,
			Player{.forwardSpeed = 400, .rotationSpeed = 75, .shootCooldown = 0.2, .specialCooldown = 10, .mainAction = CreateHedgehog, .specialAction = CreateTorpedo } });

		//Initialize ship type models
		shipModels.insert({ ShipType::torpedoBoat, resources::models["LaMuerte.obj"] });
		shipModels.insert({ ShipType::submarine, resources::models["LaMuerte.obj"] });
		shipModels.insert({ ShipType::cannonBoat, resources::models["LaMuerte.obj"] });
		shipModels.insert({ ShipType::hedgehogBoat, resources::models["LaMuerte.obj"] });
	}

	//Get the min and max bounds of every player
	std::array<float, 4> GetPlayerBounds()
	{
		std::array<float, 4> playerBounds{ -INFINITY, -INFINITY, INFINITY, INFINITY };

		for (auto itr = entities.begin(); itr != entities.end();)
		{
			//Get the entity and increment the iterator
			ecs::Entity entity = *itr++;

			Transform& transform = ecs::GetComponent<Transform>(entity);

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

	static void OnCollision(Collision collision)
	{
		// collision.a is always a player
		// Get references to the involved components
		Player& player = ecs::GetComponent<Player>(collision.a);

		// Slow player down when off track
		if (collision.type == Collision::Type::tilemapCollision)
		{
			ecs::GetComponent<Rigidbody>(collision.a).velocity *= player.offtrackSpeedScale;
		}

		// Check if the collision involves a checkpoint
		if (ecs::HasComponent<CheckPoint>(collision.b))
		{
			CheckPoint& checkpoint = ecs::GetComponent<CheckPoint>(collision.b);	// Get checkpoint component
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
							ecs::GetComponent<TextRenderer>(winScreen).text = "Player " + std::to_string(player.id + 1);
							ecs::GetComponent<SpriteRenderer>(winScreen).enabled = true;
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
		else if (ecs::HasComponent<Projectile>(collision.b))
		{
			Projectile& projectile = ecs::GetComponent<Projectile>(collision.b); // projectile is collision.b 

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

				//Destroy torpedo at end of frame
				//TODO: actually fix entity deletion bug
				std::function<void()> destroyTorpedo = [collision]() { ecs::DestroyEntity(collision.b); };
				TimerSystem::ScheduleFunction(destroyTorpedo, -1);
			}
		}
	}

	/// PlayerController Update 
	void Update(GLFWwindow* window, float dt)
	{
		//Don't do anything until countdown is done
		if (countdownTimer > 0)
		{
			countdownTimer -= dt;
			return;
		}

		// Iterate through entities in the system
		for (auto itr = entities.begin(); itr != entities.end();)
		{
			// Get the entity and increment the iterator
			ecs::Entity entity = *itr++;

			// Get necessary components
			Player& player = ecs::GetComponent<Player>(entity);
			Transform& transform = ecs::GetComponent<Transform>(entity);
			Transform& modelTransform = ecs::GetComponent<Transform>(player.renderedEntity);
			Rigidbody& rigidbody = ecs::GetComponent<Rigidbody>(entity);
			PolygonCollider& collider = ecs::GetComponent<PolygonCollider>(entity);

			// Initialize inputs
			float accelerationInput = input::GetTotalInputValue("Throttle" + std::to_string(player.id));
			
			float rotateInput = input::GetInputValue("Turn" + std::to_string(player.id), GLFW_GAMEPAD_AXIS_LEFT_X);
			bool projetileInput = input::GetPressed("Shoot" + std::to_string(player.id));

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
				}
				else
				{
					++it; // Move to the next element
				}
			}

			player._speedScale = 1; // 100%
			for (auto& hitProjectile : player.hitProjectiles)
			{
				switch (hitProjectile.first.hitType)
				{
					case HitStates::Stop:
						// Rotate player
						TransformSystem::Rotate(player.renderedEntity, 0, 360.0f * dt, 0);
					break;
					case HitStates::Additive:
						player._speedScale += max(hitProjectile.first.hitSpeedFactor, 0.f);
						break;
					case HitStates::Multiplicative:
						player._speedScale += max(player._speedScale *= hitProjectile.first.hitSpeedFactor, 0.f);
						break;
					default:
						break;
				}
			}

			/* Movement */

			// CALCULATE POSITION AND ROTATE 
			// Calculate the forward direction
			Vector2 forwardDirection = Vector2(cos(glm::radians(modelTransform.rotation.y)), sin(glm::radians(modelTransform.rotation.y)));
			forwardDirection.Normalize();

			// Initialize the impulse as zero
			Vector2 forwardImpulse(0.0f, 0.0f);

			// Apply acceleration impulse if positive input is received
			if (accelerationInput > 0.0f)
			{
				forwardImpulse = forwardDirection * accelerationInput * dt * player.forwardSpeed;
			}
			// Apply deceleration impulse if negative input is received
			if (accelerationInput < 0.0f)
			{
				forwardImpulse = forwardDirection * accelerationInput * dt * player.reverseSpeed;
			}
			if (rotateInput != 0.0f)
			{
				// Slow rotation based on throttle setting
				// TODO: this function could be improved by testing
				float rotationScalar = 1 - log10(2.0f * max(0.5f, accelerationInput));

				float trueRotateInput = -rotateInput * player.rotationSpeed * rotationScalar * dt;

				// Apply forward impulse if rotating or receiving a rotation command
				TransformSystem::Rotate(player.renderedEntity, 0, trueRotateInput, 0);

				// Set min speed while turning

				// TODO: Scale based on rotation speed
				Vector2 minRotateImpulse = forwardDirection * dt * player._speedScale * std::abs(trueRotateInput) * player.minSpeedWhileTurning;

				if (minRotateImpulse.Length() > forwardImpulse.Length())
				{
					forwardImpulse = minRotateImpulse;
				}
			}

			collider.rotationOverride = modelTransform.rotation.y + 1080;

			// Apply the resulting impulse to the object
			PhysicsSystem::Impulse(entity, forwardImpulse * player._speedScale);

			/* Shooting */

			bool reachedMaxAmmoThisFrame = false;
			bool shotThisFrame = false;

			// If not max ammo
			if (player.ammo < player.maxAmmo)
			{
				player._ammoRechargeTimer += dt;

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
			player._shootTimer += dt;

			// If the projectile cooldown has passed
			while (player._shootTimer >= player.shootCooldown)
			{
				if (!projetileInput)
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
				player._forwardDirection = forwardDirection;
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
		}
	}

	//Spawn 1-4 players, all in a line from top to bottom
	void CreatePlayers(int count, Vector2 startPos, std::vector<ShipType> shipTypes)
	{
		Vector2 offset(0, 60);
		for (int i = 0; i < count; i++)
		{
			//Make all the necessary entities
			ecs::Entity player = ecs::NewEntity();
			ecs::Entity playerNameText = ecs::NewEntity();
			ecs::Entity playerRender = ecs::NewEntity();
			ecs::Entity torpIndicator1 = ecs::NewEntity();
			ecs::Entity torpIndicator2 = ecs::NewEntity();


			//Create the player entity which contains everything but rendering
			//Player component is a bit special
			ecs::AddComponent(player, shipComponents[shipTypes[i]]);
			Player& playerComponent = ecs::GetComponent<Player>(player);
			playerComponent.id = i;
			playerComponent.renderedEntity = playerRender;
			playerComponent.nameText = playerNameText;

			ecs::AddComponent(player, Transform{ .position = Vector3(startPos - offset * i, 100), .rotation = Vector3(0, 0, 0), .scale = Vector3(7) });
			ecs::AddComponent(player, Rigidbody{ .drag = 0.025, .restitution = 1 });
			vector<Vector2> colliderVerts{ Vector2(2, 2), Vector2(2, -1), Vector2(-5, -1), Vector2(-5, 2) };
			ecs::AddComponent(player, PolygonCollider{ .vertices = colliderVerts, .callback = PlayerController::OnCollision, .visualise = false });

			//Create the player's name tag
			ecs::AddComponent(playerNameText, TextRenderer{ .font = resources::niagaraFont, .text = "P" + to_string(i + 1), .color = Vector3(0.5, 0.8, 0.2) });
			ecs::AddComponent(playerNameText, Transform{ .position = Vector3(-2, 2, 1) , .scale = Vector3(0.1) });
			TransformSystem::AddParent(playerNameText, player);

			//Create the player's rendered entity
			ecs::AddComponent(playerRender, Transform{ .rotation = Vector3(45, 0, 0) });
			ecs::AddComponent(playerRender, ModelRenderer{ .model = shipModels[shipTypes[i]] });
			TransformSystem::AddParent(playerRender, player);

			//Create the players's torpedo indicators
			ecs::AddComponent(torpIndicator1, SpriteRenderer{ .texture = resources::uiTextures["UI_Green_Torpedo_Icon.png"] });
			ecs::AddComponent(torpIndicator1, Transform{ .position = Vector3(-2, -2, 10), .scale = Vector3(2, .5, 1) });
			TransformSystem::AddParent(torpIndicator1, player);
			ecs::AddComponent(torpIndicator2, SpriteRenderer{ .texture = resources::uiTextures["UI_Green_Torpedo_Icon.png"] });
			ecs::AddComponent(torpIndicator2, Transform{ .position = Vector3(2, -2, 10), .scale = Vector3(2, .5, 1) });
			TransformSystem::AddParent(torpIndicator2, player);
		}
	}
};
//Static member definitions
ecs::Entity PlayerController::winScreen = winScreen;
bool PlayerController::hasWon = false;
int PlayerController::lapCount = 1;