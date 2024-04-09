#pragma once
//Author: Sauli Hanell month.day.year 2.9.2024

#include <engine/Application.h>
#include <functional>
#include "engine/Input.h"  
#include "engine/Timing.h"


enum GameStates { PauseMenuMainState, pauseMenuOptionsState, SelectShipState, GameState };
enum ShipPlayDifficulty { easy, normal, hard };

static bool canStartLoadingMap;
static bool isSceneloaded;
std::vector<ShipType> playerShips;

//PlayerShipSelection
ECS_REGISTER_COMPONENT(PlayerSelection)
struct PlayerSelection
{

	int playerID;
	bool isActivePlayer = false;
	int selection;
	float cooldownToMoveSelection;
	bool ready = false;
	float timeArrowBigTime;
	bool isArrowBig;

	engine::ecs::Entity arrowUp;
	engine::ecs::Entity arrowDown;
	engine::ecs::Entity	shipModel;
	engine::ecs::Entity	readyText;
	engine::ecs::Entity playerWindow;
	engine::ecs::Entity shipInfo;

	engine::ecs::Entity shipNameEntity;
	engine::ecs::Entity baseSpeedEntity;
	engine::ecs::Entity boostEntity;
	engine::ecs::Entity specialEntity;


};


ECS_REGISTER_SYSTEM(PlayerSelectSystem, PlayerSelection)
class PlayerSelectSystem : public engine::ecs::System
{
	const float cooldownTime = 1.2f;

	bool isLoadingMap = false;
	const double startGameTimerInitValue = 30.f;
	double startGameCurrentTime;

	Vector2 gameStartTimerPosition;
	engine::ecs::Entity startGameTimerEntity;
	engine::ecs::Entity sceneParent;
	bool startGameTimer = false;
	bool isPlayersReady = false;
	bool isReseted;

	const float throttleMoveWaitTime = 0.5f;
	float throttleCurrentWaitedTimeUP = 0;
	float throttleCurrentWaitedTimeDown = 0;
	engine::ecs::Entity selectionWindow;

	map<int, ShipType>selectedShipsAtTheFrame;

	void SetSelectedPlayerShips(map<int, ShipType> selectedShipsAtTheFrame)
	{
		

		for (const auto& pair : selectedShipsAtTheFrame)
		{
			playerShips.push_back(ShipType(pair.second));
			std::cout << " " << pair.second << "  ";
		}
		

	};
public:
	
	//std::vector<ShipType> playerShips;
	//static bool canStartLoadingMap;
	

	////static bool canStartLoadingMap;
	////static std::vector<ShipType>playerShips;

	//static bool GetCanStartLoadingMap() {
	//	return canStartLoadingMap;
	//}
	//static  std::vector<ShipType> GetPlayerShips() {	
	//	return	PlayerShips;
	//}


	//std::unordered_map<ShipType, engine::Model*> shipModels;
	vector<engine::Model*> shipModels;
	bool isShipSelectionMenuOn = false;

	vector< std::function<void()> >shipButtonFunctions
	{
		BackToUIMenu

	};


	std::string PrintShipInfos(PlayerSelection& playerSelection)
	{
		//SHIP STATS

		string shipName;
		float baseSpeed;
		float boost;
		float special;



		std::vector<std::string>shipInfoToText;

		ShipType shipType = ShipType(playerSelection.selection);

		//TODO: GET REFERENCE ABOUT STATS AND PUT THEM HERE	
		switch (shipType)
		{

		case ShipType::torpedoBoat:
		{
			shipName = "Torpedo boat";
			baseSpeed = 0;
			boost = 0;
			special = 0;
			break;
		}
		case ShipType::submarine:
		{
			shipName = "Submarine";
			baseSpeed = 0;
			boost = 0;
			special = 0;
			break;
		}
		case ShipType::cannonBoat:
		{
			shipName = "Cannon boat";
			baseSpeed = 0;
			boost = 0;
			special = 0;
			break;
		}
		case ShipType::hedgehogBoat:
		{
			shipName = "Hedgehog boat";
			baseSpeed = 0;
			boost = 0;
			special = 0;
			break;
		}
		case ShipType::pirateShip:
		{
			shipName = "PirateShip";
			baseSpeed = 0;
			boost = 0;
			special = 0;
			break;
		}
		default:
			break;
		}


		string shipInfo =
		{
			"SHIP INFO\n\n"


			"shipName: " + shipName + "\n" +
			"BaseSpeed: " + to_string(baseSpeed) + "\n" +
			"Boost: " + to_string(boost) + "\n" +
			"special: " + to_string(special)

		};
		engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.shipInfo).text = "STATS";
		engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.shipInfo).color = Vector3(232, 235, 52);
		
		engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.shipNameEntity).text = "shipName: " + shipName;
		engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.specialEntity).text = "special: " + to_string(special);
		engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.boostEntity).text = "Boost: " + to_string(boost);
		engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.baseSpeedEntity).text = "BaseSpeed: " + to_string(baseSpeed);

		return  shipInfo;

	}
	std::string PrintShipInfo(ShipType shipType)
	{
		//SHIP STATS

		ShipPlayDifficulty PlayDifficulty;
		string shipName;
		float baseSpeed;
		float boost;
		float special;
		

	
		

		

		std::vector<std::string>shipInfoToText;

		//TODO: GET REFERENCE ABOUT STATS AND PUT THEM HERE	
		switch (shipType)
		{

		case ShipType::torpedoBoat:
		{
			shipName = "torpedoBoat";
			baseSpeed =0;
			boost =0;
			special =0;
			break;
		}
		case ShipType::submarine:
		{
			shipName = "submarine";
			baseSpeed =0;
			boost =0;
			special =0;
			break;
		}
		case ShipType::cannonBoat:
		{
			shipName = "cannonBoat";
			baseSpeed =0;
			boost =0;
			special =0;
			break;
		}
		case ShipType::hedgehogBoat:
		{
			shipName = "hedgehogBoat";
			baseSpeed  =0;
			boost  =0;
			special  =0;
			break;
		}
		case ShipType::pirateShip:
		{
			shipName = "pirateShip";
			baseSpeed  =0;
			boost  =0;
			special  =0;
			break;
		}
		default:
			break;
		}
		
	
		string shipInfo =
		{ 
			"SHIP INFO\n\n"	


			"shipName: "+shipName+"\n" +
			"BaseSpeed: " +to_string(baseSpeed) + "\n" +
			"Boost: "+to_string(boost )+ "\n" +
			"special: "+ to_string(special)

		};

		return  shipInfo;

	}

	void Init()
	{

		startGameCurrentTime = startGameTimerInitValue;

		shipModels.push_back({ resources::models["Ship_PT_109_Torpedo.obj"] });
		shipModels.push_back({ resources::models["Ship_U_99_Submarine.obj"] });
		shipModels.push_back({ resources::models["Ship_Yamato_Cannon.obj"] });
		shipModels.push_back({ resources::models["Ship_HMCS_Sackville_Hedgehog.obj"] });

		sceneParent = engine::ecs::NewEntity();
		engine::ecs::AddComponent(sceneParent, engine::Transform{ .position = Vector3(0,0,0), .rotation = Vector3(0, 0, 0), .scale = 0.7f });
		engine::ecs::AddComponent(sceneParent, engine::TextRenderer{ .font = resources::niagaraFont, .text = "",.scale = 0.005f,.color = Vector3(0, 0, 250), .uiElement = true });


		startGameTimerEntity = engine::ecs::NewEntity();
		engine::ecs::AddComponent(startGameTimerEntity, engine::Transform{ .position = gameStartTimerPosition, .rotation = Vector3(0, 0, 0), .scale = Vector3(0.04f) });
		//engine::ecs::AddComponent(startGameTimerEntity, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Arrow.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(startGameTimerEntity, engine::TextRenderer{ .font = resources::niagaraFont, .text = "&",.scale = 0.05f,.color = Vector3(200, 140, 50), .uiElement = true });


		float statsOffset =-0.1f;
		float statsOffsetY = 100.0f;
		float statsOffsetEatch = -90.0f;
		float scaleForStatsChilds = .9f;
		float statsParentScale = 0.004f;

		float arrowsPosX = 0.8f;
		float arrowUPposY = 0.28f;
		float arrowDownposY = 0.18f;

		Vector2 shipPos = (0.7f, -0.2f, -0.1f);
		for (int i = 0; i < 4; i++)
		{


			selectionWindow = engine::ecs::NewEntity();


			engine::ecs::Entity arrowUp = engine::ecs::NewEntity();
			engine::ecs::Entity arrowDown = engine::ecs::NewEntity();
			engine::ecs::Entity shipModel = engine::ecs::NewEntity();
			engine::ecs::Entity readyText = engine::ecs::NewEntity();

			engine::ecs::Entity shipInfo = engine::ecs::NewEntity();

			engine::ecs::Entity shipNameEntity = engine::ecs::NewEntity();
			engine::ecs::Entity baseSpeedEntity = engine::ecs::NewEntity();
			engine::ecs::Entity boostEntity = engine::ecs::NewEntity();
			engine::ecs::Entity specialEntity = engine::ecs::NewEntity();


			//engine::ecs::AddComponent(pausedImage, engine::Transform{ .position = Vector3(0,.8f,-0.1f), .scale = Vector3(0.35f) });
			//engine::ecs::AddComponent(pausedImage, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Paused.png"],  .enabled = false, .uiElement = true });
			//engine::ecs::AddComponent(pausedImage, PauseComponent{ .upper = pausedImage, .lower = optionsButton, .selectedTexture = resources::menuTextures["UI_Paused.png"], .unselectedTexture = resources::menuTextures["UI_Paused.png"], .operation = PauseSystem::OnResumePressed });


			engine::ecs::AddComponent(arrowUp, engine::Transform{ .position = Vector3(arrowsPosX, arrowUPposY, -0.1f), .rotation = Vector3(0, 0, -90), .scale = Vector3(0.04f) });
			engine::ecs::AddComponent(arrowUp, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Arrow.png"], .enabled = false, .uiElement = true });

			engine::ecs::AddComponent(arrowDown, engine::Transform{ .position = Vector3(arrowsPosX, arrowDownposY, -0.1f), .rotation = Vector3(0, 0, 90), .scale = Vector3(0.04f) });
			engine::ecs::AddComponent(arrowDown, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Arrow.png"], .enabled = false, .uiElement = true });

			engine::ecs::AddComponent(shipModel, engine::Transform{ .position = Vector3(0.7f, -0.2f, -0.1f) , .scale = 0 });
			engine::ecs::AddComponent(shipModel, engine::ModelRenderer{ .model = shipModels[0], .uiElement = true });

			engine::ecs::AddComponent(readyText, engine::Transform{ .position = Vector3(0,-0.2f,-0.1f) , .scale = 0 });
			engine::ecs::AddComponent(readyText, engine::TextRenderer{ .font = resources::niagaraFont, .text = "unready", .uiElement = true });

			//shipInfo Entity
			engine::ecs::AddComponent(shipInfo, engine::Transform{ .position = Vector3(-1,.7f,-0.1f) , .scale = statsParentScale });
			engine::ecs::AddComponent(shipInfo, engine::TextRenderer{ .font = resources::niagaraFont, .text = "Stats Info Here",.color = Vector3(57, 150, 54),.uiElement = true });

			//shipNameEntity
			engine::ecs::AddComponent(shipNameEntity, engine::Transform{ .position = Vector3(-statsOffset,0*statsOffsetEatch - statsOffsetY,-0.1f) , .scale = scaleForStatsChilds });
			engine::ecs::AddComponent(shipNameEntity, engine::TextRenderer{ .font = resources::niagaraFont, .text = "shipName Entity",.color = Vector3(57, 150, 54),.uiElement = true });

			//baseSpeedEntity
			engine::ecs::AddComponent(baseSpeedEntity, engine::Transform{ .position = Vector3(-statsOffset, 1 * statsOffsetEatch - statsOffsetY,-0.1f) , .scale = scaleForStatsChilds });
			engine::ecs::AddComponent(baseSpeedEntity, engine::TextRenderer{ .font = resources::niagaraFont, .text = "baseSpeed Entity",.color = Vector3(57, 150, 54),.uiElement = true });
			//boostEntity
			engine::ecs::AddComponent(boostEntity, engine::Transform{ .position = Vector3(-statsOffset,2 * statsOffsetEatch - statsOffsetY,-0.1f) , .scale = scaleForStatsChilds });
			engine::ecs::AddComponent(boostEntity, engine::TextRenderer{ .font = resources::niagaraFont, .text = "boost Entity",.color = Vector3(57, 150, 54),.uiElement = true });
			//specialEntity
			engine::ecs::AddComponent(specialEntity, engine::Transform{ .position = Vector3(-statsOffset, 3 * statsOffsetEatch - statsOffsetY,-0.1f) , .scale = scaleForStatsChilds });
			engine::ecs::AddComponent(specialEntity, engine::TextRenderer{ .font = resources::niagaraFont, .text = "special Entity",.color = Vector3(57, 150, 54),.uiElement = true });



			float offsetY = 0.85f;
			float offsetX = 0.80f;
			Vector3 offsetPlayerWindows;
			switch (i)
			{
			case 0:
			{
				offsetPlayerWindows = Vector3(-offsetX, offsetY, 0);
				break;
			}
			case 1:
			{
				offsetPlayerWindows = Vector3(-offsetX, -offsetY, 0);
				break;
			}
			case 2:
			{
				offsetPlayerWindows = Vector3(offsetX, -offsetY, 0);
				break;
			}
			case 3:
			{
				offsetPlayerWindows = Vector3(offsetX, offsetY, 0);
				break;
			}
			}

			engine::ecs::AddComponent(selectionWindow, engine::Transform{ .position = offsetPlayerWindows, .scale = Vector3(0.5, 0.5, -0.1f) });
			engine::ecs::AddComponent(selectionWindow, PlayerSelection{ .playerID = i, .arrowUp = arrowUp, .arrowDown = arrowDown, .shipModel = shipModel, .readyText = readyText, .playerWindow = selectionWindow, .shipInfo = shipInfo,.shipNameEntity= shipNameEntity,.baseSpeedEntity= baseSpeedEntity,.boostEntity= boostEntity ,.specialEntity = specialEntity, });

			engine::ecs::AddComponent(selectionWindow, engine::TextRenderer{ .font = resources::niagaraFont, .text = "PRESS { SHOOT } BUTTON TO JOIN!",.offset=Vector3(0,0.15f,0),.uiElement = true});

			auto& selectWin = engine::ecs::GetComponent<engine::TextRenderer>(selectionWindow);
			selectWin.scale = 0.001f;
			engine::ecs::GetComponent<engine::TextRenderer>(selectionWindow).color = Vector3(255, 0, 0);
			engine::ecs::GetComponent<engine::TextRenderer>(selectionWindow).scale = 0.002f;
			engine::TransformSystem::AddParent(selectionWindow, sceneParent);
			engine::TransformSystem::AddParent(arrowUp, selectionWindow);
			engine::TransformSystem::AddParent(arrowDown, selectionWindow);
			engine::TransformSystem::AddParent(shipModel, selectionWindow);
			engine::TransformSystem::AddParent(readyText, selectionWindow);
			engine::TransformSystem::AddParent(shipInfo, selectionWindow);

			engine::TransformSystem::AddParent(shipNameEntity, shipInfo);
			engine::TransformSystem::AddParent(specialEntity, shipInfo);
			engine::TransformSystem::AddParent(boostEntity, shipInfo);
			engine::TransformSystem::AddParent(baseSpeedEntity, shipInfo);
			
		}


	}

	void Update()
	{

		//std::cout << "\n throttleCurrentWaitedTimeUP:" << throttleCurrentWaitedTimeUP << "\n";

		for (engine::ecs::Entity entity : entities)
		{

			PlayerSelection& playerSelection = engine::ecs::GetComponent<PlayerSelection>(entity);


			//add player to selectedShipsAtTheFrame if not yet have that player ID
			if (!selectedShipsAtTheFrame.contains(playerSelection.playerID))
				selectedShipsAtTheFrame.insert({ playerSelection.playerID, ShipType(0) });


			float accelerationInput = input::GetTotalInputValue("Throttle" + std::to_string(playerSelection.playerID));

			auto select = input::GetNewPress("Shoot" + std::to_string(playerSelection.playerID));


			auto& playerReadyText = engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.readyText).text;

			

			//Shoot input adds player to the game and after that makes player Ready/UNready
			if (select)
			{

				if (!playerSelection.isActivePlayer)
				{
					engine::ecs::GetComponent< engine::SpriteRenderer>(playerSelection.arrowUp).enabled = true;
					engine::ecs::GetComponent< engine::SpriteRenderer>(playerSelection.arrowDown).enabled = true;
					playerReadyText = "UNready";
					playerSelection.isActivePlayer = true;
					engine::ecs::GetComponent< engine::Transform>(playerSelection.shipModel).scale = 0.2;
					//engine::ecs::GetComponent< engine::Transform>(playerSelection.shipModel).position = (0.9f, 0.2f, -0.1f);
					engine::ecs::GetComponent<engine::TextRenderer>(playerSelection.playerWindow).text = "P" + to_string(playerSelection.playerID);
				}
				else
				{


					playerReadyText = playerReadyText == "UNready" ? "Ready" : "UNready";
					playerSelection.ready = !playerSelection.ready;

					//Select correct player in selectedShipsAtTheFrame and set shipType be playerID in index 
					selectedShipsAtTheFrame[playerSelection.playerID] = ShipType(playerSelection.playerID);


					if (playerSelection.ready)
					{
						printf(" TIMER s TARTED \n");
						startGameTimer = true;
						engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.readyText).color = Vector3(0, 255, 0);
					}
					else
					{
						engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.readyText).color = Vector3(0, 0, 255);
						startGameTimer = false;
					}
				}
			}
			//IF READY CAN'T CHANGE SHIP
			if (!playerSelection.ready && playerSelection.isActivePlayer)
			{


				//Throttle Up 
				if (accelerationInput > 0.5f && throttleCurrentWaitedTimeUP > throttleMoveWaitTime)
				{
					throttleCurrentWaitedTimeUP = 0;
					//std::cout << "\n--Ship Selection moveUP--\n";



					playerSelection.selection++;
					if (playerSelection.selection >= shipModels.size())
						playerSelection.selection = 0;




					engine::ecs::GetComponent< engine::Transform>(playerSelection.arrowUp).scale = Vector3(0.08f);
					engine::ecs::GetComponent< engine::ModelRenderer>(playerSelection.shipModel).model = shipModels[playerSelection.selection];
					playerSelection.isArrowBig = true;
				}


				//Throttle Down 
				else if (accelerationInput < -0.5f && throttleCurrentWaitedTimeDown > throttleMoveWaitTime)
				{
					throttleCurrentWaitedTimeDown = 0;

					printf("\nShip Selection moveDown\n");



					playerSelection.selection--;
					if (playerSelection.selection < 0) 
					{
						playerSelection.selection = shipModels.size() - 1;
					}

					engine::ecs::GetComponent< engine::Transform>(playerSelection.arrowDown).scale = Vector3(0.08f);
					engine::ecs::GetComponent< engine::ModelRenderer>(playerSelection.shipModel).model = shipModels[playerSelection.selection];
					playerSelection.isArrowBig = true;

						
				}

				//Set arrow indicator scale to normal
				else
				{
					if (playerSelection.isArrowBig)
					{

						playerSelection.timeArrowBigTime += engine::deltaTime;
						if (playerSelection.timeArrowBigTime > 0.15f)
						{

							engine::ecs::GetComponent< engine::Transform>(playerSelection.arrowDown).scale = Vector3(0.03f);
							engine::ecs::GetComponent< engine::Transform>(playerSelection.arrowUp).scale = Vector3(0.03f);

							playerSelection.isArrowBig = false;
							playerSelection.timeArrowBigTime = 0;
						}
					}
				}

				//SETS SHIP INFO TEXT BY THE ShipType 			
				PrintShipInfos(playerSelection);
			}

			auto& ShipModelTransform = engine::ecs::GetComponent< engine::Transform>(playerSelection.shipModel);

			//double a = (int)(ShipModelTransform.rotation.y + 160 * engine::deltaTime) % 360;
			double yRotation = ShipModelTransform.rotation.y;
			if (ShipModelTransform.rotation.y < 360)
			{
				yRotation += 35 * engine::deltaTime;
			}
			else  yRotation = ShipModelTransform.rotation.y - 360 + 35 * engine::deltaTime;

			ShipModelTransform.rotation = Vector3(6, yRotation, 0);

			//std::cout << "\n" << "isArrowBig: " << playerSelection.isArrowBig << "time:" << playerSelection.timeArrowBigTime << "\n";





		//GAME TIMER 
			if (startGameTimer && startGameCurrentTime > 0)
			{

				//print timer in screen

				startGameCurrentTime -= engine::deltaTime;

				std::cout << "Time:" << startGameCurrentTime << "\n";

				if (startGameCurrentTime < 0) 
				{
					engine::ecs::GetComponent<engine::TextRenderer>(startGameTimerEntity).text = "Loading...";

				}
				else 
				{
					std::string result;
					for (char c : to_string(startGameCurrentTime)) {
						//if (isdigit(c)) { // Check if character is a digit
							result += c;
							if (result.size() == 3) // Break after first three digits
								break;
						//}
					}

					engine::ecs::GetComponent<engine::TextRenderer>(startGameTimerEntity).text = result;
				}

				engine::ecs::GetComponent<engine::TextRenderer>(startGameTimerEntity).color = Vector3(255, 0, 0);
				isReseted = false;
				//canStartLoadingMap = true;
			}
			if (!startGameTimer && !isReseted)
			{
				isReseted = true;
				printf(" TIMER REsETed \n");
				startGameCurrentTime = startGameTimerInitValue;
				engine::ecs::GetComponent<engine::TextRenderer>(startGameTimerEntity).text = "";
				engine::ecs::GetComponent<engine::TextRenderer>(startGameTimerEntity).offset = Vector3(-1.f, 0, 0);
				engine::ecs::GetComponent<engine::TextRenderer>(startGameTimerEntity).color = Vector3(200, 140, 50);
				//canStartLoadingMap = false;
			}

		}
		if ((startGameCurrentTime <= 0 || isPlayersReady)) //TODO:: CANT LAOD new map	canStartLoadingMap
		{	//START GAME
			canStartLoadingMap = true;

			
			printf("============= GAME STARTING ==========\n");
			SetSelectedPlayerShips(selectedShipsAtTheFrame);
			
			
			/*engine::Camera* cam;

			LoadLevel1(cam);*/

			//SetSelectedPlayerShips(selectedShipsAtTheFrame);

			/*
			std::cout << "\nGAME STARTING with these ships:";


			for (const auto& pair : selectedShipsAtTheFrame)
			{
				playerShips.push_back(ShipType(pair.second));
				std::cout << " " << pair.second << "  ";
			}

			std::cout << "\n";
		   */
		   //start game implemation...	
		}

		throttleCurrentWaitedTimeUP += engine::deltaTime;
		throttleCurrentWaitedTimeDown += engine::deltaTime;


	}




	void ToggleMenuPlayerSelection()
	{
		printf("in MenuPlayerSelection\n");

		for (engine::ecs::Entity entity : entities)
		{



			PlayerSelection& playerSelection = engine::ecs::GetComponent<PlayerSelection>(entity);

			if (isShipSelectionMenuOn)// Show every thing
			{



				engine::ecs::GetComponent< engine::SpriteRenderer>(playerSelection.arrowUp).enabled = false;
				engine::ecs::GetComponent< engine::SpriteRenderer>(playerSelection.arrowDown).enabled = false;
				engine::ecs::GetComponent< engine::Transform>(playerSelection.shipModel).scale = 0;

				//.-.-.-.-.-.-.-.-.-TODO::  SHIP MODEL NOT CHANGE OR SHOW UP and Ready text missing too .-.-.-.-.-.-.-.-.-
				engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.readyText).text = "";
				engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.readyText).color = Vector3(0, 0, 255);
				engine::ecs::GetComponent< engine::Transform>(playerSelection.readyText).scale = 0.003f;

				engine::ecs::GetComponent<engine::TextRenderer>(playerSelection.playerWindow).scale = 0.002f;
				std::cout << "\nisShipSelectionMenuOn: " << isShipSelectionMenuOn << "\n";
			}

			if (!isShipSelectionMenuOn)	//hide every thing
			{


				engine::ecs::GetComponent< engine::SpriteRenderer>(playerSelection.arrowUp).enabled = false;
				engine::ecs::GetComponent< engine::SpriteRenderer>(playerSelection.arrowDown).enabled = false;
				engine::ecs::GetComponent< engine::Transform>(playerSelection.shipModel).scale = 0;

				engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.readyText).text = "UNready";
				engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.readyText).scale = Vector3(1, 1, 1);
				std::cout << "\nisShipSelectionMenuOn: " << isShipSelectionMenuOn << "\n";
			}



			printf("Out MenuPlayerSelection\n\n\n");
		}
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
			input::bindDigitalInput(GLFW_KEY_M, { "Boost" + std::to_string(KeyboardPlayer) });
		*/


		//printf("IN side MENU SYSTEM INIT\n");
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
		engine::ecs::AddComponent(pausedImage, engine::Transform{ .position = Vector3(0, .8f, -0.1f), .scale = Vector3(0.35f) });
		engine::ecs::AddComponent(pausedImage, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Paused.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(pausedImage, PauseComponent{ .upper = pausedImage, .lower = optionsButton, .selectedTexture = resources::menuTextures["UI_Paused.png"], .unselectedTexture = resources::menuTextures["UI_Paused.png"], .operation = PauseSystem::OnResumePressed });


		engine::ecs::AddComponent(resumeButton, engine::Transform{ .position = Vector3(0, .5f, -0.1f), .scale = Vector3(0.25f) });
		engine::ecs::AddComponent(resumeButton, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Resume_N.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(resumeButton, PauseComponent{ .upper = quitGameButton, .lower = optionsButton, .selectedTexture = resources::menuTextures["UI_Resume.png"], .unselectedTexture = resources::menuTextures["UI_Resume_N.png"], .operation = PauseSystem::OnResumePressed });

		engine::ecs::AddComponent(optionsButton, engine::Transform{ .position = Vector3(0, .3f, -0.1f), .scale = Vector3(.25f) });
		engine::ecs::AddComponent(optionsButton, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Options_N.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(optionsButton, PauseComponent{ .upper = resumeButton, .lower = mainMenuButton, .selectedTexture = resources::menuTextures["UI_Options.png"], .unselectedTexture = resources::menuTextures["UI_Options_N.png"], .operation = PauseSystem::OnOptionsPressed });

		engine::ecs::AddComponent(mainMenuButton, engine::Transform{ .position = Vector3(0, .1f, -0.1f), .scale = Vector3(0.25f) });
		engine::ecs::AddComponent(mainMenuButton, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_BackToMenu_N.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(mainMenuButton, PauseComponent{ .upper = optionsButton, .lower = quitGameButton, .selectedTexture = resources::menuTextures["UI_BackToMenu.png"], .unselectedTexture = resources::menuTextures["UI_BackToMenu_N.png"], .operation = PauseSystem::OnMainMenuPressed });

		engine::ecs::AddComponent(quitGameButton, engine::Transform{ .position = Vector3(0, -0.7f, -0.1f), .scale = Vector3(0.25f) });
		engine::ecs::AddComponent(quitGameButton, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_QuitGame_N.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(quitGameButton, PauseComponent{ .upper = mainMenuButton, .lower = resumeButton, .selectedTexture = resources::menuTextures["UI_QuitGame.png"], .unselectedTexture = resources::menuTextures["UI_QuitGame_N.png"], .operation = PauseSystem::OnQuitGamePressed });


		////                      ---------  OPTIONS BUTTONS init --------
		//optionsResumeButton
		engine::ecs::AddComponent(optionsResumeButton, engine::Transform{ .position = Vector3(0, .3f, -0.1f), .scale = Vector3(.25f) });
		engine::ecs::AddComponent(optionsResumeButton, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Resume.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(optionsResumeButton, PauseComponent{ .upper = fullscreenEntity, .lower = musicSliderEntity, .selectedTexture = resources::menuTextures["UI_Resume.png"], .unselectedTexture = resources::menuTextures["UI_Resume_N.png"], .operation = BackToUIMenu, .isOptionsMenu = true });
		//musicSliderEntity
		engine::ecs::AddComponent(musicSliderEntity, engine::Transform{ .position = Vector3(0, .1f, -0.1f), .scale = Vector3(0.25f) });
		engine::ecs::AddComponent(musicSliderEntity, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Music_Slider.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(musicSliderEntity, PauseComponent{ .upper = optionsResumeButton, .lower = fullscreenEntity, .selectedTexture = resources::menuTextures["UI_Music_Slider.png"], .unselectedTexture = resources::menuTextures["UI_Music_Slider_N.png"], .operation = PauseSystem::OnMainMenuPressed, .isOptionsMenu = true,.isSlider = true });
		//fullscreenEntity
		engine::ecs::AddComponent(fullscreenEntity, engine::Transform{ .position = Vector3(0, -0.7f, -0.1f), .scale = Vector3(0.25f) });
		engine::ecs::AddComponent(fullscreenEntity, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Fullscreen.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(fullscreenEntity, PauseComponent{ .upper = musicSliderEntity, .lower = optionsResumeButton, .selectedTexture = resources::menuTextures["UI_Fullscreen.png"], .unselectedTexture = resources::menuTextures["UI_Fullscreen_N.png"], .operation = PauseSystem::OnQuitGamePressed, .isOptionsMenu = true });
		//musicSliderNub
		engine::ecs::AddComponent(musicSliderNub, engine::Transform{ .position = engine::ecs::GetComponent<engine::Transform>(musicSliderEntity).position + Vector3(0, -0.2f, -0.1), .scale = Vector3(0.15f) });
		engine::ecs::AddComponent(musicSliderNub, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Slider_Button.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(musicSliderNub, PauseComponent{ .selectedTexture = resources::menuTextures["UI_Slider_Button.png"], .unselectedTexture = resources::menuTextures["UI_Slider_Button.png"], .isOptionsMenu = true,.isSlider = true });

	}

	void Update()
	{


		//printf("IN MENU SYSTEM UPDATE()\n");
		//if (input::GetNewPress("Pause"))
		//{
		//	//printf("Pause\n");
		//	isGamePause = !isGamePause;
		//	ToggleShowUIMenu();
		//}
		if (engine::ecs::GetSystem<PauseSystem>()->isGamePause)
		{

			for (size_t i = 0; i < 4; i++)
			{

				float accelerationInput = input::GetTotalInputValue("Throttle" + std::to_string(i));


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
		engine::ecs::AddComponent(fullscreenEntity, engine::Transform{ .position = Vector3(0.7, -0.7f, -0.1f), .scale = Vector3(0.25f) });
		engine::ecs::AddComponent(fullscreenEntity, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Fullscreen_N.png"], .enabled = false, .uiElement = true });
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



