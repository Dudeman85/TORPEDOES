#pragma once 
#include <engine/Application.h>
#include <GL/gl.h>


// Declaration of the entity component system (ECS) instance
using namespace engine;

ECS_REGISTER_COMPONENT(Player)
struct Player
{
	float projectileSpeed = 500;  // Attack state
	bool attackHeld = false;     // Indicates if the attack button is held
	float acerationSpeed = 1;    // Acceleration speed
	float minAceleration = 1;    // Minimum acceleration while rotating
	float rotationSpeed = 75;    // Rotation speed
	float projectileTime = 0;    // projectile Time 
	int lap = 1;
	float projectileTime1 = 0.0f;  // time 1  projectile 
	float projectileTime2 = 0.0f;  // time 2  projectile
	float projectileTime3 = 0.0f;  // time 3  peojectile
	int previousCheckpoint = -1;
	bool hitPlayer = false;
	float hitPlayerTime = 0;
	bool playExlposionSound = false;
	int playerID = 0;
	ecs::Entity playerFont;
	string playername;
	string playerLap;
};
ECS_REGISTER_COMPONENT(CheckPoint)
struct CheckPoint
{
	int checkPointID = 0;
	bool Finish_line = false;
};
ECS_REGISTER_COMPONENT(Projectile)
struct Projectile
{
	int ownerID = 0;
};

bool HAS_WON = false;

// Player controller System. Requires Player , Tranform , Rigidbody , PolygonCollider &  ModelRenderer
ECS_REGISTER_SYSTEM(PlayerController, Player, Transform, Rigidbody, PolygonCollider, ModelRenderer)
class PlayerController : public ecs::System
{

	float starTimer = 19.0; // start Time 
	Model* torpedomodel;
	void CreateProjectile(Vector2 direction, float projectileSpeed, Vector3 spawnPosition, Vector3 sapawnRotation, int owerID)
	{

		ecs::Entity projectile = ecs::NewEntity();
		ecs::AddComponent(projectile, Transform{ .position = spawnPosition, .rotation = sapawnRotation, .scale = Vector3(10) });
		ecs::AddComponent(projectile, Rigidbody{ .velocity = direction * projectileSpeed });
		ecs::AddComponent(projectile, ModelRenderer{ .model = torpedomodel });
		std::vector<Vector2> Torpedoverts{ Vector2(2, 0.5), Vector2(2, -0.5), Vector2(-2, -0.5), Vector2(-2, 0.5) };
		ecs::AddComponent(projectile, PolygonCollider{ .vertices = Torpedoverts, .callback = PlayerController::OnprojectilCollision, .trigger = true, .visualise = false,  .rotationOverride = sapawnRotation.y });
		ecs::AddComponent(projectile, Projectile{ .ownerID = owerID });



	}
	static void CreateAnimation(Vector3 animPosition)
	{
		ecs::Entity projecAnim = ecs::NewEntity();
		animPosition.z += 100;
		ecs::AddComponent(projecAnim, Transform{ .position = animPosition,  .scale = Vector3(20) });
		ecs::AddComponent(projecAnim, SpriteRenderer{ });
		ecs::AddComponent(projecAnim, Animator{ .onAnimationEnd = ecs::DestroyEntity });
		AnimationSystem::AddAnimation(projecAnim, *ExplosionAnim, "explosion");
		AnimationSystem::PlayAnimation(projecAnim, "explosion", false);

	};

public:
	float getTimer() const {
		return starTimer;
	}


	static ecs::Entity playerWin;


	static Animation* ExplosionAnim;
	void Init()
	{
		torpedomodel = new Model("torpedo.obj");
	}
	~PlayerController()
	{
		delete torpedomodel;
	}
	// OnCollision 

	static void OnCollision(Collision collision)
	{
		// Get references to the involved components
		Player& player = ecs::GetComponent<Player>(collision.a);  // collision.a on sama kun player entitety eli on sama kuin ( laMuerte) 
		Transform& playertranform = ecs::GetComponent<Transform>(collision.a);

		PolygonCollider& playerCollider = ecs::GetComponent<PolygonCollider>(collision.a);

		if (collision.type == Collision::Type::tilemapCollision)
		{
			ecs::GetComponent<Rigidbody>(collision.a).velocity *= 0.9995f;

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
		else if (ecs::HasComponent<Projectile>(collision.b))
		{
			Rigidbody& rigidbody = ecs::GetComponent<Rigidbody>(collision.b);
			Transform& projectransfor = ecs::GetComponent<Transform>(collision.b);
			Projectile& projectile = ecs::GetComponent<Projectile>(collision.b); // tällä on Entity on collision.b 
			//Projectile& projectile = ecs::GetComponent<Projectile>(collision.a);
			if (player.playerID != projectile.ownerID)
			{
				player.hitPlayer = true;
				CreateAnimation(projectransfor.position + rigidbody.velocity / 15);
				projectransfor.position.y += 10000000; // destroy Entity "almost"
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
				projectransfor.position.y += 10000000;

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
			Rigidbody& rigidbody = ecs::GetComponent<Rigidbody>(entity);
			PolygonCollider& collider = ecs::GetComponent<PolygonCollider>(entity);
			//initialize input zero 
			float accelerationInput = 0;
			float rotateInput = 0;
			bool ProjetileInput = 0;
			// Starte Time 
			if (starTimer <= 0)
			{
				accelerationInput = 0;
				rotateInput = 0;
				ProjetileInput = 0;
				// Get keyboard input		 
				if (player.playerID == 0)
				{
					//Player 0 only gets keyboard input
					accelerationInput += +glfwGetKey(window, GLFW_KEY_A) - glfwGetKey(window, GLFW_KEY_Z);
					rotateInput += -glfwGetKey(window, GLFW_KEY_COMMA) + glfwGetKey(window, GLFW_KEY_PERIOD);
					ProjetileInput = glfwGetKey(window, GLFW_KEY_SPACE);
				}
				else
				{
					// Check joystick input
					int present = glfwJoystickPresent(player.playerID - 1);
					// If the joystick is present, check its state
					if (present == GLFW_TRUE)
					{
						GLFWgamepadstate state;
						glfwGetGamepadState(player.playerID - 1, &state);
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
						if (player.playerID == 1)
						{
							accelerationInput += rightStickY;
							rotateInput += rightStickX;
							ProjetileInput = static_cast<float>(*buttonpointer);
						}
						else if (player.playerID == 2)
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
			starTimer -= dt;
			//printf("starTimer: %i\n", int(starTimer));
			// topedo hit logica 
			if (player.hitPlayer == true)
			{
				// Hacer que el jugador gire 360 grados instantáneamente en el eje Y
				TransformSystem::Rotate(entity, 0, 360.0f * dt, 0);


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
			Vector2 forwardDirection = Vector2(cos(glm::radians(transform.rotation.y)), sin(glm::radians(transform.rotation.y)));
			forwardDirection.Normalize();

			// Initialize the impulse as zero
			Vector2 forwardImpulse(0.0f, 0.0f);
			if (rotateInput != 0.0f)
			{
				// Apply forward impulse if rotating or receiving a rotation command
				TransformSystem::Rotate(entity, 0, -rotateInput * player.rotationSpeed * dt, 0);
				forwardImpulse = forwardDirection * player.minAceleration * dt;
			}

			// Apply acceleration impulse if positive input is received
			if (accelerationInput > 0.0f)
			{

				forwardImpulse = forwardDirection * accelerationInput * dt * player.acerationSpeed;
			}
			// Apply deceleration impulse if negative input is received
			if (accelerationInput < 0.0f)
			{

				forwardImpulse = forwardDirection * accelerationInput * dt * player.acerationSpeed * 0.3;
			}

			// "Check if the variable 'ProjectileInput' is true and if the projectile time is equal to or less than zero."
			if (ProjetileInput && player.projectileTime3 <= 0.0f)
			{
				// "Create a projectile using the parameters of the player object."
				if (player.projectileTime1 <= 0.0f)
				{
					CreateProjectile(forwardDirection, player.projectileSpeed, transform.position, transform.rotation, player.playerID);
					// Reset the projectile time to a cooldown 
					player.projectileTime1 = 5.0f;
					// "Create a cooldown time between shots."
					player.projectileTime3 = 0.2f;
				}

				else if (player.projectileTime2 <= 0.0f)
				{
					CreateProjectile(forwardDirection, player.projectileSpeed, transform.position, transform.rotation, player.playerID);
					player.projectileTime2 = 5.0f;
					player.projectileTime3 = 0.2f;
				}

			}

			//Add up all the player positions
			avgPosition += transform.position;
			//Get the min and max bounds of each player together
			if (playerBounds[0] < transform.position.y)
				playerBounds[0] = transform.position.y;
			if (playerBounds[1] < transform.position.x)
				playerBounds[1] = transform.position.x;
			if (playerBounds[2] > transform.position.y)
				playerBounds[2] = transform.position.y;
			if (playerBounds[3] > transform.position.x)
				playerBounds[3] = transform.position.x;

			// Decrease the projectile time by the elapsed time (dt)
			player.projectileTime1 -= dt;
			player.projectileTime2 -= dt;
			player.projectileTime3 -= dt;


			collider.rotationOverride = transform.rotation.y + 1080;

			// Apply the resulting impulse to the object
			PhysicsSystem::Impulse(entity, forwardImpulse);
			TransformSystem::SetPosition(player.playerFont, transform.position + Vector3(0, 20, 0));
		}
	};

	Vector3 avgPosition;
	std::array<float, 4> playerBounds{ -INFINITY, -INFINITY, INFINITY, INFINITY };
};

Animation* PlayerController::ExplosionAnim = ExplosionAnim;
ecs::Entity PlayerController::playerWin = playerWin;
