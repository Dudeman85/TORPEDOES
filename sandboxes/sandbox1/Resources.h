#pragma once
#include <engine/Application.h>
#include <engine/ResourceManagement.h>

namespace resources
{
	std::unordered_map<std::string, engine::Texture*> uiTextures;
	std::unordered_map<std::string, engine::Texture*> menuTextures;

	std::unordered_map<std::string, engine::Model*> models;

	engine::Font* niagaraFont;

	Tilemap* level1Map;

	engine::Animation explosionAnimation;
	engine::Animation crowdAnims;
	engine::Animation countdownAnim;

	//Load all the global resources here
	//Stuff that is only used in one system can be loaded there 
	void LoadResources(engine::Camera* cam)
	{
		uiTextures = engine::PreloadTextures("GUI");
		menuTextures = engine::PreloadTextures("menuUI", true, GL_LINEAR_MIPMAP_NEAREST);

		models = engine::PreloadModels("3dmodels");

		niagaraFont = new engine::Font("Niagara Solid.ttf", 0, 0, 48);

		level1Map = new Tilemap(cam);
		level1Map->loadMap("/levels/level3.tmx");

		explosionAnimation = engine::AnimationsFromSpritesheet("/spritesheets/explosion.png", 6, 1, std::vector<int>(6, 150))[0];
		crowdAnims = engine::AnimationsFromSpritesheet("/spritesheets/CrowdCheer14.png", 3, 1, std::vector<int>(3, 150))[0];
		countdownAnim = engine::AnimationsFromSpritesheet("/spritesheets/UI_Countdown_Ver2.png", 5, 1, std::vector<int>(5, 1000))[0];
	}

	void UnloadResources()
	{
		for (const auto& model : models)
		{
			delete model.second;
		}
		for (const auto& texture : uiTextures)
		{
			delete texture.second;
		}
		for (const auto& texture : menuTextures)
		{
			delete texture.second;
		}

		delete niagaraFont;
		delete level1Map;
	}
}