#pragma once
//Author: Sauli Hanell month.day.year 2.9.2024

#include <engine/Application.h>
#include <functional>
#include "engine/Input.h"  

//Pause Component
ECS_REGISTER_COMPONENT(PauseComponent)
struct PauseComponent
{
	ecs::Entity upper, lower;
	engine::Texture* selectedTexture;
	engine::Texture* unselectedTexture;
	std::function<void()> operation;
	bool isOptionsMenu = false;
	bool isSlider = false;
	float sliderValue = 0;
};

ECS_REGISTER_SYSTEM(PauseSystem, PauseComponent, Transform)
class PauseSystem : public ecs::System
{
	ecs::Entity pausedImage;

	ecs::Entity resumeButton;
	ecs::Entity optionsButton;

	ecs::Entity mainMenuButton;

	ecs::Entity quitGameButton;

	//std::map<std::string, ecs::Entity> pauseButtons{ {"return", resumeButton}, {"options", optionsButton}, {"mainMenu", mainMenuButton},{"quitGame", quitGameButton} };


	//options buttons
	ecs::Entity optionsResumeButton;
	ecs::Entity musicSliderEntity, musicSliderNub;;
	ecs::Entity fullscreenEntity;

	vector<ecs::Entity>  optionsButtons = { {optionsResumeButton},{musicSliderEntity}, {fullscreenEntity} };
	//std::map<std::string, ecs::Entity> optionsButtons{ {"return", optionsResumeButton}, {"music", musicSliderEntity}, {"fullscreen", fullscreenEntity} };

	static GLFWwindow* window;
	float moveWaitedTimerUP;
	float moveWaitedTimerDown;
	const  float  delay = 0.8f;

public:
	ecs::Entity currentSelection;
	bool isGamePause;

	void Update()
	{
		printf("IN MENU SYSTEM UPDATE()\n");
		if (input::GetNewPress("Pause"))
		{
			printf("Pause\n");
			isGamePause = !isGamePause;
			ToggleShowUIMenu();
		}
		if (ecs::GetSystem<PauseSystem>()->isGamePause)
		{
			if (input::GetInputValue("Move0", GLFW_GAMEPAD_AXIS_LEFT_Y) >= 0.5f)
			{
				std::cout << "upTimer" << moveWaitedTimerUP << "\n";
				moveWaitedTimerUP += deltaTime;

				if (moveWaitedTimerUP >= delay)
				{
					printf("\n\n move up input\n\n");
					MoveUpper();
					moveWaitedTimerUP = 0;
				}

			}
			else
			{
				moveWaitedTimerUP = 0;
			}
			if (input::GetInputValue("Move0", GLFW_GAMEPAD_AXIS_LEFT_Y) <= -0.5f)
			{
				moveWaitedTimerDown += deltaTime;

				if (moveWaitedTimerDown >= delay)
				{
					printf("move down input\n");
					MoveLower();
					moveWaitedTimerDown = 0;
				}

			}
			else
			{
				moveWaitedTimerDown = 0;
			}

			//if (input::GetNewPress("MoveUp"))
			//{
			//	MoveUpper();
			//}
			//if (input::GetNewPress("MoveDown"))
			//{
			//	MoveLower();
			//}

			//if (IsCurrentPauseComponentSlider())
			//{

			//	if (input::GetNewPress("MoveRight"))
			//	{
			//		/*this dosent works*///UpdateSlider();
			//		MoveSliderRight();

			//	}
			//	if (input::GetNewPress("MoveLeft"))
			//	{

			//		//selectedPauseComponent.sliderValue += -0.01;
			//		/*this dosent works*///UpdateSlider();
			//		MoveSliderLeft();


			//	}

			//}
			//if (input::GetNewPress("Select"))
			//{
			//	Selected();
			//}

		}

	}
	void Init(GLFWwindow* mainWindow)
	{
		printf("IN side MENU SYSTEM INIT\n");
		PauseSystem::window = mainWindow;
		input::initialize(window);

		//INPUTS  START	
		input::ConstructDigitalEvent("MoveUp");
		input::ConstructDigitalEvent("MoveDown");
		input::ConstructDigitalEvent("Select");
		input::ConstructDigitalEvent("Pause");
		input::ConstructDigitalEvent("MoveRight");
		input::ConstructDigitalEvent("MoveLeft");

		//TODO: add controller input
		input::bindDigitalInput(GLFW_KEY_LEFT, { "MoveLeft" });
		input::bindDigitalInput(GLFW_KEY_RIGHT, { "MoveRight" });
		input::bindDigitalInput(GLFW_KEY_UP, { "MoveUp" });
		input::bindDigitalInput(GLFW_KEY_DOWN, { "MoveDown" });
		input::bindDigitalInput(GLFW_KEY_ENTER, { "Select" });
		input::bindDigitalInput(GLFW_KEY_P, { "Pause" });


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


		//                      ---------  PAUSE BUTTONS init-------
		ecs::AddComponent(pausedImage, Transform{ .position = Vector3(0,.8f,-0.1f), .scale = Vector3(0.35f) });
		ecs::AddComponent(pausedImage, SpriteRenderer{ .texture = resources::menuTextures["UI_Paused.png"],  .enabled = false, .uiElement = true });
		ecs::AddComponent(pausedImage, PauseComponent{ .upper = pausedImage, .lower = optionsButton, .selectedTexture = resources::menuTextures["UI_Paused.png"], .unselectedTexture = resources::menuTextures["UI_Paused.png"], .operation = PauseSystem::OnResumePressed });


		ecs::AddComponent(resumeButton, Transform{ .position = Vector3(0,.5f,-0.1f), .scale = Vector3(0.25f) });
		ecs::AddComponent(resumeButton, SpriteRenderer{ .texture = resources::menuTextures["UI_Resume_N.png"],  .enabled = false, .uiElement = true });
		ecs::AddComponent(resumeButton, PauseComponent{ .upper = quitGameButton, .lower = optionsButton, .selectedTexture = resources::menuTextures["UI_Resume.png"], .unselectedTexture = resources::menuTextures["UI_Resume_N.png"], .operation = PauseSystem::OnResumePressed });

		ecs::AddComponent(optionsButton, Transform{ .position = Vector3(0,.3f,-0.1f), .scale = Vector3(.25f) });
		ecs::AddComponent(optionsButton, SpriteRenderer{ .texture = resources::menuTextures["UI_Options_N.png"],  .enabled = false, .uiElement = true });
		ecs::AddComponent(optionsButton, PauseComponent{ .upper = resumeButton, .lower = mainMenuButton, .selectedTexture = resources::menuTextures["UI_Options.png"], .unselectedTexture = resources::menuTextures["UI_Options_N.png"], .operation = PauseSystem::OnOptionsPressed });

		ecs::AddComponent(mainMenuButton, Transform{ .position = Vector3(0,.1f,-0.1f), .scale = Vector3(0.25f) });
		ecs::AddComponent(mainMenuButton, SpriteRenderer{ .texture = resources::menuTextures["UI_BackToMenu_N.png"],  .enabled = false, .uiElement = true });
		ecs::AddComponent(mainMenuButton, PauseComponent{ .upper = optionsButton, .lower = quitGameButton, .selectedTexture = resources::menuTextures["UI_BackToMenu.png"], .unselectedTexture = resources::menuTextures["UI_BackToMenu_N.png"], .operation = PauseSystem::OnMainMenuPressed });

		ecs::AddComponent(quitGameButton, Transform{ .position = Vector3(0,-0.7f,-0.1f), .scale = Vector3(0.25f) });
		ecs::AddComponent(quitGameButton, SpriteRenderer{ .texture = resources::menuTextures["UI_QuitGame_N.png"],  .enabled = false, .uiElement = true });
		ecs::AddComponent(quitGameButton, PauseComponent{ .upper = mainMenuButton, .lower = resumeButton, .selectedTexture = resources::menuTextures["UI_QuitGame.png"], .unselectedTexture = resources::menuTextures["UI_QuitGame_N.png"], .operation = PauseSystem::OnQuitGamePressed });



		////                      ---------  OPTIONS BUTTONS init --------
		//optionsResumeButton
		ecs::AddComponent(optionsResumeButton, Transform{ .position = Vector3(0,.3f,-0.1f), .scale = Vector3(.25f) });
		ecs::AddComponent(optionsResumeButton, SpriteRenderer{ .texture = resources::menuTextures["UI_Resume.png"],  .enabled = false, .uiElement = true });
		ecs::AddComponent(optionsResumeButton, PauseComponent{ .upper = fullscreenEntity, .lower = musicSliderEntity, .selectedTexture = resources::menuTextures["UI_Resume.png"], .unselectedTexture = resources::menuTextures["UI_Resume_N.png"], .operation = BackToUIMenu, .isOptionsMenu = true });
		//musicSliderEntity
		ecs::AddComponent(musicSliderEntity, Transform{ .position = Vector3(0,.1f,-0.1f), .scale = Vector3(0.25f) });
		ecs::AddComponent(musicSliderEntity, SpriteRenderer{ .texture = resources::menuTextures["UI_Music_Slider.png"],  .enabled = false, .uiElement = true });
		ecs::AddComponent(musicSliderEntity, PauseComponent{ .upper = optionsResumeButton, .lower = fullscreenEntity, .selectedTexture = resources::menuTextures["UI_Music_Slider.png"], .unselectedTexture = resources::menuTextures["UI_Music_Slider_N.png"], .operation = PauseSystem::OnMainMenuPressed, .isOptionsMenu = true,.isSlider = true });
		//fullscreenEntity
		ecs::AddComponent(fullscreenEntity, Transform{ .position = Vector3(0,-0.7f,-0.1f), .scale = Vector3(0.25f) });
		ecs::AddComponent(fullscreenEntity, SpriteRenderer{ .texture = resources::menuTextures["UI_Fullscreen.png"],  .enabled = false, .uiElement = true });
		ecs::AddComponent(fullscreenEntity, PauseComponent{ .upper = musicSliderEntity, .lower = optionsResumeButton, .selectedTexture = resources::menuTextures["UI_Fullscreen.png"], .unselectedTexture = resources::menuTextures["UI_Fullscreen_N.png"], .operation = PauseSystem::OnQuitGamePressed, .isOptionsMenu = true });
		//musicSliderNub
		ecs::AddComponent(musicSliderNub, Transform{ .position = ecs::GetComponent<Transform>(musicSliderEntity).position + Vector3(0,-0.2f,-0.1), .scale = Vector3(0.15f) });
		ecs::AddComponent(musicSliderNub, SpriteRenderer{ .texture = resources::menuTextures["UI_Slider_Button.png"],  .enabled = false, .uiElement = true });
		ecs::AddComponent(musicSliderNub, PauseComponent{ .selectedTexture = resources::menuTextures["UI_Slider_Button.png"], .unselectedTexture = resources::menuTextures["UI_Slider_Button.png"], .isOptionsMenu = true,.isSlider = true });

	}

	void AddButton(ecs::Entity entity, Vector3 pos, Texture selectedTexture, Texture unselectedTexture, Vector3 scaleNormal, Vector3 scaleSelected)
	{
		ecs::AddComponent(fullscreenEntity, Transform{ .position = Vector3(0.7,-0.7f,-0.1f), .scale = Vector3(0.25f) });
		ecs::AddComponent(fullscreenEntity, SpriteRenderer{ .texture = resources::menuTextures["UI_Fullscreen_N.png"],  .enabled = false, .uiElement = true });
		ecs::AddComponent(fullscreenEntity, PauseComponent{ .upper = mainMenuButton, .lower = resumeButton, .selectedTexture = resources::menuTextures["UI_Fullscreen_N.png"], .unselectedTexture = resources::menuTextures["UI_Fullscreen.png"], .operation = PauseSystem::OnQuitGamePressed });
	}
	void MoveUpper()
	{
		printf("Move upper\n");
		PauseComponent& pauseComponent = ecs::GetComponent<PauseComponent>(currentSelection);
		SpriteRenderer& unselectedSpriteRenderer = ecs::GetComponent<SpriteRenderer>(currentSelection);
		Transform& unselectedTransform = ecs::GetComponent<Transform>(currentSelection);

		unselectedTransform.scale = Vector3(0.25f);
		unselectedSpriteRenderer.texture = pauseComponent.unselectedTexture;

		currentSelection = pauseComponent.upper;
		PauseComponent& pauseComponent1 = ecs::GetComponent<PauseComponent>(currentSelection);
		SpriteRenderer& selectedSpriteRenderer = ecs::GetComponent<SpriteRenderer>(currentSelection);
		Transform& selectedSpriteTransform = ecs::GetComponent<Transform>(currentSelection);
		selectedSpriteTransform.scale = Vector3(0.32f);
		selectedSpriteRenderer.texture = pauseComponent1.selectedTexture;


	}
	void MoveLower()
	{
		printf("Move lower\n");
		PauseComponent& pauseComponent = ecs::GetComponent<PauseComponent>(currentSelection);
		SpriteRenderer& unselectedSpriteRenderer = ecs::GetComponent<SpriteRenderer>(currentSelection);
		Transform& unselectedTransform = ecs::GetComponent<Transform>(currentSelection);

		unselectedTransform.scale = Vector3(0.25f);
		unselectedSpriteRenderer.texture = pauseComponent.unselectedTexture;

		currentSelection = pauseComponent.lower;
		PauseComponent& pauseComponent1 = ecs::GetComponent<PauseComponent>(currentSelection);
		SpriteRenderer& selectedSpriteRenderer = ecs::GetComponent<SpriteRenderer>(currentSelection);
		Transform& selectedSpriteTransform = ecs::GetComponent<Transform>(currentSelection);
		selectedSpriteTransform.scale = Vector3(0.32f);
		selectedSpriteRenderer.texture = pauseComponent1.selectedTexture;

	}
	bool IsCurrentPauseComponentSlider()
	{
		return ecs::GetComponent<PauseComponent>(currentSelection).isSlider;
	}
	PauseComponent& GetCurrentSelectedPauseComponent()
	{
		return ecs::GetComponent<PauseComponent>(currentSelection);
	}
	ecs::Entity GetCurrentSelection()
	{
		return currentSelection;
	}
	static void BackToUIMenu()
	{
		ecs::GetSystem<PauseSystem>()->ToggleShowUIMenu();

	}
	static  void OnResumePressed()
	{
		printf("OnResumePressed()\n");

		ecs::GetSystem<PauseSystem>()->isGamePause = !ecs::GetSystem<PauseSystem>()->isGamePause;
		ecs::GetSystem<PauseSystem>()->ToggleShowUIMenu();
	}
	static  void OnOptionsPressed()
	{

		printf("open options in pause menu\n");

		ecs::GetSystem<PauseSystem>()->ToggleShowUIOptionsMenu();

	}
	static void OnQuitGamePressed()
	{
		printf("OnQuitGamePressed()\n");
		glfwSetWindowShouldClose(PauseSystem::window, true);
	}
	static  void OnMainMenuPressed()
	{
		printf("OnMainMenuPressed()\n");
	}
	void Selected() {

		PauseComponent& pauseComponent = ecs::GetComponent<PauseComponent>(currentSelection);
		SpriteRenderer& pauseSpriteRenderer = ecs::GetComponent<SpriteRenderer>(currentSelection);
		pauseSpriteRenderer.texture = pauseComponent.selectedTexture;
		pauseComponent.operation();

	}

	void ToggleShowUIMenu()
	{

		printf("menu UI\n");
		for (auto itr = entities.begin(); itr != entities.end();)
		{
			//Get the entity and increment the iterator
			ecs::Entity entity = *itr++;
			bool& isOptionsMenu = ecs::GetComponent<PauseComponent>(entity).isOptionsMenu;
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

		PauseComponent& pauseComponent = ecs::GetComponent<PauseComponent>(currentSelection);
		SpriteRenderer& selectedSpriteRenderer = ecs::GetComponent<SpriteRenderer>(currentSelection);
		Transform& selectedSpriteTransform = ecs::GetComponent<Transform>(currentSelection);
		selectedSpriteTransform.scale = Vector3(0.32f);
		selectedSpriteRenderer.texture = pauseComponent.selectedTexture;

	}
	void SetCurrentSelection(ecs::Entity entity)
	{
		PauseComponent& pauseComponent = ecs::GetComponent<PauseComponent>(entity);
		SpriteRenderer& selectedSpriteRenderer = ecs::GetComponent<SpriteRenderer>(entity);
		Transform& selectedSpriteTransform = ecs::GetComponent<Transform>(entity);
		selectedSpriteTransform.scale = Vector3(0.32f);
		selectedSpriteRenderer.texture = pauseComponent.selectedTexture;
	}

	void UpdateSlider()
	{
		// TODO: change 	sliderValue dosent update in main Fix need pointer or reference


		PauseComponent& pauseComponent = ecs::GetComponent<PauseComponent>(currentSelection);
		Transform& selectedSliderTransform = ecs::GetComponent<Transform>(currentSelection);
		PauseComponent& pauseComponentNub = ecs::GetComponent<PauseComponent>(musicSliderNub);
		Transform& nubTransform = ecs::GetComponent<Transform>(musicSliderNub);
		nubTransform.position = Vector3(pauseComponentNub.sliderValue, 0, 0);
		printf("pauseComponentNub.sliderValue ");
		std::cout << pauseComponentNub.sliderValue;
		printf("\n");
		std::cout << "Nub pos:" << "x:" << nubTransform.position.x << " y:" << nubTransform.position.y << "\n";

	}

	void MoveSliderRight()
	{
		PauseComponent& pauseComponentNub = ecs::GetComponent<PauseComponent>(musicSliderNub);
		Transform& nubTransform = ecs::GetComponent<Transform>(musicSliderNub);
		nubTransform.position += Vector3(0.01f, 0, 0);
		nubTransform.position.x = clamp(nubTransform.position.x, -0.17f, 0.17f);
		printf("pauseComponentNub.sliderValue ");
		std::cout << pauseComponentNub.sliderValue;
		printf("\n");
		std::cout << "Nub pos:" << "x:" << nubTransform.position.x << " y:" << nubTransform.position.y << "\n";
	}
	void MoveSliderLeft()
	{
		PauseComponent& pauseComponentNub = ecs::GetComponent<PauseComponent>(musicSliderNub);
		Transform& nubTransform = ecs::GetComponent<Transform>(musicSliderNub);
		nubTransform.position -= Vector3(0.01f, 0, 0);
		nubTransform.position.x = clamp(nubTransform.position.x, -0.17f, 0.17f);
		printf("pauseComponentNub.sliderValue ");
		std::cout << pauseComponentNub.sliderValue;
		printf("\n");
		std::cout << "Nub pos:" << "x:" << nubTransform.position.x << " y:" << nubTransform.position.y << "\n";

	}
	void ToggleShowUIOptionsMenu()
	{

		printf("options menu UI\n");


		PauseComponent& pauseComponentOld = ecs::GetComponent<PauseComponent>(currentSelection);
		SpriteRenderer& selectedSpriteRendererOld = ecs::GetComponent<SpriteRenderer>(currentSelection);
		Transform& selectedSpriteTransformOld = ecs::GetComponent<Transform>(currentSelection);
		selectedSpriteTransformOld.scale = Vector3(0.25f);
		selectedSpriteRendererOld.texture = pauseComponentOld.unselectedTexture;

		currentSelection = optionsResumeButton;

		for (auto itr = entities.begin(); itr != entities.end();)
		{
			//Get the entity and increment the iterator
			ecs::Entity entity = *itr++;
			bool& isOptionsMenu = ecs::GetComponent<PauseComponent>(entity).isOptionsMenu;

			bool& enabled = ecs::GetComponent<SpriteRenderer>(entity).enabled;
			PauseComponent& pauseComponent = ecs::GetComponent<PauseComponent>(entity);
			SpriteRenderer& spriteRenderer = ecs::GetComponent<SpriteRenderer>(entity);
			spriteRenderer.texture = pauseComponent.unselectedTexture;

			enabled = !enabled;



		}

		PauseComponent& pauseComponent = ecs::GetComponent<PauseComponent>(currentSelection);
		SpriteRenderer& selectedSpriteRenderer = ecs::GetComponent<SpriteRenderer>(currentSelection);
		Transform& selectedSpriteTransform = ecs::GetComponent<Transform>(currentSelection);
		selectedSpriteTransform.scale = Vector3(0.32f);
		selectedSpriteRenderer.texture = pauseComponent.selectedTexture;

	}

};
GLFWwindow* PauseSystem::window = window;


