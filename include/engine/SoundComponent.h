#pragma once

#include "engine/AudioEngine.h"
#include "engine/Component.h"
#include "engine/ECSCore.h"

#include "vector"

// We don't need a soundcomponent: We only need to know each soundComponent's location (audio shouldn't be a component)
// Only component we need is listeningcomponent, so we can set listening direction to be in relation to it.
// Except we don't need that either, we can just set a single listening position in SoundSystem

// TL;DR: We only need a single SoundSystem, with a multimap of sound locations and their positions, 
// as well as the listening position.

ECS_COMPONENT(SoundComponent)
{
	// unnecessary
};

struct SoundSystem : public System
{
private:
	// variables:
	
	// multimap of sound locations and their positions (audio, positioncomponent)
	// listening position (positioncomponent)
	// list of sound engines (audioengine)

public:

	// functions:
	
	// add/remove/get sound engine
	// set/get sound engine settings

	// add/remove/find sound (asynchronous/fence not required)
	// set/get sound settings

	// if a sound/listening position updates: change all audio component's direction
};
