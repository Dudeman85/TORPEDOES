#pragma once
#include <chrono>

//ECS modules
#include <engine/ECSCore.h>	
#include <engine/Sprite.h>
#include <engine/Model.h>
#include <engine/Transform.h>
#include <engine/Physics.h>
#include <engine/Collision.h>
#include <engine/UserInterface.h>
#include <engine/TextRender.h>
#include <engine/Primitive.h>

//Other engine libs
#include <engine/GL/Window.h>
#include <engine/Image.h>
#include <engine/AL/SoundDevice.h>
#include <engine/AL/SoundSource.h>
#include <engine/AL/SoundBuffer.h>
#include <engine/AL/MusicBuffer.h>

using namespace std;

extern ECS ecs;

namespace engine
{
	//Set this to true to not use OpenAL
	bool NO_OPENAL = false;

	float ENGINE_DT = 0;
	float ENGINE_PT = 0;

	//A class to store all the default engine modules
	class EngineLib
	{
	public:
		double deltaTime = 0;
		double programTime = 0;

		SoundDevice* soundDevice;

		// TODO: Unnecessary: Each system should be a singleton, and add 
		// it's initialization into the ECS_REQUIRED_COMPONENTS - macro
		shared_ptr<TransformSystem> transformSystem;
		shared_ptr<SpriteRenderSystem> spriteRenderSystem;
		shared_ptr<ModelRenderSystem> modelRenderSystem;
		shared_ptr<AnimationSystem> animationSystem;
		shared_ptr<CollisionSystem> collisionSystem;
		shared_ptr<PhysicsSystem> physicsSystem;
		shared_ptr<TextRenderSystem> textRenderSystem;
		shared_ptr<PrimitiveRenderSystem> primitiveRenderSystem;

		EngineLib()
		{
			//Init time
			lastFrame = chrono::high_resolution_clock::now();

			//Make sure OpenGL context has been created
			assert(OPENGL_INITIALIZED && "OpenGL has not been initialized! Create a window, or manually create the OpenGL context before initializing EngineLib!");

			//OpenAL can be disabled if desired, no sound will play
			if (!NO_OPENAL)
				soundDevice = SoundDevice::getDevice()->getDevice();

			//Register all default engine systems
			SystemManager::getInstance().setAllSignatures();

			// TODO: remove these

			//Transform System
			transformSystem = ecs.registerSystem<TransformSystem>();

			//Sprite Render System
			spriteRenderSystem = ecs.registerSystem<SpriteRenderSystem>();

			//Model Render System
			modelRenderSystem = ecs.registerSystem<ModelRenderSystem>();

			//Animation System
			animationSystem = ecs.registerSystem<AnimationSystem>();

			//Collision System
			collisionSystem = ecs.registerSystem<CollisionSystem>();

			//Physics System
			physicsSystem = ecs.registerSystem<PhysicsSystem>();
			// We don't even need this if we had singletons
			PhysicsSystem::collisionSystem = collisionSystem;

			//TextRender System
			textRenderSystem = ecs.registerSystem<TextRenderSystem>();

			//Primitive System
			primitiveRenderSystem = ecs.registerSystem<PrimitiveRenderSystem>();
		}

		//Updates all default engine systems, calculates and returns delta time
		double Update(Camera* cam)
		{
			//TODO: Make better
			collisionSystem->cam = cam;

			//Clear the screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//Update engine systems
			//Physics must be before collision
			physicsSystem->Update(deltaTime);
			//Animation must be before sprite rendering
			modelRenderSystem->Update(cam);
			animationSystem->Update(deltaTime);
			spriteRenderSystem->Update(cam);
			textRenderSystem->Update(cam);
			primitiveRenderSystem->Update(cam);
			//Collision system should be after rendering 
			collisionSystem->Update();
			//Transform must be after physics and rendering
			transformSystem->Update();

			//Calculate Delta Time
			chrono::time_point thisFrame = chrono::high_resolution_clock::now();
			chrono::duration<double> duration = thisFrame - lastFrame;
			deltaTime = duration.count();
			lastFrame = thisFrame;
			programTime += deltaTime;

			ENGINE_DT = deltaTime;
			ENGINE_PT = programTime;

			return deltaTime;
		}

		static double GetDeltaTime() 
		{
			return ENGINE_DT;
		}
		static double GetProgramTime() 
		{
			return ENGINE_PT;
		}

	private:
		chrono::time_point<chrono::high_resolution_clock> lastFrame;
	};
}