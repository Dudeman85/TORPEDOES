#pragma once

#include "engine/ECS.h"
#include "engine/AudioEngine.h"
#include "engine/Transform.h"

#include "vector"

namespace engine
{
	

	ECS_REGISTER_COMPONENT(SoundComponent)
	struct SoundComponent
	{
		std::map<std::string, Audio*> Sounds;
		DistanceModel distanceModel = DistanceModel::LINEAR; // default
		float maxDistance = 1500.0f;
		float referenceDistance = 200.0f;
		float rolloffFactor = 1.0f;
		float baseVolume = 1.0f;

		std::unordered_map<std::string, float> startDelays; // Delay in milliseconds for each sound
		std::unordered_map<std::string, float> stopDelays;  // Stop time in milliseconds for each sound
	};

	// we only have 1 position to listen from
	ECS_REGISTER_COMPONENT(ListeningComponent)
	struct ListeningComponent
	{

	};


	// Player controller System
	ECS_REGISTER_SYSTEM(SoundSystem, SoundComponent, engine::Transform)
	class SoundSystem : public engine::ecs::System
	{
	private:
		std::map<std::string, AudioEngine*> AudioEngines;

	public:
		Vector3 ListeningPosition;


		void Update()
		{
			SetListeningPosition(ListeningPosition);

			// Iterate through entities in the system
			for (engine::ecs::Entity entity : entities)
			{
				// Get necessary components
				SoundComponent& soundComponent = engine::ecs::GetComponent<SoundComponent>(entity);
				engine::Transform& soundTransform = engine::ecs::GetComponent<engine::Transform>(entity);

				for (auto& sound : soundComponent.Sounds)
				{
					

					//calculate distance between listener and audio source
					float distance = (ListeningPosition - soundTransform.position).Length();

					DistanceModel model = soundComponent.distanceModel;

					float attenuation = 1.0f;

					if (distance <= soundComponent.referenceDistance) {
						attenuation = 1.0f;
					}
					else {
						switch (model)
						{
						case DistanceModel::LINEAR:
							attenuation = std::max(0.0f, 1.0f - (distance / soundComponent.maxDistance));  // Linear attenuation
							break;

						case DistanceModel::INVERSE:
							attenuation = 1.0f / (1.0f + soundComponent.rolloffFactor * (distance - soundComponent.referenceDistance));  // Inverse attenuation
							break;

						case DistanceModel::EXPONENTIAL:
							attenuation = pow(0.5f, (distance - soundComponent.referenceDistance) / soundComponent.rolloffFactor);  // Exponential attenuation
							break;
						case DistanceModel::NONE:
							break;
						}
					}
					// Set the audio volume based on the calculated attenuation
					float attenuatedVolume = (soundComponent.baseVolume * sound.second->Volume * attenuation);
					sound.second->absoluteVolume(std::clamp(attenuatedVolume, 0.0f, soundComponent.baseVolume));

					sound.second->setAbsoluteDirection(soundTransform.position - ListeningPosition);


				}
				
			}
		}

		AudioEngine* AddSoundEngine(std::string soundEngineName, float ambientVolume = 1)
		{
			auto it = AudioEngines.find(soundEngineName);
			if (it != AudioEngines.end())
			{
				std::cout << "Warning: Sound engine with name " << soundEngineName << " already exists.\n";
				return nullptr;
			}

			AudioEngine* NewAudioEngine = new AudioEngine(ambientVolume);
			AudioEngines[soundEngineName] = NewAudioEngine;
			return NewAudioEngine;
		}

		AudioEngine* FindAudioEngine(const std::string& soundEngineName)
		{
			auto it = AudioEngines.find(soundEngineName);
			if (it == AudioEngines.end()) 
			{
				return nullptr;
			}

			return it->second;
		}

		void SetListeningPosition(Vector3 listeningPosition)
		{
			ListeningPosition = listeningPosition;

			for (auto& audioEngine : AudioEngines)
			{
				audioEngine.second->setListenerPosition(ListeningPosition);
			}
		}

		//Set the global volume percentage
		void SetGlobalVolume(float volume) 
		{
			for (auto& audioEngine : AudioEngines)
			{
				audioEngine.second->setAmbientVolume(volume);
			}
		}

		// Free up memory
		void Uninitialize()
		{
			for (auto& AudioEngineToDelete : AudioEngines)
			{
				delete AudioEngineToDelete.second;
			}
		}
	};

	Audio* AddAudio(std::string AudioEngineName, std::string AudioFile, bool loop, float volume, DistanceModel model, float maxDist = 1000.0f, float refDist = 1.0f, float rolloff = 1.0f)
	{
		auto* audio = engine::ecs::GetSystem<engine::SoundSystem>()->FindAudioEngine(AudioEngineName)->createAudio(AudioFile, loop, volume);

		if (audio) 
		{
			audio->setVolume(volume);
			audio->setDistanceModel(model);
			audio->setMaxDistance(maxDist);
			audio->setReferenceDistance(refDist);
			audio->setRolloffFactor(rolloff);
		}
		return audio;
	}

};
