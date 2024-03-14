#pragma once
#include <engine/Application.h>
#include <functional>
#include <engine/Input.h>  
#include "PlayerController.h"
//Author: Sauli Hanell month.day.year 2.22.2024

enum  shipTypes { LaMuerte, pt_10, u_99, Hedgehog };


ECS_REGISTER_COMPONENT(U_99Component)
struct U_99Component
{
	float projectileTimer;
	float cooldown = 2;
};

ECS_REGISTER_SYSTEM(U_99System, U_99Component, Transform)
class U_99System : public ecs::System
{

public:
	void Update()
	{
		for (auto itr = entities.begin(); itr != entities.end();)
		{
			//Get the entity and increment the iterator
			ecs::Entity entity = *itr++;

			U_99Component& u_99Component = ecs::GetComponent<U_99Component>(entity);


			if (u_99Component.cooldown < u_99Component.projectileTimer)
			{
				u_99Component.projectileTimer = 0;
				ShootProjectile();
			}
			else
			{
				u_99Component.projectileTimer += deltaTime;
				std::cout << " projectile cooldown " << u_99Component.projectileTimer << " \n";
			}
		}

	}

	void ShootProjectile()
	{
		std::cout << "shoot projectile \n";
	}



};


class MyClass : public ecs::System
{
};

ECS_REGISTER_COMPONENT(Pt_10Component, Texture)
struct Pt_10Component
{
	float projectileTimer;
	float cooldown = 2;
};

ECS_REGISTER_SYSTEM(Pt_10System, Pt_10Component, Transform)
class Pt_10System : public MyClass
{
public:
	void Update()
	{
		for (auto itr = entities.begin(); itr != entities.end();)
		{
			//Get the entity and increment the iterator
			ecs::Entity entity = *itr++;

			Pt_10Component& pt_10Component = ecs::GetComponent<Pt_10Component>(entity);
			Player& player = ecs::GetComponent<Player>(entity);

			if (pt_10Component.cooldown < pt_10Component.projectileTimer && input::GetNewPress("Shoot" + std::to_string(player.id)))
			{
				pt_10Component.projectileTimer = 0;
				ShootProjectile();
			}
			else
			{
				pt_10Component.projectileTimer += deltaTime;
				std::cout << " projectile cooldown " << pt_10Component.projectileTimer << " \n";
			}
		}

	}

	void ShootProjectile()
	{
		std::cout << "shoot projectile \n";
	}
};

ECS_REGISTER_COMPONENT(LaMuerteComponent, Texture)
struct LaMuerteComponent
{
	float projectileTimer;
	float cooldown = 2;
};

ECS_REGISTER_SYSTEM(LaMuerteSystem, LaMuerteComponent, Transform)
class LaMuerteSystem : public ecs::System
{
public:

	void Update()
	{
		for (auto itr = entities.begin(); itr != entities.end();)
		{
			//Get the entity and increment the iterator
			ecs::Entity entity = *itr++;

			LaMuerteComponent& laMuerteComponent = ecs::GetComponent<LaMuerteComponent>(entity);

			if (laMuerteComponent.cooldown < laMuerteComponent.projectileTimer)
			{
				laMuerteComponent.projectileTimer = 0;
				ShootProjectile();
			}
			else
			{
				laMuerteComponent.projectileTimer += deltaTime;
				std::cout << " projectile cooldown " << laMuerteComponent.projectileTimer << " \n";
			}

		}
	}

	void ShootProjectile()
	{
		std::cout << "shoot projectile \n";
	}
};


ECS_REGISTER_COMPONENT(HedgehogComponent)
struct HedgehogComponent
{
	float projectileTimer;
	float cooldown = 2;
};

ECS_REGISTER_SYSTEM(HedgehogSystem, HedgehogComponent, Transform)
class HedgehogSystem : public ecs::System
{

	~HedgehogSystem() {
		/*	delete ;*/

	}
public:
	void Update(HedgehogComponent& hedgehog)
	{

		if (hedgehog.cooldown < hedgehog.projectileTimer)
		{
			hedgehog.projectileTimer = 0;
			ShootProjectile();
		}
		else
		{
			hedgehog.projectileTimer += deltaTime;
			std::cout << " projectile cooldown " << hedgehog.projectileTimer << " \n";
		}


	}

	void ShootProjectile()
	{
		std::cout << "shoot projectile \n";
	}


};


