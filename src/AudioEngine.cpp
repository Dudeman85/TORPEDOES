#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <filesystem>

#include <engine/Constants.h>
#include "engine/AudioEngine.h"
#include <cmath>

using namespace std::filesystem;

AudioEngine::AudioEngine(float volume)
{
	engineConfig = ma_engine_config_init();
	engineConfig.listenerCount = 1; 
	if (ma_engine_init(&engineConfig, &soundEngine) != MA_SUCCESS)
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
	setAbsoluteDirection(location);
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
	Volume = std::max(newVolume, 0.f);


}
void Audio::absoluteVolume(float absVolume)
{
	ma_sound_set_volume(audioSound, std::max(absVolume, 0.f));
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
void Audio::setAbsoluteDirection(Vector3 Location)
{
	ma_sound_set_pinned_listener_index(audioSound, 1);
	ma_sound_set_positioning(audioSound, ma_positioning_relative);
	ma_sound_set_position(audioSound, Location.x, Location.y, Location.z);

	ma_sound_set_min_distance(audioSound, 0);
	ma_sound_set_max_distance(audioSound, 0);
}
const Vector3 Audio::getAbsoluteDirection()
{
	ma_vec3f direction = ma_sound_get_position(audioSound);
	return Vector3(direction.x, direction.y, direction.z);
}
void Audio::setPitch(float newPitch)
{
	ma_sound_set_pitch(audioSound, newPitch);
}

void AudioEngine::setListenerPosition(Vector3 position)
{
	ma_engine_listener_set_position(&soundEngine, 0, position.x, position.y, position.z);
}

void Audio::setDistanceModel(DistanceModel model)
{
	switch (model)
	{
	case DistanceModel::LINEAR:
		ma_sound_set_attenuation_model(audioSound, ma_attenuation_model_linear);
		break;
	case DistanceModel::INVERSE:
		ma_sound_set_attenuation_model(audioSound, ma_attenuation_model_inverse);
		break;
	case DistanceModel::EXPONENTIAL:
		ma_sound_set_attenuation_model(audioSound, ma_attenuation_model_exponential);
		break;
	case DistanceModel::NONE:
		ma_sound_set_attenuation_model(audioSound, ma_attenuation_model_none);
		break;
	}
}

void Audio::setMaxDistance(float maxDist)
{
	ma_sound_set_max_distance(audioSound, maxDist);
}

void Audio::setReferenceDistance(float refDist)
{
	ma_sound_set_min_distance(audioSound, refDist);
}

void Audio::setRolloffFactor(float rolloff)
{
	// Setting rolloff factor would vary based on the exact implementation, 
	// as miniaudio doesn’t directly expose this. 
	// You might need to customize the attenuation equations or use a library function if available.
	float rolloffFactor = rolloff; // Store for any custom calculations.
}

void Audio::setStartTimeMilliseconds(float milliseconds)
{
	ma_sound_set_start_time_in_milliseconds(audioSound, milliseconds);
}

void Audio::setStopTimeMilliseconds(float milliseconds)
{
	ma_sound_set_stop_time_in_milliseconds(audioSound, milliseconds);
}