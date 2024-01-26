#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <filesystem>

#include <engine/Constants.h>
#include "engine/AudioEngine.h"

using namespace std::filesystem;

AudioEngine::AudioEngine(float volume)
{
	engineConfig = ma_engine_config_init();
	engineConfig.listenerCount = 1;
	auto aresult = ma_engine_init(&engineConfig, &soundEngine);
	if (aresult != MA_SUCCESS)
	{
		throw std::runtime_error("Warning: Failed to initialize an audio engine");
	}
	ambientVolume = volume;
	ma_engine_set_volume(&soundEngine, volume);
}
AudioEngine::~AudioEngine()
{
	ma_engine_uninit(&soundEngine);
}

void AudioEngine::setAmbientVolume(float newVolume)
{
	ambientVolume = newVolume;
	ma_engine_set_volume(&soundEngine, ambientVolume);
}
const float AudioEngine::getAmbientVolume()
{
	return ambientVolume;
}

Audio* AudioEngine::createAudio(std::string soundName, bool loop, float volume, Vector3 location)
{
	Audio* newAudio = new Audio(this, soundName, loop, volume, location);
	newAudio->play();
	return newAudio;
}

Audio::Audio(AudioEngine* owningEngine, std::string soundName, bool loop, float volume, Vector3 location)
{
	owner = owningEngine;
	setSound(soundName);
	setLoop(loop);
	setVolume(volume);
	setDirection(location);
}

Audio::~Audio()
{
	ma_sound_uninit(audioSound);
}

void Audio::play()
{
	auto resultp = ma_sound_seek_to_pcm_frame(audioSound, 0);
	if (resultp != MA_SUCCESS)
	{
		throw std::runtime_error("Warning: Failed to seek first sound frame");
	}
	auto result = ma_sound_start(audioSound);
	if (result != MA_SUCCESS)
	{
		throw std::runtime_error("Warning: Failed to start sound");
	}
}
void Audio::pause()
{
	auto result = ma_sound_stop(audioSound);
	if (result != MA_SUCCESS)
	{
		throw std::runtime_error("Waning: Failed to pause sound");
	}
}
void Audio::unpause()
{
	if (!getAtEnd())
	{ 
		ma_sound_start(audioSound);
	}
}

void Audio::stop()
{
	auto result = ma_sound_stop(audioSound);
	if (result != MA_SUCCESS)
	{
		throw std::runtime_error("Warning: Failed to stop sound");
	}
	delete this;
}
const bool Audio::getPlaying()
{
	return ma_sound_is_playing(audioSound);
}

void Audio::setSound(const std::string& fileName)
{
	audioSound = new ma_sound;
	std::string filename = assetPath + fileName;
	auto result = ma_sound_init_from_file(&owner->soundEngine, filename.c_str(), 0, NULL, NULL, audioSound);
	if (result != MA_SUCCESS)
	{
		throw std::runtime_error("Failed to read audio file");
	}
}

void Audio::setVolume(float newVolume)
{
	if (newVolume < 0) { newVolume = 0; }
	ma_sound_set_volume(audioSound, newVolume);
}
const float Audio::getVolume()
{
	return ma_sound_get_volume(audioSound);
}
void Audio::addVolume(float addVolume)
{
	float Volume = ma_sound_get_volume(audioSound) + addVolume;
	if (Volume < 0) { Volume = 0; }
	ma_sound_set_volume(audioSound, Volume);
}
void Audio::setLoop(bool loop)
{
	ma_sound_set_looping(audioSound, loop);
}
const bool Audio::getLoop()
{
	return ma_sound_is_looping(audioSound);
}
const bool Audio::getAtEnd()
{
	return ma_sound_at_end(audioSound);
}
void Audio::setDirection(Vector3 Location)
{
	ma_sound_set_pinned_listener_index(audioSound, 1);
	ma_sound_set_positioning(audioSound, ma_positioning_relative);
	ma_sound_set_position(audioSound, Location.x, Location.y, Location.z);
}
const Vector3 Audio::getDirection()
{
	ma_vec3f direction = ma_sound_get_position(audioSound);
	return Vector3(direction.x, direction.y, direction.z);
}
