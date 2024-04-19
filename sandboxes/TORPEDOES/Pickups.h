#pragma once
#include <engine/ECS.h>

using namespace engine;

struct PickupComponent
{

};

class PickupSystem : public ecs::System
{
public:
	void Update() 
	{
		//For each entity
		for (ecs::Entity entity : entities)
		{

		}
	}

	void SpawnPickup() 
	{

	}
};
