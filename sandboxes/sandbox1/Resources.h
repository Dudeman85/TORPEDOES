#pragma once
#include <engine/Application.h>

namespace resources
{
	Font* niagaraFont;

	Texture* winSprite;
	Texture* torpCooldownTexture;
	Texture* torpReadyTexture;

	Model* torpedoModel;
	Model* laMuerteModel;
	Model* checkPointModel;
	Model* finishLineModel;

	Animation explosionAnimation;
	Animation crowdAnims;
	Animation countdownAnim;	

	Tilemap* level1Map;

	void LoadResources(Camera* cam)
	{
		niagaraFont = new Font("Niagara Solid.ttf", 0, 0, 48);

		checkPointModel = new Model("/3dmodels/Checkpoint.obj");
		finishLineModel = new Model("/3dmodels/Finish_line.obj");
		torpedoModel = new Model("/3dmodels/torpedo.obj");
		laMuerteModel = new Model("/3dmodels/LaMuerte.obj");

		torpCooldownTexture = new Texture("/GUI/UI_Red_Torpedo_Icon.png");
		torpReadyTexture = new Texture("/GUI/UI_Green_Torpedo_Icon.png");
		winSprite = new Texture("/GUI/winner.png");

		level1Map = new Tilemap(cam);
		level1Map->loadMap("/levels/level3.tmx");

		explosionAnimation = AnimationsFromSpritesheet("/spritesheets/explosion.png", 6, 1, vector<int>(6, 150))[0];
		crowdAnims = AnimationsFromSpritesheet("/spritesheets/CrowdCheer14.png", 3, 1, vector<int>(3, 150))[0];
		countdownAnim = AnimationsFromSpritesheet("/spritesheets/UI_Countdown_Ver2.png", 5, 1, vector<int>(5, 1000))[0];
	}

	void UnloadResources()
	{
		delete niagaraFont;
		delete checkPointModel;
		delete finishLineModel;
		delete winSprite;
		delete level1Map;
	}
}