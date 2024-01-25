#pragma once
#include <engine/Application.h>
#include "PickupController.h";

extern ECS ecs;

using namespace engine;

struct Scoreboard
{
	int victoryCondition = 0;
};

class ScoreController : public System
{
public:
	ScoreController(int& ref, const char* path, int spritesWide, int spritesHigh, int delays, glm::vec3 pos = glm::vec3(0.f, 0.f, 0.f)) : collectedRef(ref)
	{
		int spriteSize = spritesWide * spritesHigh;
		anims = AnimationsFromSpritesheet(path, spritesWide, spritesHigh, vector<int>(spriteSize, delays));
		position = pos;
	};

	void Update(Entity player)
	{
		AnimationSystem::AddAnimations(scoreboard, anims, vector<string>{"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Victory"});
		for (auto const& entity : entities)
		{
			BoxCollider& collider = ecs.getComponent<BoxCollider>(entity);

			if (collider.collisions.end() == find_if(collider.collisions.begin(), collider.collisions.end(), [player](const Collision& collision) { return collision.a == player; }))
			{
				collectedRef = PickupController().collected;
				if (collectedRef < anims.size())
				{
					string animation = to_string(collectedRef);
					AnimationSystem::PlayAnimation(scoreboard, animation);
				}
				break;
			}
		}
	}

	Entity CreateScoreboard(float x, float y)
	{
		scoreboard = ecs.newEntity();
		ecs.addComponent(scoreboard, Transform{ .x = x, .y = y, .xScale = 40, .yScale = 20 });
		ecs.addComponent(scoreboard, Sprite{});
		ecs.addComponent(scoreboard, Animator{});
		ecs.addComponent(scoreboard, Scoreboard{});

		return scoreboard;
	}

	void SetPosition(float x, float y, float z)
	{
		position.x = x / 2;
		position.y = y / 2;
		position.z = z / 2;
	}

	glm::vec3 position;
	Entity scoreboard;
	vector<Animation> anims;
	int& collectedRef;
	const char* path;
};