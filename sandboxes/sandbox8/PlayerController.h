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

ECS_REGISTER_COMPONENT(Player)
struct Player
{
	int id = 0;

	// Movement stats
	float forwardSpeed = 400;
	float reverseSpeed = 133;

	float rotationSpeed = 100;
	float minSpeedWhileTurning = 60;

	float offtrackSpeedScale = 0.991f;

	float _speedScale = 1;
	float _boostScale = 1;
	bool _offroadThisFrame = false;
	Vector2 _forwardDirection;

	// Action cooldowns
	float shootCooldown = 0.2f;			// Time between shots
	float specialCooldown = 0.8f;		// Time between special uses
	float ammoRechargeCooldown = 1.f;	// Time between gaining ammo

	int ammo = 0;
	int maxAmmo = 2;

	// Action timers
	float _ammoRechargeTimer = 0.0f;
	float _shootTimer = 0.0f;
	float _specialTimer = 0.0f;

	// Action functions
	std::function<void(engine::ecs::Entity, int)> mainAction;
	std::function<void(engine::ecs::Entity, int)> specialAction;

	// Checkpoint
	int previousCheckpoint = -1;
	int lap = 1;

	// Hit
	std::vector<std::pair<Projectile, float>> hitProjectiles;

	// Rendered child entities
	engine::ecs::Entity renderedEntity;
	engine::ecs::Entity nameText;
};

ECS_REGISTER_COMPONENT(CheckPoint)
struct CheckPoint
{
	int checkPointID = 0;
	bool Finish_line = false;
};

void CreateTorpedo(engine::ecs::Entity entity)
{
	using namespace engine;

	Player& player = ecs::GetComponent<Player>(entity);
	Transform& transform = ecs::GetComponent<Transform>(entity);
	Transform& modelTransform = ecs::GetComponent<Transform>(player.renderedEntity);

	float speed = 500;

	ecs::Entity torpedo = ecs::NewEntity();
	ecs::AddComponent(torpedo, Projectile{ .ownerID = player.id, .speed = 500 });

	Projectile& torpedoProjectile = ecs::GetComponent<Projectile>(torpedo);

	ecs::AddComponent(torpedo, Transform{ .position = transform.position, .rotation = modelTransform.rotation, .scale = Vector3(10) });
	ecs::AddComponent(torpedo, Rigidbody{ .velocity = player._forwardDirection * torpedoProjectile.speed });
	std::vector<Vector2> Torpedoverts{ Vector2(2, 0.5), Vector2(2, -0.5), Vector2(-2, -0.5), Vector2(-2, 0.5) };
	ecs::AddComponent(torpedo, PolygonCollider{ .vertices = Torpedoverts, .callback = OnProjectileCollision, .trigger = true, .visualise = false,  .rotationOverride = transform.position.y });

	ecs::AddComponent(torpedo, ModelRenderer{ .model = resources::models[torpedoProjectile.model] });
}

void CreateHedgehog(engine::ecs::Entity entity)
{
	using namespace engine;

	Player& player = ecs::GetComponent<Player>(entity);
	Transform& transform = ecs::GetComponent<Transform>(entity);
	Transform& modelTransform = ecs::GetComponent<Transform>(player.renderedEntity);

	float speed = 500.0f;

	ecs::Entity hedgehog = ecs::NewEntity();
	ecs::AddComponent(hedgehog, Transform{ .position = transform.position, .rotation = modelTransform.rotation });

	Vector3 finalVelocity = Vector3(player._forwardDirection.x, player._forwardDirection.y, 0.0f) * speed;

	ecs::AddComponent(hedgehog, Rigidbody{ .velocity = finalVelocity });

	ecs::AddComponent(hedgehog, ModelRenderer{ .model = resources::models["hedgehog.obj"] });
	std::vector<Vector2> Hedgehogverts{ Vector2(0.2, 0.25), Vector2(0.2, -0.25), Vector2(-0.2, -0.25), Vector2(-0.2, 0.25) };
	ecs::AddComponent(hedgehog, PolygonCollider{ .vertices = Hedgehogverts, .callback = OnProjectileCollision, .trigger = true, .visualise = true,  .rotationOverride = transform.position.y });
	ecs::AddComponent(hedgehog, Projectile{ .ownerID = player.id });
	ecs::AddComponent(hedgehog, Hedgehog{});
}

void CreateShell(engine::ecs::Entity entity)
{
	using namespace engine;

	Player& player = ecs::GetComponent<Player>(entity);
	Transform& transform = ecs::GetComponent<Transform>(entity);
	Transform& modelTransform = ecs::GetComponent<Transform>(player.renderedEntity);

	float speed = 500;

	ecs::Entity shell = ecs::NewEntity();
	ecs::AddComponent(shell, Projectile{ .ownerID = player.id, .speed = 500, .hitType = HitStates::Additive, .hitSpeedFactor = -0.15f, .hitTime = 2.f });

	Projectile& shellProjectile = ecs::GetComponent<Projectile>(shell);

	ecs::AddComponent(shell, Transform{ .position = transform.position, .rotation = modelTransform.rotation, .scale = Vector3(10) });
	ecs::AddComponent(shell, Rigidbody{ .velocity = player._forwardDirection * shellProjectile.speed });
	ecs::AddComponent(shell, ModelRenderer{ .model = resources::models[shellProjectile.model = "Weapon_HedgehogAmmo.obj"] });
	std::vector<Vector2> Shellverts{ Vector2(2, 0.5), Vector2(2, -0.5), Vector2(-2, -0.5), Vector2(-2, 0.5) };
	ecs::AddComponent(shell, PolygonCollider{ .vertices = Shellverts, .callback = OnProjectileCollision, .trigger = true, .visualise = false,  .rotationOverride = transform.position.y });
}

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

// Player controller System. Requires Player , Tranform , Rigidbody , PolygonCollider
ECS_REGISTER_SYSTEM(PlayerController, Player, engine::Transform, engine::Rigidbody, engine::PolygonCollider)
class PlayerController : public engine::ecs::System
{
	static engine::ecs::Entity winScreen;
	static bool hasWon;
	static int lapCount; // How many laps to race through

	//A map from a ship type to a pre-initialized Player component with the proper stats
	std::unordered_map<ShipType, Player> shipComponents;
	//A map from a ship type to its 3D model
	std::unordered_map<ShipType, engine::Model*> shipModels;

public:
	float countdownTimer = 0;

	void Init()
	{
		//Create the entity to be shown at a win
		winScreen = engine::ecs::NewEntity();
		engine::ecs::AddComponent(winScreen, engine::TextRenderer{ .font = resources::niagaraFont, .text = "", .offset = Vector3(-1.5, 2, 1), .scale = Vector3(0.03f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
		engine::ecs::AddComponent(winScreen, engine::SpriteRenderer{ .texture = resources::uiTextures["winner.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(winScreen, engine::Transform{ .position = Vector3(0, 0, 0.5f), .scale = Vector3(0.3f) });

		//Initialize each ship type's stats
		shipComponents.insert({ ShipType::torpedoBoat,
			Player{.forwardSpeed = 400, .rotationSpeed = 75, .shootCooldown = 0.2, .specialCooldown = 0.8, .mainAction = CreateTorpedo, .specialAction = Boost } });
		shipComponents.insert({ ShipType::submarine,
			Player{.forwardSpeed = 400, .rotationSpeed = 75, .shootCooldown = 0.2, .specialCooldown = 0.8, .mainAction = CreateTorpedo, .specialAction = Boost } });
		shipComponents.insert({ ShipType::cannonBoat,
			Player{.forwardSpeed = 400, .rotationSpeed = 75, .shootCooldown = 0.2, .specialCooldown = 0.8, .mainAction = CreateShell, .specialAction = Boost } });
		shipComponents.insert({ ShipType::hedgehogBoat,
			Player{.forwardSpeed = 400, .rotationSpeed = 75, .shootCooldown = 0.2, .specialCooldown = 0.8, .mainAction = CreateHedgehog, .specialAction = Boost } });

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

				engine::SoundComponent& soundComponent = engine::ecs::GetComponent<engine::SoundComponent>(collision.a);
				soundComponent.Sounds["Dink"]->play();

				//Destroy torpedo at end of frame
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
					engine::TransformSystem::Rotate(player.renderedEntity, 0, 360.0f * engine::deltaTime, 0);
					break;
				case HitStates::Additive:
					player._speedScale += std::max(hitProjectile.first.hitSpeedFactor, 0.f);
					break;
				case HitStates::Multiplicative:
					player._speedScale += std::max(player._speedScale *= hitProjectile.first.hitSpeedFactor, 0.f);
					break;
				default:
					break;
				}
			}

			/* Movement */

			// CALCULATE POSITION AND ROTATION
			// Calculate the forward direction
			Vector2 forwardDirection = Vector2(cos(glm::radians(modelTransform.rotation.y)), sin(glm::radians(modelTransform.rotation.y)));
			forwardDirection.Normalize();

			// Initialize the impulse as zero
			Vector2 forwardImpulse(0.0f, 0.0f);

			// Apply acceleration impulse if positive input is received
			if (accelerationInput > 0.0f)
			{
				forwardImpulse = forwardDirection * accelerationInput * player.forwardSpeed;
			}
			// Apply deceleration impulse if negative input is received
			if (accelerationInput < 0.0f)
			{
				forwardImpulse = forwardDirection * accelerationInput * player.reverseSpeed;
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

				Vector2 minRotateImpulse = forwardDirection * std::abs(trueRotateInput) * player.minSpeedWhileTurning * engine::deltaTime;

				if (minRotateImpulse.Length() > forwardImpulse.Length())
				{
					forwardImpulse = minRotateImpulse;
				}
			}

			collider.rotationOverride = modelTransform.rotation.y + 1080;

			// Increase the special timer
			player._specialTimer += engine::deltaTime;

			// If the special cooldown has passed
			while (player._specialTimer >= player.specialCooldown)
			{
				if (!input::GetPressed("Boost" + std::to_string(player.id)))
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
			engine::PhysicsSystem::Impulse(entity, (forwardImpulse * player._speedScale * finalBoostScale) * engine::deltaTime);

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
				if (!input::GetNewPress("Shoot" + std::to_string(player.id)))
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
			engine::ecs::Entity player = engine::ecs::NewEntity();
			engine::ecs::Entity playerNameText = engine::ecs::NewEntity();
			engine::ecs::Entity playerRender = engine::ecs::NewEntity();
			engine::ecs::Entity torpIndicator1 = engine::ecs::NewEntity();
			engine::ecs::Entity torpIndicator2 = engine::ecs::NewEntity();

			//Create the player entity which contains everything but rendering
			//Player component is a bit special
			engine::ecs::AddComponent(player, shipComponents[shipTypes[i]]);
			Player& playerComponent = engine::ecs::GetComponent<Player>(player);
			playerComponent.id = i;
			playerComponent.renderedEntity = playerRender;
			playerComponent.nameText = playerNameText;

			engine::ecs::AddComponent(player, engine::Transform{ .position = Vector3(startPos - offset * i, 100), .rotation = Vector3(0, 0, 0), .scale = Vector3(7) });
			engine::ecs::AddComponent(player, engine::Rigidbody{ .drag = 0.025 });
			vector<Vector2> colliderVerts{ Vector2(2, 2), Vector2(2, -1), Vector2(-5, -1), Vector2(-5, 2) };
			engine::ecs::AddComponent(player, engine::PolygonCollider{ .vertices = colliderVerts, .callback = PlayerController::OnCollision, .visualise = false });

			//Create the player's name tag
			engine::ecs::AddComponent(playerNameText, engine::TextRenderer{ .font = resources::niagaraFont, .text = "P" + to_string(i + 1), .color = Vector3(0.5, 0.8, 0.2) });
			engine::ecs::AddComponent(playerNameText, engine::Transform{ .position = Vector3(-2, 2, 1) , .scale = Vector3(0.1) });
			engine::TransformSystem::AddParent(playerNameText, player);

			//Create the player's rendered entity
			engine::ecs::AddComponent(playerRender, engine::Transform{ .rotation = Vector3(45, 0, 0), .scale = Vector3(1.5) });
			engine::ecs::AddComponent(playerRender, engine::ModelRenderer{ .model = shipModels[shipTypes[i]] });
			engine::TransformSystem::AddParent(playerRender, player);

			//Create the players's torpedo indicators
			engine::ecs::AddComponent(torpIndicator1, engine::SpriteRenderer{ .texture = resources::uiTextures["UI_Green_Torpedo_Icon.png"] });
			engine::ecs::AddComponent(torpIndicator1, engine::Transform{ .position = Vector3(-2, -2, 10), .scale = Vector3(2, .5, 1) });
			engine::TransformSystem::AddParent(torpIndicator1, player);
			engine::ecs::AddComponent(torpIndicator2, engine::SpriteRenderer{ .texture = resources::uiTextures["UI_Green_Torpedo_Icon.png"] });
			engine::ecs::AddComponent(torpIndicator2, engine::Transform{ .position = Vector3(2, -2, 10), .scale = Vector3(2, .5, 1) });
			engine::TransformSystem::AddParent(torpIndicator2, player);

			// Works
			Audio* audio = engine::AddAudio("Gameplay", "audio/dink.wav", false, 100000);
			audio->pause();

			engine::ecs::AddComponent(player, engine::SoundComponent{ .Sounds =
				{
					{"Dink", audio}
				} }
			);
		}
	}
};
//Static member definitions
engine::ecs::Entity PlayerController::winScreen = winScreen;
bool PlayerController::hasWon = false;
int PlayerController::lapCount = 1;





/// the fist version .
///// PlayerControlle Update 
//void Update(GLFWwindow* window, float dt)
//{
//	// Iterate through entities in the system
//	for (auto itr = entities.begin(); itr != entities.end();)
//	{
//		//Get the entity and increment the iterator
//		ecs::Entity entity = *itr++;
//
//		// Get player, transform, and rigidbody components
//		Player& player = ecs::GetComponent<Player>(entity);
//		Transform& transform = ecs::GetComponent<Transform>(entity);
//		Transform& modelTransform = ecs::GetComponent<Transform>(player.renderedEntity);
//		Rigidbody& rigidbody = ecs::GetComponent<Rigidbody>(entity);
//		PolygonCollider& collider = ecs::GetComponent<PolygonCollider>(entity);
//
//		//initialize input zero 
//		float accelerationInput = 0;
//		float rotateInput = 0;
//		bool ProjetileInput = 0;
//		// Starte Time 
//		if (starTimer <= 0)
//		{
//			accelerationInput = 0;
//			rotateInput = 0;
//			ProjetileInput = 0;
//			// Get keyboard input		 
//			if (player.playerID == 0)
//			{
//				//Player 0 only gets keyboard input
//				accelerationInput += +glfwGetKey(window, GLFW_KEY_A) - glfwGetKey(window, GLFW_KEY_Z);
//				rotateInput += -glfwGetKey(window, GLFW_KEY_COMMA) + glfwGetKey(window, GLFW_KEY_PERIOD);
//				ProjetileInput = glfwGetKey(window, GLFW_KEY_SPACE);
//			}
//			else
//			{
//				// Check joystick input
//				int present = glfwJoystickPresent(player.playerID - 1);
//				// If the joystick is present, check its state
//				if (present == GLFW_TRUE)
//				{
//					GLFWgamepadstate state;
//					glfwGetGamepadState(player.playerID - 1, &state);
//					// Get joystick input, such as rotation and acceleration
//				   // Also check if the left and right buttons are pressed
//					//float rightStickX = state.axes[0];
//
//					// Get joystick axes' values
//					int count;
//					const float* axesStartPointer = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
//					const float* axesStartPointer1 = glfwGetJoystickAxes(GLFW_JOYSTICK_2, &count);
//					// Two first values are same input
//					const float* axesThirdPointer = axesStartPointer + 1;
//					const float* axesThirdPointer1 = axesStartPointer1 + 1;
//					// First value is X input
//					float rightStickX = *axesStartPointer;
//					// Third value is Y input
//					float rightStickY = *axesThirdPointer;
//
//					float rightStickX1 = *axesStartPointer1;
//
//					float rightStickY1 = *axesThirdPointer1;
//
//					//std::cout << *axesStartPointer << "\n";
//
//					const unsigned char* buttonpointer = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
//					const unsigned char* buttonpointer1 = glfwGetJoystickButtons(GLFW_JOYSTICK_2, &count);
//
//					const unsigned char* buttonsecondpointer = buttonpointer + 1;
//					const unsigned char* buttonsecondpointer1 = buttonpointer1 + 1;
//
//
//
//					std::cout << static_cast<float>(*buttonpointer) << " " << static_cast<float>(*buttonsecondpointer) << "\n";
//
//
//
//
//					const float* next = axesStartPointer + 1; // Increment the pointer by 1 to move to the next element
//					for (int i = 1; i < count; i++) // Start from 1 since you already printed the first element
//					{
//						//std::cout << *next << "\n";
//						next = next + 1; // Increment the pointer by 1 to move to the next element
//					}
//
//					const int buttonLeft = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
//					const int buttonRight = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
//					const int button_A = state.buttons[GLFW_GAMEPAD_BUTTON_A];
//
//					// Calculate acceleration based on joystick values
//					// and check if it's rotating left or right
//					//const float ltValue = state.axes[4]; // Left trigger
//					//const float rtValue = state.axes[5]; // Right trigger
//					//accelerationInput += +rtValue - ltValue;
//					if (player.playerID == 1)
//					{
//						accelerationInput += rightStickY;
//						rotateInput += rightStickX;
//						ProjetileInput = static_cast<float>(*buttonpointer);
//					}
//					else if (player.playerID == 2)
//					{
//						accelerationInput += rightStickY1;
//						rotateInput += rightStickX1;
//						ProjetileInput = static_cast<float>(*buttonpointer1);
//					}
//
//					/*accelerationInput += rightStickY;
//					rotateInput += +buttonRight - buttonLeft;
//					ProjetileInput = button_A;
//
//					// Deadzone for the right joystick
//					if (fabs(rightStickX) > 0.2)
//					{
//						// Set rotation input to rigth sticke
//						rotateInput += rightStickX;
//					}*/
//				}
//			}
//		}
//		if (player.playerID == 0)
//			starTimer -= dt;
//		//printf("starTimer: %i\n", int(starTimer));
//		// topedo hit logica 
//		if (player.hitPlayer == true)
//		{
//			// Hacer que el jugador gire 360 grados instantáneamente en el eje Y
//			TransformSystem::Rotate(player.renderedEntity, 0, 360.0f * dt, 0);
//
//
//			// Restablecer hitPlayer después de un cierto tiempo (por ejemplo, 2 segundos)
//			if (player.hitPlayerTime >= 2)
//			{
//				player.hitPlayer = false;
//				player.hitPlayerTime = 0.0f; // Restablecer el tiempo de duración de hitPlayer
//
//			}
//			else
//			{
//				accelerationInput = 0;
//				rotateInput = 0;
//				ProjetileInput = false;
//
//				player.hitPlayerTime += dt; // Incrementar el tiempo de duración de hitPlayer
//			}
//		}
//
//
//		accelerationInput = std::clamp(accelerationInput, -1.0f, 1.0f);
//		rotateInput = std::clamp(rotateInput, -1.0f, 1.0f);
//
//		// CALCULATE POSITION AND ROTATE 
//		// Calculate the forward direction
//		Vector2 forwardDirection = Vector2(cos(glm::radians(modelTransform.rotation.y)), sin(glm::radians(modelTransform.rotation.y)));
//		forwardDirection.Normalize();
//
//		// Initialize the impulse as zero
//		Vector2 forwardImpulse(0.0f, 0.0f);
//		if (rotateInput != 0.0f)
//		{
//			// Apply forward impulse if rotating or receiving a rotation command
//			TransformSystem::Rotate(player.renderedEntity, 0, -rotateInput * player.rotationSpeed * dt, 0);
//			forwardImpulse = forwardDirection * player.minAceleration * dt;
//		}
//
//		// Apply acceleration impulse if positive input is received
//		if (accelerationInput > 0.0f)
//		{
//
//			forwardImpulse = forwardDirection * accelerationInput * dt * player.acerationSpeed;
//		}
//		// Apply deceleration impulse if negative input is received
//		if (accelerationInput < 0.0f)
//		{
//
//			forwardImpulse = forwardDirection * accelerationInput * dt * player.acerationSpeed * 0.3;
//		}
//
//		// "Check if the variable 'ProjectileInput' is true and if the projectile time is equal to or less than zero."
//		if (ProjetileInput && player.projectileTime3 <= 0.0f)
//		{
//			// "Create a projectile using the parameters of the player object."
//			if (player.projectileTime1 <= 0.0f)
//			{
//				CreateProjectile(forwardDirection, player.projectileSpeed, transform.position, modelTransform.rotation, player.playerID);
//				// Reset the projectile time to a cooldown 
//				player.projectileTime1 = 0.0f;
//				// "Create a cooldown time between shots."
//				player.projectileTime3 = 0.2f;
//			}
//
//			else if (player.projectileTime2 <= 0.0f)
//			{
//				CreateProjectile(forwardDirection, player.projectileSpeed, transform.position, modelTransform.rotation, player.playerID);
//				player.projectileTime2 = 0.0f;
//				player.projectileTime3 = 0.2f;
//			}
//
//		}
//
//		// Decrease the projectile time by the elapsed time (dt)
//		player.projectileTime1 -= dt;
//		player.projectileTime2 -= dt;
//		player.projectileTime3 -= dt;
//
//		collider.rotationOverride = modelTransform.rotation.y + 1080;
//
//		// Apply the resulting impulse to the object
//		PhysicsSystem::Impulse(entity, forwardImpulse);
//	}
//}