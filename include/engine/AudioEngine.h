#pragma once

#include <engine/Vector.h>
#include "miniaudio.h"

class AudioEngine;

using engine::Vector3;

class Audio
{
public:
	Audio(AudioEngine* owningEngine, std::string soundName, bool loop, float volume, Vector3 location);
	~Audio();

	void play();									// Starts audio from beginning
	void pause();									// Pauses audio, can be continued
	void unpause();									// Continues audio from pause
	void stop();									// Stops the audio completely
	const bool getPlaying();						// Gets audio playing status

	void setSound(const std::string& fileName);		// Sets audio file to play
	void setVolume(float NewVolume);				// Sets audio volume
	const float getVolume();						// Gets audio volume
	void addVolume(float AddVolume);				// Adds audio volume
	void setLoop(bool Loop);						// Sets audio looping status
	const bool getLoop();							// Gets audio looping status
	const bool getAtEnd();							// Gets audio completion status
	void setDirection(Vector3 Location);			// Sets the absolute direction the sound is coming from headphone's perspective, distance lowers volume
	const Vector3 getDirection();					// Gets the absolute direction the sound is coming from headphone's perspective, distance lowers volume

private:
	AudioEngine* owner;								// AudioEngine which owns this audio TODO: Remove
	ma_sound* audioSound = nullptr;
};

class AudioEngine
{
public:
	AudioEngine(float ambientVolume = 1);
	~AudioEngine();

	Audio* createAudio(std::string soundName, bool loop = false, float volume = 1, Vector3 location = {0,0,0}); // Plays an audio with selected audio file, returns a pointer to the audio. Non-looping audios are removed upon finishing.
	
	void setAmbientVolume(float newVolume);			// Sets the ambient volume
	const float getAmbientVolume();					// Gets the ambient volume

	ma_engine soundEngine;							// miniaudio engine

private:
	float ambientVolume;							// Engine's volume, decrease to affect all audios played from it
	ma_engine_config engineConfig;					
};
