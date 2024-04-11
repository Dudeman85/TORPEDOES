#include "PlayerController.h"
#include "engine/Input.h"  
#include "MenuSystem.h"	
#include "GameCamera.h"
#include "engine/SoundComponent.h"

using namespace engine;

int checkPointNumber = 0;
bool isGamePaused = false;
void CreateCheckpoint(Vector3 position, Vector3 rotation, Vector3 scale, engine::Model* checkPointModel, float hitboxrotation, bool finish_line = false)
{
	engine::ecs::Entity checkpoint = engine::ecs::NewEntity();

	engine::ecs::AddComponent(checkpoint, engine::Transform{.position = position, .rotation = rotation, .scale = scale });
	engine::ecs::AddComponent(checkpoint, engine::ModelRenderer{.model = checkPointModel });
	engine::ecs::AddComponent(checkpoint, CheckPoint{ .checkPointID = checkPointNumber , .Finish_line = finish_line });
	std::vector<Vector2> CheckpointcolliderVerts{ Vector2(4, 8), Vector2(4, -8), Vector2(-4, -8), Vector2(-4, 8) };
	engine::ecs::AddComponent(checkpoint, engine::PolygonCollider({ .vertices = CheckpointcolliderVerts, .trigger = true, .visualise = false, .rotationOverride = hitboxrotation }));

	checkPointNumber++;
};

void CreateCrowd(Vector3 pos, engine::Animation& anim)
{
	engine::ecs::Entity crowd = engine::ecs::NewEntity();
	engine::ecs::AddComponent(crowd, engine::Transform{.position = pos, .scale = Vector3(100, 30, 0) });
	engine::ecs::AddComponent(crowd, engine::SpriteRenderer{});
	engine::ecs::AddComponent(crowd, engine::Animator{});
	engine::AnimationSystem::AddAnimation(crowd, anim, "CrowdCheer");
	engine::AnimationSystem::PlayAnimation(crowd, "CrowdCheer", true);
}

//Play the countdown timer and freeze players untill it is done
void PlayCountdown()
{
	engine::ecs::Entity countdown = engine::ecs::NewEntity();
	engine::ecs::AddComponent(countdown, engine::Transform{.position = Vector3(1475, -1270, 10), .scale = Vector3(60, 100, 0) });
	engine::ecs::AddComponent(countdown, engine::SpriteRenderer{});
	engine::ecs::AddComponent(countdown, engine::Animator{.onAnimationEnd = engine::ecs::DestroyEntity });
	engine::AnimationSystem::AddAnimation(countdown, resources::countdownAnim, "CountDown");
	engine::AnimationSystem::PlayAnimation(countdown, "CountDown", false);
	engine::ecs::GetSystem<PlayerController>()->countdownTimer = 5;
}

//Create everything for level 1
void LoadLevel1(engine::Camera* cam)
{
	engine::collisionSystem->cam = cam;

	//TEST
	//resources::level1Map->enabledLayers[1] = false;

	//Set this level's tilemap
	engine::spriteRenderSystem->SetTilemap(resources::level1Map);
	engine::collisionSystem->SetTilemap(resources::level1Map);
	engine::PhysicsSystem::SetTileProperty(1, engine::TileProperty{ true });

	std::vector<ShipType> ships{ShipType::torpedoBoat, ShipType::submarine, ShipType::hedgehogBoat, ShipType::cannonBoat};
	engine::ecs::GetSystem<PlayerController>()->CreatePlayers(4, Vector2(1434.0f, -1370.0f), ships);

	//Make all the checkpoints manually
	CreateCheckpoint(Vector3(2100.226807, -963.837402, 100.000000), Vector3(30.000000, 159.245773, 0.000000), Vector3(17), resources::models["Checkpoint.obj"], 45.0f);
	CreateCheckpoint(Vector3(2597.463135, -684.973389, 100.000000), Vector3(45.000000, 180.022018, 0.000000), Vector3(17), resources::models["Checkpoint.obj"], 45.0f);
	CreateCheckpoint(Vector3(1668.260010, -990.794373, 100.000000), Vector3(45.000000, 147.891968, 0.000000), Vector3(17), resources::models["Checkpoint.obj"], 45.0f);
	CreateCheckpoint(Vector3(1043.635132, -875.206543, 100.000000), Vector3(45.000000, 179.241272, 0.000000), Vector3(17), resources::models["Checkpoint.obj"], 45.0f);
	CreateCheckpoint(Vector3(943.931152, -293.566711, 100.000000), Vector3(45.000000, 107.476852, 0.000000), Vector3(17), resources::models["Checkpoint.obj"], 45.0f);
	CreateCheckpoint(Vector3(586.608276, -1249.448486, 100.000000), Vector3(45.000000, 40.070156, 0.000000), Vector3(17), resources::models["Checkpoint.obj"], 90.0f);
	CreateCheckpoint(Vector3(1513.692383, -1462.996187, 50.000000), Vector3(45.000000, .901711, 0.000000), Vector3(14), resources::models["Finish_line.obj"], -1, true); // 10

	//Make the crowds manually
	CreateCrowd({ 1530, -1700, 10 }, resources::crowdAnims);
	CreateCrowd({ 1545, -1715, 11 }, resources::crowdAnims);
	CreateCrowd({ 1520, -1730, 12 }, resources::crowdAnims);

	PlayCountdown();
}

// Create everything for level 2
void LoadLevel2(engine::Camera* cam)
{
	engine::collisionSystem->cam = cam;

	std::vector<ShipType> ships{ShipType::torpedoBoat, ShipType::submarine, ShipType::hedgehogBoat, ShipType::cannonBoat};
	engine::ecs::GetSystem<PlayerController>()->CreatePlayers(1, Vector2(1200.0f, -1700.0f), ships);

	// Set this level's tilemap
	engine::spriteRenderSystem->SetTilemap(resources::level2Map);
	engine::collisionSystem->SetTilemap(resources::level2Map);
	engine::PhysicsSystem::SetTileProperty(1, engine::TileProperty { true });

	// Make all the checkpoint's manually
	CreateCheckpoint(Vector3(1470.000000, -980.000000, 0.000000), Vector3(42.500000, -37.500000, -32.500000), Vector3(14), resources::models["Prop_Buoy.obj"], 45.0f);
	CreateCheckpoint(Vector3(2145.000000, -1470.000000, 0.000000), Vector3(32.500000, 0.000000, 0.000000), Vector3(14), resources::models["Prop_Buoy.obj"], 45.0f);
	CreateCheckpoint(Vector3(2390.000000, -1640.000000, 0.000000), Vector3(0.000000, -57.500000, -90.000000), Vector3(14), resources::models["Prop_Buoy.obj"], 45.0f);
	CreateCheckpoint(Vector3(1230.000000, -1700.000000, 0.000000), Vector3(0.000000, -50.000000, -90.000000), Vector3(11), resources::models["Finish_line.obj"], 45.0f);
}

// Create everything for level 3
void LoadLevel3(engine::Camera* cam)
{
	engine::collisionSystem->cam = cam;

	std::vector<ShipType> ships{ShipType::torpedoBoat, ShipType::submarine, ShipType::hedgehogBoat, ShipType::cannonBoat};
	engine::ecs::GetSystem<PlayerController>()->CreatePlayers(1, Vector2(1434.0f, -1370.0f), ships);

	//set this level's tilemap
	engine::spriteRenderSystem->SetTilemap(resources::level3Map);
	engine::collisionSystem->SetTilemap(resources::level3Map);
	engine::PhysicsSystem::SetTileProperty(1, engine::TileProperty{true});

	// Make all the checkpoints manually
	CreateCheckpoint(Vector3(2635.000000, -1355.000000, 0.000000), Vector3(77.500000, -40.000000, -7.500000), Vector3(20), resources::models["Prop_Buoy_Checkpoint.obj"], 45.0f);
}

//Bind all input events here
void SetupInput()
{
	input::ConstructDigitalEvent("Pause");
	input::bindDigitalInput(GLFW_KEY_P, { "Pause" });
	input::ConstructDigitalEvent("Menu");
	input::bindDigitalInput(GLFW_KEY_U, { "Menu" });
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
	int KeyboardPlayer = 0;

	input::bindAnalogInput(GLFW_KEY_RIGHT, { input::digitalPositiveInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, { "Turn" + std::to_string(KeyboardPlayer) });
	input::bindAnalogInput(GLFW_KEY_LEFT, { input::digitalNegativeInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, { "Turn" + std::to_string(KeyboardPlayer) });

	input::bindAnalogInput(GLFW_KEY_A, { input::digitalPositiveInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, { "Throttle" + std::to_string(KeyboardPlayer) });
	input::bindAnalogInput(GLFW_KEY_Z, { input::digitalNegativeInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, { "Throttle" + std::to_string(KeyboardPlayer) });
	input::bindAnalogInput(GLFW_KEY_UP, { input::digitalPositiveInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, { "Throttle" + std::to_string(KeyboardPlayer) });
	input::bindAnalogInput(GLFW_KEY_DOWN, { input::digitalNegativeInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, { "Throttle" + std::to_string(KeyboardPlayer) });

	input::bindDigitalInput(GLFW_KEY_N, { "Shoot" + std::to_string(KeyboardPlayer) });
	input::bindDigitalInput(GLFW_KEY_M, { "Boost" + std::to_string(KeyboardPlayer) });
}

int main()
{
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
	LoadLevel2(&cam);

	// Load the third level
	//LoadLevel3(&cam);


	//Object placement editor
	engine::ecs::Entity placementEditor = ecs::NewEntity();
	ecs::AddComponent(placementEditor, Transform{ .position = Vector3(500, -500, 0), .scale = 20 });
	ecs::AddComponent(placementEditor, ModelRenderer{ .model = resources::models["Prop_Buoy.obj"] });


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
		hedgehogSystem->Update();
		UpdateCam(&cam, resources::level2Map);
		engine::Update(&cam);


		// if paused or Pause pressed update PauseSystem
		if (input::GetNewPress("Pause"))
		{
			pauseSystem->isGamePause = !(pauseSystem->isGamePause);
			isGamePaused = !isGamePaused;
			//printf("\nGamePause pressed\n");
			pauseSystem->ToggleShowUIOptionsMenu();

		}
		if (pauseSystem->isGamePause)
		{
			//printf("\nGamePaused \n");
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

		glfwSwapBuffers(window);
	}

	soundSystem->Uninitialize();

	engine::UninitializeTimers();
	input::uninitialize();

	engine::ecs::DestroyAllEntities(true);
	glfwTerminate();
	return 0;
}