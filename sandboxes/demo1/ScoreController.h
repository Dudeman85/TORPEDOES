#pragma once
#include <engine/Application.h>
#include "PickupController.h";

extern ECS ecs;

using namespace engine;

struct Scoreboard
{
	const char* path = NULL;
	int spritesWide = 0;
	int spritesHigh = 0;
	int delays = 0;
	glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);
	int victoryCondition = 0;
};

class ScoreController : public System
{
public:
	ScoreController(int& ref, const Scoreboard& sb) : collectedRef(ref), scoreboard(sb)
	{
		int spriteSize = sb.spritesWide * sb.spritesHigh;
		anims = AnimationsFromSpritesheet(sb.path, sb.spritesWide, sb.spritesHigh, vector<int>(spriteSize, sb.delays));
	};

	void Update(Entity player)
	{
		AnimationSystem::AddAnimations(scoreboardEntity, anims, vector<string>{"0", "1"});
		for (auto const& entity : entities)
		{
			collectedRef = PickupController().collected;
			do
			{
				if (collectedRef < anims.size())
				{
					string animation = to_string(collectedRef);
					AnimationSystem::PlayAnimation(scoreboardEntity, animation);
				}

			} while (collectedRef != scoreboard.victoryCondition);
		}
	}

	Entity CreateScoreboard(float x, float y)
	{
		scoreboardEntity = ecs.newEntity();
		ecs.addComponent(scoreboardEntity, Transform{ .x = x, .y = y, .xScale = 40, .yScale = 20 });
		ecs.addComponent(scoreboardEntity, Sprite{});
		ecs.addComponent(scoreboardEntity, Animator{});
		ecs.addComponent(scoreboardEntity, Scoreboard{});

		return scoreboardEntity;
	}

	void SetPosition(float x, float y, float z)
	{
		position.x = x / 2;
		position.y = y / 2;
		position.z = z / 2;
	}

	Entity scoreboardEntity;
	vector<Animation> anims;
	int& collectedRef;
	Scoreboard scoreboard;
};