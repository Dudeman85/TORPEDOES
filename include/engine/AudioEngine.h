#pragma once

#include <engine/Vector.h>
#include "miniaudio.h"

class AudioEngine;

///Class to create and store audio data
class Audio
{
public:
	///Constructor
	Audio(AudioEngine* owningEngine, std::string soundName, bool loop, float volume, Vector3 location);
	~Audio();
	///Starts audio from beginning
	void play();		
	///Pauses audio, can be continued
	void pause();	
	///Continues audio from pause
	void unpause();			
	///Stops the audio completely
	void stop();	
	///Gets audio playing status
	const bool getPlaying();						
	///Sets audio file to play
	void setSound(const std::string& fileName);		
	///Sets audio volume
	void setVolume(float NewVolume);	
	///Gets audio volume
	const float getVolume();	
	///Adds audio volume
	void addVolume(float AddVolume);		
	///Sets audio looping status
	void setLoop(bool Loop);		
	///Gets audio looping status
	const bool getLoop();	
	///Gets audio completion status
	const bool getAtEnd();	
	///Sets the absolute direction the sound is coming from headphone's perspective, distance lowers volume
	void setDirection(Vector3 Location);	
	///Gets the absolute direction the sound is coming from headphone's perspective, distance lowers volume
	const Vector3 getDirection();					

private:
	AudioEngine* owner;								// AudioEngine which owns this audio TODO: Remove
	ma_sound* audioSound = nullptr;
};

///Class to store audio engine data
class AudioEngine
{
public:
	///Constructor
	AudioEngine(float ambientVolume = 1);
	~AudioEngine();
	///Plays an audio with selected audio file, returns a pointer to the audio. Non-looping audios are removed upon finishing.
	Audio* createAudio(std::string soundName, bool loop = false, float volume = 1, Vector3 location = {0,0,0}); // Plays an audio with selected audio file, returns a pointer to the audio. Non-looping audios are removed upon finishing.
	///Sets the ambient volume
	void setAmbientVolume(float newVolume);
	///Gets the ambient volume
	const float getAmbientVolume();					
	///miniaudio engine
	ma_engine soundEngine;							

private:
	float ambientVolume;							// Engine's volume, decrease to affect all audios played from it
	ma_engine_config engineConfig;					
};
