#pragma once
//Author: Sauli Hanell month.day.year 2.9.2024

#include <engine/Application.h>
#include "engine/Input.h"  
#include "engine/Timing.h"
#include <functional>
#include "engine/SoundComponent.h"

using namespace engine;

enum GameState { mainMenuState, selectPlayersState, mapSelection, gamePlayState, pauseMenuState };
static GameState gameState = mainMenuState;
static bool canStartLoadingMap;
static bool isSceneloaded;
static float volume = 0.5f;
std::unordered_map<int, ShipType> playerShips;
static void LoadLevel6(engine::Camera* cam);
static void LoadLevel5(engine::Camera* cam);
static void LoadLevel4(engine::Camera* cam);
static void LoadLevel3(engine::Camera* cam);
static void LoadLevel2(engine::Camera* cam);
static void LoadLevel1(engine::Camera* cam);
void ToggleShipSelectMenu();
static void ReturnToMainMenu();

class PlayerSelectSystem;
ECS_REGISTER_COMPONENT(Level)
struct Level
{
	// PASS
};
ECS_REGISTER_SYSTEM(LevelSelectionSystem, Level)
class LevelSelectionSystem : public engine::ecs::System
{
	vector < engine::ecs::Entity> entityList;

	engine::ecs::Entity arrowRight;
	engine::ecs::Entity levelSelectionBackground;
	engine::ecs::Entity currentSelectedLevel;
	engine::ecs::Entity arrowsPivot;
	engine::ecs::Entity mapName;
	engine::ecs::Entity mapSelectText;

	int mapLevelIndex = 0;

	//REFERENCE TO CAMRA TO CALL LEVEL METHOD 
	engine::Camera* cam;

	vector<Texture*> mapImages;

	//DELAY TO CALL INPUT ACTION AGAIN
	double mapChangeTime = 0.42;
	bool readyToMoveLeft = true;
	bool readyToMoveRight = true;

	//COLORS
	Vector3 playTextColor = Vector3(255, 205, 0);
	Vector3 mapTextColor = Vector3(255, 255, 255);

	//ARROWS PLACEMENT
	float arrowPosHight = 0.88f;
	float arrowsOffsetX = 0.6f;

	std::vector<std::string> mapNames = { "Lake", "Caves", "Beach", "River", "Ocean", "River2" };

public:
	engine::ecs::Entity arrowLeft;
	int firstPlayer;

	void Init()
	{
		//Init variables
		mapLevelIndex = 0;
		readyToMoveLeft = true;
		readyToMoveRight = true;

		arrowRight = ecs::NewEntity();
		levelSelectionBackground = ecs::NewEntity();
		currentSelectedLevel = ecs::NewEntity();
		arrowsPivot = ecs::NewEntity();
		arrowLeft = ecs::NewEntity();
		mapName = ecs::NewEntity();
		mapSelectText = ecs::NewEntity();

		mapImages.clear();
		mapImages.push_back(resources::menuTextures["level1.png"]);
		mapImages.push_back(resources::menuTextures["level2.png"]);
		mapImages.push_back(resources::menuTextures["level3.png"]);
		mapImages.push_back(resources::menuTextures["level4.png"]);
		mapImages.push_back(resources::menuTextures["level5.png"]);
		mapImages.push_back(resources::menuTextures["level6.png"]);

		// Level select Teksti
		engine::ecs::AddComponent(arrowsPivot, engine::Transform{ .position = Vector3(0, arrowPosHight, 0), .scale = Vector3(1) });
		engine::ecs::AddComponent(mapName, engine::Transform{ .position = Vector3(-0.10f, 0.82f, 0.0f) });
		engine::ecs::AddComponent(mapName, engine::TextRenderer{ .font = resources::niagaraFont, .text = mapNames[0], .offset = Vector3(0.9f * 0.003f, 0.005f, 0.0f), .scale = Vector3(0.003f), .color = mapTextColor, .uiElement = true });

		engine::ecs::AddComponent(mapSelectText, engine::Transform{ .position = Vector3(-0.43f, -0.96f, 0.95), .scale = Vector3(1) });
		engine::ecs::AddComponent(mapSelectText, engine::TextRenderer
			{
				.font = resources::niagaraFont,
					.text = "Press Start to Play!", .offset = Vector3(0, 0, 0), .scale = Vector3(0.003f), .color = playTextColor, .uiElement = true }
					);

		engine::ecs::AddComponent(levelSelectionBackground, engine::Transform{ .position = Vector3(0, 0, -0.5f), .scale = Vector3(1) });
		engine::ecs::AddComponent(levelSelectionBackground, SpriteRenderer{ .texture = resources::menuTextures["Selection_BG_Var3.png"], .enabled = true,.uiElement = true });

		engine::ecs::AddComponent(currentSelectedLevel, engine::Transform{ .position = Vector3(0, 0, -0.5f), .scale = Vector3(0.8f) });
		engine::ecs::AddComponent(currentSelectedLevel, SpriteRenderer{ .texture = resources::menuTextures["level1.png"], .enabled = true,.uiElement = true });

		engine::ecs::AddComponent(arrowLeft, engine::Transform{ .position = Vector3(arrowsOffsetX, 0, -0.1f), .rotation = Vector3(0, 0, 180), .scale = Vector3(0.04f) });
		engine::ecs::AddComponent(arrowLeft, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Arrow.png"], .enabled = true, .uiElement = true });
		//	engine::ecs::AddComponent(arrowLeft, engine::TextRenderer{ .font = resources::niagaraFont, .text = "<  ", .scale = Vector2(0.004f), .color = Vector3(255,0,0.99),.uiElement = true });

		engine::ecs::AddComponent(arrowRight, engine::Transform{ .position = Vector3(-arrowsOffsetX, 0, -0.1f), .rotation = Vector3(0, 0, 0), .scale = Vector3(0.04f) });
		engine::ecs::AddComponent(arrowRight, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Arrow.png"], .enabled = true, .uiElement = true });
		//	engine::ecs::AddComponent(arrowRight, engine::TextRenderer{ .font = resources::niagaraFont, .text = "  >", .scale = Vector2(0.01f), .color = Vector3(255,0,0.99),.uiElement = true });

		engine::TransformSystem::AddParent(arrowRight, arrowsPivot);
		engine::TransformSystem::AddParent(arrowLeft, arrowsPivot);
	}
	//Defined at 1300
	void LoadThisLevel(int mapIndex);

	void Update()
	{
		float turnInput = input::GetTotalInputValue("Turn" + to_string(firstPlayer));
		int dpadInput = input::GetPressed("MenuDpadRight" + to_string(firstPlayer)) - input::GetPressed("MenuDpadLeft" + to_string(firstPlayer));
		turnInput += dpadInput;

		Audio* switchAudio = engine::AddAudio("Background", "audio/leftright.wav", false, 0.01f, DistanceModel::LINEAR);
		switchAudio->pause();
		if (turnInput > 0 && readyToMoveLeft)
		{
			readyToMoveLeft = false;

			TimerSystem::ScheduleFunction([this]() { readyToMoveLeft = true; }, mapChangeTime);
			switchAudio->play();
			mapLevelIndex++;
			if (mapLevelIndex >= mapImages.size())
			{
				mapLevelIndex = 0;
			}

			ecs::GetComponent<TextRenderer>(mapName).text = mapNames[mapLevelIndex];
			ecs::GetComponent<Transform>(mapName).position.x = -0.026f * mapNames[mapLevelIndex].size();

			TransformSystem::SetScale(arrowLeft, Vector3(0.08f));
			TimerSystem::ScheduleFunction([this]() { TransformSystem::SetScale(arrowLeft, Vector3(0.04f)); }, 0.1);
			ecs::GetComponent<SpriteRenderer>(currentSelectedLevel).texture = mapImages[mapLevelIndex];
		}

		if (turnInput < 0 && readyToMoveRight)
		{
			readyToMoveRight = false;
			TimerSystem::ScheduleFunction([this]() { readyToMoveRight = true; }, mapChangeTime);
			switchAudio->play();
			mapLevelIndex--;

			if (mapLevelIndex < 0)
			{
				mapLevelIndex = mapImages.size() - 1;
			}

			ecs::GetComponent<TextRenderer>(mapName).text = mapNames[mapLevelIndex];
			ecs::GetComponent<Transform>(mapName).position.x = -0.026f * mapNames[mapLevelIndex].size();

			TransformSystem::SetScale(arrowRight, Vector3(0.08f));
			TimerSystem::ScheduleFunction([this]() { TransformSystem::SetScale(arrowRight, Vector3(0.04f)); }, 0.1);
			ecs::GetComponent<SpriteRenderer>(currentSelectedLevel).texture = mapImages[mapLevelIndex];
		}

		if (input::GetNewPress("Shoot" + std::to_string(firstPlayer)) || input::GetNewPress("Pause" + std::to_string(firstPlayer)))
		{
			LoadThisLevel(mapLevelIndex);
		}

		//Back to ship select
		if (input::GetNewPress("Boost" + std::to_string(firstPlayer)))
		{
			ToggleShipSelectMenu();
		}
	}
};

ECS_REGISTER_COMPONENT(PlayerSelection)
struct PlayerSelection
{
	int playerID;
	bool hasJoined = false;
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
	engine::ecs::Entity maneuvarabilityEntity;
	engine::ecs::Entity boostEntity;
	engine::ecs::Entity specialEntity;

	engine::ecs::Entity backgroundImage;

	float throttleCurrentWaitedTimeUp = 0;
	float throttleCurrentWaitedTimeDown = 0;
};

ECS_REGISTER_SYSTEM(PlayerSelectSystem, PlayerSelection)
class PlayerSelectSystem : public engine::ecs::System
{
	bool isLoadingMap = false;

	Vector2 gameStartTimerPosition;
	engine::ecs::Entity startGameTimerEntity;
	engine::ecs::Entity sceneParent;
	bool startGameTimer = false;
	bool isPlayersReady = false;
	bool isReseted;

	/* Timer */
	const float startLevelTime = 4.f;			// Time we are counting seconds
	const float startLevelLoadingTime = 0.1f;	// Time we have "loading" text before we start the level
	float startLevelTimer = 0;					// Timer until we start the level

	const float throttleMoveWaitTime = 0.4f;
	engine::ecs::Entity selectionWindow;

	int playersThatAreReadyAmount = 0;

	//This is a temporary constol scheme display for playtesting
	engine::ecs::Entity controlScheme;

public:
	std::unordered_map<int, ShipType>selectedShipsAtTheFrame;

	vector<engine::Model*> shipModels;
	vector<engine::Model*> shipModelsReady;
	bool isShipSelectionMenuOn = false;

	void UpdateShipInfos(PlayerSelection& playerSelection)
	{
		//Disable all if player has not joined
		if (!playerSelection.hasJoined)
		{
			engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.shipInfo).text = "PRESS SHOOT TO JOIN!";
			engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.shipInfo).color = Vector3(57, 150, 54);
			engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.shipNameEntity).text = "";
			engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.baseSpeedEntity).text = "";
			engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.maneuvarabilityEntity).text = "";
			engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.boostEntity).text = "";
			engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.specialEntity).text = "";
			return;
		}

		//SHIP STATS

		std::string shipName;
		Vector3 nameColor;
		Vector3 speedColor = { 8, 201, 31 };
		Vector3 maneuverColor;
		std::string baseSpeed;
		std::string maneuverability;
		std::string mainAttack;
		std::string special;

		ShipType shipType = ShipType(playerSelection.selection);

		//TODO: Get reference to stats and add them here
		switch (shipType)
		{
		case ShipType::torpedoBoat:
			shipName = "Torpedo Boat";
			baseSpeed = "Fast";
			speedColor = { 8, 201, 31 };
			maneuverability = "Low";
			maneuverColor = { 186, 30, 2 };
			mainAttack = "Torpedo";
			special = "Boost";
			break;
		case ShipType::submarine:
			shipName = "Submarine";
			baseSpeed = "Medium";
			speedColor = { 235, 151, 26 };
			maneuverability = "Medium";
			maneuverColor = { 235, 151, 26 };
			mainAttack = "Torpedo";
			special = "Submerge";
			break;
		case ShipType::cannonBoat:
			shipName = "Battleship";
			baseSpeed = "Slow";
			speedColor = { 186, 30, 2 };
			maneuverability = "Fast";
			maneuverColor = { 8, 201, 31 };
			mainAttack = "Cannon";
			special = "Boost";
			break;
		case ShipType::hedgehogBoat:
			shipName = "Destroyer";
			baseSpeed = "Medium";
			speedColor = { 235, 151, 26 };
			maneuverability = "Medium";
			maneuverColor = { 235, 151, 26 };
			mainAttack = "Hedghehog Mortar";
			special = "Cruise Missile";
			break;
		case ShipType::pirateShip:
			shipName = "PirateShip";
			baseSpeed = "Medium";
			maneuverability = "Medium";
			mainAttack = "Cannonballs";
			special = "Reload";
			break;
		default:
			break;
		}

		//NameColoring
		switch (playerSelection.playerID)
		{
		case 0:			//green		
			nameColor = nameColor = Vector3(0, 255, 0);
			break;
		case 1:			  //orange
			nameColor = nameColor = Vector3(255, 162, 0);
			break;
		case 2:			//red	
			nameColor = Vector3(255, 0, 0);
			break;
		case 3:			   //purple

			nameColor = Vector3(196, 0, 255);

			break;
		default:
			break;
		}

		std::string playerNumStats = "Player " + to_string(playerSelection.playerID + 1);
		engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.shipInfo).text = playerNumStats;
		engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.shipInfo).color = nameColor;

		engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.shipNameEntity).text = shipName;
		engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.baseSpeedEntity).text = "Speed: " + baseSpeed;
		engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.baseSpeedEntity).color = speedColor;
		engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.maneuvarabilityEntity).text = "Turning: " + maneuverability;
		engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.maneuvarabilityEntity).color = maneuverColor;
		engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.boostEntity).text = "Weapon: " + mainAttack;
		engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.specialEntity).text = "Special: " + special;
	}

	void Init()
	{
		startGameTimer = false;
		isPlayersReady = false;
		selectedShipsAtTheFrame.clear();
		playersThatAreReadyAmount = 0;
		shipModels.clear();
		shipModelsReady.clear();

		shipModels.push_back({ resources::models["Ship_PT_109_Wireframe.obj"] });
		shipModels.push_back({ resources::models["Ship_U_99_Wireframe.obj"] });
		shipModels.push_back({ resources::models["Ship_Yamato_Wireframe.obj"] });
		shipModels.push_back({ resources::models["Ship_HMCS_Sackville_Wireframe_Hedgehog.obj"] });

		shipModelsReady.push_back({ resources::models["Ship_PT_109_Torpedo.obj"] });
		shipModelsReady.push_back({ resources::models["Ship_U_99_Submarine.obj"] });
		shipModelsReady.push_back({ resources::models["Ship_Yamato_Cannon.obj"] });
		shipModelsReady.push_back({ resources::models["Ship_HMCS_Sackville_Hedgehog.obj"] });

		sceneParent = engine::ecs::NewEntity();
		engine::ecs::AddComponent(sceneParent, engine::Transform{ .position = Vector3(0, 0, 0), .rotation = Vector3(0, 0, 0), .scale = 0.7f });
		engine::ecs::AddComponent(sceneParent, engine::TextRenderer{ .font = resources::niagaraFont, .text = "", .scale = 0.005f, .color = Vector3(0, 0, 250), .uiElement = true });

		startGameTimerEntity = engine::ecs::NewEntity();
		engine::ecs::AddComponent(startGameTimerEntity, engine::Transform{ .position = Vector3(-0.1, 0.08, -0.5), .rotation = Vector3(0, 0, 0), .scale = Vector3(0.04f) });
		engine::ecs::AddComponent(startGameTimerEntity, engine::TextRenderer{ .font = resources::niagaraFont, .text = "", .scale = 0.05f, .color = Vector3(200, 140, 50), .uiElement = true });

		float statsOffset = -0.1f;
		float statsOffsetY = 90.0f;
		float statsOffsetEatch = -80.0f;
		float scaleForStatsChilds = .9f;
		float statsParentScale = 0.004f;

		float arrowsPosX = 0.55f;
		float arrowUPposY = 0.22f;
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
			engine::ecs::Entity maneuvarabilityEntity = engine::ecs::NewEntity();
			engine::ecs::Entity boostEntity = engine::ecs::NewEntity();
			engine::ecs::Entity specialEntity = engine::ecs::NewEntity();

			engine::ecs::Entity backgroundImage = engine::ecs::NewEntity();

			engine::ecs::AddComponent(arrowUp, engine::Transform{ .position = Vector3(arrowsPosX + 0.2f, arrowUPposY, -0.1f), .rotation = Vector3(0, 0, 180), .scale = Vector3(0.04f) });
			engine::ecs::AddComponent(arrowUp, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Arrow.png"], .enabled = false, .uiElement = true });

			engine::ecs::AddComponent(arrowDown, engine::Transform{ .position = Vector3(arrowsPosX, arrowUPposY, -0.1f), .rotation = Vector3(0, 0, 0), .scale = Vector3(0.04f) });
			engine::ecs::AddComponent(arrowDown, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Arrow.png"], .enabled = false, .uiElement = true });

			engine::ecs::AddComponent(shipModel, engine::Transform{ .position = Vector3(0.65f, -0.2f, -0.1f), .scale = 0 });
			engine::ecs::AddComponent(shipModel, engine::ModelRenderer{ .model = shipModels[0], .uiElement = true, .textures = { resources::playerIdToTexture[i] } });

			engine::ecs::AddComponent(backgroundImage, engine::Transform{ .position = Vector3(0, 0, -0.9f), .rotation = Vector3(0, 0, 0), .scale = Vector3(1.2) });
			engine::ecs::AddComponent(backgroundImage, engine::SpriteRenderer{ .texture = resources::menuTextures["Selection_BG_Var3.png"], .enabled = false, .uiElement = true });

			engine::ecs::AddComponent(readyText, engine::Transform{ .position = Vector3(arrowsPosX - 0.2f, arrowDownposY + 0.25f, 0.4f), .scale = 0.004f });
			engine::ecs::AddComponent(readyText, engine::TextRenderer{ .font = resources::niagaraFont, .text = "Not Ready", .uiElement = true });

			//shipInfo Entity
			engine::ecs::AddComponent(shipInfo, engine::Transform{ .position = Vector3(-1, .75f, -0.1f), .scale = statsParentScale });
			engine::ecs::AddComponent(shipInfo, engine::TextRenderer{ .font = resources::niagaraFont, .text = "PRESS SHOOT TO JOIN!", .color = Vector3(57, 150, 54), .uiElement = true });

			//shipNameEntity
			engine::ecs::AddComponent(shipNameEntity, engine::Transform{ .position = Vector3(-statsOffset, 0 * statsOffsetEatch - statsOffsetY, -0.1f), .scale = scaleForStatsChilds * 1.5f });
			engine::ecs::AddComponent(shipNameEntity, engine::TextRenderer{ .font = resources::niagaraFont, .text = "", .color = Vector3(57, 150, 54), .uiElement = true });

			//baseSpeedEntity
			engine::ecs::AddComponent(baseSpeedEntity, engine::Transform{ .position = Vector3(-statsOffset, 1 * statsOffsetEatch - statsOffsetY, -0.1f), .scale = scaleForStatsChilds });
			engine::ecs::AddComponent(baseSpeedEntity, engine::TextRenderer{ .font = resources::niagaraFont, .text = "", .color = Vector3(57, 150, 54), .uiElement = true });
			//Rotation speed text
			engine::ecs::AddComponent(maneuvarabilityEntity, engine::Transform{ .position = Vector3(-statsOffset, 2 * statsOffsetEatch - statsOffsetY, -0.1f), .scale = scaleForStatsChilds });
			engine::ecs::AddComponent(maneuvarabilityEntity, engine::TextRenderer{ .font = resources::niagaraFont, .text = "",.uiElement = true });
			//boostEntity
			engine::ecs::AddComponent(boostEntity, engine::Transform{ .position = Vector3(-statsOffset, 3 * statsOffsetEatch - statsOffsetY, -0.1f), .scale = scaleForStatsChilds });
			engine::ecs::AddComponent(boostEntity, engine::TextRenderer{ .font = resources::niagaraFont, .text = "", .color = Vector3(57, 150, 54), .uiElement = true });
			//specialEntity
			engine::ecs::AddComponent(specialEntity, engine::Transform{ .position = Vector3(-statsOffset, 4 * statsOffsetEatch - statsOffsetY, -0.1f), .scale = scaleForStatsChilds });
			engine::ecs::AddComponent(specialEntity, engine::TextRenderer{ .font = resources::niagaraFont, .text = "", .color = Vector3(57, 150, 54), .uiElement = true });

			float offsetY = 0.83f;
			float offsetX = 0.83f;
			Vector3 offsetPlayerWindows;
			switch (i)
			{
			case 0:
				offsetPlayerWindows = Vector3(-offsetX, offsetY, 0);
				break;
			case 1:
				offsetPlayerWindows = Vector3(offsetX, offsetY, 0);
				break;
			case 2:
				offsetPlayerWindows = Vector3(-offsetX, -offsetY, 0);
				break;
			case 3:
				offsetPlayerWindows = Vector3(offsetX, -offsetY, 0);
				break;
			default:
				break;
			}

			engine::ecs::AddComponent(selectionWindow, engine::Transform{ .position = offsetPlayerWindows, .scale = Vector3(0.5, 0.5, -0.1f) });
			engine::ecs::AddComponent(selectionWindow, PlayerSelection{ .playerID = i, .arrowUp = arrowUp, .arrowDown = arrowDown, .shipModel = shipModel, .readyText = readyText, .playerWindow = selectionWindow, .shipInfo = shipInfo,.shipNameEntity = shipNameEntity,.baseSpeedEntity = baseSpeedEntity,.maneuvarabilityEntity = maneuvarabilityEntity,.boostEntity = boostEntity ,.specialEntity = specialEntity, .backgroundImage = backgroundImage });

			PlayerSelection& playerselection = ecs::GetComponent<PlayerSelection>(selectionWindow);
			engine::ecs::AddComponent(selectionWindow, engine::TextRenderer{ .font = resources::niagaraFont, .text = "", .offset = Vector3(0, 0.15f, 0), .uiElement = true });

			engine::TextRenderer& selectWin = engine::ecs::GetComponent<engine::TextRenderer>(selectionWindow);
			selectWin.scale = 0.001f;
			engine::ecs::GetComponent<engine::TextRenderer>(selectionWindow).color = Vector3(255, 0, 0);
			engine::ecs::GetComponent<engine::TextRenderer>(selectionWindow).scale = 0.002f;
			engine::TransformSystem::AddParent(selectionWindow, sceneParent);
			engine::TransformSystem::AddParent(arrowUp, selectionWindow);
			engine::TransformSystem::AddParent(arrowDown, selectionWindow);
			engine::TransformSystem::AddParent(shipModel, selectionWindow);
			engine::TransformSystem::AddParent(readyText, selectionWindow);
			engine::TransformSystem::AddParent(shipInfo, selectionWindow);

			engine::TransformSystem::AddParent(backgroundImage, selectionWindow);

			engine::TransformSystem::AddParent(shipNameEntity, shipInfo);
			engine::TransformSystem::AddParent(specialEntity, shipInfo);
			engine::TransformSystem::AddParent(boostEntity, shipInfo);
			engine::TransformSystem::AddParent(baseSpeedEntity, shipInfo);
			engine::TransformSystem::AddParent(maneuvarabilityEntity, shipInfo);
		}

		//Temporary control scheme display for playtesting
		controlScheme = ecs::NewEntity();
		engine::ecs::AddComponent(controlScheme, engine::Transform{ .position = Vector3(0.02, 0.11, 0.5), .scale = Vector3(1.5, 1.9, 0) * 0.2 });
		resources::menuTextures["UI_Controls.png"]->SetScalingFilter(GL_LINEAR);
		engine::ecs::AddComponent(controlScheme, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Controls.png"], .uiElement = true });
	}

	void StartGame()
	{
		if (selectedShipsAtTheFrame.size() != 0 && selectedShipsAtTheFrame.size() == playersThatAreReadyAmount)
		{
			// Timer is on: Increment timer
			startLevelTimer += engine::deltaTime;

			std::string result;

			if (startLevelTimer > startLevelTime)
			{
				// Loading time started
				result = "";
			}
			else
			{
				// Set text as current time
				std::ostringstream stream;
				stream << std::fixed << std::setprecision(2) << (startLevelTime - startLevelTimer);

				result = stream.str();
			}

			engine::ecs::GetComponent<engine::TextRenderer>(startGameTimerEntity).text = result;
			engine::ecs::GetComponent<engine::TextRenderer>(startGameTimerEntity).color = Vector3(255, 0, 0);
		}
		else
		{
			// Timer is off: Zero timer
			startLevelTimer = 0;

			engine::ecs::GetComponent<engine::TextRenderer>(startGameTimerEntity).text = "";

			return;
		}

		bool pressedStart = false;
		for (engine::ecs::Entity entity : entities)
		{
			PlayerSelection& playerSelection = engine::ecs::GetComponent<PlayerSelection>(entity);
			if(playerSelection.ready)
				pressedStart |= input::GetNewPress("Pause" + std::to_string(playerSelection.playerID));
		}

		if (startLevelTimer >= (startLevelTime + startLevelLoadingTime) || pressedStart)
		{
			auto& firstPlayer = ecs::GetSystem<LevelSelectionSystem>()->firstPlayer;
			firstPlayer = 5;
			for (auto player : selectedShipsAtTheFrame)
			{
				if (player.first < firstPlayer)
				{
					firstPlayer = player.first;
				}
			}
			printf("============= GAME STARTING ==========\n");

			// Reached timer end: Load level
			//canStartLoadingMap = true;
			startLevelTimer = 0;

			playerShips = selectedShipsAtTheFrame;
			isShipSelectionMenuOn = false;
			ToggleMenuPlayerSelection();
			std::shared_ptr<LevelSelectionSystem> levelSelectionSystem = engine::ecs::GetSystem<LevelSelectionSystem>();
			levelSelectionSystem->Init();

			gameState = mapSelection;
		}
	}

	void Update()
	{
		for (engine::ecs::Entity entity : entities)
		{
			PlayerSelection& playerSelection = engine::ecs::GetComponent<PlayerSelection>(entity);

			float turnInput = input::GetTotalInputValue("Turn" + std::to_string(playerSelection.playerID));
			int turnDpadInput = input::GetPressed("MenuDpadRight" + std::to_string(playerSelection.playerID)) - input::GetPressed("MenuDpadLeft" + std::to_string(playerSelection.playerID));
			turnInput += turnDpadInput;
			bool aPressed = input::GetNewPress("Shoot" + std::to_string(playerSelection.playerID));
			bool bPressed = input::GetNewPress("Boost" + std::to_string(playerSelection.playerID));

			std::string& playerReadyText = engine::ecs::GetComponent<engine::TextRenderer>(playerSelection.readyText).text;

			//Add player if not already joined
			if (!playerSelection.hasJoined)
			{
				// Shoot input adds player to the game and after that makes player Ready/Not ready
				if (aPressed)
				{
					// Add player to selectedShipsAtTheFrame if not yet have that player ID
					selectedShipsAtTheFrame.insert({ playerSelection.playerID, ShipType::torpedoBoat });

					playerReadyText = "Not Ready";
					playerSelection.hasJoined = true;

					engine::ecs::GetComponent< engine::SpriteRenderer>(playerSelection.arrowUp).enabled = true;
					engine::ecs::GetComponent< engine::SpriteRenderer>(playerSelection.arrowDown).enabled = true;
					engine::ecs::GetComponent< engine::SpriteRenderer>(playerSelection.backgroundImage).enabled = true;
					engine::ecs::GetComponent< engine::Transform>(playerSelection.shipModel).scale = 0.2;
					engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.readyText).color = Vector3(200, 0, 0);

					UpdateShipInfos(playerSelection);
				}
				//Hold be to return to main menu
				if (bPressed)
				{
					ReturnToMainMenu();
					return;
				}
			}
			else
			{
				if (!playerSelection.ready)
				{
					// Ready player with a
					if (aPressed)
					{
						resources::confirmSound->play();
						playerReadyText = "   Ready";
						playerSelection.ready = true;

						//Select correct player in selectedShipsAtTheFrame and set shipType be playerID in index 
						selectedShipsAtTheFrame[playerSelection.playerID] = ShipType(playerSelection.selection);

						engine::ecs::GetComponent< engine::ModelRenderer>(playerSelection.shipModel).model = shipModelsReady[playerSelection.selection];
						engine::ecs::GetComponent< engine::ModelRenderer>(playerSelection.shipModel).textures = { resources::playerIdToTexture[playerSelection.playerID] };
						engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.readyText).color = Vector3(0, 255, 0);

						playersThatAreReadyAmount++;

						UpdateShipInfos(playerSelection);
					}
					//Unjoin player
					else if (bPressed)
					{
						// Remove player from selectedShipsAtTheFrame
						selectedShipsAtTheFrame.erase(playerSelection.playerID);

						playerReadyText = "";
						playerSelection.hasJoined = false;

						engine::ecs::GetComponent< engine::SpriteRenderer>(playerSelection.arrowUp).enabled = false;
						engine::ecs::GetComponent< engine::SpriteRenderer>(playerSelection.arrowDown).enabled = false;
						engine::ecs::GetComponent< engine::SpriteRenderer>(playerSelection.backgroundImage).enabled = false;
						engine::ecs::GetComponent< engine::Transform>(playerSelection.shipModel).scale = 0;
						engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.readyText).color = Vector3(200, 0, 0);

						UpdateShipInfos(playerSelection);
					}
				}
				//Make unready
				else
				{
					// Unready player with a or b
					if (aPressed || bPressed)
					{
						playerReadyText = "Not Ready";
						playerSelection.ready = false;

						engine::ecs::GetComponent< engine::ModelRenderer>(playerSelection.shipModel).model = shipModels[playerSelection.selection];
						engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.readyText).color = Vector3(200, 0, 0);

						playersThatAreReadyAmount--;

						UpdateShipInfos(playerSelection);
					}
				}

				// Ready players can't change ships
				if (!playerSelection.ready)
				{
					if (turnInput == 0)
					{
						// Throttle in deadzone

						playerSelection.throttleCurrentWaitedTimeUp = throttleMoveWaitTime;
						playerSelection.throttleCurrentWaitedTimeDown = throttleMoveWaitTime;

						// Set arrow size to normal
						playerSelection.timeArrowBigTime += engine::deltaTime;
						if (playerSelection.timeArrowBigTime > 0.15f)
						{
							engine::ecs::GetComponent< engine::Transform>(playerSelection.arrowDown).scale = Vector3(0.03f);
							engine::ecs::GetComponent< engine::Transform>(playerSelection.arrowUp).scale = Vector3(0.03f);

							playerSelection.isArrowBig = false;
							playerSelection.timeArrowBigTime = 0;
						}

						goto SkipTurnInput;
					}

					if (turnInput > 0)
					{
						// Throttle up
						playerSelection.throttleCurrentWaitedTimeUp += engine::deltaTime;

						while (playerSelection.throttleCurrentWaitedTimeUp >= throttleMoveWaitTime)
						{
							// Next ship
							playerSelection.throttleCurrentWaitedTimeUp -= throttleMoveWaitTime;

							resources::moveSound->play();
							playerSelection.selection++;
							if (playerSelection.selection >= shipModels.size())
							{
								playerSelection.selection = 0;
							}
						}
						engine::ecs::GetComponent< engine::Transform>(playerSelection.arrowDown).scale = Vector3(0.03f);
						engine::ecs::GetComponent< engine::Transform>(playerSelection.arrowUp).scale = Vector3(0.08f);
						engine::ecs::GetComponent< engine::ModelRenderer>(playerSelection.shipModel).model = shipModels[playerSelection.selection];
						playerSelection.isArrowBig = true;
					}
					else
					{
						// No throttle input up
						playerSelection.throttleCurrentWaitedTimeUp = 0;
					}

					if (turnInput < 0)
					{
						// Throttle down
						playerSelection.throttleCurrentWaitedTimeDown += engine::deltaTime;

						while (playerSelection.throttleCurrentWaitedTimeDown >= throttleMoveWaitTime)
						{
							// Previous ship
							playerSelection.throttleCurrentWaitedTimeDown -= throttleMoveWaitTime;

							resources::moveSound->play();
							playerSelection.selection--;
							if (playerSelection.selection < 0)
							{
								playerSelection.selection = shipModels.size() - 1;
							}
						}
						engine::ecs::GetComponent< engine::Transform>(playerSelection.arrowUp).scale = Vector3(0.03f);
						engine::ecs::GetComponent< engine::Transform>(playerSelection.arrowDown).scale = Vector3(0.08f);
						engine::ecs::GetComponent< engine::ModelRenderer>(playerSelection.shipModel).model = shipModels[playerSelection.selection];
						playerSelection.isArrowBig = true;
					}
					else
					{
						// No throttle input down
						playerSelection.throttleCurrentWaitedTimeDown = 0;
					}

					// Set ship info	
					UpdateShipInfos(playerSelection);
				}
			}
		SkipTurnInput:

			engine::Transform& ShipModelTransform = engine::ecs::GetComponent<engine::Transform>(playerSelection.shipModel);

			double yRotation = ShipModelTransform.rotation.y;
			if (ShipModelTransform.rotation.y < 360)
			{
				yRotation += 35 * engine::deltaTime;
			}
			else  yRotation = ShipModelTransform.rotation.y - 360 + 35 * engine::deltaTime;

			ShipModelTransform.rotation = Vector3(6, yRotation, 0);
		}

		StartGame();
	}

	void ToggleMenuPlayerSelection()
	{
		for (engine::ecs::Entity entity : entities)
		{
			PlayerSelection& playerSelection = engine::ecs::GetComponent<PlayerSelection>(entity);

			if (isShipSelectionMenuOn)// Show everything
			{
				canStartLoadingMap = false;

				engine::ecs::GetComponent< engine::SpriteRenderer>(playerSelection.backgroundImage).enabled = true;

				engine::ecs::GetComponent< engine::SpriteRenderer>(playerSelection.arrowUp).enabled = true;
				engine::ecs::GetComponent< engine::SpriteRenderer>(playerSelection.arrowDown).enabled = true;
				engine::ecs::GetComponent< engine::ModelRenderer>(playerSelection.shipModel).enabled = true;

				UpdateShipInfos(playerSelection);

				std::cout << "\nisShipSelectionMenuOn: " << isShipSelectionMenuOn << "\n";
			}

			if (!isShipSelectionMenuOn)	//hide every thing
			{
				engine::ecs::GetComponent< engine::SpriteRenderer>(playerSelection.backgroundImage).enabled = false;

				engine::ecs::GetComponent< engine::SpriteRenderer>(playerSelection.arrowUp).enabled = false;
				engine::ecs::GetComponent< engine::SpriteRenderer>(playerSelection.arrowDown).enabled = false;
				engine::ecs::GetComponent< engine::ModelRenderer>(playerSelection.shipModel).enabled = false;

				engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.readyText).text = "";

				engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.specialEntity).text = "";
				engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.boostEntity).text = "";
				engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.baseSpeedEntity).text = "";
				engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.maneuvarabilityEntity).text = "";
				engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.shipNameEntity).text = "";
				engine::ecs::GetComponent< engine::TextRenderer>(playerSelection.shipInfo).text = "";

				engine::ecs::GetComponent<engine::TextRenderer>(startGameTimerEntity).text = "";

				//This is a temporary control cheme disaply for playtesting
				engine::ecs::GetComponent<engine::SpriteRenderer>(controlScheme).enabled = false;
			}
		}
	}
};

void ToggleShipSelectMenu()
{
	ecs::DestroyAllEntities();
	ecs::GetSystem<PlayerSelectSystem>()->Init();
	ecs::GetSystem<PlayerSelectSystem>()->isShipSelectionMenuOn = true;
	gameState = selectPlayersState;
}

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
	bool isTitle = false;
};

ECS_REGISTER_SYSTEM(PauseSystem, PauseComponent, engine::Transform)
class PauseSystem : public engine::ecs::System
{
	enum inGamePauseOptionsStates { main, options, sounds };
	inGamePauseOptionsStates pauseMenuAt;
	engine::ecs::Entity pausedImage;
	engine::ecs::Entity optionsImage;

	engine::ecs::Entity resumeButton;
	engine::ecs::Entity optionsButton;

	engine::ecs::Entity mainMenuButton;

	engine::ecs::Entity quitGameButton;

	//std::map<std::string, ecs::Entity> pauseButtons{ {"return", resumeButton}, {"options", optionsButton}, {"mainMenu", mainMenuButton},{"quitGame", quitGameButton} };

	//options buttons
	engine::ecs::Entity optionsResumeButton;
	engine::ecs::Entity musicSliderEntity, musicSliderNub;
	engine::ecs::Entity fullscreenEntity;

	vector<engine::ecs::Entity>  optionsButtons = { {optionsResumeButton},{musicSliderEntity}, {fullscreenEntity} };
	//std::map<std::string, ecs::Entity> optionsButtons{ {"return", optionsResumeButton}, {"music", musicSliderEntity}, {"fullscreen", fullscreenEntity} };

	float repeatInputDelay;
	float moveWaitedTimerDown;
	const float delay = 0.3f;

	//Volume slider
	const float sliderLeftBound = -0.168;
	const float sliderRightBound = 0.166;
	const float sliderLeftBoundMin = -0.13;
	const float sliderRightBoundMin = 0.13;

public:
	int playerWhichPaused = 0;
	bool isGamePause = false;
	engine::ecs::Entity currentSelection;

	void Init()
	{
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
		optionsImage = engine::ecs::NewEntity();

		//                      ---------  PAUSE BUTTONS init-------
		engine::ecs::AddComponent(pausedImage, engine::Transform{ .position = Vector3(0, .8f, -0.1f), .scale = Vector3(0.35f) });
		engine::ecs::AddComponent(pausedImage, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Paused.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(pausedImage, PauseComponent{ .upper = pausedImage, .lower = optionsButton, .selectedTexture = resources::menuTextures["UI_Paused.png"], .unselectedTexture = resources::menuTextures["UI_Paused.png"], .operation = PauseSystem::OnResumePressed, .isTitle = true });

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
		//options Title
		engine::ecs::AddComponent(optionsImage, engine::Transform{ .position = Vector3(0, .8f, -0.1f), .scale = Vector3(.35f) });
		engine::ecs::AddComponent(optionsImage, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Options_N.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(optionsImage, PauseComponent{ .unselectedTexture = resources::menuTextures["UI_Options_N.png"], .isOptionsMenu = true, .isTitle = true });
		//optionsResumeButton
		engine::ecs::AddComponent(optionsResumeButton, engine::Transform{ .position = Vector3(0, .5f, -0.1f), .scale = Vector3(.25f) });
		engine::ecs::AddComponent(optionsResumeButton, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Resume.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(optionsResumeButton, PauseComponent{ .upper = fullscreenEntity, .lower = musicSliderEntity, .selectedTexture = resources::menuTextures["UI_Resume.png"], .unselectedTexture = resources::menuTextures["UI_Resume_N.png"], .operation = BackToUIMenu, .isOptionsMenu = true });
		//musicSliderEntity
		engine::ecs::AddComponent(musicSliderEntity, engine::Transform{ .position = Vector3(0, .3f, -0.1f), .scale = Vector3(0.25f) });
		engine::ecs::AddComponent(musicSliderEntity, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_SFX_Slider.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(musicSliderEntity, PauseComponent{ .upper = optionsResumeButton, .lower = fullscreenEntity, .selectedTexture = resources::menuTextures["UI_SFX_Slider.png"], .unselectedTexture = resources::menuTextures["UI_SFX_Slider_N.png"], .isOptionsMenu = true,.isSlider = true });
		//musicSliderNub
		engine::ecs::AddComponent(musicSliderNub, engine::Transform{ .position = engine::ecs::GetComponent<engine::Transform>(musicSliderEntity).position + Vector3(0, -0.1f, 0.1), .scale = Vector3(0.05f) });
		engine::ecs::AddComponent(musicSliderNub, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Slider_Button.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(musicSliderNub, PauseComponent{ .selectedTexture = resources::menuTextures["UI_Slider_Button.png"], .unselectedTexture = resources::menuTextures["UI_Slider_Button.png"], .isOptionsMenu = true, .isTitle = true });
		//fullscreenEntity
		engine::ecs::AddComponent(fullscreenEntity, engine::Transform{ .position = Vector3(0, .0f, -0.1f), .scale = Vector3(0.25f) });
		engine::ecs::AddComponent(fullscreenEntity, engine::SpriteRenderer{ .texture = resources::menuTextures["UI_Windowed.png"], .enabled = false, .uiElement = true });
		engine::ecs::AddComponent(fullscreenEntity, PauseComponent{ .upper = musicSliderEntity, .lower = optionsResumeButton, .selectedTexture = resources::menuTextures["UI_Windowed.png"], .unselectedTexture = resources::menuTextures["UI_Windowed_N.png"], .operation = OnFullScreenPressed, .isOptionsMenu = true });
	}

	void Update()
	{
		float verticalInput = input::GetTotalInputValue("MenuVertical");
		int verticalDpadInput = input::GetPressed("MenuDpadDown") - input::GetPressed("MenuDpadUp");
		verticalInput += verticalDpadInput;

		float horizontalInput = input::GetTotalInputValue("MenuHorizontal");
		int horizontalDpadInput = input::GetPressed("MenuDpadRight") - input::GetPressed("MenuDpadLeft");
		horizontalInput += horizontalDpadInput;

		bool confirmInput = input::GetNewPress("MenuConfirm");
		bool backInput = input::GetNewPress("MenuBack");
		bool pauseInput = input::GetNewPress("Pause");

		repeatInputDelay -= engine::deltaTime;

		//Move up/down
		if (verticalInput >= 0.5f)
		{
			if (repeatInputDelay <= 0)
			{
				MoveLower();
				repeatInputDelay = delay;
			}
		}
		else if (verticalInput <= -0.5f)
		{
			if (repeatInputDelay <= 0)
			{
				MoveUpper();
				repeatInputDelay = delay;
			}
		}
		else
		{
			repeatInputDelay = 0;
		}

		//Move slider left/right
		PauseComponent& pc = ecs::GetComponent<PauseComponent>(currentSelection);
		if (pc.isSlider)
		{
			if (horizontalInput >= 0.5f)
			{
				MusicSliderRight();
			}
			else if (horizontalInput <= -0.5f)
			{
				MusicSliderLeft();
			}
		}

		//A pressed
		if (confirmInput)
		{
			Selected();
		}

		//B pressed
		if (backInput)
		{
			if (pauseMenuAt == options)
			{
				BackToUIMenu();
			}
			else
			{
				OnResumePressed();
			}
		}
		//Return to game
		if (pauseInput)
		{
			if (pauseMenuAt == options)
			{
				BackToUIMenu();
			}
			OnResumePressed();
		}
	}

	void MoveUpper()
	{
		//Old selection
		PauseComponent& pauseComponent = engine::ecs::GetComponent<PauseComponent>(currentSelection);
		engine::SpriteRenderer& unselectedSpriteRenderer = engine::ecs::GetComponent<engine::SpriteRenderer>(currentSelection);
		engine::Transform& unselectedTransform = engine::ecs::GetComponent<engine::Transform>(currentSelection);

		if (pauseComponent.isSlider)
		{
			Transform& nubTransform = ecs::GetComponent<Transform>(musicSliderNub);
			nubTransform.position.y = unselectedTransform.position.y - 0.083f;
			nubTransform.scale = Vector3(0.043f);
			nubTransform.position.x = std::lerp(sliderLeftBoundMin, sliderRightBoundMin, volume);
		}

		unselectedTransform.scale = Vector3(0.25f);
		unselectedSpriteRenderer.texture = pauseComponent.unselectedTexture;

		//New selection
		currentSelection = pauseComponent.upper;
		PauseComponent& pauseComponent1 = engine::ecs::GetComponent<PauseComponent>(currentSelection);
		engine::SpriteRenderer& selectedSpriteRenderer = engine::ecs::GetComponent<engine::SpriteRenderer>(currentSelection);
		engine::Transform& selectedSpriteTransform = engine::ecs::GetComponent<engine::Transform>(currentSelection);
		selectedSpriteTransform.scale = Vector3(0.32f);
		selectedSpriteRenderer.texture = pauseComponent1.selectedTexture;

		if (pauseComponent1.isSlider)
		{
			Transform& nubTransform = ecs::GetComponent<Transform>(musicSliderNub);
			nubTransform.position.y = selectedSpriteTransform.position.y - 0.107f;
			nubTransform.scale = Vector3(0.063f);
			nubTransform.position.x = std::lerp(sliderLeftBound, sliderRightBound, volume);
		}
	}
	void MoveLower()
	{
		//Old selection
		PauseComponent& pauseComponent = engine::ecs::GetComponent<PauseComponent>(currentSelection);
		engine::SpriteRenderer& unselectedSpriteRenderer = engine::ecs::GetComponent<engine::SpriteRenderer>(currentSelection);
		engine::Transform& unselectedTransform = engine::ecs::GetComponent<engine::Transform>(currentSelection);

		unselectedTransform.scale = Vector3(0.25f);
		unselectedSpriteRenderer.texture = pauseComponent.unselectedTexture;

		if (pauseComponent.isSlider)
		{
			Transform& nubTransform = ecs::GetComponent<Transform>(musicSliderNub);
			nubTransform.position.y = unselectedTransform.position.y - 0.083f;
			nubTransform.scale = Vector3(0.043f);
			nubTransform.position.x = std::lerp(sliderLeftBoundMin, sliderRightBoundMin, volume);
		}

		//New selection
		currentSelection = pauseComponent.lower;
		PauseComponent& pauseComponent1 = engine::ecs::GetComponent<PauseComponent>(currentSelection);
		engine::SpriteRenderer& selectedSpriteRenderer = engine::ecs::GetComponent<engine::SpriteRenderer>(currentSelection);
		engine::Transform& selectedSpriteTransform = engine::ecs::GetComponent<engine::Transform>(currentSelection);
		selectedSpriteTransform.scale = Vector3(0.32f);
		selectedSpriteRenderer.texture = pauseComponent1.selectedTexture;

		if (pauseComponent1.isSlider)
		{
			Transform& nubTransform = ecs::GetComponent<Transform>(musicSliderNub);
			nubTransform.position.y = selectedSpriteTransform.position.y - 0.107f;
			nubTransform.scale = Vector3(0.063f);
			nubTransform.position.x = std::lerp(sliderLeftBound, sliderRightBound, volume);
		}
	}

	static void BackToUIMenu()
	{
		engine::ecs::GetSystem<PauseSystem>()->pauseMenuAt = main;
		engine::ecs::GetSystem<PauseSystem>()->ToggleShowUIMenu();
	}
	static void OnResumePressed()
	{
		engine::ecs::GetSystem<PauseSystem>()->isGamePause = !engine::ecs::GetSystem<PauseSystem>()->isGamePause;
		engine::ecs::GetSystem<PauseSystem>()->ToggleShowUIMenu();

		gameState = gamePlayState;
		engine::enablePhysics = true;
		engine::enableAnimation = true;
	}
	static void OnOptionsPressed()
	{
		engine::ecs::GetSystem<PauseSystem>()->ToggleShowUIOptionsMenu();
	}
	static void OnQuitGamePressed()
	{
		glfwSetWindowShouldClose(mainWindow, true);
	}
	static void OnMainMenuPressed()
	{
		ReturnToMainMenu();
	}
	static void OnFullScreenPressed()
	{
		ToggleFullscreen();
		ecs::GetSystem<PauseSystem>()->UpdateFullscreenIcon();
	}
	void UpdateFullscreenIcon()
	{
		SpriteRenderer& sr = ecs::GetComponent<SpriteRenderer>(fullscreenEntity);
		PauseComponent& pauseComp = ecs::GetComponent<PauseComponent>(fullscreenEntity);

		if (!mainWindowFullscreen)
		{
			//Make windowed
			pauseComp.selectedTexture = resources::menuTextures["UI_Windowed.png"];
			pauseComp.unselectedTexture = resources::menuTextures["UI_Windowed_N.png"];
		}
		else
		{
			//Make Fullscreen
			pauseComp.selectedTexture = resources::menuTextures["UI_Fullscreen.png"];
			pauseComp.unselectedTexture = resources::menuTextures["UI_Fullscreen_N.png"];
		}
		//Set the button sprite
		if (currentSelection == fullscreenEntity)
			sr.texture = pauseComp.selectedTexture;
		else
			sr.texture = pauseComp.unselectedTexture;
	}

	void Selected()
	{
		PauseComponent& pauseComponent = engine::ecs::GetComponent<PauseComponent>(currentSelection);
		if (pauseComponent.operation)
			pauseComponent.operation();
	}

	void ToggleShowUIMenu()
	{
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

			PauseComponent& pauseComponent = engine::ecs::GetComponent<PauseComponent>(entity);
			if (!pauseComponent.isTitle)
			{
				engine::SpriteRenderer& selectedSpriteRenderer = engine::ecs::GetComponent<engine::SpriteRenderer>(entity);
				engine::Transform& selectedSpriteTransform = engine::ecs::GetComponent<engine::Transform>(entity);
				selectedSpriteTransform.scale = Vector3(0.25f);
				selectedSpriteRenderer.texture = pauseComponent.unselectedTexture;
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

	void ToggleShowUIOptionsMenu()
	{
		pauseMenuAt = options;
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

		//Set the right fullscreen/windowed button
		UpdateFullscreenIcon();

		//Set the music slider nub
		Transform& nubTransform = ecs::GetComponent<Transform>(musicSliderNub);
		nubTransform.position.y = ecs::GetComponent<Transform>(musicSliderEntity).position.y - 0.083f;
		nubTransform.scale = Vector3(0.043f);
		nubTransform.position.x = std::lerp(sliderLeftBoundMin, sliderRightBoundMin, volume);

		PauseComponent& pauseComponent = engine::ecs::GetComponent<PauseComponent>(currentSelection);
		engine::SpriteRenderer& selectedSpriteRenderer = engine::ecs::GetComponent<engine::SpriteRenderer>(currentSelection);
		engine::Transform& selectedSpriteTransform = engine::ecs::GetComponent<engine::Transform>(currentSelection);
		selectedSpriteTransform.scale = Vector3(0.32f);
		selectedSpriteRenderer.texture = pauseComponent.selectedTexture;
	}

	void MusicSliderLeft()
	{
		Transform& tf = ecs::GetComponent<Transform>(musicSliderNub);

		//Decrease volume
		if (volume > 0)
		{
			volume -= engine::deltaTime;
		}
		volume = std::max(volume, 0.f);

		//Move slider
		tf.position.x = std::lerp(sliderLeftBound, sliderRightBound, volume);

		ecs::GetSystem<SoundSystem>()->SetGlobalVolume(volume);
	}
	void MusicSliderRight()
	{
		Transform& tf = ecs::GetComponent<Transform>(musicSliderNub);

		//Increase volume
		if (volume < 1)
		{
			volume += engine::deltaTime;
		}
		volume = std::min(volume, 1.f);

		//Move slider
		tf.position.x = std::lerp(sliderLeftBound, sliderRightBound, volume);

		ecs::GetSystem<SoundSystem>()->SetGlobalVolume(volume);
	}
};

enum class MainMenuState { Off, Splash, Main, Options, Credits, Any };

ECS_REGISTER_COMPONENT(MainMenuComponent)
struct MainMenuComponent
{
	ecs::Entity upper, lower;
	Texture* selectedTexture;
	Texture* unselectedTexture;
	std::function<void()> operation;
	ecs::Entity slider = 0;
	float sliderValue = 0;
	std::vector<MainMenuState> showInStates;
};

ECS_REGISTER_SYSTEM(MainMenuSystem, MainMenuComponent)
class MainMenuSystem : public ecs::System
{
public:
	MainMenuState state = MainMenuState::Off;

	const float delay = 0.3f;
	float repeatInputDelay;

	const Vector2 offset{ -0.5, -0.35 };

	//Slider bounds
	const float sliderLeftBound = -0.168 + offset.x;
	const float sliderRightBound = 0.166 + offset.x;
	const float sliderLeftBoundMin = -0.13 + offset.x;
	const float sliderRightBoundMin = 0.13 + offset.x;

	ecs::Entity currentSelection;
	ecs::Entity startText;
	ecs::Entity startGame;
	ecs::Entity resume;
	ecs::Entity fullscreen;

	//Make and show the main menu
	void Init()
	{
		//Splash screen entities
		ecs::Entity splashScreen = ecs::NewEntity();
		ecs::AddComponent(splashScreen, Transform{ .position = {0, 0, -0.5}, .scale = {1, 1, 0} });
		ecs::AddComponent(splashScreen, SpriteRenderer{ .texture = resources::menuTextures["UI_Title_Background_1.png"], .uiElement = true });
		ecs::AddComponent(splashScreen, MainMenuComponent{ .showInStates = {MainMenuState::Splash, MainMenuState::Main, MainMenuState::Options} });

		startText = ecs::NewEntity();
		ecs::AddComponent(startText, Transform{ .position = { -0.5, 0.2, -0.1 }, .rotation = { 0, 0, 15 }, .scale = { .45, .1, 0 } });
		ecs::AddComponent(startText, SpriteRenderer{ .texture = resources::menuTextures["UI_PressStart.png"], .uiElement = true });
		ecs::AddComponent(startText, MainMenuComponent{ .showInStates = {MainMenuState::Splash} });
		ecs::AddComponent(startText, SoundComponent{ .Sounds = {{"Confirm", resources::confirmSound}, {"MoveSelection", resources::moveSound}}, .maxDistance = 200000, .referenceDistance = 200000, .rolloffFactor = 0 });

		//Main menu entities
		startGame = ecs::NewEntity();
		ecs::Entity options = ecs::NewEntity();
		ecs::Entity credits = ecs::NewEntity();
		ecs::Entity quitGame = ecs::NewEntity();

		ecs::AddComponent(startGame, Transform{ .position = Vector3(0, .6f, -0.1f) + offset, .scale = Vector3(0.32f) });
		ecs::AddComponent(startGame, SpriteRenderer{ .texture = resources::menuTextures["UI_StartGame.png"], .enabled = false, .uiElement = true });
		ecs::AddComponent(startGame, MainMenuComponent{ .upper = quitGame, .lower = options, .selectedTexture = resources::menuTextures["UI_StartGame.png"], .unselectedTexture = resources::menuTextures["UI_StartGame_N.png"], .operation = OnStartGamePressed, .showInStates = {MainMenuState::Main} });

		ecs::AddComponent(options, Transform{ .position = Vector3(0, .4f, -0.1f) + offset, .scale = Vector3(.25f) });
		ecs::AddComponent(options, SpriteRenderer{ .texture = resources::menuTextures["UI_Options_N.png"], .enabled = false, .uiElement = true });
		ecs::AddComponent(options, MainMenuComponent{ .upper = startGame, .lower = credits, .selectedTexture = resources::menuTextures["UI_Options.png"], .unselectedTexture = resources::menuTextures["UI_Options_N.png"], .operation = OnOptionsPressed, .showInStates = {MainMenuState::Main} });

		ecs::AddComponent(credits, Transform{ .position = Vector3(0, .2f, -0.1f) + offset, .scale = Vector3(0.25f) });
		ecs::AddComponent(credits, SpriteRenderer{ .texture = resources::menuTextures["UI_Credits_N.png"], .enabled = false, .uiElement = true });
		ecs::AddComponent(credits, MainMenuComponent{ .upper = options, .lower = quitGame, .selectedTexture = resources::menuTextures["UI_Credits.png"], .unselectedTexture = resources::menuTextures["UI_Credits_N.png"], .operation = OnCreditsPressed, .showInStates = {MainMenuState::Main} });

		ecs::AddComponent(quitGame, Transform{ .position = Vector3(0, -0.3f, -0.1f) + offset, .scale = Vector3(0.25f) });
		ecs::AddComponent(quitGame, SpriteRenderer{ .texture = resources::menuTextures["UI_QuitGame_N.png"], .enabled = false, .uiElement = true });
		ecs::AddComponent(quitGame, MainMenuComponent{ .upper = credits, .lower = startGame, .selectedTexture = resources::menuTextures["UI_QuitGame.png"], .unselectedTexture = resources::menuTextures["UI_QuitGame_N.png"], .operation = PauseSystem::OnQuitGamePressed, .showInStates = {MainMenuState::Main} });

		//Options entities
		ecs::Entity optionsImage = ecs::NewEntity();
		resume = ecs::NewEntity();
		ecs::Entity sfx = ecs::NewEntity();
		ecs::Entity sfxSlider = ecs::NewEntity();
		fullscreen = ecs::NewEntity();

		ecs::AddComponent(optionsImage, Transform{ .position = Vector3(0, .65f, -0.1f) + offset, .scale = Vector3(.35f) });
		ecs::AddComponent(optionsImage, SpriteRenderer{ .texture = resources::menuTextures["UI_Options_N.png"], .enabled = false, .uiElement = true });
		ecs::AddComponent(optionsImage, MainMenuComponent{ .unselectedTexture = resources::menuTextures["UI_Options_N.png"], .showInStates = {MainMenuState::Options} });

		ecs::AddComponent(resume, Transform{ .position = Vector3(0, .4f, -0.1f) + offset, .scale = Vector3(.25f) });
		ecs::AddComponent(resume, SpriteRenderer{ .texture = resources::menuTextures["UI_Resume.png"], .enabled = false, .uiElement = true });
		ecs::AddComponent(resume, MainMenuComponent{ .upper = fullscreen, .lower = sfx, .selectedTexture = resources::menuTextures["UI_Resume.png"], .unselectedTexture = resources::menuTextures["UI_Resume_N.png"], .operation = OnOptionsResumePressed, .showInStates = {MainMenuState::Options} });

		ecs::AddComponent(sfx, Transform{ .position = Vector3(0, .2f, -0.1f) + offset, .scale = Vector3(0.25f) });
		ecs::AddComponent(sfx, SpriteRenderer{ .texture = resources::menuTextures["UI_SFX_Slider_N.png"], .enabled = false, .uiElement = true });
		ecs::AddComponent(sfx, MainMenuComponent{ .upper = resume, .lower = fullscreen, .selectedTexture = resources::menuTextures["UI_SFX_Slider.png"], .unselectedTexture = resources::menuTextures["UI_SFX_Slider_N.png"], .slider = sfxSlider, .showInStates = {MainMenuState::Options} });

		ecs::AddComponent(sfxSlider, Transform{ .position = ecs::GetComponent<Transform>(sfx).position + Vector3(0, -0.083f, 0.1), .scale = Vector3(0.043f) });
		ecs::AddComponent(sfxSlider, SpriteRenderer{ .texture = resources::menuTextures["UI_Slider_Button.png"], .enabled = false, .uiElement = true });
		ecs::AddComponent(sfxSlider, MainMenuComponent{ .selectedTexture = resources::menuTextures["UI_Slider_Button.png"], .unselectedTexture = resources::menuTextures["UI_Slider_Button.png"], .showInStates = {MainMenuState::Options} });
		ecs::GetComponent<Transform>(sfxSlider).position.x = std::lerp(sliderLeftBoundMin, sliderRightBoundMin, volume);

		ecs::AddComponent(fullscreen, Transform{ .position = Vector3(0, -.1f, -0.1f) + offset, .scale = Vector3(0.25f) });
		ecs::AddComponent(fullscreen, SpriteRenderer{ .texture = resources::menuTextures["UI_Windowed_N.png"], .enabled = false, .uiElement = true });
		ecs::AddComponent(fullscreen, MainMenuComponent{ .upper = sfx, .lower = resume, .selectedTexture = resources::menuTextures["UI_Windowed.png"], .unselectedTexture = resources::menuTextures["UI_Windowed_N.png"], .operation = OnFullScreenPressed, .showInStates = {MainMenuState::Options} });

		//Credits entities
		ecs::Entity creditsScreen = ecs::NewEntity();
		ecs::AddComponent(creditsScreen, Transform{ .position = { 0, -0, 0.5 }, .scale = { 1, 1, 0 } });
		ecs::AddComponent(creditsScreen, SpriteRenderer{ .texture = resources::menuTextures["UI_Credits_Screen.png"], .uiElement = true });
		ecs::AddComponent(creditsScreen, MainMenuComponent{ .showInStates = {MainMenuState::Credits} });
				
		//Setup the state
		gameState = mainMenuState;
		ChangeState(MainMenuState::Splash, 0);
	}

	//Destroy the main menu
	void Unload()
	{
		state = MainMenuState::Off;

		// Iterate through entities in the system
		for (ecs::Entity entity : entities)
		{
			ecs::DestroyEntity(entity);
		}
	}

	//Handle animations and input
	void Update()
	{
		//Get Inputs
		float verticalInput = input::GetTotalInputValue("MenuVertical");
		int verticalDpadInput = input::GetPressed("MenuDpadDown") - input::GetPressed("MenuDpadUp");
		verticalInput += verticalDpadInput;

		float horizontalInput = input::GetTotalInputValue("MenuHorizontal");
		int horizontalDpadInput = input::GetPressed("MenuDpadRight") - input::GetPressed("MenuDpadLeft");
		horizontalInput += horizontalDpadInput;

		bool confirmInput = input::GetNewPress("MenuConfirm");
		bool backInput = input::GetNewPress("MenuBack");
		bool pauseInput = input::GetNewPress("Pause");
		repeatInputDelay -= engine::deltaTime;

		switch (state)
		{
		case MainMenuState::Off:
			return;
			break;

		case MainMenuState::Splash:
			//Splash title animation
			TransformSystem::SetScale(startText, Vector3(.45, .1, 0) + Vector3((std::sin(programTime * 4) / 700), (std::sin(programTime * 4) / 3150), 0) * 35);
			//Go to Main selection
			if (confirmInput || pauseInput)
			{
				resources::confirmSound->play();
				ChangeState(MainMenuState::Main, startGame);
			}
			break;

		case MainMenuState::Main:
			//Move up/down
			if (verticalInput >= 0.5f)
			{
				if (repeatInputDelay <= 0)
				{
					MoveSelection(false);
					repeatInputDelay = delay;
				}
			}
			else if (verticalInput <= -0.5f)
			{
				if (repeatInputDelay <= 0)
				{
					MoveSelection(true);
					repeatInputDelay = delay;
				}
			}
			else
			{
				repeatInputDelay = 0;
			}

			//A pressed
			if (confirmInput)
			{
				Selected();
			}
			break;

		case MainMenuState::Options:
		{
			//Move up/down
			if (verticalInput >= 0.5f)
			{
				if (repeatInputDelay <= 0)
				{
					MoveSelection(false);
					repeatInputDelay = delay;
				}
			}
			else if (verticalInput <= -0.5f)
			{
				if (repeatInputDelay <= 0)
				{
					MoveSelection(true);
					repeatInputDelay = delay;
				}
			}
			else
			{
				repeatInputDelay = 0;
			}

			//Move slider left/right
			MainMenuComponent& mmc = ecs::GetComponent<MainMenuComponent>(currentSelection);
			if (ecs::EntityExists(mmc.slider))
			{
				if (horizontalInput >= 0.5f)
				{
					SliderRight(mmc.slider);
				}
				else if (horizontalInput <= -0.5f)
				{
					SliderLeft(mmc.slider);
				}
			}

			//A pressed
			if (confirmInput)
			{
				Selected();
			}

			//B pressed
			if (backInput)
			{
				ChangeState(MainMenuState::Main, startGame);
			}
			break;
		}
		case MainMenuState::Credits:
			//Any key pressed
			if (backInput || confirmInput || pauseInput)
			{
				ChangeState(MainMenuState::Main, startGame);
			}

			break;
		}
	}

	void Selected()
	{
		MainMenuComponent& mmc = ecs::GetComponent<MainMenuComponent>(currentSelection);
		if (mmc.operation)
		{
			resources::confirmSound->play();
			mmc.operation();
		}
	}

	void MoveSelection(bool up)
	{
		resources::moveSound->play();

		//Old selection
		MainMenuComponent& oldMMC = ecs::GetComponent<MainMenuComponent>(currentSelection);
		SpriteRenderer& oldSR = ecs::GetComponent<SpriteRenderer>(currentSelection);
		Transform& oldTF = ecs::GetComponent<Transform>(currentSelection);

		oldTF.scale = Vector3(0.25f);
		oldSR.texture = oldMMC.unselectedTexture;

		if (ecs::EntityExists(oldMMC.slider))
		{
			Transform& sliderTF = ecs::GetComponent<Transform>(oldMMC.slider);
			sliderTF.position.y = oldTF.position.y - 0.083f;
			sliderTF.scale = Vector3(0.043f);
			sliderTF.position.x = std::lerp(sliderLeftBoundMin, sliderRightBoundMin, volume);
		}

		//New selection
		currentSelection = up ? oldMMC.upper : oldMMC.lower;
		MainMenuComponent& newMMC = ecs::GetComponent<MainMenuComponent>(currentSelection);
		SpriteRenderer& newSR = ecs::GetComponent<SpriteRenderer>(currentSelection);
		Transform& newTF = ecs::GetComponent<Transform>(currentSelection);

		newTF.scale = Vector3(0.32f);
		newSR.texture = newMMC.selectedTexture;

		if (ecs::EntityExists(newMMC.slider))
		{
			Transform& sliderTF = ecs::GetComponent<Transform>(newMMC.slider);
			sliderTF.position.y = newTF.position.y - 0.107f;
			sliderTF.scale = Vector3(0.063f);
			sliderTF.position.x = std::lerp(sliderLeftBound, sliderRightBound, volume);
		}
	}

	void SliderLeft(ecs::Entity slider)
	{
		//Decrease volume
		if (volume > 0)
		{
			volume -= engine::deltaTime;
		}
		volume = std::max(volume, 0.f);

		//Move slider
		Transform& tf = ecs::GetComponent<Transform>(slider);
		tf.position.x = std::lerp(sliderLeftBound, sliderRightBound, volume);

		ecs::GetSystem<SoundSystem>()->SetGlobalVolume(volume);
	}
	void SliderRight(ecs::Entity slider)
	{
		//Increase volume
		if (volume < 1)
		{
			volume += engine::deltaTime;
		}
		volume = std::min(volume, 1.f);

		//Move slider
		Transform& tf = ecs::GetComponent<Transform>(slider);
		tf.position.x = std::lerp(sliderLeftBound, sliderRightBound, volume);

		ecs::GetSystem<SoundSystem>()->SetGlobalVolume(volume);
	}

	static void OnStartGamePressed()
	{
		ecs::GetSystem<PlayerSelectSystem>()->Init();
		ecs::GetSystem<PlayerSelectSystem>()->isShipSelectionMenuOn = true;
		gameState = selectPlayersState;
		ecs::GetSystem<MainMenuSystem>()->Unload();
	}

	static void OnOptionsPressed()
	{
		auto mms = ecs::GetSystem<MainMenuSystem>();
		mms->ChangeState(MainMenuState::Options, mms->resume);
		mms->UpdateFullscreenIcon();
	}

	static void OnCreditsPressed()
	{
		auto mms = ecs::GetSystem<MainMenuSystem>();
		mms->ChangeState(MainMenuState::Credits, 0);
	}

	static void OnOptionsResumePressed()
	{
		auto mms = ecs::GetSystem<MainMenuSystem>();
		mms->ChangeState(MainMenuState::Main, mms->startGame);
	}

	static void OnFullScreenPressed()
	{
		ToggleFullscreen();
		ecs::GetSystem<MainMenuSystem>()->UpdateFullscreenIcon();
	}
	void UpdateFullscreenIcon()
	{
		SpriteRenderer& sr = ecs::GetComponent<SpriteRenderer>(fullscreen);
		MainMenuComponent& pauseComp = ecs::GetComponent<MainMenuComponent>(fullscreen);

		if (!mainWindowFullscreen)
		{
			//Make windowed
			pauseComp.selectedTexture = resources::menuTextures["UI_Windowed.png"];
			pauseComp.unselectedTexture = resources::menuTextures["UI_Windowed_N.png"];
		}
		else
		{
			//Make Fullscreen
			pauseComp.selectedTexture = resources::menuTextures["UI_Fullscreen.png"];
			pauseComp.unselectedTexture = resources::menuTextures["UI_Fullscreen_N.png"];
		}
		//Set the button sprite
		if (currentSelection == fullscreen)
			sr.texture = pauseComp.selectedTexture;
		else
			sr.texture = pauseComp.unselectedTexture;
	}

	void ChangeState(MainMenuState newState, ecs::Entity newSelection)
	{
		state = newState;

		//Iterate through entities in the system
		for (ecs::Entity entity : entities)
		{
			MainMenuComponent& mmc = ecs::GetComponent<MainMenuComponent>(entity);
			SpriteRenderer& sr = ecs::GetComponent<SpriteRenderer>(entity);

			//Show all entities which should be shown
			if (std::find(mmc.showInStates.begin(), mmc.showInStates.end(), state) != mmc.showInStates.end() ||
				std::find(mmc.showInStates.begin(), mmc.showInStates.end(), MainMenuState::Any) != mmc.showInStates.end())
			{
				sr.enabled = true;
			}
			else
			{
				sr.enabled = false;
			}
		}

		//Set the previous selected to unselected
		if (ecs::EntityExists(currentSelection))
		{
			MainMenuComponent& selectedMMC = ecs::GetComponent<MainMenuComponent>(currentSelection);
			SpriteRenderer& selectedSR = ecs::GetComponent<SpriteRenderer>(currentSelection);
			selectedSR.texture = selectedMMC.unselectedTexture;
			TransformSystem::SetScale(currentSelection, Vector3(0.25f));
		}

		//Set the new selected to selected
		currentSelection = newSelection;
		if (ecs::EntityExists(currentSelection))
		{
			MainMenuComponent& selectedMMC = ecs::GetComponent<MainMenuComponent>(currentSelection);
			SpriteRenderer& selectedSR = ecs::GetComponent<SpriteRenderer>(currentSelection);
			selectedSR.texture = selectedMMC.selectedTexture;
			TransformSystem::SetScale(currentSelection, Vector3(0.32f));
		}
	}
};

void LevelSelectionSystem::LoadThisLevel(int mapIndex)
{
	ecs::DestroyAllEntities();
	gameState = gamePlayState;
	switch (mapIndex)
	{
	case 0:
		LoadLevel1(cam);
		break;
	case 1:
		LoadLevel2(cam);
		break;
	case 2:
		LoadLevel3(cam);
		break;
	case 3:
		LoadLevel4(cam);
		break;
	case 4:
		LoadLevel5(cam);
		break;
	case 5:
		LoadLevel6(cam);
		break;
	default:
		std::cout << "NO LEVEL ON THAT INDEX" << mapIndex << std::endl;
		break;
	}
	ecs::GetSystem<PauseSystem>()->Init();
}