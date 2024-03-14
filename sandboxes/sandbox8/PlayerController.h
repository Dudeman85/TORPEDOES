#pragma once 
#include "Resources.h"
#include <engine/Application.h>
#include <GL/gl.h>
#include "Projectiles.h"

// Declaration of the entity component system (ECS) instance
using namespace engine;

ECS_REGISTER_COMPONENT(Player)
struct Player
{
	float projectileSpeed = 500.0f;// Attack state
	bool attackHeld = false;     // Indicates if the attack button is held
	float accelerationSpeed = 1;    // Acceleration speed
	float minAceleration = 1;    // Minimum acceleration while rotating
	float rotationSpeed = 75;    // Rotation speed
	float projectileTime = 0;    // projectile Time 
	int lap = 1;
	float actionTimer1 = 0.0f;  // time 1  projectile 
	float projectileTime2 = 0.0f;  // time 2  projectile
	float projectileTime3 = 0.0f;  // time 3  peojectile
	int previousCheckpoint = -1;
	bool hitPlayer = false;
	float hitPlayerTime = 0;
	bool playExlposionSound = false;
	int id = 0;
	// new Projectil 
	float hedgehogSpeed = 200.0f;

	ecs::Entity nameText;
	string playername;
	string playerLap;

	//Child entities
	ecs::Entity renderedEntity;
};
ECS_REGISTER_COMPONENT(CheckPoint)
struct CheckPoint
{
	int checkPointID = 0;
	bool Finish_line = false;
};


bool HAS_WON = false;

// Player controller System. Requires Player , Tranform , Rigidbody , PolygonCollider
ECS_REGISTER_SYSTEM(PlayerController, Player, Transform, Rigidbody, PolygonCollider)
class PlayerController : public ecs::System
{
	float countdownTimer = 4; // start Time 

public:
	float getTimer() const
	{
		return countdownTimer;
	}
	static ecs::Entity playerWin;

	void Init()
	{
		ecs::Entity playerWin = ecs::NewEntity();
		ecs::AddComponent(playerWin, TextRenderer{ .font = resources::niagaraFont, .text = "", .offset = Vector3(-1.0f, 1.1f, 1.0f), .scale = Vector3(0.02f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
		ecs::AddComponent(playerWin, SpriteRenderer{ .texture = resources::winSprite, .enabled = false, .uiElement = true });
		ecs::AddComponent(playerWin, Transform{ .position = Vector3(0, 0, 0), .scale = Vector3(0.5f) });
	}

	std::array<float, 4> GetPlayerBounds()
	{
		std::array<float, 4> playerBounds{ -INFINITY, -INFINITY, INFINITY, INFINITY };

		//Get the min and max bounds of each player together

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
		// Get references to the involved components
		Player& player = ecs::GetComponent<Player>(collision.a);  // collision.a on sama kun player entitety eli on sama kuin ( laMuerte) 
		Transform& playertranform = ecs::GetComponent<Transform>(collision.a);

		PolygonCollider& playerCollider = ecs::GetComponent<PolygonCollider>(collision.a);

		if (collision.type == Collision::Type::tilemapCollision)
		{
			ecs::GetComponent<Rigidbody>(collision.a).velocity *= 0.99f;

		}
		// Check if the collision involves a checkpoint

		// true tai false 
		if (ecs::HasComponent<CheckPoint>(collision.b)) // varmista onko osuu checkpoint
		{
			CheckPoint& checkpoint = ecs::GetComponent<CheckPoint>(collision.b);  // hae checkpoint componenti
			if (player.previousCheckpoint + 1 == checkpoint.checkPointID)        // tarkista onko pelaja osu oiken checkpoit
			{
				player.previousCheckpoint = checkpoint.checkPointID;              // asenta pelaja vimene checkpoin osunut 
				if (checkpoint.Finish_line)
				{
					if (player.lap == 1)
					{
						if (!HAS_WON)
						{
							HAS_WON = true;
							TextRenderer& winText = ecs::GetComponent<TextRenderer>(playerWin);
							SpriteRenderer& winSprite = ecs::GetComponent<SpriteRenderer>(playerWin);
							winSprite.enabled = true;
							winText.text = player.playername;
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
			Torpedo& torpedo = ecs::GetComponent<Torpedo>(collision.b); // tällä on Entity on collision.b 
			//Projectile& projectile = ecs::GetComponent<Projectile>(collision.a);
			if (player.id != torpedo.ownerID)
			{
				player.hitPlayer = true;
				CreateAnimation(projectransfor.position + rigidbody.velocity / 15);
				ecs::DestroyEntity(collision.b);
				player.playExlposionSound = true;
			}
		}
	}
	// check if projectil collision tilemap Trigger
	static void OnprojectilCollision(Collision collision)
	{
		Transform& projectransfor = ecs::GetComponent<Transform>(collision.a);
		if (collision.type == Collision::Type::tilemapTrigger)
		{
			if (collision.b != 1)
			{   // Do animation where projectile impact 
				CreateAnimation(projectransfor.position);
				ecs::DestroyEntity(collision.a);
			}
		}
	}

	/// PlayerControlle Update 
	void Update(GLFWwindow* window, float dt)
	{
		// Iterate through entities in the system
		for (auto itr = entities.begin(); itr != entities.end();)
		{
			//Get the entity and increment the iterator
			ecs::Entity entity = *itr++;

			// Get player, transform, and rigidbody components
			Player& player = ecs::GetComponent<Player>(entity);
			Transform& transform = ecs::GetComponent<Transform>(entity);
			Transform& modelTransform = ecs::GetComponent<Transform>(player.renderedEntity);
			Rigidbody& rigidbody = ecs::GetComponent<Rigidbody>(entity);
			PolygonCollider& collider = ecs::GetComponent<PolygonCollider>(entity);

			//initialize input zero 
			float accelerationInput = 0;
			float rotateInput = 0;
			bool ProjetileInput = 0;
			// Starte Time 
			if (countdownTimer <= 0)
			{
				accelerationInput = 0;
				rotateInput = 0;
				ProjetileInput = 0;
				// Get keyboard input		 
				if (player.id == 0)
				{
					//Player 0 only gets keyboard input
					accelerationInput += +glfwGetKey(window, GLFW_KEY_A) - glfwGetKey(window, GLFW_KEY_Z);
					rotateInput += -glfwGetKey(window, GLFW_KEY_COMMA) + glfwGetKey(window, GLFW_KEY_PERIOD);
					ProjetileInput = glfwGetKey(window, GLFW_KEY_SPACE);
				}
				else
				{
					// Check joystick input
					int present = glfwJoystickPresent(player.id - 1);
					// If the joystick is present, check its state
					if (present == GLFW_TRUE)
					{
						GLFWgamepadstate state;
						glfwGetGamepadState(player.id - 1, &state);
						// Get joystick input, such as rotation and acceleration
					   // Also check if the left and right buttons are pressed
						//float rightStickX = state.axes[0];

						// Get joystick axes' values
						int count;
						const float* axesStartPointer = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
						const float* axesStartPointer1 = glfwGetJoystickAxes(GLFW_JOYSTICK_2, &count);
						// Two first values are same input
						const float* axesThirdPointer = axesStartPointer + 1;
						const float* axesThirdPointer1 = axesStartPointer1 + 1;
						// First value is X input
						float rightStickX = *axesStartPointer;
						// Third value is Y input
						float rightStickY = *axesThirdPointer;

						float rightStickX1 = *axesStartPointer1;

						float rightStickY1 = *axesThirdPointer1;

						//std::cout << *axesStartPointer << "\n";

						const unsigned char* buttonpointer = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
						const unsigned char* buttonpointer1 = glfwGetJoystickButtons(GLFW_JOYSTICK_2, &count);

						const unsigned char* buttonsecondpointer = buttonpointer + 1;
						const unsigned char* buttonsecondpointer1 = buttonpointer1 + 1;



						std::cout << static_cast<float>(*buttonpointer) << " " << static_cast<float>(*buttonsecondpointer) << "\n";




						const float* next = axesStartPointer + 1; // Increment the pointer by 1 to move to the next element
						for (int i = 1; i < count; i++) // Start from 1 since you already printed the first element
						{
							//std::cout << *next << "\n";
							next = next + 1; // Increment the pointer by 1 to move to the next element
						}

						const int buttonLeft = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
						const int buttonRight = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
						const int button_A = state.buttons[GLFW_GAMEPAD_BUTTON_A];

						// Calculate acceleration based on joystick values
						// and check if it's rotating left or right
						//const float ltValue = state.axes[4]; // Left trigger
						//const float rtValue = state.axes[5]; // Right trigger
						//accelerationInput += +rtValue - ltValue;
						if (player.id == 1)
						{
							accelerationInput += rightStickY;
							rotateInput += rightStickX;
							ProjetileInput = static_cast<float>(*buttonpointer);
						}
						else if (player.id == 2)
						{
							accelerationInput += rightStickY1;
							rotateInput += rightStickX1;
							ProjetileInput = static_cast<float>(*buttonpointer1);
						}

						/*accelerationInput += rightStickY;
						rotateInput += +buttonRight - buttonLeft;
						ProjetileInput = button_A;

						// Deadzone for the right joystick
						if (fabs(rightStickX) > 0.2)
						{
							// Set rotation input to rigth sticke
							rotateInput += rightStickX;
						}*/
					}
				}
			}
			if (player.id == 0)
				countdownTimer -= dt;
			//printf("starTimer: %i\n", int(starTimer));
			// topedo hit logica 
			if (player.hitPlayer == true)
			{
				// Hacer que el jugador gire 360 grados instantáneamente en el eje Y
				TransformSystem::Rotate(player.renderedEntity, 0, 360.0f * dt, 0);


				// Restablecer hitPlayer después de un cierto tiempo (por ejemplo, 2 segundos)
				if (player.hitPlayerTime >= 2)
				{
					player.hitPlayer = false;
					player.hitPlayerTime = 0.0f; // Restablecer el tiempo de duración de hitPlayer

				}
				else
				{
					accelerationInput = 0;
					rotateInput = 0;
					ProjetileInput = false;

					player.hitPlayerTime += dt; // Incrementar el tiempo de duración de hitPlayer
				}
			}


			accelerationInput = std::clamp(accelerationInput, -1.0f, 1.0f);
			rotateInput = std::clamp(rotateInput, -1.0f, 1.0f);

			// CALCULATE POSITION AND ROTATE 
			// Calculate the forward direction
			Vector2 forwardDirection = Vector2(cos(glm::radians(modelTransform.rotation.y)), sin(glm::radians(modelTransform.rotation.y)));
			forwardDirection.Normalize();

			// Initialize the impulse as zero
			Vector2 forwardImpulse(0.0f, 0.0f);
			if (rotateInput != 0.0f)
			{
				// Apply forward impulse if rotating or receiving a rotation command
				TransformSystem::Rotate(player.renderedEntity, 0, -rotateInput * player.rotationSpeed * dt, 0);
				forwardImpulse = forwardDirection * player.minAceleration * dt;
			}

			// Apply acceleration impulse if positive input is received
			if (accelerationInput > 0.0f)
			{

				forwardImpulse = forwardDirection * accelerationInput * dt * player.accelerationSpeed;
			}
			// Apply deceleration impulse if negative input is received
			if (accelerationInput < 0.0f)
			{

				forwardImpulse = forwardDirection * accelerationInput * dt * player.accelerationSpeed * 0.3;
			}

			// "Check if the variable 'ProjectileInput' is true and if the projectile time is equal to or less than zero."
			if (ProjetileInput && player.projectileTime3 <= 0.0f)
			{
				// "Create a projectile using the parameters of the player object."
				if (player.actionTimer1 <= 0.0f)
				{
					CreateHedgehog(forwardDirection, player.projectileSpeed, transform.position, modelTransform.rotation, player.id);
					// Reset the projectile time to a cooldown 
					player.actionTimer1 = 0.0f;
					// "Create a cooldown time between shots."
					player.projectileTime3 = 0.2f;
				}

				else if (player.projectileTime2 <= 0.0f)
				{
					CreateHedgehog(forwardDirection, player.projectileSpeed, transform.position, modelTransform.rotation, player.id);
					player.projectileTime2 = 0.0f;
					player.projectileTime3 = 0.2f;
				}

			}

			// Decrease the projectile time by the elapsed time (dt)
			player.actionTimer1 -= dt;
			player.projectileTime2 -= dt;
			player.projectileTime3 -= dt;

			collider.rotationOverride = modelTransform.rotation.y + 1080;

			// Apply the resulting impulse to the object
			PhysicsSystem::Impulse(entity, forwardImpulse);
		}
	}

	//Spawn 1-4 players, all in a line from top to bottom
	static void CreatePlayers(int count, Vector2 startPos)
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
			ecs::AddComponent(player, Player{ .accelerationSpeed = 300, .minAceleration = 120, .id = i, .nameText = playerNameText, .renderedEntity = playerRender });
			ecs::AddComponent(player, Transform{ .position = Vector3(startPos - offset * i, 100), .rotation = Vector3(0, 0, 0), .scale = Vector3(7) });
			ecs::AddComponent(player, Rigidbody{ .drag = 0.025 });
			vector<Vector2> colliderVerts{ Vector2(2, 2), Vector2(2, -1), Vector2(-5, -1), Vector2(-5, 2) };
			ecs::AddComponent(player, PolygonCollider{ .vertices = colliderVerts, .callback = PlayerController::OnCollision, .visualise = true });


			//Create the player's name tag
			ecs::AddComponent(playerNameText, TextRenderer{ .font = resources::niagaraFont, .text = "P" + to_string(i + 1), .color = Vector3(0.5, 0.8, 0.2) });
			ecs::AddComponent(playerNameText, Transform{ .position = Vector3(-2, 2, 1) , .scale = Vector3(0.1) });
			TransformSystem::AddParent(playerNameText, player);

			//Create the player's rendered entity
			ecs::AddComponent(playerRender, Transform{ .rotation = Vector3(45, 0, 0) });
			ecs::AddComponent(playerRender, ModelRenderer{ .model = resources::laMuerteModel });
			TransformSystem::AddParent(playerRender, player);

			//Create the players's torpedo indicators
			ecs::AddComponent(torpIndicator1, SpriteRenderer{ .texture = resources::torpReadyTexture });
			ecs::AddComponent(torpIndicator1, Transform{ .position = Vector3(-2, -2, 10), .scale = Vector3(2, .5, 1) });
			TransformSystem::AddParent(torpIndicator1, player);
			ecs::AddComponent(torpIndicator2, SpriteRenderer{ .texture = resources::torpReadyTexture });
			ecs::AddComponent(torpIndicator2, Transform{ .position = Vector3(2, -2, 10), .scale = Vector3(2, .5, 1) });
			TransformSystem::AddParent(torpIndicator2, player);
		}
	}

	Vector3 avgPosition;
	std::array<float, 4> playerBounds{ -INFINITY, -INFINITY, INFINITY, INFINITY };
};

ecs::Entity PlayerController::playerWin = playerWin;






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