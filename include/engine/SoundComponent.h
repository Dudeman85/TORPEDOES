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
	};

	// Can't be arsed to implement this, we only have 1 position to listen from anyway
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
		Vector2 ListeningPosition;

		void Update()
		{
			// Iterate through entities in the system
			for (engine::ecs::Entity entity : entities)
			{
				// Get necessary components
				SoundComponent& soundComponent = engine::ecs::GetComponent<SoundComponent>(entity);
				engine::Transform& soundTransform = engine::ecs::GetComponent<engine::Transform>(entity);

				for (auto& sound : soundComponent.Sounds)
				{
					sound.second->setAbsoluteDirection(ListeningPosition - soundTransform.position);
					std::cout << "pos: " << (ListeningPosition - soundTransform.position).x << ", " << (ListeningPosition - soundTransform.position).y << "\n";
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

		void SetListeningPosition(Vector2 listeningPosition)
		{
			ListeningPosition = listeningPosition;
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

	Audio* AddAudio(std::string AudioEngineName, std::string AudioFile, bool loop, float volume)
	{
		return engine::ecs::GetSystem<engine::SoundSystem>()->FindAudioEngine(AudioEngineName)->createAudio(AudioFile, loop, volume);
	}
};
