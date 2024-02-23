#pragma once
#include <engine/Application.h>

namespace resources
{
	Font* niagaraFont;
	Model* checkPointModel;
	Model* finishLineModel;
	Texture* winSprite;
	Tilemap* level1Map;
	Animation explosionAnimation;
	Animation crowdAnims;
	Animation countdownAnim;

	void LoadResources(Camera* cam)
	{
		niagaraFont = new Font("Niagara Solid.ttf", 0, 0, 48);

		checkPointModel = new Model("/3dmodels/Checkpoint.obj");
		finishLineModel = new Model("/3dmodels/Finish_line.obj");

		winSprite = new Texture("/GUI/winner.png");

		level1Map = new Tilemap(cam);
		level1Map->loadMap("/levels/level1.tmx");

		explosionAnimation = AnimationsFromSpritesheet("/spritesheets/explosion.png", 6, 1, vector<int>(6, 150))[0];
		crowdAnims = AnimationsFromSpritesheet("/spritesheets/CrowdCheer14.png", 3, 1, vector<int>(3, 150))[0];
		countdownAnim = AnimationsFromSpritesheet("/spritesheets/UI_Countdown_Ver2.png", 5, 1, vector<int>(5, 1000))[0];
	}

	void UnloadResources()
	{

	}
}