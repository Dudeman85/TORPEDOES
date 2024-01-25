#pragma once
#include <chrono>

//ECS modules
#include <engine/ECS.h>	
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

namespace engine
{
	#ifdef _DEBUG
	#ifdef PROJECT_NAME
	const std::string assetPath = PROJECT_NAME;
	#endif

	#else

	#endif

	//Set this to true to not use OpenAL
	bool NO_OPENAL = false;

	float deltaTime = 0;
	float programTime = 0;
	chrono::time_point<chrono::high_resolution_clock> _lastFrame;
	SoundDevice* soundDevice;

	//Engine system pointers (for peak performance)
	shared_ptr<CollisionSystem> collisionSystem;
	shared_ptr<PhysicsSystem> physicsSystem;
	shared_ptr<ModelRenderSystem> modelRenderSystem;
	shared_ptr<AnimationSystem> animationSystem;
	shared_ptr<SpriteRenderSystem> spriteRenderSystem;
	shared_ptr<TextRenderSystem> textRenderSystem;
	shared_ptr<PrimitiveRenderSystem> primitiveRenderSystem;
	shared_ptr<TransformSystem> transformSystem;

	void EngineInit()
	{
		//Init time
		_lastFrame = chrono::high_resolution_clock::now();

		//Make sure OpenGL context has been created
		assert(OPENGL_INITIALIZED && "OpenGL has not been initialized! Create a window, or manually create the OpenGL context before calling EngineInit!");

		//OpenAL can be disabled if desired, no sound will play
		if (!NO_OPENAL)
			soundDevice = SoundDevice::getDevice()->getDevice();

		//Get the engine systems
		collisionSystem = ecs::GetSystem<CollisionSystem>();
		physicsSystem = ecs::GetSystem<PhysicsSystem>();
		modelRenderSystem = ecs::GetSystem<ModelRenderSystem>();
		animationSystem = ecs::GetSystem<AnimationSystem>();
		spriteRenderSystem = ecs::GetSystem<SpriteRenderSystem>();
		textRenderSystem = ecs::GetSystem<TextRenderSystem>();
		primitiveRenderSystem = ecs::GetSystem<PrimitiveRenderSystem>();
		transformSystem = ecs::GetSystem<TransformSystem>();
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
		chrono::duration<double> duration = thisFrame - _lastFrame;
		deltaTime = duration.count();
		_lastFrame = thisFrame;
		programTime += deltaTime;

		return deltaTime;
	}
}