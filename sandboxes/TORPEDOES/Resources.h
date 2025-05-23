#pragma once
#include <engine/Application.h>
#include <engine/ResourceManagement.h>

namespace resources
{
	using namespace engine;

	std::unordered_map<std::string, Texture*> uiTextures;
	std::unordered_map<std::string, Texture*> menuTextures;
	Texture* subUnderwaterTexture;

	std::unordered_map<std::string, Model*> models;

	Font* niagaraFont;

	Tilemap* level1Map; // Green tilemap
	Tilemap* level2Map; // Cave tilemap
	Tilemap* level3Map; // Tropical tilemap
	Tilemap* level4Map; // Long tilemap	
	Tilemap* level5Map; // Ocean Tilemap
	Tilemap* level6Map; // river2 Tilemap

	Animation explosionAnimation;
	Animation WaterexplosionAnimation;
	Animation crowdAnims1;
	Animation crowdAnims2;
	Animation countdownAnim;
	Animation divingAnim;
	Animation continuousDivingAnim;
	std::vector<Animation> wakeAnims;

	std::vector<Texture*> playerIdToTexture;

	std::vector<Audio*> explosion;
	std::vector<Audio*> torpedoLaunch;
	std::vector<Audio*> submerge; 
	Audio* gameMusic;
	Audio* countdownSound;
	Audio* confirmSound;
	Audio* moveSound;

	//Load all the global resources here
	//Stuff that is only used in one system can be loaded there 
	void LoadResources(Camera* cam)
	{
		uiTextures = engine::PreloadTextures("GUI");
		menuTextures = engine::PreloadTextures("menuUI", true, GL_LINEAR);

		//Set the player textures
		playerIdToTexture = {
			new Texture(assetPath + "3dmodels/Player_Green.png"),
			new Texture(assetPath + "3dmodels/Player_Orange.png"),
			new Texture(assetPath + "3dmodels/Player_Red.png"),
			new Texture(assetPath + "3dmodels/Player_Purple.png"),
		};
		subUnderwaterTexture = new Texture(assetPath + "3dmodels/Player_Underwater.png");

		models = engine::PreloadModels("3dmodels", true, GL_NEAREST);

		niagaraFont = new Font("Niagara Solid.ttf", 0, 0, 600);

		level1Map = new Tilemap(cam);
		level1Map->loadMap("/levels/level1.tmx");
		level2Map = new Tilemap(cam);
		level2Map->loadMap("/levels/level2.tmx");
		level3Map = new Tilemap(cam);
		level3Map->loadMap("levels/level3.tmx");
		level4Map = new Tilemap(cam);
		level4Map->loadMap("/levels/level4.tmx");
		level5Map = new Tilemap(cam);
		level5Map->loadMap("/levels/level5.tmx");
		level6Map = new Tilemap(cam);
		level6Map->loadMap("/levels/level6.tmx");

		explosionAnimation = AnimationsFromSpritesheet("/spritesheets/explosion.png", 6, 1, vector<int>(6, 150))[0];
		WaterexplosionAnimation = AnimationsFromSpritesheet("/spritesheets/Hedgehog_Explosion.png", 14, 1, vector<int>(14, 70))[0];
		crowdAnims1 = AnimationsFromSpritesheet("/spritesheets/CrowdCheer14.png", 3, 1, vector<int>(3, 150))[0];
		crowdAnims2 = AnimationsFromSpritesheet("/spritesheets/CrowdCheer15.png", 4, 1, vector<int>(4, 196))[0];
		countdownAnim = AnimationsFromSpritesheet("/spritesheets/UI_Countdown.png", 5, 1, vector<int>(5, 1000))[0];
		divingAnim = AnimationsFromSpritesheet("/spritesheets/Diving_Sheet.png", 8, 1, vector<int>(8, 100))[0];
		continuousDivingAnim = AnimationsFromSpritesheet("/spritesheets/Underwater_Sheet.png", 4, 1, vector<int>(4, 50))[0];
		wakeAnims = AnimationsFromSpritesheet("/spritesheets/Booster_Sheet.png", 4, 2, vector<int>(8, 70));

		for (size_t i = 0; i < 4; i++)
		{
			explosion.push_back(engine::AddAudio("Gameplay", "audio/explosion.wav", false, 0.2f, DistanceModel::LINEAR));
			torpedoLaunch.push_back(engine::AddAudio("Gameplay", "audio/torpedoshoot.wav", false, 0.15f, DistanceModel::LINEAR));
			submerge.push_back(engine::AddAudio("Gameplay", "audio/submerge_01.wav", false, 0.15f, DistanceModel::LINEAR));
		}
		gameMusic = engine::AddAudio("Music", "audio/TheStruggleLoop1.wav", true, 0.05f, DistanceModel::NONE);
		countdownSound = engine::AddAudio("Gameplay", "audio/CountdownSound.wav", false, 0.13f, DistanceModel::NONE, 5000.0f, 1.0f, 0.0f);
		confirmSound = engine::AddAudio("Gameplay", "audio/select.wav", false, 0.4f, DistanceModel::NONE, 5000.0f, 1.0f, 0.0f);
		moveSound = engine::AddAudio("Gameplay", "audio/leftright.wav", false, 0.4f, DistanceModel::NONE, 5000.0f, 1.0f, 0.0f);
	}

	Audio* NextAvailableAudio(std::vector<Audio*>& list)
	{
		for (size_t i = 0; i < list.size(); i++)
		{
			if (!list[i]->getPlaying())
			{
				list[i]->play();
				return list[i];
			}
		}
		return list.front();
	}

	void UnloadResources()
	{
		for (const auto model : models)
		{
			delete model.second;
		}
		for (const auto texture : uiTextures)
		{
			delete texture.second;
		}
		for (const auto texture : menuTextures)
		{
			delete texture.second;
		}

		delete niagaraFont;
		delete level1Map;
		delete level2Map;
		delete level3Map;
		delete level4Map;
		delete level5Map;
		delete level6Map;
	}
}