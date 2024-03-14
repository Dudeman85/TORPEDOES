#pragma once 
#include <engine/Application.h>

// Declaration of the entity component system (ECS) instance
extern ECS ecs; // (Entity Component system)
using namespace engine;

// Definition of the Player structure, storing player attributes
struct Player
{

	float projectileSpeed = 300;               // Attack state
	bool attackHeld = false;     // Indicates if the attack button is held
	float accelerationSpeed = 1;    // Acceleration speed
	float minAcceleration = 1;    // Minimum acceleration while rotating
	float rotationSpeed = 50; // Rotation speed
	float projectileTime = 0;
	int lap = 0;
	float actionTimer1 = 0.0f;
	float projectileRechargeTime = 0.0f;
	float projectileShootCooldown = 0.0f;
	int previousCheckpoint = -1;

	int id;
};
struct CheckPoint
{
	int checkPointID = 0;
	bool Finish_line = false;

};



// Player controller System. Requires Player , Tranform , Rigidbody , PolygonCollider &  ModelRenderer
class PlayerController : public System
{

	Model* torpedomodel;
	void CreateProjectile(Vector2 direction, float projectileSpeed, Vector3 spawnPosition, Vector3 sapawnRotation)
	{
		Entity projectile = ecs.newEntity();
		ecs.addComponent(projectile, Transform{ .position = spawnPosition, .rotation = sapawnRotation, .scale = Vector3(10) });
		ecs.addComponent(projectile, Rigidbody{ direction * projectileSpeed });
		ecs.addComponent(projectile, ModelRenderer{ .model = torpedomodel });

	}

public:

	PlayerController()
	{
		torpedomodel = new Model("assets/torpedo.obj");
	}
	~PlayerController()
	{
		delete torpedomodel;
	}

	static void OnCollision(Collision collision) // onCollision function 
	{
		// collision.b on sama kun checkpoint Entitety 
		Player& player = ecs.getComponent<Player>(collision.a);              // collision.a on sama kun player entitety eli on sama kuin ( laMuerte) 
		// true tai false 
		if (ecs.hasComponent<CheckPoint>(collision.b)) // varmista onko osuu checkpoint
		{
			CheckPoint& checkpoint = ecs.getComponent<CheckPoint>(collision.b);  // hae checkpoint componenti
			if (player.previousCheckpoint + 1 == checkpoint.checkPointID)        // tarkista onko pelaja osu oiken checkpoit
			{
				player.previousCheckpoint = checkpoint.checkPointID;              // asenta pelaja vimene checkpoin osunut 
				if (checkpoint.Finish_line)
				{
					player.previousCheckpoint = -1;
					player.lap++;
					cout << "kierros" << player.lap << endl;

				}

			}
		}
	}

	void Update(GLFWwindow* window, float dt)
	{
		// Iterate through entities in the system
		for (auto const& entity : entities)
		{
			// Get player, transform, and rigidbody components
			Player& player = ecs.getComponent<Player>(entity);
			Transform& transform = ecs.getComponent<Transform>(entity);
			Rigidbody& rigidbody = ecs.getComponent<Rigidbody>(entity);
			PolygonCollider& collider = ecs.getComponent<PolygonCollider>(entity);
			//initialize input zero 
			float accelerationInput = 0;
			float rotateInput = 0;
			bool ProjetileInput = 0;
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
					float rightStickX = state.axes[0];
					const int buttonLeft = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
					const int buttonRight = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
					const int button_A = state.buttons[GLFW_GAMEPAD_BUTTON_A];

					// Calculate acceleration based on joystick values
					// and check if it's rotating left or right
					const float ltValue = state.axes[4]; // Left trigger
					const float rtValue = state.axes[5]; // Right trigger
					accelerationInput += +rtValue - ltValue;
					rotateInput += +buttonRight - buttonLeft;
					ProjetileInput = button_A;

					// Deadzone for the right joystick
					if (fabs(rightStickX) > 0.2)
					{
						// Set rotation input to rigth sticke 
						rotateInput += rightStickX;
					}
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

				forwardImpulse = forwardDirection * accelerationInput * dt * player.accelerationSpeed * 0.3;
			}

			// "Check if the variable 'ProjectileInput' is true and if the projectile time is equal to or less than zero."
			if (ProjetileInput && player.projectileShootCooldown <= 0.0f)
			{
				// "Create a projectile using the parameters of the player object."
				if (player.actionTimer1 <= 0.0f)
				{
					CreateProjectile(forwardDirection, player.projectileSpeed, transform.position, transform.rotation);
					// Reset the projectile time to a cooldown 
					player.actionTimer1 = 5.0f;
					// "Create a cooldown time between shots."
					player.projectileShootCooldown = 0.2f;
				}

				else if (player.projectileRechargeTime <= 0.0f)
				{
					CreateProjectile(forwardDirection, player.projectileSpeed, transform.position, transform.rotation);
					player.projectileRechargeTime = 5.0f;
					player.projectileShootCooldown = 0.2f;
				}

			}


			// Decrease the projectile time by the elapsed time (dt)
			player.actionTimer1 -= dt;
			player.projectileRechargeTime -= dt;
			player.projectileShootCooldown -= dt;


			collider.rotationOverride = transform.rotation.y + 1080;

			// Apply the resulting impulse to the object
			PhysicsSystem::Impulse(entity, forwardImpulse);
		}
	};
};
