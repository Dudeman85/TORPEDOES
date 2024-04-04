#pragma once
//Author: Sauli Hanell month.day.year 2.9.2024

#include <engine/Application.h>
#include <functional>
#include "engine/Input.h"  
#include "engine/Timing.h"

//PlayerShipSelection
ECS_REGISTER_COMPONENT(PlayerSelection)
struct PlayerSelection
{

	int playerID;
	int selection;
	float cooldownToMoveSelection;
	bool ready = false;
	vector<engine::ecs::Entity> visuals;

};

ECS_REGISTER_SYSTEM(PlayerSelectSystem, PlayerSelection)
class PlayerSelectSystem : public engine::ecs::System
{
	const float cooldownTime = 2;
	//ecs::Entity playerSelection;

	bool isPlayersReady = false;

	const float throttleMoveWaitTime = 0.8f;
	float throttleCurrentWaitedTimeUP = 0;
	float throttleCurrentWaitedTimeDown = 0;
	engine::ecs::Entity selectionWindow;

	engine::ecs::Entity arrowUp;
	engine::ecs::Entity arrowDown;
	engine::ecs::Entity	shipModel;
	engine::ecs::Entity	readyText;

public:
	std::unordered_map<ShipType, engine::Model*> shipModels;

	bool isShipSelectionMenuOn = false;

	vector< std::function<void()> >shipButtonFunctions
	{
		BackToUIMenu

	};
	void Init()
	{
		shipModels.insert({ ShipType::torpedoBoat, resources::models["Ship_PT_109_Torpedo.obj"] });
		shipModels.insert({ ShipType::submarine, resources::models["Ship_U_99_Submarine.obj"] });
		shipModels.insert({ ShipType::cannonBoat, resources::models["Ship_Yamato_Cannon.obj"] });
		shipModels.insert({ ShipType::hedgehogBoat, resources::models["Ship_HMCS_Sackville_Hedgehog.obj"] });

		for (int i = 0; i < 4; i++)
		{
			input::ConstructAnalogEvent("Throttle" + std::to_string(i));
			input::ConstructDigitalEvent("Shoot" + std::to_string(i));

			// Controller input
			input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_A, { "Shoot" + std::to_string(i) });




			input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_A, { "Shoot" + std::to_string(i) });
			input::bindDigitalControllerInput(i, GLFW_KEY_SPACE, { "Shoot" + std::to_string(i) });

			// Keyboard input
			input::bindAnalogInput(GLFW_KEY_UP, input::digitalPositiveInput, { "Throttle" + std::to_string(2) }, 0);
			input::bindAnalogInput(GLFW_KEY_DOWN, input::digitalNegativeInput, { "Throttle" + std::to_string(2) }, 0);



			selectionWindow = engine::ecs::NewEntity();
			arrowUp = engine::ecs::NewEntity();
			arrowDown = engine::ecs::NewEntity();
			shipModel = engine::ecs::NewEntity();
			readyText = engine::ecs::NewEntity();

			vector<engine::ecs::Entity> visuals
			{
				arrowUp,
					arrowDown,
					shipModel,
					readyText
			};
			//engine::ecs::AddComponent(pausedImage, engine::Transform{ .position = Vector3(0,.8f,-0.1f), .scale = Vector3(0.35f) });
			//engine::ecs::AddComponent(pausedImage, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Paused.png"],  .enabled = false, .uiElement = true });
			//engine::ecs::AddComponent(pausedImage, PauseComponent{ .upper = pausedImage, .lower = optionsButton, .selectedTexture = resources::menuTextures["UI_Paused.png"], .unselectedTexture = resources::menuTextures["UI_Paused.png"], .operation = PauseSystem::OnResumePressed });


			engine::ecs::AddComponent(arrowUp, engine::Transform{.position = Vector3(0.5f, 0, -0.1f), .rotation = Vector3(0, 0, 90), .scale = Vector3(0.04f) });
			engine::ecs::AddComponent(arrowUp, engine::SpriteRenderer{.texture = resources::menuTextures["UI_Arrow.png"], .enabled = false, .uiElement = true });

			engine::ecs::AddComponent(arrowDown, engine::Transform{.position = Vector3(-0.5f, 0, -0.1f), .rotation = Vector3(0, 0, -90), .scale = Vector3(0.04f) });
			engine::ecs::AddComponent(arrowDown, engine::SpriteRenderer{.texture = resources::menuTextures["UI_Arrow.png"], .enabled = false, .uiElement = true });

			engine::ecs::AddComponent(shipModel, engine::Transform{ .position = Vector3(0,0,-0.1f) , .scale = 0 });
			engine::ecs::AddComponent(shipModel, engine::ModelRenderer{ .model = shipModels[torpedoBoat] });

			engine::ecs::AddComponent(readyText, engine::Transform{ .position = Vector3(0,-0.2f,-0.1f) , .scale = 0 });
			engine::ecs::AddComponent(readyText, engine::TextRenderer{ .font = resources::niagaraFont, .text = "unready" });


			engine::ecs::AddComponent(selectionWindow, engine::Transform{.position = Vector3(0, 0, 0), .scale = Vector3(1, 1, -0.1f) });
			engine::ecs::AddComponent(selectionWindow, PlayerSelection{ .playerID = i , .visuals = visuals });


			engine::TransformSystem::AddParent(arrowUp, selectionWindow);
			engine::TransformSystem::AddParent(arrowDown, selectionWindow);
			engine::TransformSystem::AddParent(shipModel, selectionWindow);
			engine::TransformSystem::AddParent(readyText, selectionWindow);



		}


	}
	void Update()
	{

		//std::cout << "\n throttleCurrentWaitedTimeUP:" << throttleCurrentWaitedTimeUP << "\n";
		throttleCurrentWaitedTimeUP += engine::deltaTime;
		throttleCurrentWaitedTimeDown += engine::deltaTime;
		for (engine::ecs::Entity entity : entities)
		{
		/*for (auto itr = entities.begin(); itr != entities.end();)
		{
			engine::ecs::Entity entity = *itr++;*/


			//Player& player = ecs::GetComponent<Player>(entity);
			PlayerSelection& playerSelection = engine::ecs::GetComponent<PlayerSelection>(entity);
			if (playerSelection.playerID)
			{

				std::unordered_map<ShipType, engine::Model*>::iterator it;


				//Check players input	
			//	if (playerSelection.playerID == 0)
				{

					float accelerationInput = input::GetTotalInputValue("Throttle" + std::to_string(playerSelection.playerID));

					auto select = input::GetNewPress("Shoot" + std::to_string(playerSelection.playerID));


					if (accelerationInput > 0.5f && throttleCurrentWaitedTimeUP > throttleMoveWaitTime)
					{
						throttleCurrentWaitedTimeUP = 0;
						std::cout << "\n--Ship Selection moveUP--\n";


						engine::ecs::Entity visualShipModelEntity = playerSelection.visuals[2];


						// Iterate through all ship models using the iterator
						for (it = shipModels.begin(); it != shipModels.end(); ++it)
						{
							// it->first is the ShipType
							// it->second is the engine::Model* (the model pointer)

							// You can access the ship type and model pointer using the iterator
							ShipType shipType = it->first;
							engine::Model* model = it->second;

							if (shipType == visualShipModelEntity)
							{
								if (++it == shipModels.end())
								{
									it = shipModels.begin();
									model = it->second;
								}
								else
								{
									++it;
									model = it->second;
								}
								engine::ecs::GetComponent<engine::ModelRenderer>(visualShipModelEntity).model = model;
								break;
							}

						}


					}


					//if(input::GetNewPress("MoveDown" + std::to_string(playerSelection.playerID)))
					if (accelerationInput < -0.5f && throttleCurrentWaitedTimeDown > throttleMoveWaitTime)
					{
						throttleCurrentWaitedTimeDown = 0;

						printf("\n\n\nShip Selection moveDown\n\n\n");
						engine::ecs::Entity visualShipModelEntity = playerSelection.visuals[2];

						playerSelection.selection++;
						if (playerSelection.selection > 4)
							playerSelection.selection = 0;

						//engine::ecs::GetComponent<engine::ModelRenderer>(visualShipModelEntity).model = shipModels[playerSelection.selection];



						// Iterate through all ship models using the iterator
						for (it = shipModels.begin(); it != shipModels.end(); ++it)
						{
							// it->first is the ShipType
							// it->second is the engine::Model* (the model pointer)

							// You can access the ship type and model pointer using the iterator
							ShipType shipType = it->first;
							engine::Model* model = it->second;

							if (it == shipModels.begin())
							{
								it = shipModels.end();

								model = it->second;
							}
							else
							{
								--it;
								model = it->second;
							}
							engine::ecs::GetComponent<engine::ModelRenderer>(visualShipModelEntity).model = model;
							break;
						}
					}
				}
			}


		}



		if (isPlayersReady)
		{
			printf("start game\n");
			//start game implemation...	
		}
	}




	void ToggleMenuPlayerSelection()
	{
		printf("in MenuPlayerSelection\n");
		
		for (engine::ecs::Entity entity : entities)
		{
		

			if (isShipSelectionMenuOn)// Show every thing
			{
				engine::ecs::GetComponent< engine::SpriteRenderer>(arrowUp).enabled = true;
				engine::ecs::GetComponent< engine::SpriteRenderer>(arrowDown).enabled = true;
				engine::ecs::GetComponent< engine::Transform>(shipModel).scale = 0.1;

				//.-.-.-.-.-.-.-.-.-TODO::  SHIP MODEL NOT CHANGE OR SHOW UP and Ready text missing too .-.-.-.-.-.-.-.-.-
				engine::ecs::GetComponent< engine::TextRenderer>(readyText).text = "UNready";
				engine::ecs::GetComponent< engine::TextRenderer>(readyText).color = Vector3(255, 0, 0);
				engine::ecs::GetComponent< engine::Transform>(readyText).scale = 0.003f;
				
				std::cout << "\nisShipSelectionMenuOn: " << isShipSelectionMenuOn << "\n";
			}

			if (!isShipSelectionMenuOn)	//hide every thing
			{

				engine::ecs::GetComponent< engine::SpriteRenderer>(arrowUp).enabled = false;
				engine::ecs::GetComponent< engine::SpriteRenderer>(arrowDown).enabled = false;
				engine::ecs::GetComponent< engine::Transform>(shipModel).scale = 0;

				engine::ecs::GetComponent< engine::TextRenderer>(readyText).text = "UNready";
				engine::ecs::GetComponent< engine::TextRenderer>(readyText).scale = Vector3(1, 1, 1);
				std::cout << "\nisShipSelectionMenuOn: " << isShipSelectionMenuOn << "\n";
			}

			printf("in MenuPlayerSelection for loop\n");
		}
		printf("Out MenuPlayerSelection\n\n\n");
	};
	static void BackToUIMenu()
	{
		engine::ecs::GetSystem<PlayerSelectSystem>()->ToggleMenuPlayerSelection();

	}
};


ECS_REGISTER_COMPONENT(PauseComponent)
struct PauseComponent
{
	engine::ecs::Entity upper, lower;
	engine::Texture* selectedTexture;
	engine::Texture* unselectedTexture;
	std::function<void()> operation;
	bool isOptionsMenu = false;
	bool isSlider = false;
	float sliderValue = 0;
};

ECS_REGISTER_SYSTEM(PauseSystem, PauseComponent, engine::Transform)
class PauseSystem : public engine::ecs::System
{
	engine::ecs::Entity pausedImage;

	engine::ecs::Entity resumeButton;
	engine::ecs::Entity optionsButton;

	engine::ecs::Entity mainMenuButton;

	engine::ecs::Entity quitGameButton;

	//std::map<std::string, ecs::Entity> pauseButtons{ {"return", resumeButton}, {"options", optionsButton}, {"mainMenu", mainMenuButton},{"quitGame", quitGameButton} };

	//options buttons
	engine::ecs::Entity optionsResumeButton;
	engine::ecs::Entity musicSliderEntity, musicSliderNub;;
	engine::ecs::Entity fullscreenEntity;

	vector<engine::ecs::Entity>  optionsButtons = { {optionsResumeButton},{musicSliderEntity}, {fullscreenEntity} };
	//std::map<std::string, ecs::Entity> optionsButtons{ {"return", optionsResumeButton}, {"music", musicSliderEntity}, {"fullscreen", fullscreenEntity} };

	static GLFWwindow* window;
	float moveWaitedTimerUP;
	float moveWaitedTimerDown;
	const  float  delay = 0.8f;

public:

	bool isGamePause = false;
	engine::ecs::Entity currentSelection;

	void Init(GLFWwindow* mainWindow)
	{

	/*	int KeyboardPlayer = 2;

		input::bindAnalogInput(GLFW_KEY_RIGHT, input::digitalPositiveInput, { "Turn" + std::to_string(KeyboardPlayer) }, 0);
		input::bindAnalogInput(GLFW_KEY_LEFT, input::digitalNegativeInput, { "Turn" + std::to_string(KeyboardPlayer) }, 0);

		input::bindAnalogInput(GLFW_KEY_A, input::digitalPositiveInput, { "Throttle" + std::to_string(KeyboardPlayer) }, 0);
		input::bindAnalogInput(GLFW_KEY_Z, input::digitalNegativeInput, { "Throttle" + std::to_string(KeyboardPlayer) }, 0);
		input::bindAnalogInput(GLFW_KEY_UP, input::digitalPositiveInput, { "Throttle" + std::to_string(KeyboardPlayer) }, 0);
		input::bindAnalogInput(GLFW_KEY_DOWN, input::digitalNegativeInput, { "Throttle" + std::to_string(KeyboardPlayer) }, 0);

		input::bindDigitalInput(GLFW_KEY_N, { "Shoot" + std::to_string(KeyboardPlayer) });
		input::bindDigitalInput(GLFW_KEY_M, { "Boost" + std::to_string(KeyboardPlayer) });*/


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


		resumeButton = engine::ecs::NewEntity();
		optionsButton = engine::ecs::NewEntity();
		mainMenuButton = engine::ecs::NewEntity();
		quitGameButton = engine::ecs::NewEntity();

		optionsResumeButton = engine::ecs::NewEntity();
		musicSliderEntity = engine::ecs::NewEntity();
		fullscreenEntity = engine::ecs::NewEntity();


		musicSliderNub = engine::ecs::NewEntity();
		currentSelection = mainMenuButton;

		pausedImage = engine::ecs::NewEntity();


		//                      ---------  PAUSE BUTTONS init-------
		engine::ecs::AddComponent(pausedImage, engine::Transform{.position = Vector3(0, .8f, -0.1f), .scale = Vector3(0.35f) });
		engine::ecs::AddComponent(pausedImage, engine::SpriteRenderer{.texture = resources::menuTextures["UI_Paused.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(pausedImage, PauseComponent{ .upper = pausedImage, .lower = optionsButton, .selectedTexture = resources::menuTextures["UI_Paused.png"], .unselectedTexture = resources::menuTextures["UI_Paused.png"], .operation = PauseSystem::OnResumePressed });


		engine::ecs::AddComponent(resumeButton, engine::Transform{.position = Vector3(0, .5f, -0.1f), .scale = Vector3(0.25f) });
		engine::ecs::AddComponent(resumeButton, engine::SpriteRenderer{.texture = resources::menuTextures["UI_Resume_N.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(resumeButton, PauseComponent{ .upper = quitGameButton, .lower = optionsButton, .selectedTexture = resources::menuTextures["UI_Resume.png"], .unselectedTexture = resources::menuTextures["UI_Resume_N.png"], .operation = PauseSystem::OnResumePressed });

		engine::ecs::AddComponent(optionsButton, engine::Transform{.position = Vector3(0, .3f, -0.1f), .scale = Vector3(.25f) });
		engine::ecs::AddComponent(optionsButton, engine::SpriteRenderer{.texture = resources::menuTextures["UI_Options_N.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(optionsButton, PauseComponent{ .upper = resumeButton, .lower = mainMenuButton, .selectedTexture = resources::menuTextures["UI_Options.png"], .unselectedTexture = resources::menuTextures["UI_Options_N.png"], .operation = PauseSystem::OnOptionsPressed });

		engine::ecs::AddComponent(mainMenuButton, engine::Transform{.position = Vector3(0, .1f, -0.1f), .scale = Vector3(0.25f) });
		engine::ecs::AddComponent(mainMenuButton, engine::SpriteRenderer{.texture = resources::menuTextures["UI_BackToMenu_N.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(mainMenuButton, PauseComponent{ .upper = optionsButton, .lower = quitGameButton, .selectedTexture = resources::menuTextures["UI_BackToMenu.png"], .unselectedTexture = resources::menuTextures["UI_BackToMenu_N.png"], .operation = PauseSystem::OnMainMenuPressed });

		engine::ecs::AddComponent(quitGameButton, engine::Transform{.position = Vector3(0, -0.7f, -0.1f), .scale = Vector3(0.25f) });
		engine::ecs::AddComponent(quitGameButton, engine::SpriteRenderer{.texture = resources::menuTextures["UI_QuitGame_N.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(quitGameButton, PauseComponent{ .upper = mainMenuButton, .lower = resumeButton, .selectedTexture = resources::menuTextures["UI_QuitGame.png"], .unselectedTexture = resources::menuTextures["UI_QuitGame_N.png"], .operation = PauseSystem::OnQuitGamePressed });


		////                      ---------  OPTIONS BUTTONS init --------
		//optionsResumeButton
		engine::ecs::AddComponent(optionsResumeButton, engine::Transform{.position = Vector3(0, .3f, -0.1f), .scale = Vector3(.25f) });
		engine::ecs::AddComponent(optionsResumeButton, engine::SpriteRenderer{.texture = resources::menuTextures["UI_Resume.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(optionsResumeButton, PauseComponent{ .upper = fullscreenEntity, .lower = musicSliderEntity, .selectedTexture = resources::menuTextures["UI_Resume.png"], .unselectedTexture = resources::menuTextures["UI_Resume_N.png"], .operation = BackToUIMenu, .isOptionsMenu = true });
		//musicSliderEntity
		engine::ecs::AddComponent(musicSliderEntity, engine::Transform{.position = Vector3(0, .1f, -0.1f), .scale = Vector3(0.25f) });
		engine::ecs::AddComponent(musicSliderEntity, engine::SpriteRenderer{.texture = resources::menuTextures["UI_Music_Slider.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(musicSliderEntity, PauseComponent{ .upper = optionsResumeButton, .lower = fullscreenEntity, .selectedTexture = resources::menuTextures["UI_Music_Slider.png"], .unselectedTexture = resources::menuTextures["UI_Music_Slider_N.png"], .operation = PauseSystem::OnMainMenuPressed, .isOptionsMenu = true,.isSlider = true });
		//fullscreenEntity
		engine::ecs::AddComponent(fullscreenEntity, engine::Transform{.position = Vector3(0, -0.7f, -0.1f), .scale = Vector3(0.25f) });
		engine::ecs::AddComponent(fullscreenEntity, engine::SpriteRenderer{.texture = resources::menuTextures["UI_Fullscreen.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(fullscreenEntity, PauseComponent{ .upper = musicSliderEntity, .lower = optionsResumeButton, .selectedTexture = resources::menuTextures["UI_Fullscreen.png"], .unselectedTexture = resources::menuTextures["UI_Fullscreen_N.png"], .operation = PauseSystem::OnQuitGamePressed, .isOptionsMenu = true });
		//musicSliderNub
		engine::ecs::AddComponent(musicSliderNub, engine::Transform{.position = engine::ecs::GetComponent<engine::Transform>(musicSliderEntity).position + Vector3(0, -0.2f, -0.1), .scale = Vector3(0.15f) });
		engine::ecs::AddComponent(musicSliderNub, engine::SpriteRenderer{.texture = resources::menuTextures["UI_Slider_Button.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(musicSliderNub, PauseComponent{ .selectedTexture = resources::menuTextures["UI_Slider_Button.png"], .unselectedTexture = resources::menuTextures["UI_Slider_Button.png"], .isOptionsMenu = true,.isSlider = true });

	}

	void Update()
	{


		//printf("IN MENU SYSTEM UPDATE()\n");
		if (input::GetNewPress("Pause"))
		{
			printf("Pause\n");
			isGamePause = !isGamePause;
			ToggleShowUIMenu();
		}
		if (engine::ecs::GetSystem<PauseSystem>()->isGamePause)
		{

			for (size_t i = 0; i < 4; i++)
			{
				float accelerationInput = input::GetTotalInputValue("Throttle" + std::to_string(i));

				//input::ConstructAnalogEvent("Throttle" + std::to_string(i));
				//input::ConstructAnalogEvent("Turn" + std::to_string(i));

				//input::ConstructDigitalEvent("Shoot" + std::to_string(i));
				//input::ConstructDigitalEvent("Boost" + std::to_string(i));

				//// Controller input
				//input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_A, { "Shoot" + std::to_string(i) });
				//input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_B, { "Boost" + std::to_string(i) });

				//input::bindAnalogControllerInput(i,
				//	{
				//		{ input::digitalPositiveInput, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER },
				//		{ input::digitalNegativeInput, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER }
				//	}, { "Throttle" + std::to_string(i) });

				//input::bindAnalogControllerInput(i, { { input::controllerMixedInput, GLFW_GAMEPAD_AXIS_LEFT_X }, }, { "Turn" + std::to_string(i) });

				if (i != 2)
				{

					if (accelerationInput >= 0.5f)
					{
						std::cout << "upTimer" << moveWaitedTimerUP << "\n";
						moveWaitedTimerUP += engine::deltaTime;

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
					if (accelerationInput <= -0.5f)
					{
						std::cout << "downTimer" << moveWaitedTimerUP << "\n";
						moveWaitedTimerUP += engine::deltaTime;

						if (moveWaitedTimerUP >= delay)
						{
							printf("\n\n move down input\n\n");
							MoveLower();
							moveWaitedTimerDown = 0;
						}

					}
					else
					{
						moveWaitedTimerDown = 0;
					}
				}

				if (i == 2)   //KEYBOARD INPUT IN OPTIONS
				{

					//moveUP

					if (input::GetNewPress("Throttle2") >= 0.5f)
					{
						MoveUpper();

					}

					//moveDown
					if (input::GetNewPress("Throttle2") <= -0.5f)
					{
						MoveLower();

					}

				}

			}

			if (IsCurrentPauseComponentSlider())
			{

				if (input::GetNewPress("MoveLeft"))
				{
					/*this dosent works*///UpdateSlider();

					MoveSliderLeft();
				}
				if (input::GetNewPress("MoveRight"))
				{
					MoveSliderRight();
					//selectedPauseComponent.sliderValue += -0.01;
					/*this dosent works*///UpdateSlider();
				}
			}
			if (input::GetNewPress("Select"))
			{
				Selected();
			}
		}
	}


	void AddButton(engine::ecs::Entity entity, Vector3 pos, engine::Texture selectedTexture, engine::Texture unselectedTexture, Vector3 scaleNormal, Vector3 scaleSelected)
	{
		engine::ecs::AddComponent(fullscreenEntity, engine::Transform{.position = Vector3(0.7, -0.7f, -0.1f), .scale = Vector3(0.25f) });
		engine::ecs::AddComponent(fullscreenEntity, engine::SpriteRenderer{.texture = resources::menuTextures["UI_Fullscreen_N.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(fullscreenEntity, PauseComponent{ .upper = mainMenuButton, .lower = resumeButton, .selectedTexture = resources::menuTextures["UI_Fullscreen_N.png"], .unselectedTexture = resources::menuTextures["UI_Fullscreen.png"], .operation = PauseSystem::OnQuitGamePressed });
	}
	void MoveUpper()
	{
		printf("Move upper\n");
		PauseComponent& pauseComponent = engine::ecs::GetComponent<PauseComponent>(currentSelection);
		engine::SpriteRenderer& unselectedSpriteRenderer = engine::ecs::GetComponent<engine::SpriteRenderer>(currentSelection);
		engine::Transform& unselectedTransform = engine::ecs::GetComponent<engine::Transform>(currentSelection);

		unselectedTransform.scale = Vector3(0.25f);
		unselectedSpriteRenderer.texture = pauseComponent.unselectedTexture;

		currentSelection = pauseComponent.upper;
		PauseComponent& pauseComponent1 = engine::ecs::GetComponent<PauseComponent>(currentSelection);
		engine::SpriteRenderer& selectedSpriteRenderer = engine::ecs::GetComponent<engine::SpriteRenderer>(currentSelection);
		engine::Transform& selectedSpriteTransform = engine::ecs::GetComponent<engine::Transform>(currentSelection);
		selectedSpriteTransform.scale = Vector3(0.32f);
		selectedSpriteRenderer.texture = pauseComponent1.selectedTexture;


	}
	void MoveLower()
	{
		printf("Move lower\n");
		PauseComponent& pauseComponent = engine::ecs::GetComponent<PauseComponent>(currentSelection);
		engine::SpriteRenderer& unselectedSpriteRenderer = engine::ecs::GetComponent<engine::SpriteRenderer>(currentSelection);
		engine::Transform& unselectedTransform = engine::ecs::GetComponent<engine::Transform>(currentSelection);

		unselectedTransform.scale = Vector3(0.25f);
		unselectedSpriteRenderer.texture = pauseComponent.unselectedTexture;

		currentSelection = pauseComponent.lower;
		PauseComponent& pauseComponent1 = engine::ecs::GetComponent<PauseComponent>(currentSelection);
		engine::SpriteRenderer& selectedSpriteRenderer = engine::ecs::GetComponent<engine::SpriteRenderer>(currentSelection);
		engine::Transform& selectedSpriteTransform = engine::ecs::GetComponent<engine::Transform>(currentSelection);
		selectedSpriteTransform.scale = Vector3(0.32f);
		selectedSpriteRenderer.texture = pauseComponent1.selectedTexture;
	}
	bool IsCurrentPauseComponentSlider()
	{
		return engine::ecs::GetComponent<PauseComponent>(currentSelection).isSlider;
	}
	PauseComponent& GetCurrentSelectedPauseComponent()
	{
		return engine::ecs::GetComponent<PauseComponent>(currentSelection);
	}
	engine::ecs::Entity GetCurrentSelection()
	{
		return currentSelection;
	}
	static void BackToUIMenu()
	{
		engine::ecs::GetSystem<PauseSystem>()->ToggleShowUIMenu();
	}
	static  void OnResumePressed()
	{
		printf("OnResumePressed()\n");

		engine::ecs::GetSystem<PauseSystem>()->isGamePause = !engine::ecs::GetSystem<PauseSystem>()->isGamePause;
		engine::ecs::GetSystem<PauseSystem>()->ToggleShowUIMenu();
	}
	static  void OnOptionsPressed()
	{
		printf("open options in pause menu\n");

		engine::ecs::GetSystem<PauseSystem>()->ToggleShowUIOptionsMenu();
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
	void Selected()
	{

		engine::deltaTime;

		PauseComponent& pauseComponent = engine::ecs::GetComponent<PauseComponent>(currentSelection);
		engine::SpriteRenderer& pauseSpriteRenderer = engine::ecs::GetComponent<engine::SpriteRenderer>(currentSelection);
		pauseSpriteRenderer.texture = pauseComponent.selectedTexture;
		pauseComponent.operation();
	}

	void ToggleShowUIMenu()
	{
		printf("menu UI\n");
		for (engine::ecs::Entity entity : entities)
		{
			bool& isOptionsMenu = engine::ecs::GetComponent<PauseComponent>(entity).isOptionsMenu;
			bool& enabled = engine::ecs::GetComponent<engine::SpriteRenderer>(entity).enabled;
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

		PauseComponent& pauseComponent = engine::ecs::GetComponent<PauseComponent>(currentSelection);
		engine::SpriteRenderer& selectedSpriteRenderer = engine::ecs::GetComponent<engine::SpriteRenderer>(currentSelection);
		engine::Transform& selectedSpriteTransform = engine::ecs::GetComponent<engine::Transform>(currentSelection);
		selectedSpriteTransform.scale = Vector3(0.32f);
		selectedSpriteRenderer.texture = pauseComponent.selectedTexture;

	}
	void SetCurrentSelection(engine::ecs::Entity entity)
	{
		PauseComponent& pauseComponent = engine::ecs::GetComponent<PauseComponent>(entity);
		engine::SpriteRenderer& selectedSpriteRenderer = engine::ecs::GetComponent<engine::SpriteRenderer>(entity);
		engine::Transform& selectedSpriteTransform = engine::ecs::GetComponent<engine::Transform>(entity);
		selectedSpriteTransform.scale = Vector3(0.32f);
		selectedSpriteRenderer.texture = pauseComponent.selectedTexture;
	}

	void UpdateSlider()
	{
		// TODO: change 	sliderValue dosent update in main Fix need pointer or reference
		PauseComponent& pauseComponent = engine::ecs::GetComponent<PauseComponent>(currentSelection);
		engine::Transform& selectedSliderTransform = engine::ecs::GetComponent<engine::Transform>(currentSelection);
		PauseComponent& pauseComponentNub = engine::ecs::GetComponent<PauseComponent>(musicSliderNub);
		engine::Transform& nubTransform = engine::ecs::GetComponent<engine::Transform>(musicSliderNub);
		nubTransform.position = Vector3(pauseComponentNub.sliderValue, 0, 0);
		//printf("pauseComponentNub.sliderValue ");
		std::cout << pauseComponentNub.sliderValue;

		//std::cout << "Nub pos:" << "x:" << nubTransform.position.x << " y:" << nubTransform.position.y << "\n";

	}

	void MoveSliderRight()
	{
		PauseComponent& pauseComponentNub = engine::ecs::GetComponent<PauseComponent>(musicSliderNub);
		engine::Transform& nubTransform = engine::ecs::GetComponent<engine::Transform>(musicSliderNub);
		nubTransform.position += Vector3(0.01f, 0, 0);
		nubTransform.position.x = clamp(nubTransform.position.x, -0.17f, 0.17f);
		//printf("pauseComponentNub.sliderValue ");
		std::cout << pauseComponentNub.sliderValue;

		//std::cout << "Nub pos:" << "x:" << nubTransform.position.x << " y:" << nubTransform.position.y << "\n";
	}
	void MoveSliderLeft()
	{
		PauseComponent& pauseComponentNub = engine::ecs::GetComponent<PauseComponent>(musicSliderNub);
		engine::Transform& nubTransform = engine::ecs::GetComponent<engine::Transform>(musicSliderNub);
		nubTransform.position -= Vector3(0.01f, 0, 0);
		nubTransform.position.x = clamp(nubTransform.position.x, -0.17f, 0.17f);

		std::cout << pauseComponentNub.sliderValue;

		//std::cout << "Nub pos:" << "x:" << nubTransform.position.x << " y:" << nubTransform.position.y << "\n";

	}
	void ToggleShowUIOptionsMenu()
	{
		printf("options menu UI\n");

		PauseComponent& pauseComponentOld = engine::ecs::GetComponent<PauseComponent>(currentSelection);
		engine::SpriteRenderer& selectedSpriteRendererOld = engine::ecs::GetComponent<engine::SpriteRenderer>(currentSelection);
		engine::Transform& selectedSpriteTransformOld = engine::ecs::GetComponent<engine::Transform>(currentSelection);
		selectedSpriteTransformOld.scale = Vector3(0.25f);
		selectedSpriteRendererOld.texture = pauseComponentOld.unselectedTexture;

		currentSelection = optionsResumeButton;

		for (engine::ecs::Entity entity : entities)
		{
			bool& isOptionsMenu = engine::ecs::GetComponent<PauseComponent>(entity).isOptionsMenu;

			bool& enabled = engine::ecs::GetComponent<engine::SpriteRenderer>(entity).enabled;
			PauseComponent& pauseComponent = engine::ecs::GetComponent<PauseComponent>(entity);
			engine::SpriteRenderer& spriteRenderer = engine::ecs::GetComponent<engine::SpriteRenderer>(entity);
			spriteRenderer.texture = pauseComponent.unselectedTexture;

			enabled = !enabled;
		}

		PauseComponent& pauseComponent = engine::ecs::GetComponent<PauseComponent>(currentSelection);
		engine::SpriteRenderer& selectedSpriteRenderer = engine::ecs::GetComponent<engine::SpriteRenderer>(currentSelection);
		engine::Transform& selectedSpriteTransform = engine::ecs::GetComponent<engine::Transform>(currentSelection);
		selectedSpriteTransform.scale = Vector3(0.32f);
		selectedSpriteRenderer.texture = pauseComponent.selectedTexture;
	}
};
GLFWwindow* PauseSystem::window = window;



