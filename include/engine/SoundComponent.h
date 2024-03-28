#pragma once

#include "engine/GL/Window.h"
#include "engine/ECS.h"
#include "engine/AudioEngine.h"
#include "engine/Transform.h"

#include "vector"

// We don't need a soundcomponent: We only need to know each soundComponent's location (audio shouldn't be a component)
// Only component we need is listeningcomponent, so we can set listening direction to be in relation to it.
// Except we don't need that either, we can just set a single listening position in SoundSystem

// TL;DR: We only need a single SoundSystem, with a multimap of sound locations and their positions, 
// as well as the listening position.

ECS_REGISTER_COMPONENT(SoundComponent)
struct SoundComponent
{
	Audio* Sound;
};

// Can't be arsed to implement this, we only have 1 position to listen from anyway
ECS_REGISTER_COMPONENT(ListeningComponent)
struct ListeningComponent
{


};

// Player controller System. Requires Player , Tranform , Rigidbody , PolygonCollider
ECS_REGISTER_SYSTEM(SoundSystem, SoundComponent, engine::Transform)
class SoundSystem : public engine::ecs::System
{
private:
	// variables:
	
	// multimap of sound locations and their positions (audio, positioncomponent)

	// listening position (positioncomponent)
	// list of sound engines (audioengine)

public:

	/// PlayerController Update 
	void Update(GLFWwindow* window, float dt)
	{
	
	}

	// functions:
	
	// add/remove/get sound engine
	// set/get sound engine settings

	// add/remove/find sound (asynchronous/fence not required)
	// set/get sound settings

	// if a sound/listening position updates: change all audio component's direction
};
