#pragma once
#define WIN32_LEAN_AND_MEAN
#include <chrono>

//ECS modules
#include <engine/ECS.h>	
#include <engine/Sprite.h>
#include <engine/ModelSystem.h>
#include <engine/Transform.h>
#include <engine/Physics.h>
#include <engine/Collision.h>
#include <engine/TextRender.h>
#include <engine/Primitive.h>

//Other engine libs
#include <engine/GL/Window.h>
#include <engine/Image.h>
#include <engine/AudioEngine.h>

namespace engine
{
	// TODO: replace with double
	float deltaTime = 0;
	// TODO: replace with double
	float programTime = 0;
	chrono::time_point<chrono::high_resolution_clock> _lastFrame;

	//If true updates physics and collision systems
	bool enablePhysics = true;
	//If true updates animation system
	bool enableAnimation = true;
	//If true enables sprite, model, text, and primitive rendering systems
	bool enableRendering = true;

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

		//Get the engine systems
		collisionSystem = ecs::GetSystem<CollisionSystem>();
		physicsSystem = ecs::GetSystem<PhysicsSystem>();
		modelRenderSystem = ecs::GetSystem<ModelRenderSystem>();
		modelRenderSystem->Init();
		animationSystem = ecs::GetSystem<AnimationSystem>();
		spriteRenderSystem = ecs::GetSystem<SpriteRenderSystem>();
		spriteRenderSystem->Init();
		textRenderSystem = ecs::GetSystem<TextRenderSystem>();
		textRenderSystem->Init();
		primitiveRenderSystem = ecs::GetSystem<PrimitiveRenderSystem>();
		primitiveRenderSystem->Init();
		transformSystem = ecs::GetSystem<TransformSystem>();
		ecs::SetComponentDestructor<Transform>(TransformSystem::OnTransformRemoved);
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
		if (enablePhysics)
			physicsSystem->Update(deltaTime);
		//Animation must be before sprite rendering
		if (enableAnimation)
			animationSystem->Update(deltaTime);
		if (enableRendering)
		{
			//ModelRenderer must be before SpriteRenderer
			modelRenderSystem->Update(cam);
			spriteRenderSystem->Update(cam);
			textRenderSystem->Update(cam);
			primitiveRenderSystem->Update(cam);
		}
		//Collision system should be after rendering
		if (enablePhysics)
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