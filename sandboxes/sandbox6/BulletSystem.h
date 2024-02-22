#pragma once
#include <engine/Application.h>

//Bullet Component
ECS_REGISTER_COMPONENT(Bullet)
struct Bullet 
{
	float lifeTime = 5;
};

ECS_REGISTER_SYSTEM(BulletSystem, Bullet, Transform)
class BulletSystem : public ecs::System
{
public:
	void Update()
	{
		// Iterate through entities in the system
		for (auto itr = entities.begin(); itr != entities.end();)
		{
			//Get the entity and increment the iterator
			ecs::Entity entity = *itr++;

			Bullet& bulletComponent = ecs::GetComponent<Bullet>(entity);

			if (bulletComponent.lifeTime < 0)
			{
				ecs::DestroyEntity(entity);
			}
			else
			{
				bulletComponent.lifeTime -= deltaTime;
			}
		}
	}
};