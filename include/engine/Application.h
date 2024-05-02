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
#include <engine/SoundComponent.h>

//Other engine libs
#include <engine/GL/Window.h>
#include <engine/Image.h>
#include <engine/AudioEngine.h>
#include <engine/Timing.h>

namespace engine
{
	//If true updates physics and collision systems
	bool enablePhysics = true;
	//If true updates animation system
	bool enableAnimation = true;
	//If true enables sprite, model, text, and primitive rendering systems
	bool enableRendering = true;

	//Engine system pointers (for peak performance)
	shared_ptr<TimerSystem> timerSystem;
	shared_ptr<CollisionSystem> collisionSystem;
	shared_ptr<PhysicsSystem> physicsSystem;
	shared_ptr<ModelRenderSystem> modelRenderSystem;
	shared_ptr<AnimationSystem> animationSystem;
	shared_ptr<SpriteRenderSystem> spriteRenderSystem;
	shared_ptr<TextRenderSystem> textRenderSystem;
	shared_ptr<PrimitiveRenderSystem> primitiveRenderSystem;
	shared_ptr<TransformSystem> transformSystem;
	shared_ptr<SoundSystem> soundSystem;

	void EngineInit()
	{
		//Make sure OpenGL context has been created
		assert(OPENGL_INITIALIZED && "OpenGL has not been initialized! Create a window, or manually create the OpenGL context before calling EngineInit!");

		//Get the engine systems
		timerSystem = ecs::GetSystem<TimerSystem>();
		timerSystem->Init();
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
		soundSystem = ecs::GetSystem<SoundSystem>();
		transformSystem = ecs::GetSystem<TransformSystem>();
		ecs::SetComponentDestructor<Transform>(TransformSystem::OnTransformRemoved);
	}

	//Updates all default engine systems, returns delta time
	double Update(Camera* cam)
	{
		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Update engine systems
		//Physics must be before collision
		if (enablePhysics)
			physicsSystem->Update();
		//Animation must be before sprite rendering
		if (enableAnimation)
			animationSystem->Update();
		if (enableRendering)
		{
			//ModelRenderer must be before SpriteRenderer
			modelRenderSystem->Update(cam);
			spriteRenderSystem->Update(cam);
			modelRenderSystem->DrawUIElements(cam); //This is a bandaid patch for UI models
			textRenderSystem->Update(cam);
			primitiveRenderSystem->Update(cam);
		}
		//sound Update
		soundSystem->Update();

		//Collision system should be after rendering
		if (enablePhysics)
			collisionSystem->Update(cam);
		//Transform must be after physics, collision and rendering
		transformSystem->Update();
		//Timer must be last
		timerSystem->Update();

		return deltaTime;
	}
}