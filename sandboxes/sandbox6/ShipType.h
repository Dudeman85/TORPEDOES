#pragma once
#include <engine/Application.h>
#include <functional>
//Author: Sauli Hanell month.day.year 2.22.2024

enum  shipTypes { LaMuerte, pt_10, u_99, Hedgehog};

//ECS_REGISTER_COMPONENT(ShipType, Texture)
//struct ShipTypeComponent
//{
//	ecs::Entity moveRight, moveLeft;
//	engine::Model* selectedModel;
//	engine::Texture* selectedtexture;
//	std::function<void()> specialSkill;
//
//};

//template <typename T> T ShipType(shipTypes)
//{
//
//	engine::Model* selectedModel;
//	engine::Texture* selectedtexture;
//	std::function<void()> specialSkill;
//
//	return (x > y) ? x : y;
//}

//ECS_REGISTER_SYSTEM(ShipType, ShipTypeComponent, Transform)
//class SelectShipTypeSystem : public ecs::System
//{
//
//};
ECS_REGISTER_COMPONENT(u_99Component)
struct u_99Component
{
	float projectileTimer;

	float cooldown;

};

ECS_REGISTER_SYSTEM(u_99Component, Transform)
class u_99System : public ecs::System
{
	void Update() 
	{
	}
};


ECS_REGISTER_COMPONENT(pt_10Component, Texture)
struct pt_10Component
{
	ecs::Entity upper, lower;
	engine::Texture* texture;
	std::function<void()> specialSkill;

};

ECS_REGISTER_SYSTEM(pt_10Component, Transform)
class pt_10System : public ecs::System
{

};

ECS_REGISTER_COMPONENT(LaMuerteComponent, Texture)
struct LaMuerteComponent
{
	ecs::Entity upper, lower;
	engine::Texture* selectedTexture;
	engine::Texture* unselectedTexture;
	std::function<void()> specialSkill;
	bool isOptionsMenu = false;
	bool isSlider = false;
	float sliderValue = 0;
};

ECS_REGISTER_SYSTEM( LaMuerteComponent, Transform)
class LaMuerteSystem : public ecs::System
{

};


ECS_REGISTER_COMPONENT(HedgehogComponent)
struct HedgehogComponent
{
	ecs::Entity upper, lower;
	engine::Texture* selectedTexture;
	engine::Texture* unselectedTexture;
	std::function<void()> specialSkill;
	bool isOptionsMenu = false;
	bool isSlider = false;
	float sliderValue = 0;
};

ECS_REGISTER_SYSTEM(HedgehogSystem, HedgehogComponent, Transform)
class HedgehogSystem : public ecs::System
{
	ecs::Entity pausedImage;

	ecs::Entity resumeButton;
	ecs::Entity optionsButton;

	ecs::Entity mainMenuButton;

	ecs::Entity quitGameButton;

	std::map<std::string, ecs::Entity> pauseButtons{ {"return", resumeButton}, {"options", optionsButton}, {"mainMenu", mainMenuButton},{"quitGame", quitGameButton} };



	ecs::Entity optionsResumeButton;
	ecs::Entity musicSliderEntity, musicSliderNub;;
	ecs::Entity fullscreenEntity;

	vector<ecs::Entity>  optionsButtons = { {optionsResumeButton},{musicSliderEntity}, {fullscreenEntity} };
	std::map<std::string, ecs::Entity> optionsButtons{ {"return", optionsResumeButton}, {"music", musicSliderEntity}, {"fullscreen", fullscreenEntity} };

	engine::Texture* currentSelected_Texture;

	vector<Texture*> allTextures;
	static GLFWwindow* window;

public:
	ecs::Entity currentSelection;
	bool isGamePause;

	~HedgehogSystem() {
		delete currentSelected_Texture;
		
		for (Texture* texture : allTextures)
		{
			delete texture;
		}
	}
	void Update()
	{


	}
	void Init(GLFWwindow* mainWindow)
	{
		HedgehogSystem::window = mainWindow;
		resumeButton = ecs::NewEntity();
		optionsButton = ecs::NewEntity();
		mainMenuButton = ecs::NewEntity();
		quitGameButton = ecs::NewEntity();

		optionsResumeButton = ecs::NewEntity();
		musicSliderEntity = ecs::NewEntity();
		fullscreenEntity = ecs::NewEntity();


		musicSliderNub = ecs::NewEntity();
		currentSelection = mainMenuButton;

		pausedImage = ecs::NewEntity();


		allTextures.push_back(new engine::Texture("menuUI/UI_Resume_N.png", GL_LINEAR_MIPMAP_NEAREST));
		allTextures.push_back(new engine::Texture("menuUI/UI_Resume.png", GL_LINEAR_MIPMAP_NEAREST));

		allTextures.push_back(new engine::Texture("menuUI/UI_Options_N.png", GL_LINEAR_MIPMAP_NEAREST));
		allTextures.push_back(new engine::Texture("menuUI/UI_Options.png", GL_LINEAR_MIPMAP_NEAREST));

		allTextures.push_back(new engine::Texture("menuUI/UI_BackToMenu_N.png", GL_LINEAR_MIPMAP_NEAREST));
		allTextures.push_back(new engine::Texture("menuUI/UI_BackToMenu.png", GL_LINEAR_MIPMAP_NEAREST));

		allTextures.push_back(new engine::Texture("menuUI/UI_QuitGame_N.png", GL_LINEAR_MIPMAP_NEAREST));
		allTextures.push_back(new engine::Texture("menuUI/UI_QuitGame.png", GL_LINEAR_MIPMAP_NEAREST));

	
		allTextures.push_back(new engine::Texture("menuUI/UI_Resume.png", GL_LINEAR_MIPMAP_NEAREST));              // 	9
		allTextures.push_back(new engine::Texture("menuUI/UI_Resume_N.png", GL_LINEAR_MIPMAP_NEAREST));            // 8

		allTextures.push_back(new engine::Texture("menuUI/UI_Music_Slider.png", GL_LINEAR_MIPMAP_NEAREST));        // 11
		allTextures.push_back(new engine::Texture("menuUI/UI_Music_Slider_N.png", GL_LINEAR_MIPMAP_NEAREST));      // 10

		allTextures.push_back(new engine::Texture("menuUI/UI_Fullscreen.png", GL_LINEAR_MIPMAP_NEAREST));          // 13
		allTextures.push_back(new engine::Texture("menuUI/UI_Fullscreen_N.png", GL_LINEAR_MIPMAP_NEAREST));        // 12


		allTextures.push_back(new engine::Texture("menuUI/UI_Paused.png", GL_LINEAR_MIPMAP_NEAREST));              // 14

		allTextures.push_back(new engine::Texture("menuUI/UI_Slider_Button.png", GL_LINEAR_MIPMAP_NEAREST));       // 15

		                      //---------  PAUSE BUTTONS init-------
		ecs::AddComponent(pausedImage, Transform{ .position = Vector3(0,.8f,-0.1f), .scale = Vector3(0.35f) });
		ecs::AddComponent(pausedImage, SpriteRenderer{ .texture = allTextures[14],  .enabled = false, .uiElement = true });
		ecs::AddComponent(pausedImage, HedgehogComponent{ .upper = pausedImage, .lower = optionsButton, .selectedTexture = allTextures[14], .unselectedTexture = allTextures[14], .operation = HedgehogSystem::OnResumePressed });


		ecs::AddComponent(resumeButton, Transform{ .position = Vector3(0,.5f,-0.1f), .scale = Vector3(0.25f) });
		ecs::AddComponent(resumeButton, SpriteRenderer{ .texture = allTextures[0],  .enabled = false, .uiElement = true });
		ecs::AddComponent(resumeButton, HedgehogComponent{ .upper = quitGameButton, .lower = optionsButton, .selectedTexture = allTextures[1], .unselectedTexture = allTextures[0], .operation = HedgehogSystem::OnResumePressed });

		ecs::AddComponent(optionsButton, Transform{ .position = Vector3(0,.3f,-0.1f), .scale = Vector3(.25f) });
		ecs::AddComponent(optionsButton, SpriteRenderer{ .texture = allTextures[2],  .enabled = false, .uiElement = true });
		ecs::AddComponent(optionsButton, HedgehogComponent{ .upper = resumeButton, .lower = mainMenuButton, .selectedTexture = allTextures[3], .unselectedTexture = allTextures[2], .operation = HedgehogSystem::OnOptionsPressed });

		ecs::AddComponent(mainMenuButton, Transform{ .position = Vector3(0,.1f,-0.1f), .scale = Vector3(0.25f) });
		ecs::AddComponent(mainMenuButton, SpriteRenderer{ .texture = allTextures[4],  .enabled = false, .uiElement = true });
		ecs::AddComponent(mainMenuButton, HedgehogComponent{ .upper = optionsButton, .lower = quitGameButton, .selectedTexture = allTextures[5], .unselectedTexture = allTextures[4], .operation = HedgehogSystem::OnMainMenuPressed });

		ecs::AddComponent(quitGameButton, Transform{ .position = Vector3(0,-0.7f,-0.1f), .scale = Vector3(0.25f) });
		ecs::AddComponent(quitGameButton, SpriteRenderer{ .texture = allTextures[6],  .enabled = false, .uiElement = true });
		ecs::AddComponent(quitGameButton, HedgehogComponent{ .upper = mainMenuButton, .lower = resumeButton, .selectedTexture = allTextures[7], .unselectedTexture = allTextures[6], .operation = HedgehogSystem::OnQuitGamePressed });


	}

	void AddButton(ecs::Entity entity, Vector3 pos, Texture selectedTexture, Texture unselectedTexture, Vector3 scaleNormal, Vector3 scaleSelected)
	{
		ecs::AddComponent(fullscreenEntity, Transform{ .position = Vector3(0.7,-0.7f,-0.1f), .scale = Vector3(0.25f) });
		ecs::AddComponent(fullscreenEntity, SpriteRenderer{ .texture = allTextures[12],  .enabled = false, .uiElement = true });
		ecs::AddComponent(fullscreenEntity, HedgehogComponent{ .upper = mainMenuButton, .lower = resumeButton, .selectedTexture = allTextures[12], .unselectedTexture = allTextures[13], .operation = PauseSystem::OnQuitGamePressed });

	}
	void MoveUpper()
	{
	

	}
	void MoveLower()
	{
		
	}
	
	ecs::Entity GetCurrentSelection()
	{
		return currentSelection;
	}
	
	static  void OnMainMenuPressed()
	{
		printf("OnMainMenuPressed()\n");
	}
	void Selected() {

		HedgehogComponent& hedgehogComponent = ecs::GetComponent<HedgehogComponent>(currentSelection);
		SpriteRenderer& hedgehogSpriteRenderer = ecs::GetComponent<SpriteRenderer>(currentSelection);
		hedgehogSpriteRenderer.texture = hedgehogComponent.selectedTexture;
		hedgehogComponent.operation();

	}

	void ToggleShowUIMenu()
	{

		printf("menu UI\n");
		for (auto itr = entities.begin(); itr != entities.end();)
		{
			//Get the entity and increment the iterator
			ecs::Entity entity = *itr++;
			bool& isOptionsMenu = ecs::GetComponent<HedgehogComponent>(entity).isOptionsMenu;
			bool& enabled = ecs::GetComponent<SpriteRenderer>(entity).enabled;
			if (!isOptionsMenu)
			{


				enabled = !enabled;
			}
			else
			{
				enabled = false;
			}



		}
		currentSelection = resumeButton;

		
	}
	


};
GLFWwindow* HedgehogSystem::window = window;

