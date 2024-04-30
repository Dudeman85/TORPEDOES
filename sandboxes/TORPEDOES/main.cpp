#include "PlayerController.h"
#include "engine/Input.h"  
#include "MenuSystem.h"	
#include "GameCamera.h"
#include "engine/SoundComponent.h"
#include "Pickups.h"

using namespace engine;

int checkPointNumber = 0;
bool isGamePaused = false;

static void CreateCheckpoint(Vector3 position, Vector3 rotation, Vector3 scale, engine::Model* checkPointModel, float hitboxrotation, bool finishLine = false)
{
	engine::ecs::Entity checkpoint = engine::ecs::NewEntity();

	engine::ecs::AddComponent(checkpoint, engine::Transform{ .position = position, .rotation = rotation, .scale = scale });
	engine::ecs::AddComponent(checkpoint, engine::ModelRenderer{ .model = checkPointModel });
	engine::ecs::AddComponent(checkpoint, CheckPoint{ .checkPointID = checkPointNumber , .Finish_line = finishLine });
	std::vector<Vector2> CheckpointcolliderVerts;
	if (finishLine)
	{
		CheckpointcolliderVerts = { Vector2(1, 8), Vector2(1, -8), Vector2(-1, -8), Vector2(-1, 8) };
	}
	else {
		CheckpointcolliderVerts = { Vector2(4, 8), Vector2(4, -8), Vector2(-4, -8), Vector2(-4, 8) };
	}
	engine::ecs::AddComponent(checkpoint, engine::PolygonCollider({ .vertices = CheckpointcolliderVerts, .trigger = true, .visualise = true, .rotationOverride = hitboxrotation }));

	checkPointNumber++;
};

static void CreateCrowd(Vector3 pos, engine::Animation& anim)
{
	engine::ecs::Entity crowd = engine::ecs::NewEntity();
	engine::ecs::AddComponent(crowd, engine::Transform{ .position = pos, .scale = Vector3(100, 30, 0) });
	engine::ecs::AddComponent(crowd, engine::SpriteRenderer{});
	engine::ecs::AddComponent(crowd, engine::Animator{.playbackSpeed = (((float)rand() / (RAND_MAX)) + 2) / 2 });
	engine::AnimationSystem::AddAnimation(crowd, anim, "CrowdCheer");
	engine::AnimationSystem::PlayAnimation(crowd, "CrowdCheer", true);
}

//Play the countdown timer and freeze players untill it is done
static void PlayCountdown(Vector3 pos)
{
	engine::ecs::Entity countdown = engine::ecs::NewEntity();
	engine::ecs::AddComponent(countdown, engine::Transform{ .position = pos, .scale = Vector3(60, 100, 100) });
	engine::ecs::AddComponent(countdown, engine::SpriteRenderer{});
	engine::ecs::AddComponent(countdown, engine::Animator{ .onAnimationEnd = engine::ecs::DestroyEntity });
	engine::AnimationSystem::AddAnimation(countdown, resources::countdownAnim, "CountDown");
	engine::AnimationSystem::PlayAnimation(countdown, "CountDown", false);
	engine::ecs::GetSystem<PlayerController>()->countdownTimer = 3;
}

// Create everything for level 1
void LoadLevel1(engine::Camera* cam)
{
	engine::collisionSystem->cam = cam;

	std::vector<ShipType> ships{ ShipType::torpedoBoat, ShipType::submarine, ShipType::hedgehogBoat, ShipType::cannonBoat };
	
	// Set this level's tilemap
	engine::spriteRenderSystem->SetTilemap(resources::level1Map);
	engine::collisionSystem->SetTilemap(resources::level1Map);
	engine::PhysicsSystem::SetTileProperty(1, engine::TileProperty{ true });

	//Create the players
	engine::ecs::GetSystem<PlayerController>()->CreatePlayers(playerShips, Vector2(1225.0f, -420.0f));


	////Make all the checkpoints manually
	CreateCheckpoint(Vector3(1925.000000, -895.000000, 100.000000), Vector3(27.500000, 27.500000, 10.000000), Vector3(18.f), resources::models["Prop_Buoy.obj"], 110.f);
	CreateCheckpoint(Vector3(2590.000000, -1475.000000, 100.000000), Vector3(30.000000, 37.500000, 5.000000), Vector3(18.f), resources::models["Prop_Buoy.obj"],110.f);
	CreateCheckpoint(Vector3(3590.000000, -1425.000000, 100.000000), Vector3(30.000000, 12.500000, -5.000000), Vector3(17.5f), resources::models["Prop_Buoy.obj"], 95.f);
	CreateCheckpoint(Vector3(3280.000000, -640.000000, 100.000000), Vector3(35.000000, 77.500000, 17.500000), Vector3(17.5f), resources::models["Prop_Buoy.obj"], 160.f);
	CreateCheckpoint(Vector3(1770.000000, -1585.000000, 100.000000), Vector3(50.000000, -62.500000, 0.000000), Vector3(18.5f), resources::models["Prop_Buoy.obj"], 20.f);
	CreateCheckpoint(Vector3(820.000000, -1610.000000, 100.000000), Vector3(47.500000, -77.500000, -5.000000), Vector3(20.5f), resources::models["Prop_Buoy.obj"], 10.f);
	CreateCheckpoint(Vector3(545.000000, -900.000000, 100.000000), Vector3(35.000000, -22.500000, 0.000000), Vector3(17.5f), resources::models["Prop_Buoy.obj"], 70.f);
	CreateCheckpoint(Vector3(1475.000000, -460.000000, 100.000000), Vector3(-25.000000, -90.000000, -90.000000), Vector3(24.f), resources::models["Prop_Goal_Ver2.obj"], 360.f, true);

	//crossroad
	//engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2200.000000, -1075.000000, 0.300000));
	//engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2200.000000, -1145.000000, 0.300000));
	//engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2200.000000, -1230.000000, 0.300000));

	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2170.000000, -1190.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2200.000000, -1085.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2270.000000, -1150.000000, 0.300000));

	//center slalom
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3270.000000, -1125.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3270.000000, -1045.000000, 0.300000));

	////hairpin top
	//engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1305.000000, -995.000000, 0.300000));
	//engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1170.000000, -995.000000, 0.300000));
	//engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1020.000000, -995.000000, 0.300000));

	//second last corner
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(700.000000, -1710.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(615.000000, -1645.000000, 0.300000));


	////Make the crowds manually
	//CreateCrowd({ 1530, -1700, 10 }, resources::crowdAnims);
	//CreateCrowd({ 1545, -1715, 11 }, resources::crowdAnims);
	//CreateCrowd({ 1520, -1730, 12 }, resources::crowdAnims);

	
	PlayCountdown(Vector3(1225.0f, -320.0f, 0.0f));
	PlayerController::lapCount = 3;
}

// Create everything for level 2
void LoadLevel2(engine::Camera* cam)
{
	engine::collisionSystem->cam = cam;

	std::vector<ShipType> ships{ ShipType::torpedoBoat, ShipType::submarine, ShipType::hedgehogBoat, ShipType::cannonBoat };
	engine::ecs::GetSystem<PlayerController>()->CreatePlayers(playerShips, Vector2(1160.0f, -1600.0f));

	// Set this level's tilemap
	engine::spriteRenderSystem->SetTilemap(resources::level2Map);
	engine::collisionSystem->SetTilemap(resources::level2Map);
	engine::PhysicsSystem::SetTileProperty(1, engine::TileProperty{ true });

	// Make all the checkpoint's manually
	CreateCheckpoint(Vector3(1455.000000, -995.000000, 100.000000), Vector3(35.000000, -15.000000, -20.000000), Vector3(17), resources::models["Prop_Buoy.obj"], 65.0f);				// First checkpoint
	CreateCheckpoint(Vector3(2430.000000, -1630.000000, 100.000000), Vector3(7.500000, 92.500000, 90.000000), Vector3(17), resources::models["Prop_Buoy_Vertical.obj"], 0.0f);			// Second checkpoint
	CreateCheckpoint(Vector3(3595.000000, -1095.000000, 100.000000), Vector3(40.000000, 0.000000, 47.500000), Vector3(17), resources::models["Prop_Buoy_Vertical.obj"], 130.0f);		// Third checkpoint
	CreateCheckpoint(Vector3(2715.000000, -1130.000000, 100.000000), Vector3(45.000000, 0.000000, 0.000000), Vector3(17), resources::models["Prop_Buoy.obj"], 90.0f);					// Fourth checkpoint
	CreateCheckpoint(Vector3(2415.000000, -500.000000, 100.000000), Vector3(25.000000, -17.500000, -30.000000), Vector3(17), resources::models["Prop_Buoy.obj"], 55.0f);				// Fifth checkpoint
	CreateCheckpoint(Vector3(1505.000000, -335.000000, 100.000000), Vector3(52.500000, -32.500000, -5.000000), Vector3(17), resources::models["Prop_Buoy.obj"], 60.0f);					// Sixth checkpoint
	CreateCheckpoint(Vector3(600.000000, -665.000000, 100.000000), Vector3(40.000000, -25.000000, -17.500000), Vector3(17), resources::models["Prop_Buoy.obj"], 60.0f);					// Seventh checkpoint
	CreateCheckpoint(Vector3(530.000000, -1675.000000, 100.000000), Vector3(45.000000, 32.500000, 47.500000), Vector3(17), resources::models["Prop_Buoy_Vertical.obj"], 145.0f);		// Eight checkpoint
	CreateCheckpoint(Vector3(1245.000000, -1680.000000, 100.000000), Vector3(75.000000, -90.000000, 7.500000), Vector3(22), resources::models["Prop_Goal_Ver2.obj"], 0.0f, true);	// Finish line

	//first loop
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1495.000000, -1240.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1665.000000, -995.000000, 0.300000));

	//second loop triple
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3365.000000, -1060.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3365.000000, -1025.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3365.000000, -985.000000, 0.300000));

	//second loop, between stones
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2950.000000, -1585.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2885.000000, -1055.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2710.000000, -1790.000000, 0.300000));

	//topright hairpin
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3355.000000, -410.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3355.000000, -375.000000, 0.300000));

	//before last loop
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2255.000000, -800.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2155.000000, -605.000000, 0.300000));

	//before chicane
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(425.000000, -855.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(480.000000, -855.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(540.000000, -855.000000, 0.300000));

	// Make the crowds manually
	// Start/Finish line crowd
	CreateCrowd({ 1030.000000, -1850.000000, 100 }, resources::crowdAnims);	// First row, first crowd
	CreateCrowd({ 1230.000000, -1850.000000, 100 }, resources::crowdAnims);	// First row, second crowd
	CreateCrowd({ 1430.000000, -1850.000000, 100 }, resources::crowdAnims);	// First row, third crowd
	CreateCrowd({ 1010.000000, -1865.000000, 101 }, resources::crowdAnims);	// Second row, first crowd
	CreateCrowd({ 1210.000000, -1865.000000, 101 }, resources::crowdAnims);	// Second row, second crowd
	CreateCrowd({ 1410.000000, -1865.000000, 101 }, resources::crowdAnims);	// Second row, third crowd	
	// ********************
	// Cave crowd
	CreateCrowd({ 3165.000000, -480.000000, 100 }, resources::crowdAnims);	// First row, first crowd
	// ********************
	// Overhang crowd
	CreateCrowd({ 1210.000000, -850.000000, 100 }, resources::crowdAnims); // First row, first crowd
	CreateCrowd({ 1410.000000, -850.000000, 100 }, resources::crowdAnims); // First row, second crowd
	CreateCrowd({ 1610.000000, -850.000000, 100 }, resources::crowdAnims); // First row, third crowd
	CreateCrowd({ 1810.000000, -850.000000, 100 }, resources::crowdAnims); // First row, fourth crowd
	CreateCrowd({ 1250.000000, -860.000000, 101 }, resources::crowdAnims); // Second row, first crowd
	CreateCrowd({ 1450.000000, -860.000000, 101 }, resources::crowdAnims); // Second row, second crowd
	CreateCrowd({ 1650.000000, -860.000000, 101 }, resources::crowdAnims); // Second row, third crowd
	CreateCrowd({ 1850.000000, -860.000000, 101 }, resources::crowdAnims); // Second row, fourth crowd
	// ********************

	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1455.000000, -1295.000000, 0.300000));

	PlayCountdown(Vector3(1150.0f, -1500.0f, 0.0f));
	PlayerController::lapCount = 3;
}

//Create everything for level 3
static void LoadLevel3(engine::Camera* cam)
{
	engine::collisionSystem->cam = cam;

	//Set this level's tilemap
	engine::spriteRenderSystem->SetTilemap(resources::level3Map);
	engine::collisionSystem->SetTilemap(resources::level3Map);
	engine::PhysicsSystem::SetTileProperty(1, engine::TileProperty{ true });

	//Create the players
	engine::ecs::GetSystem<PlayerController>()->CreatePlayers(playerShips, Vector2(2480.0f, -1520.0f));

	//Make all the checkpoints manually
	CreateCheckpoint(Vector3(3015.000000, -760.000000, 100.000000), Vector3(27.500000, 47.500000, 7.500000), Vector3(19), resources::models["Prop_Buoy.obj"], 37.5f + 90.f);
	CreateCheckpoint(Vector3(2645.000000, -975.000000, 100.000000), Vector3(27.500000, -40.000000, -7.500000), Vector3(19), resources::models["Prop_Buoy.obj"], -20.0f + 90.f);
	CreateCheckpoint(Vector3(2140.000000, -635.000000, 100.000000), Vector3(27.500000, 0.000000, 12.500000), Vector3(15.f), resources::models["Prop_Buoy.obj"], -2.5f + 90.f);
	CreateCheckpoint(Vector3(875.000000, -465.000000, 100.000000), Vector3(37.500000, -15.000000, 0.000000), Vector3(12.5f), resources::models["Prop_Buoy.obj"], 15.0f + 90.f);
	CreateCheckpoint(Vector3(1170.000000, -1250.000000, 100.000000), Vector3(37.500000, 0.000000, 0.000000), Vector3(13), resources::models["Prop_Buoy.obj"], 13.0f + 80.f);
	CreateCheckpoint(Vector3(2555.000000, -1600.000000, 100.000000), Vector3(-17.500000, -87.500000, -90.000000), Vector3(20.5f), resources::models["Prop_Goal_Ver2.obj"], 360.f, true);

	//Pickups
	//first corner
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3215.000000, -1280.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3265.000000, -1280.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3315.000000, -1280.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3375.000000, -1280.000000, 0.300000));

	//first hairpin
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2860.000000, -425.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2810.000000, -425.000000, 0.300000));
	/*
	// middle hook
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2385.000000, -1045.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2385.000000, -1095.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2385.000000, -1150.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2385.000000, -1205.000000, 0.300000));
	*/

	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1825.000000, -895.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1880.000000, -825.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(940.000000, -680.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(985.000000, -315.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1060.000000, -315.000000, 0.300000));

	/*
	//final corner
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1135.000000, -1645.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1175.000000, -1700.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1260.000000, -1740.000000, 0.300000));
	*/

	//Make the crowds manually
	CreateCrowd({ 1530, -1700, 10 }, resources::crowdAnims);
	CreateCrowd({ 1545, -1715, 11 }, resources::crowdAnims);
	CreateCrowd({ 1520, -1730, 12 }, resources::crowdAnims);


	PlayCountdown(Vector3(2480.0f, -1460.0f, 150.0f));
	PlayerController::lapCount = 3;
}

// Create everything for level 4
static void LoadLevel4(engine::Camera* cam)
{
	engine::collisionSystem->cam = cam;

	std::vector<ShipType> ships{ ShipType::torpedoBoat, ShipType::submarine, ShipType::hedgehogBoat, ShipType::cannonBoat };
	engine::ecs::GetSystem<PlayerController>()->CreatePlayers(playerShips, Vector2(1434.0f, -1370.0f));

	//set this level's tilemap
	engine::spriteRenderSystem->SetTilemap(resources::level4Map);
	engine::collisionSystem->SetTilemap(resources::level4Map);
	engine::PhysicsSystem::SetTileProperty(1, engine::TileProperty{ true });

	// Make all the checkpoints manually
	CreateCheckpoint(Vector3(15760.000000, -925.000000, 100.000000), Vector3(-12.500000, -90.000000, -87.500000), Vector3(43.0f), resources::models["Prop_Goal_Ver2.obj"], 360.f, true);
}

//Bind all input events here
static void SetupInput()
{
	input::ConstructDigitalEvent("Pause");
	input::ConstructDigitalEvent("Menu");
	input::bindDigitalInput(GLFW_KEY_U, { "Menu" });
	input::ConstructDigitalEvent("StartGame");
	input::bindDigitalInput(GLFW_KEY_G, { "StartGame" });
	// TODO: add controller pause key

	float AnalogPositiveMinDeadZone = 0;
	float AnalogPositiveMaxDeadZone = 0.2;

	float AnalogNegativeMinDeadZone = -0.2;
	float AnalogNegativeMaxDeadZone = 0;

	for (size_t i = 0; i < 4; i++)
	{
		input::ConstructAnalogEvent("Throttle" + std::to_string(i));
		input::ConstructAnalogEvent("Turn" + std::to_string(i));

		input::ConstructDigitalEvent("Shoot" + std::to_string(i));
		input::ConstructDigitalEvent("Boost" + std::to_string(i));

		// Controller input

		input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_A, { "Shoot" + std::to_string(i) });
		input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_B, { "Boost" + std::to_string(i) });
		input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_START, { "Pause" });

		input::bindAnalogControllerInput(i,
			{
				{ { input::digitalPositiveInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER },
				{ { input::digitalNegativeInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER }
			},
			{ "Throttle" + std::to_string(i) });

		input::bindAnalogControllerInput(i,
			{
				{ { input::controllerMixedInput, AnalogNegativeMinDeadZone, AnalogPositiveMaxDeadZone }, GLFW_GAMEPAD_AXIS_LEFT_X }
			},
			{ "Turn" + std::to_string(i) });
	}

	// Keyboard input
	int KeyboardPlayer = 3;

	input::bindAnalogInput(GLFW_KEY_RIGHT, { input::digitalPositiveInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, { "Turn" + std::to_string(KeyboardPlayer) });
	input::bindAnalogInput(GLFW_KEY_LEFT, { input::digitalNegativeInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, { "Turn" + std::to_string(KeyboardPlayer) });

	input::bindAnalogInput(GLFW_KEY_A, { input::digitalPositiveInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, { "Throttle" + std::to_string(KeyboardPlayer) });
	input::bindAnalogInput(GLFW_KEY_Z, { input::digitalNegativeInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, { "Throttle" + std::to_string(KeyboardPlayer) });
	input::bindAnalogInput(GLFW_KEY_UP, { input::digitalPositiveInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, { "Throttle" + std::to_string(KeyboardPlayer) });
	input::bindAnalogInput(GLFW_KEY_DOWN, { input::digitalNegativeInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, { "Throttle" + std::to_string(KeyboardPlayer) });

	input::bindDigitalInput(GLFW_KEY_N, { "Shoot" + std::to_string(KeyboardPlayer) });
	input::bindDigitalInput(GLFW_KEY_M, { "Boost" + std::to_string(KeyboardPlayer) });
	input::bindDigitalInput(GLFW_KEY_P, { "Pause" });
	input::bindDigitalInput(GLFW_KEY_G, { "StartGame" });
}

static void PlayersMenu(std::shared_ptr<PlayerSelectSystem> ShipSelectionSystem)
{
	ShipSelectionSystem->isShipSelectionMenuOn = !ShipSelectionSystem->isShipSelectionMenuOn;
	isGamePaused = !isGamePaused;


	ShipSelectionSystem->ToggleMenuPlayerSelection();


	std::cout << "is Ship selection open:" << ShipSelectionSystem->isShipSelectionMenuOn;
}

int main()
{
	GameStates currentGameState = menuMainState;
	GLFWwindow* window = engine::CreateGLWindow(1600, 900, "Window");

	engine::EngineInit();

	//Make the camera
	engine::Camera cam = engine::Camera(1120, 630);
	cam.SetPosition(Vector3(0, 0, 1500));
	cam.SetRotation(Vector3(0, 0, 0));

	//Loads all globally used resources
	resources::LoadResources(&cam);

	input::initialize(window);

	//Get pointers and call init of every custom system
	std::shared_ptr<PauseSystem> pauseSystem = engine::ecs::GetSystem<PauseSystem>();
	pauseSystem->Init(window);
	std::shared_ptr<PlayerController> playerController = engine::ecs::GetSystem<PlayerController>();
	playerController->Init();
	std::shared_ptr<HedgehogSystem> hedgehogSystem = engine::ecs::GetSystem<HedgehogSystem>();
	std::shared_ptr<PickupSystem> pickupSystem = engine::ecs::GetSystem<PickupSystem>();

	std::shared_ptr<engine::SoundSystem> soundSystem = engine::ecs::GetSystem<engine::SoundSystem>();
	soundSystem->AddSoundEngine("Gameplay");
	soundSystem->AddSoundEngine("Boat");
	soundSystem->AddSoundEngine("Background");
	soundSystem->AddSoundEngine("Music");

	std::shared_ptr<PlayerSelectSystem> ShipSelectionSystem = engine::ecs::GetSystem<PlayerSelectSystem>();
	ShipSelectionSystem->Init();

	//Bind all input actions
	SetupInput();

	//Load the first level
	//LoadLevel1(&cam);

	// Load the second level
	//LoadLevel2(&cam);

	// Load the third level
	//LoadLevel3(&cam);



	//Object placement editor
	engine::ecs::Entity placementEditor = ecs::NewEntity();
	ecs::AddComponent(placementEditor, Transform{ .position = Vector3(500, -500, 100), .scale = 20 });
	ecs::AddComponent(placementEditor, ModelRenderer{ .model = resources::models["Prop_PowerUpBox2.obj"] });

	//Testing stuff
	physicsSystem->SetTileProperty(201, TileProperty{false});


	ShipSelectionSystem->isShipSelectionMenuOn = true;
	isGamePaused = true;
	bool mapLoaded = false;

	//Game Loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		//Close window when Esc is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		//Object editor
		{
			const float editorSpeed = 0.5;
			//Hold numpad 0 to toggle rotate
			if (glfwGetKey(window, GLFW_KEY_KP_0))
			{
				//Numpad 8456 move
				if (glfwGetKey(window, GLFW_KEY_KP_4))
					TransformSystem::Rotate(placementEditor, Vector3(0, 0, 5) * editorSpeed);
				if (glfwGetKey(window, GLFW_KEY_KP_6))
					TransformSystem::Rotate(placementEditor, Vector3(0, 0, -5) * editorSpeed);
				if (glfwGetKey(window, GLFW_KEY_KP_8))
					TransformSystem::Rotate(placementEditor, Vector3(-5, 0, 0) * editorSpeed);
				if (glfwGetKey(window, GLFW_KEY_KP_5))
					TransformSystem::Rotate(placementEditor, Vector3(5, 0, 0) * editorSpeed);
				if (glfwGetKey(window, GLFW_KEY_KP_7))
					TransformSystem::Rotate(placementEditor, Vector3(0, 5, 0) * editorSpeed);
				if (glfwGetKey(window, GLFW_KEY_KP_9))
					TransformSystem::Rotate(placementEditor, Vector3(0, -5, 0) * editorSpeed);
			}
			else
			{
				//Numpad 8456 move
				if (glfwGetKey(window, GLFW_KEY_KP_8))
					TransformSystem::Translate(placementEditor, Vector3(0, 10, 0) * editorSpeed);
				if (glfwGetKey(window, GLFW_KEY_KP_4))
					TransformSystem::Translate(placementEditor, Vector3(-10, 0, 0) * editorSpeed);
				if (glfwGetKey(window, GLFW_KEY_KP_5))
					TransformSystem::Translate(placementEditor, Vector3(0, -10, 0) * editorSpeed);
				if (glfwGetKey(window, GLFW_KEY_KP_6))
					TransformSystem::Translate(placementEditor, Vector3(10, 0, 0) * editorSpeed);
			}
			//+- scale
			if (glfwGetKey(window, GLFW_KEY_KP_ADD))
				TransformSystem::Scale(placementEditor, 1 * editorSpeed);
			if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT))
				TransformSystem::Scale(placementEditor, -1 * editorSpeed);
			//Print spawn Function
			if (glfwGetKey(window, GLFW_KEY_KP_ENTER))
			{
				auto& et = ecs::GetComponent<Transform>(placementEditor);
				std::cout << et.position.ToString() << ", " << et.rotation.ToString() << ", " << et.scale.ToString() << std::endl;
			}
			//Reset Transforms
			if (glfwGetKey(window, GLFW_KEY_KP_DECIMAL))
			{
				TransformSystem::SetRotation(placementEditor, 0);
				TransformSystem::SetScale(placementEditor, 20);
			}
		}

		input::update();

		if (!isGamePaused)
			UpdateCam(&cam, collisionSystem->tilemap);
		hedgehogSystem->Update();
		pickupSystem->Update();
		engine::Update(&cam);

		if (canStartLoadingMap)
		{
			isGamePaused = false;
			canStartLoadingMap = false;
			ShipSelectionSystem->isShipSelectionMenuOn = false;
			LoadLevel4(&cam);
		}

		// if paused or Pause pressed update PauseSystem
		if (input::GetNewPress("Pause"))
		{
			pauseSystem->isGamePause = true;// !(pauseSystem->isGamePause);
			isGamePaused = !isGamePaused;
			//printf("\nGamePause pressed\n");
			pauseSystem->ToggleShowUIOptionsMenu();

		}
		if (pauseSystem->isGamePause)
		{
			printf("\nGamePaused \n");
			pauseSystem->Update();
		}

		if (input::GetNewPress("Menu"))
		{
			ShipSelectionSystem->isShipSelectionMenuOn = !ShipSelectionSystem->isShipSelectionMenuOn;
			isGamePaused = !isGamePaused;


			ShipSelectionSystem->ToggleMenuPlayerSelection();


			std::cout << "is Ship selection open:" << ShipSelectionSystem->isShipSelectionMenuOn;
		}
		if (ShipSelectionSystem->isShipSelectionMenuOn)
		{
			engine::modelRenderSystem->SetLight(Vector3(0, 0, -200), 255);
			//printf("\nShipSelectionSystem->Update()\n");
			ShipSelectionSystem->Update();
		}

		// playerControl Update for frame if not paused
		//                           XOR gate true when only if out puts are different
		/*if ((pauseSystem->isGamePause ^ ShipSelectionSystem->isShipSelectionMenuOn))*/
		if (isGamePaused)
		{
			//printf("\nNOT UPDATING playerController \n");

		}
		else
		{
			playerController->Update(window);
		}

		ecs::Update();
		glfwSwapBuffers(window);
	}

	soundSystem->Uninitialize();

	engine::UninitializeTimers();
	input::uninitialize();

	ecs::DestroyAllEntities(true);
	glfwTerminate();
	return 0;
}