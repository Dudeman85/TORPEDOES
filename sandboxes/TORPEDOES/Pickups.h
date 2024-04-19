#pragma once
#include <engine/ECS.h>
#include <engine/Vector.h>
#include <engine/ModelSystem.h>
#include "Resources.h"

using namespace engine;

ECS_REGISTER_COMPONENT(PickupComponent)
struct PickupComponent
{
	bool respawn = true;
};

ECS_REGISTER_SYSTEM(PickupSystem, PickupComponent)
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

	void SpawnPickup(Vector3 position)
	{
		ecs::Entity pickup = ecs::NewEntity();
		ecs::AddComponent(pickup, ModelRenderer{ .model = resources::models["Prop_PowerUpBox2.obj"] });
	}
};
