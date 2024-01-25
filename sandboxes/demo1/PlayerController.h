#pragma once
#include "Physics.h"
#include <engine/Application.h>
#include <cmath>

extern ECS ecs;

using namespace engine;

//Player Component
struct Player
{
	int attacking;
	bool attackHeld = false;
	bool jumpHeld = false;
	bool jumping = false;
	bool wallJumping = false;

	float maxSpeed = 2000;
	float accelSpeed = 400;
	float reactivityMult = 2;
	float maxJumpSpeed = 3800;
	float jumpAccelSpeed = 1000;

	// > 0 = yes
	float canWallJump = 0;
	int wallJumpDir = 0;
	float maxWallJumpSpeed = 4000;
	float wallJumpAccelSpeed = 1500;
	int jumpSound = 0;
	int attackSound = 0;

	//Fix because of bad tilemap collision checking
	int shouldWallslide = 0;

	static SoundSource speaker;

	Entity attackHitbox;
};

//Player Controller requires Player, Sprite, Transform, BoxCollider, Rigidbody, Animator
class PlayerController : public System
{
public:
	PlayerController()
	{
		//jumpSound = SoundBuffer::getFile()->addSoundEffect("assets/jump.wav");
		//swingSound = SoundBuffer::getFile()->addSoundEffect("assets/swing.wav");
		//stepSound = SoundBuffer::getFile()->addSoundEffect("assets/step.wav");
	}

	void Update(GLFWwindow* window, double deltaTime/*, SoundSource& speaker, SoundSource& walkSpeaker, SoundSource& swordSpeaker*/)
	{
		for (auto const& entity : entities)
		{
			Player& player = ecs.getComponent<Player>(entity);
			Transform& transform = ecs.getComponent<Transform>(entity);
			oldPhysics::Rigidbody& rigidbody = ecs.getComponent<oldPhysics::Rigidbody>(entity);
			oldPhysics::BoxCollider& collider = ecs.getComponent<oldPhysics::BoxCollider>(entity);
			Animator& animator = ecs.getComponent<Animator>(entity);
			
			int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
			GLFWgamepadstate state;
			glfwGetGamepadState(GLFW_JOYSTICK_1, &state);
			int buttonCount;
			const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

			if (player.attacking > 0)
			{
				if (!animator.playingAnimation)
				{
					if (player.attacking < 2)
					{
						Transform& swordTransform = ecs.getComponent<Transform>(player.attackHitbox);
						swordTransform.position.x = transform.rotation.y > 0 ? transform.position.x - 40 : transform.position.x + 40;
						swordTransform.position.y = transform.position.y - 10;

						player.attackSound = 1;
						player.attacking++;
						AnimationSystem::PlayAnimation(entity, "Attack " + to_string(player.attacking));
					}
					else
						player.attacking = 0;
				}
			}

			//Set can wall jump for 100ms after touching wall
			if (collider.sidesCollided[oldPhysics::Direction::left] || collider.sidesCollided[oldPhysics::Direction::right])
			{
				//If not touching ground play wallslide
				if (!collider.sidesCollided[oldPhysics::Direction::down])
					player.shouldWallslide = 4;

				//Enable Walljump
				if (!player.jumpHeld && !collider.sidesCollided[oldPhysics::Direction::down])
				{
					//Right wall
					if (collider.sidesCollided[oldPhysics::Direction::right])
					{
						player.canWallJump = 0.1;
						player.wallJumpDir = -1;
					}

					//Left wall
					if (collider.sidesCollided[oldPhysics::Direction::left])
					{
						player.canWallJump = 0.1;
						player.wallJumpDir = 1;
					}
				}

				//Wall Slide
				rigidbody.velocity.y = max(rigidbody.velocity.y, -1000.f);
			}
			


			//Right
			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] && player.attacking == 0)
			{
				//If touching ground play run and walk sound
				if (collider.sidesCollided[oldPhysics::Direction::down] && player.shouldWallslide <= 0)
				{
					AnimationSystem::PlayAnimation(entity, "Run");
					/*if (!walkSpeaker.isPlaying())
						walkSpeaker.Play(stepSound);*/
				}
				transform.rotation.y = 0;

				if (rigidbody.velocity.x < player.maxSpeed)
				{
					Vector2 impulse;

					//Decelerate faster
					if (rigidbody.velocity.x < 0)
						impulse = Vector2(min(player.accelSpeed * player.reactivityMult, player.maxSpeed - rigidbody.velocity.x), 0);
					else
						impulse = Vector2(min(player.accelSpeed, player.maxSpeed - rigidbody.velocity.x), 0);

					oldPhysics::PhysicsSystem::Impulse(entity, impulse);
				}
			}
			//Left
			else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] && player.attacking == 0)
			{
				//If touching ground play run but mirror it
				if (collider.sidesCollided[oldPhysics::Direction::down] && player.shouldWallslide <= 0)
				{
					AnimationSystem::PlayAnimation(entity, "Run");
					/*if (!walkSpeaker.isPlaying())
						walkSpeaker.Play(stepSound);*/
				}
				transform.rotation.y = 180;

				if (rigidbody.velocity.x > -player.maxSpeed)
				{
					Vector2 impulse;

					//Decelerate faster
					if (rigidbody.velocity.x > 0)
						impulse = Vector2(max(-player.accelSpeed * player.reactivityMult, -player.maxSpeed - rigidbody.velocity.x), 0);
					else
						impulse = Vector2(max(-player.accelSpeed, -player.maxSpeed - rigidbody.velocity.x), 0);

					oldPhysics::PhysicsSystem::Impulse(entity, impulse);
				}
			}
			//No direction
			else
			{
				//If touching ground play idle
				if (collider.sidesCollided[Direction::down] && player.attacking == 0)
					AnimationSystem::PlayAnimation(entity, "Idle");

				//Slow the player down aka friction
				if (abs(rigidbody.velocity.x) > 0)
				{
					//If touching ground decelerate faster
					rigidbody.velocity.x -= (rigidbody.velocity.x / abs(rigidbody.velocity.x) * min(player.accelSpeed, abs(rigidbody.velocity.x)));
				}
			}

			//When jump is pressed
			if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_A] || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]) && player.attacking == 0)
			{
				//If touching ground
				if (collider.sidesCollided[oldPhysics::Direction::down])
				{
					player.jumping = true;
					player.jumpSound = 1;
				}
				else if (player.canWallJump > 0)
				{
					player.wallJumping = true;
					player.jumpSound = 1;
				}

				//Accelerate to max jump speed while holding jump
				if (player.jumping)
				{
					//If ceiling is hit cancel the jump
					if (collider.sidesCollided[oldPhysics::Direction::up])
						player.jumping = false;

					if (rigidbody.velocity.y + player.jumpAccelSpeed < player.maxJumpSpeed)
					{
						oldPhysics::PhysicsSystem::Impulse(entity, Vector2(0, player.jumpAccelSpeed));
					}
					else //Once max speed has been reached
					{
						rigidbody.velocity.y = player.maxJumpSpeed;
						player.jumping = false;
					}
				}

				//Accelerate to max wall jump speed while holding jump
				if (player.wallJumping)
				{
					player.canWallJump = 0;

					//If ceiling is hit cancel the jump
					if (collider.sidesCollided[oldPhysics::Direction::up])
						player.wallJumping = false;

					if (rigidbody.velocity.y + player.wallJumpAccelSpeed < player.maxWallJumpSpeed)
					{
						oldPhysics::PhysicsSystem::Impulse(entity, Vector2(player.wallJumpDir * player.wallJumpAccelSpeed / 1.5, player.wallJumpAccelSpeed));
					}
					else //Once max speed has been reached
					{
						rigidbody.velocity = Vector2(player.wallJumpDir * player.maxWallJumpSpeed / 1.5, player.maxWallJumpSpeed);
						player.wallJumping = false;
					}
				}
				else
				{
					player.wallJumpDir = 0;
				}

				if (!player.jumpHeld)

					player.jumpHeld = true;
			}
			else
			{
				player.wallJumping = false;
				player.jumping = false;
				if (player.jumpHeld)
				{
					//If not touching ground smooth the analog jump apex
					if (!collider.sidesCollided[oldPhysics::Direction::down])
					{
						rigidbody.velocity = Vector2(rigidbody.velocity.x, min(rigidbody.velocity.y, 1500.0f));
					}
					player.jumpHeld = false;
				}
			}

			//If in the air play Jump
			if (!collider.sidesCollided[oldPhysics::Direction::down] && player.shouldWallslide <= 0)
				AnimationSystem::PlayAnimation(entity, "Jump");

			//This is not a good fix
			player.shouldWallslide--;
			if (player.shouldWallslide > 0)
				AnimationSystem::PlayAnimation(entity, "Wallslide");

			//Attack
			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || state.buttons[GLFW_GAMEPAD_BUTTON_B])
			{
				if (collider.sidesCollided[oldPhysics::Direction::down])
				{
					if (!player.attackHeld)
					{
						player.attackSound = 1;
						AnimationSystem::PlayAnimation(entity, "Attack 1");
						player.attacking = 1;
						player.attackHeld = true;
					}
				}
			}
			else
			{
				player.attackHeld = false;
			}

			if (player.canWallJump > 0)
				player.canWallJump -= deltaTime;

			//Lock the max fall speed
			rigidbody.velocity.y = max(rigidbody.velocity.y, -8000.f);



			

		}

	}

	uint32_t jumpSound;
	uint32_t swingSound;
	uint32_t stepSound;
};