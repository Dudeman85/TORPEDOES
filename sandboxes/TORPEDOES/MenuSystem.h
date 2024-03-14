#pragma once
//Author: Sauli Hanell month.day.year 2.9.2024

#include <engine/Application.h>
#include <functional>
#include "engine/Input.h"  

struct ButtonData
{
	int  id;

	std::string name;
	Vector3 normalScale;
	Vector3 SelectedScale;
	Texture* selected, unselected;
	std::function<void()>callback;
};
//PlayerShipSelection
ECS_REGISTER_COMPONENT(PlayerSelection)
struct PlayerSelection
{

	int playerID;
	int selection;
	float cooldownToMoveSelection;
	bool ready = false;
	vector<ecs::Entity> visuals;

};

ECS_REGISTER_SYSTEM(PlayerSelectSystem, PlayerSelection, Transform)
class PlayerSelectSystem : public ecs::System
{
	const float cooldownTime = 2;
	//ecs::Entity playerSelection;

	bool isPlayersReady = false;
	vector<Texture*> textures;

	ecs::Entity selectionWindow;

	ecs::Entity arrowRight;
	ecs::Entity arrowLeft;
	ecs::Entity	shipModel;
	ecs::Entity	readyText;

public:
	~PlayerSelectSystem()
	{

	}

	bool isShipSelectionMenuOn = false;

	vector< std::function<void()> >shipButtonFunctions
	{
		BackToUIMenu

	};
	void Init()
	{
		textures.push_back(new engine::Texture("GUI/UI_Green_Torpedo_Icon.png", GL_LINEAR_MIPMAP_NEAREST));
		textures.push_back(new engine::Texture("GUI/UI_Checkmark.png", GL_LINEAR_MIPMAP_NEAREST));


		input::ConstructDigitalEvent("MoveUp");
		input::ConstructDigitalEvent("MoveDown");
		input::ConstructDigitalEvent("Select");
		input::ConstructDigitalEvent("Pause");
		input::ConstructDigitalEvent("MoveRight");
		input::ConstructDigitalEvent("MoveLeft");


		input::ConstructDigitalEvent("Shoot0");
		input::ConstructDigitalEvent("Shoot1");
		input::ConstructDigitalEvent("Shoot2");
		input::ConstructDigitalEvent("Shoot3");

		input::ConstructDigitalEvent("MoveRight0");
		input::ConstructDigitalEvent("MoveLeft0");
		input::ConstructDigitalEvent("MoveRight1");
		input::ConstructDigitalEvent("MoveLeft1");
		input::ConstructDigitalEvent("MoveRight2");
		input::ConstructDigitalEvent("MoveLeft2");
		input::ConstructDigitalEvent("MoveRight3");
		input::ConstructDigitalEvent("MoveLeft3");

		input::ConstructDigitalEvent("MoveUp0");
		input::ConstructDigitalEvent("MoveDown0");
		input::ConstructDigitalEvent("MoveUp1");
		input::ConstructDigitalEvent("MoveDown1");
		input::ConstructDigitalEvent("MoveUp2");
		input::ConstructDigitalEvent("MoveDown2");
		input::ConstructDigitalEvent("MoveUp3");
		input::ConstructDigitalEvent("MoveDown3");

		input::ConstructDigitalEvent("SelectShip");
		//TODO: add controller input
		input::bindDigitalInput(GLFW_KEY_SPACE, { "Shoot0" });
		input::bindDigitalInput(GLFW_KEY_1, { "Shoot1" });
		input::bindDigitalInput(GLFW_KEY_2, { "Shoot2" });
		input::bindDigitalInput(GLFW_KEY_3, { "Shoot3" });

		input::bindDigitalInput(GLFW_KEY_LEFT, { "MoveLeft" });
		input::bindDigitalInput(GLFW_KEY_RIGHT, { "MoveRight" });
		input::bindDigitalInput(GLFW_KEY_UP, { "MoveUp0" });
		input::bindDigitalInput(GLFW_KEY_DOWN, { "MoveDown0" });
		input::bindDigitalInput(GLFW_KEY_ENTER, { "Select" });
		input::bindDigitalInput(GLFW_KEY_P, { "Pause" });
		input::bindDigitalInput(GLFW_KEY_U, { "SelectShip" });

		for (int i = 0; i < 4; i++)
		{

			selectionWindow = ecs::NewEntity();
			arrowRight = ecs::NewEntity();
			arrowLeft = ecs::NewEntity();
			shipModel = ecs::NewEntity();
			readyText = ecs::NewEntity();

			vector<ecs::Entity> visuals
			{
				arrowRight,
				arrowLeft,
				shipModel,
				readyText
			};

			ecs::AddComponent(arrowRight, Transform{ .position = Vector3(0.5f,0,-0.1f), .rotation = 180, .scale = Vector3(0.2f) });
			ecs::AddComponent(arrowRight, SpriteRenderer{ .texture = textures[0] , .enabled = false, .uiElement = true });
			ecs::AddComponent(arrowLeft, Transform{ .position = Vector3(-0.5f,0,-0.1f), .rotation = 0 , .scale = Vector3(0.2f) });
			ecs::AddComponent(arrowLeft, SpriteRenderer{ .texture = textures[0] , .enabled = false, .uiElement = true });

			ecs::AddComponent(shipModel, Transform{ .position = Vector3(0,-0.2f,-0.1f) , .scale = 0 });
			ecs::AddComponent(shipModel, ModelRenderer{ .model = resources::laMuerteModel });

			ecs::AddComponent(readyText, Transform{ .position = Vector3(0,-0.2f,-0.1f) });
			ecs::AddComponent(readyText, TextRenderer{ .font = resources::niagaraFont, .text = "unready" });


			ecs::AddComponent(selectionWindow, Transform{ .position = Vector3(0,0,0), .scale = Vector3(1,1,-0.1f) });
			ecs::AddComponent(selectionWindow, PlayerSelection{ .playerID = i , .visuals = visuals });


			TransformSystem::AddParent(arrowRight, selectionWindow);
			TransformSystem::AddParent(arrowLeft, selectionWindow);
			TransformSystem::AddParent(shipModel, selectionWindow);
			TransformSystem::AddParent(readyText, selectionWindow);



		}


	}
	void Update()
	{


		for (auto itr = entities.begin(); itr != entities.end();)
		{
			ecs::Entity entity = *itr++;


			//Player& player = ecs::GetComponent<Player>(entity);
			PlayerSelection& playerSelection = ecs::GetComponent<PlayerSelection>(entity);
			if (playerSelection.playerID)
			{
				std::cout << "\n\n\nplayerSelection.playerID" << playerSelection.playerID << "\n\n\n";

				//Check players input	
				// 
				if (playerSelection.playerID == 0)
				{


					if (input::GetNewPress("MoveUp" + playerSelection.playerID))
					{
						printf("\n\n\nShip Selection moveUP\n\n\n");

					}

					if (input::GetNewPress("MoveDown" + playerSelection.playerID))
					{
						printf("\n\n\nShip Selection moveDown\n\n\n");
					}
				}
				/*	if (input::GetNewPress("Shoot"+ id))
					{
						printf("Ship Selection Shoot\n");
					}*/

			}



		}
		if (isPlayersReady)
		{	//start game	
			printf("start game\n");
		}


	}
	void ToggleMenuPlayerSelection()
	{
		printf("in MenuPlayerSelection\n");
		//isShipSelectionMenuOn ? isShipSelectionMenuOn = false : true;
		/*isShipSelectionMenuOn =  true;*/
		for (auto itr = entities.begin(); itr != entities.end();)
		{
			//Get the entity and increment the iterator
			ecs::Entity entity = *itr++;

			if (isShipSelectionMenuOn)// Show every thing
			{
				ecs::GetComponent<SpriteRenderer>(arrowRight).enabled = true;
				ecs::GetComponent<SpriteRenderer>(arrowLeft).enabled = true;
				ecs::GetComponent<Transform>(shipModel).scale = Vector3(1, 1, -0.1f);

				ecs::GetComponent<TextRenderer>(readyText).text = "UNready";

			}

			if (!isShipSelectionMenuOn)	//hide every thing
			{

				ecs::GetComponent<SpriteRenderer>(arrowRight).enabled = false;
				ecs::GetComponent<SpriteRenderer>(arrowLeft).enabled = false;
				ecs::GetComponent<Transform>(shipModel).scale = Vector3(0, 0, -0.1f);

				ecs::GetComponent<TextRenderer>(readyText).text = "UNready";

			}

			printf("in MenuPlayerSelection for loop\n");
		}
		printf("Out MenuPlayerSelection\n\n\n");
	};
	static void BackToUIMenu()
	{
		ecs::GetSystem<PlayerSelectSystem>()->ToggleMenuPlayerSelection();

	}

};

//Pause Component
ECS_REGISTER_COMPONENT(PauseComponent)
struct PauseComponent
{
	ecs::Entity upper, lower;
	engine::Texture* selectedTexture;
	engine::Texture* unselectedTexture;
	std::function<void()> operation;
	bool isOptionsMenu=false;
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

	engine::Texture* currentSelected_Texture;

	vector<Texture*> allTextures;
	static GLFWwindow* window;




public:
	ecs::Entity currentSelection;
	bool isGamePause;

	~PauseSystem() {
		delete currentSelected_Texture;
		//TODO DELETE ALL TEXTURES
		for (Texture* texture : allTextures)
		{
			delete texture;
		}
	}
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
			if (input::GetNewPress("MoveUp"))
			{
				MoveUpper();
			}
			if (input::GetNewPress("MoveDown"))
			{
				MoveLower();
			}

			if (IsCurrentPauseComponentSlider())
			{								

				if (input::GetNewPress("MoveRight"))
				{										
					/*this dosent works*///UpdateSlider();
					MoveSliderRight();
					
				}
				if (input::GetNewPress("MoveLeft"))
				{

					//selectedPauseComponent.sliderValue += -0.01;
					/*this dosent works*///UpdateSlider();
					MoveSliderLeft();
					

				}

			}
			if (input::GetNewPress("Select"))
			{
				Selected();
			}

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




		//Odd texture index is always ' _N ' and even is just default name
		//Pausemenu button textures
		allTextures.push_back(new engine::Texture("menuUI/UI_Resume_N.png", GL_LINEAR_MIPMAP_NEAREST));
		allTextures.push_back(new engine::Texture("menuUI/UI_Resume.png", GL_LINEAR_MIPMAP_NEAREST));

		allTextures.push_back(new engine::Texture("menuUI/UI_Options_N.png", GL_LINEAR_MIPMAP_NEAREST));
		allTextures.push_back(new engine::Texture("menuUI/UI_Options.png", GL_LINEAR_MIPMAP_NEAREST));

		allTextures.push_back(new engine::Texture("menuUI/UI_BackToMenu_N.png", GL_LINEAR_MIPMAP_NEAREST));
		allTextures.push_back(new engine::Texture("menuUI/UI_BackToMenu.png", GL_LINEAR_MIPMAP_NEAREST));

		allTextures.push_back(new engine::Texture("menuUI/UI_QuitGame_N.png", GL_LINEAR_MIPMAP_NEAREST));
		allTextures.push_back(new engine::Texture("menuUI/UI_QuitGame.png", GL_LINEAR_MIPMAP_NEAREST));

		//Options buttons  textures
		allTextures.push_back(new engine::Texture("menuUI/UI_Resume.png", GL_LINEAR_MIPMAP_NEAREST));              // 	9
		allTextures.push_back(new engine::Texture("menuUI/UI_Resume_N.png", GL_LINEAR_MIPMAP_NEAREST));            // 8

		allTextures.push_back(new engine::Texture("menuUI/UI_Music_Slider.png", GL_LINEAR_MIPMAP_NEAREST));        // 11
		allTextures.push_back(new engine::Texture("menuUI/UI_Music_Slider_N.png", GL_LINEAR_MIPMAP_NEAREST));      // 10

		allTextures.push_back(new engine::Texture("menuUI/UI_Fullscreen.png", GL_LINEAR_MIPMAP_NEAREST));          // 13
		allTextures.push_back(new engine::Texture("menuUI/UI_Fullscreen_N.png", GL_LINEAR_MIPMAP_NEAREST));        // 12

		//PAUSED image
		allTextures.push_back(new engine::Texture("menuUI/UI_Paused.png", GL_LINEAR_MIPMAP_NEAREST));              // 14

		allTextures.push_back(new engine::Texture("menuUI/UI_Slider_Button.png", GL_LINEAR_MIPMAP_NEAREST));       // 15

		//                      ---------  PAUSE BUTTONS init-------
		ecs::AddComponent(pausedImage, Transform{ .position = Vector3(0,.8f,-0.1f), .scale = Vector3(0.35f) });
		ecs::AddComponent(pausedImage, SpriteRenderer{ .texture = allTextures[14],  .enabled = false, .uiElement = true });
		ecs::AddComponent(pausedImage, PauseComponent{ .upper = pausedImage, .lower = optionsButton, .selectedTexture = allTextures[14], .unselectedTexture = allTextures[14], .operation = PauseSystem::OnResumePressed });


		ecs::AddComponent(resumeButton, Transform{ .position = Vector3(0,.5f,-0.1f), .scale = Vector3(0.25f) });
		ecs::AddComponent(resumeButton, SpriteRenderer{ .texture = allTextures[0],  .enabled = false, .uiElement = true });
		ecs::AddComponent(resumeButton, PauseComponent{ .upper = quitGameButton, .lower = optionsButton, .selectedTexture = allTextures[1], .unselectedTexture = allTextures[0], .operation = PauseSystem::OnResumePressed });

		ecs::AddComponent(optionsButton, Transform{ .position = Vector3(0,.3f,-0.1f), .scale = Vector3(.25f) });
		ecs::AddComponent(optionsButton, SpriteRenderer{ .texture = allTextures[2],  .enabled = false, .uiElement = true });
		ecs::AddComponent(optionsButton, PauseComponent{ .upper = resumeButton, .lower = mainMenuButton, .selectedTexture = allTextures[3], .unselectedTexture = allTextures[2], .operation = PauseSystem::OnOptionsPressed });

		ecs::AddComponent(mainMenuButton, Transform{ .position = Vector3(0,.1f,-0.1f), .scale = Vector3(0.25f) });
		ecs::AddComponent(mainMenuButton, SpriteRenderer{ .texture = allTextures[4],  .enabled = false, .uiElement = true });
		ecs::AddComponent(mainMenuButton, PauseComponent{ .upper = optionsButton, .lower = quitGameButton, .selectedTexture = allTextures[5], .unselectedTexture = allTextures[4], .operation = PauseSystem::OnMainMenuPressed });

		ecs::AddComponent(quitGameButton, Transform{ .position = Vector3(0,-0.7f,-0.1f), .scale = Vector3(0.25f) });
		ecs::AddComponent(quitGameButton, SpriteRenderer{ .texture = allTextures[6],  .enabled = false, .uiElement = true });
		ecs::AddComponent(quitGameButton, PauseComponent{ .upper = mainMenuButton, .lower = resumeButton, .selectedTexture = allTextures[7], .unselectedTexture = allTextures[6], .operation = PauseSystem::OnQuitGamePressed });



		////                      ---------  OPTIONS BUTTONS init --------
		//optionsResumeButton
		ecs::AddComponent(optionsResumeButton, Transform{ .position = Vector3(0,.3f,-0.1f), .scale = Vector3(.25f) });
		ecs::AddComponent(optionsResumeButton, SpriteRenderer{ .texture = allTextures[8],  .enabled = false, .uiElement = true });
		ecs::AddComponent(optionsResumeButton, PauseComponent{ .upper = fullscreenEntity, .lower = musicSliderEntity, .selectedTexture = allTextures[8], .unselectedTexture = allTextures[9], .operation = BackToUIMenu, .isOptionsMenu = true});
		//musicSliderEntity
		ecs::AddComponent(musicSliderEntity, Transform{ .position = Vector3(0,.1f,-0.1f), .scale = Vector3(0.25f) });
		ecs::AddComponent(musicSliderEntity, SpriteRenderer{ .texture = allTextures[10],  .enabled = false, .uiElement = true });
		ecs::AddComponent(musicSliderEntity, PauseComponent{ .upper = optionsResumeButton, .lower = fullscreenEntity, .selectedTexture = allTextures[10], .unselectedTexture = allTextures[11], .operation = PauseSystem::OnMainMenuPressed, .isOptionsMenu = true,.isSlider = true });
		//fullscreenEntity
		ecs::AddComponent(fullscreenEntity, Transform{ .position = Vector3(0,-0.7f,-0.1f), .scale = Vector3(0.25f) });
		ecs::AddComponent(fullscreenEntity, SpriteRenderer{ .texture = allTextures[12],  .enabled = false, .uiElement = true });
		ecs::AddComponent(fullscreenEntity, PauseComponent{ .upper = musicSliderEntity, .lower = optionsResumeButton, .selectedTexture = allTextures[12], .unselectedTexture = allTextures[13], .operation = PauseSystem::OnQuitGamePressed, .isOptionsMenu = true });
		//musicSliderNub
		ecs::AddComponent(musicSliderNub, Transform{.position = ecs::GetComponent<Transform>(musicSliderEntity).position + Vector3(0,-0.2f,-0.1), .scale = Vector3(0.15f)});
		ecs::AddComponent(musicSliderNub, SpriteRenderer{ .texture = allTextures[15],  .enabled = false, .uiElement = true });
		ecs::AddComponent(musicSliderNub, PauseComponent{ .selectedTexture = allTextures[15], .unselectedTexture = allTextures[15], .isOptionsMenu = true,.isSlider = true });

	}

	void AddButton(ecs::Entity entity, Vector3 pos, Texture selectedTexture, Texture unselectedTexture, Vector3 scaleNormal, Vector3 scaleSelected)
	{
		ecs::AddComponent(fullscreenEntity, Transform{ .position = Vector3(0.7,-0.7f,-0.1f), .scale = Vector3(0.25f) });
		ecs::AddComponent(fullscreenEntity, SpriteRenderer{ .texture = allTextures[12],  .enabled = false, .uiElement = true });
		ecs::AddComponent(fullscreenEntity, PauseComponent{ .upper = mainMenuButton, .lower = resumeButton, .selectedTexture = allTextures[12], .unselectedTexture = allTextures[13], .operation = PauseSystem::OnQuitGamePressed });

	}
	void CreateColumOfButtons(float verticalOffset, vector<ButtonData> buttonData)
	{
		int i = 0;

		for (vector<ButtonData>::iterator buttonIterator = buttonData.begin(); buttonIterator < buttonData.end(); buttonIterator++)
		{
			auto& button = *buttonIterator;
			auto upper = buttonIterator--;
			auto lower = buttonIterator++;
			auto& upperButton = upper;
			auto& lowerButton = lower;
			ecs::AddComponent(button.id, Transform{ .position = *new Vector3(0,.8f,-0.1f), .scale = button.normalScale });
			ecs::AddComponent(button.id, SpriteRenderer{ .texture = &button.unselected,  .enabled = false, .uiElement = true });
			//TODO	//ecs::AddComponent(button.id, PauseComponent{ .upper = upperButton.id, .lower = lowerButton, .selectedTexture = allTextures[12], .unselectedTexture = allTextures[13], .operation = PauseSystem::OnQuitGamePressed });


		}




		//for (auto itr = buttonFuncAndTexture.begin(); itr != buttonFuncAndTexture.end(); itr++)
		//{
		//	// Access the first and second elements of the Texture array for the current key
		//	Texture* texture0 = &itr->second[0]; // Texture[0]
		//	Texture* texture1 = &itr->second[1]; // Texture[1]

		//	// Create buttons using texture0 and texture1
		//	AddButton(ecs::NewEntity(), Vector3(0, verticalOffset * i, -0.1f), texture0, texture1, scaleNormal, scaleSelected);

		//	//AddButton
		//	ecs::AddComponent(entity, Transform{ .position = pos, .scale = scaleNormal });
		//	ecs::AddComponent(entity, SpriteRenderer{ .texture = unselectedTexture,  .enabled = false, .uiElement = true });
		//	ecs::AddComponent(entity, PauseComponent{ .upper = mainMenuButton, .lower = resumeButton, .selectedTexture = allTextures[12], .unselectedTexture = allTextures[13], .operation = PauseSystem::OnQuitGamePressed });


		//	i++;
		//}
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
		nubTransform.position =  Vector3(pauseComponentNub.sliderValue, 0, 0);
		printf("pauseComponentNub.sliderValue ");
		std::cout << pauseComponentNub.sliderValue ;
		printf("\n");
		std::cout << "Nub pos:" <<"x:"<<  nubTransform.position.x << " y:" << nubTransform.position.y << "\n";

	}

	void MoveSliderRight()
	{		
		PauseComponent& pauseComponentNub = ecs::GetComponent<PauseComponent>(musicSliderNub);
		Transform& nubTransform = ecs::GetComponent<Transform>(musicSliderNub);
		nubTransform.position += Vector3(0.01f, 0, 0);	
		nubTransform.position.x  = clamp(nubTransform.position.x, -0.17f, 0.17f);
		printf("pauseComponentNub.sliderValue ");
		std::cout << pauseComponentNub.sliderValue;
		printf("\n");
		std::cout << "Nub pos:" << "x:" << nubTransform.position.x << " y:" << nubTransform.position.y << "\n";
	}
	void MoveSliderLeft()
	{
		PauseComponent& pauseComponentNub = ecs::GetComponent<PauseComponent>(musicSliderNub);
		Transform& nubTransform = ecs::GetComponent<Transform>(musicSliderNub);
		nubTransform.position -=  Vector3(0.01f, 0, 0);
		nubTransform.position.x = clamp(nubTransform.position.x, -0.17f, 0.17f);
		printf("pauseComponentNub.sliderValue ");
		std::cout << pauseComponentNub.sliderValue;
		printf("\n");
		std::cout << "Nub pos:" << "x:" << nubTransform.position.x << " y:" << nubTransform.position.y << "\n";

	}
	void ToggleShowUIShipSelection() 
	{

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


