#pragma once 
#include <engine/Application.h>
#include "Resources.h"
#include "Projectiles.h"
#include "engine/Input.h"

using namespace engine;

enum ShipType { torpedoBoat, submarine, cannonBoat, hedgehogBoat, pirateShip };

ECS_REGISTER_COMPONENT(Player)
struct Player
{
	ShipType shipType; //This might not be necessary

	int id = 0;

	//Movement stats
	float accelerationSpeed;
	float rotationSpeed;
	//Minimum acceleration while rotating
	float minAcceleration = 100;

	//Action cooldowns
	float mainCooldown;
	float specialCooldown;

	//Action timers
	float actionTimer1 = 0.0f;
	float projectileRechargeTime = 0.0f;
	float projectileShootCooldown = 0.0f;

	//Action functions
	std::function<void(ecs::Entity, int)> mainAction;
	std::function<void(ecs::Entity, int)> specialAction;

	//Checkpoint stuff
	int previousCheckpoint = -1;
	int lap = 1;

	//Hit by weapon stuff
	bool isHit = false;
	float hitTime = 0;

	//Rendered Child entities
	ecs::Entity renderedEntity;
	ecs::Entity nameText;
};

ECS_REGISTER_COMPONENT(CheckPoint)
struct CheckPoint
{
	int checkPointID = 0;
	bool Finish_line = false;
};

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
			Player{.accelerationSpeed = 400, .rotationSpeed = 75, .mainCooldown = 5, .specialCooldown = 10, .mainAction = SpawnProjectile, .specialAction = SpawnProjectile } });
		shipComponents.insert({ ShipType::submarine,
			Player{.accelerationSpeed = 800, .rotationSpeed = 75, .mainCooldown = 5, .specialCooldown = 10, .mainAction = SpawnProjectile, .specialAction = SpawnProjectile } });
		shipComponents.insert({ ShipType::cannonBoat,
			Player{.accelerationSpeed = 400, .rotationSpeed = 75, .mainCooldown = 5, .specialCooldown = 10, .mainAction = SpawnProjectile, .specialAction = SpawnProjectile } });
		shipComponents.insert({ ShipType::hedgehogBoat,
			Player{.accelerationSpeed = 400, .rotationSpeed = 75, .mainCooldown = 5, .specialCooldown = 10, .mainAction = SpawnProjectile, .specialAction = SpawnProjectile } });

		//Initialize ship type models
		shipModels.insert({ ShipType::torpedoBoat, resources::models["LaMuerte.obj"]});
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
		Transform& playertranform = ecs::GetComponent<Transform>(collision.a);
		PolygonCollider& playerCollider = ecs::GetComponent<PolygonCollider>(collision.a);

		// Slow player down when off track
		if (collision.type == Collision::Type::tilemapCollision)
		{
			ecs::GetComponent<Rigidbody>(collision.a).velocity *= 0.991f;
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
		else if (ecs::HasComponent<Torpedo>(collision.b))
		{
			Rigidbody& rigidbody = ecs::GetComponent<Rigidbody>(collision.b);
			Transform& projectransfor = ecs::GetComponent<Transform>(collision.b);
			Torpedo& torpedo = ecs::GetComponent<Torpedo>(collision.b); // Entity is collision.b 

			if (player.id != torpedo.ownerID)
			{
				player.isHit = true;
				CreateAnimation(projectransfor.position + rigidbody.velocity / 15);
				ecs::DestroyEntity(collision.b);
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
			float accelerationInput = input::GetInputValue("Move" + std::to_string(player.id), GLFW_GAMEPAD_AXIS_LEFT_Y);
			float rotateInput = input::GetInputValue("Move" + std::to_string(player.id), GLFW_GAMEPAD_AXIS_LEFT_X);
			bool ProjetileInput = input::GetPressed("Shoot" + std::to_string(player.id));

			accelerationInput = std::clamp(accelerationInput, -1.0f, 1.0f);
			rotateInput = std::clamp(rotateInput, -1.0f, 1.0f);

			// Movement

				// Torpedo hit logic
			if (player.isHit == true)
			{
				// Rotate player 360 degrees
				TransformSystem::Rotate(player.renderedEntity, 0, 360.0f * dt, 0);

				// Restabilize player when hit time has gone by
				if (player.hitTime >= 2)
				{
					player.isHit = false;
					player.hitTime = 0.0f;
				}
				else
				{
					// Ignore all input

					player.hitTime += dt; // Increment duration of hit time
				}
			}

			// CALCULATE POSITION AND ROTATE 
			// Calculate the forward direction
			Vector2 forwardDirection = Vector2(cos(glm::radians(modelTransform.rotation.y)), sin(glm::radians(modelTransform.rotation.y)));
			forwardDirection.Normalize();

			// Initialize the impulse as zero
			Vector2 forwardImpulse(0.0f, 0.0f);
			if (rotateInput != 0.0f)
			{
				//Slow rotation based on throttle setting
				//TODO: this function could be improved by testing
				float rotationScalar = 1 - log10(2.0f * std::max(0.5f, accelerationInput));
				// Apply forward impulse if rotating or receiving a rotation command
				TransformSystem::Rotate(player.renderedEntity, 0, -rotateInput * player.rotationSpeed * rotationScalar * dt, 0);
				forwardImpulse = forwardDirection * player.minAcceleration * dt;
			}

			// Apply acceleration impulse if positive input is received
			if (accelerationInput > 0.0f)
			{
				forwardImpulse = forwardDirection * accelerationInput * dt * player.accelerationSpeed;
			}
			// Apply deceleration impulse if negative input is received
			if (accelerationInput < 0.0f)
			{
				// TODO: Deacceleration rate
				forwardImpulse = forwardDirection * accelerationInput * dt * player.accelerationSpeed * 0.3;
			}

			// Check if the variable 'ProjectileInput' is true and if the projectile time is equal to or less than zero.
			if (ProjetileInput && player.projectileShootCooldown <= 0.0f)
			{
				// Create a projectile using the parameters of the player object.
				if (player.actionTimer1 <= 0.0f)
				{
					CreateProjectile(forwardDirection, 500, transform.position, modelTransform.rotation, player.id);
					// Reset the projectile time to a cooldown 
					player.actionTimer1 = 5.f;
					// Create a cooldown time between shots.
					player.projectileShootCooldown = 0.2f;
				}

				else if (player.projectileRechargeTime <= 0.0f)
				{
					CreateProjectile(forwardDirection, 500, transform.position, modelTransform.rotation, player.id);
					player.projectileRechargeTime = 5.f;
					player.projectileShootCooldown = 0.2f;
				}
			}

			// Decrease the projectile time by the elapsed time (dt)
			player.actionTimer1 -= dt;
			player.projectileRechargeTime -= dt;
			player.projectileShootCooldown -= dt;

			collider.rotationOverride = modelTransform.rotation.y + 1080;

			// Apply the resulting impulse to the object
			PhysicsSystem::Impulse(entity, forwardImpulse);
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
			ecs::AddComponent(player, Rigidbody{ .drag = 0.025 });
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