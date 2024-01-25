#pragma once
#include "Physics.h"
#include <engine/Application.h>
#include <cmath>

extern ECS ecs;

using namespace engine;

//Player Component
struct Player
{
	int playerId;
	float moveSpeed = 500.f;
	
};

//Player Controller requires Player, Sprite, Transform, BoxCollider, Rigidbody
class PlayerController : public System
{
public:
	PlayerController() {}

	void Update(GLFWwindow* window, double deltaTime, shared_ptr<oldPhysics::PhysicsSystem> physicsSystem)
	{
		for (auto const& entity : entities)
		{
			Player& player = ecs.getComponent<Player>(entity);
			oldPhysics::Rigidbody& rigidbody = ecs.getComponent<oldPhysics::Rigidbody>(entity);
			oldPhysics::BoxCollider& collider = ecs.getComponent<oldPhysics::BoxCollider>(entity);

			std::string input = "";

			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				input = "MOVE_RIGHT";
				physicsSystem->Move(entity, Vector2(player.moveSpeed, 0) * deltaTime);
			}
			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				input = "MOVE_LEFT";
				physicsSystem->Move(entity, Vector2(-player.moveSpeed, 0) * deltaTime);
			}
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			{
				input = "MOVE_UP";
				physicsSystem->Move(entity, Vector2(0, player.moveSpeed) * deltaTime);
			}
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			{
				input = "MOVE_DOWN";
				physicsSystem->Move(entity, Vector2(0, -player.moveSpeed) * deltaTime);
			}
			
			
		}
	}
};