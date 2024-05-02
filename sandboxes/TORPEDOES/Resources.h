#pragma once
#include <engine/Application.h>
#include <engine/ResourceManagement.h>

namespace resources
{
	using namespace engine;

	std::unordered_map<std::string, Texture*> uiTextures;
	std::unordered_map<std::string, Texture*> menuTextures;
	std::unordered_map<std::string, Texture*> modelTextures;

	std::unordered_map<std::string, Model*> models;

	Font* niagaraFont;

	Tilemap* level1Map; // Green tilemap
	Tilemap* level2Map; // Cave tilemap
	Tilemap* level3Map; // Tropical tilemap
	Tilemap* level4Map; // Long tilemap	

	Animation explosionAnimation;
	Animation WaterexplosionAnimation;
	Animation crowdAnims;
	Animation countdownAnim;
	Animation divingAnim;
	Animation continuousDivingAnim;

	std::vector<Texture*> playerIdToTexture;

	//Load all the global resources here
	//Stuff that is only used in one system can be loaded there 
	void LoadResources(Camera* cam)
	{
		uiTextures = engine::PreloadTextures("GUI");
		menuTextures = engine::PreloadTextures("menuUI", true, GL_LINEAR_MIPMAP_NEAREST);
		modelTextures = engine::PreloadTextures("3dmodels");

		//Set the player textures
		playerIdToTexture = { modelTextures["Player_Green.png"], modelTextures["Player_Orange.png"], modelTextures["Player_Red.png"], modelTextures["Player_Purple.png"] };

		models = engine::PreloadModels("3dmodels");

		niagaraFont = new Font("Niagara Solid.ttf", 0, 0, 48);

		level1Map = new Tilemap(cam);
		level1Map->loadMap("/levels/level1.tmx");

		level2Map = new Tilemap(cam);
		level2Map->loadMap("/levels/level2.tmx");

		level3Map = new Tilemap(cam);
		level3Map->loadMap("levels/level3.tmx");

		level4Map = new Tilemap(cam);
		level4Map->loadMap("/levels/level4.tmx");

		explosionAnimation = AnimationsFromSpritesheet("/spritesheets/explosion.png", 6, 1, vector<int>(6, 150))[0];
		WaterexplosionAnimation = AnimationsFromSpritesheet("/spritesheets/Hedgehog_Explosion.png", 14, 1, vector<int>(14, 70))[0];
		crowdAnims = AnimationsFromSpritesheet("/spritesheets/CrowdCheer14.png", 3, 1, vector<int>(3, 150))[0];
		countdownAnim = AnimationsFromSpritesheet("/spritesheets/UI_Countdown_Ver2.png", 5, 1, vector<int>(5, 1000))[0];
		divingAnim = AnimationsFromSpritesheet("/spritesheets/Diving_Sheet_Ver2.png", 8, 1, vector<int>(8, 100))[0];
		continuousDivingAnim = AnimationsFromSpritesheet("/spritesheets/Underwater_Sheet_Ver2.png", 4, 1, vector<int>(4, 50))[0];
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
	}
}