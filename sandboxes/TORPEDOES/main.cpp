#include "PlayerController.h"
#include "engine/Input.h"  
#include "MenuSystem.h"	
#include "GameCamera.h"

int checkPointNumber = 0;

void CreateCheckpoint(Vector3 position, Vector3 rotation, Vector3 scale, Model* checkPointModel, float hitboxrotation, bool finish_line = false)
{
	ecs::Entity checkpoint = ecs::NewEntity();

	ecs::AddComponent(checkpoint, Transform{ .position = position , .rotation = rotation , .scale = scale });
	ecs::AddComponent(checkpoint, ModelRenderer{ .model = checkPointModel });
	ecs::AddComponent(checkpoint, CheckPoint{ .checkPointID = checkPointNumber , .Finish_line = finish_line });
	std::vector<Vector2> CheckpointcolliderVerts{ Vector2(4, 8), Vector2(4, -8), Vector2(-4, -8), Vector2(-4, 8) };
	ecs::AddComponent(checkpoint, PolygonCollider({ .vertices = CheckpointcolliderVerts, .trigger = true, .visualise = false, .rotationOverride = hitboxrotation }));

	checkPointNumber++;
};

void CreateCrowd(Vector3 pos, Animation& anim)
{
	ecs::Entity crowd = ecs::NewEntity();
	ecs::AddComponent(crowd, Transform{ .position = pos, .scale = Vector3(100, 30, 0) });
	ecs::AddComponent(crowd, SpriteRenderer{});
	ecs::AddComponent(crowd, Animator{});
	AnimationSystem::AddAnimation(crowd, anim, "CrowdCheer");
	AnimationSystem::PlayAnimation(crowd, "CrowdCheer", true);
}

//Play the countdown timer and freeze players untill it is done
void PlayCountdown()
{
	ecs::Entity countdown = ecs::NewEntity();
	ecs::AddComponent(countdown, Transform{ .position = Vector3(1475, -1270, 10), .scale = Vector3(60, 100, 0) });
	ecs::AddComponent(countdown, SpriteRenderer{});
	ecs::AddComponent(countdown, Animator{ .onAnimationEnd = ecs::DestroyEntity });
	AnimationSystem::AddAnimation(countdown, resources::countdownAnim, "CountDown");
	AnimationSystem::PlayAnimation(countdown, "CountDown", false);
	ecs::GetSystem<PlayerController>()->countdownTimer = 5;
}

//Create everything for level 1
void LoadLevel1(Camera* cam)
{
	collisionSystem->cam = cam;

	//TEST
	//resources::level1Map->enabledLayers[1] = false;

	//Set this level's tilemap
	spriteRenderSystem->SetTilemap(resources::level1Map);
	collisionSystem->SetTilemap(resources::level1Map);
	PhysicsSystem::SetTileProperty(1, TileProperty{ true });

	std::vector<ShipType> ships{ShipType::torpedoBoat, ShipType::torpedoBoat, ShipType::torpedoBoat, ShipType::torpedoBoat};
	ecs::GetSystem<PlayerController>()->CreatePlayers(4, Vector2(1434.0f, -1370.0f), ships);

	//Make all the checkpoints manually
	CreateCheckpoint(Vector3(2100.226807, -963.837402, 100.000000), Vector3(30.000000, 159.245773, 0.000000), Vector3(17), resources::models["Checkpoint.obj"], 45.0f);
	CreateCheckpoint(Vector3(2597.463135, -684.973389, 100.000000), Vector3(45.000000, 180.022018, 0.000000), Vector3(17), resources::models["Checkpoint.obj"], 45.0f);
	CreateCheckpoint(Vector3(1668.260010, -990.794373, 100.000000), Vector3(45.000000, 147.891968, 0.000000), Vector3(17), resources::models["Checkpoint.obj"], 45.0f);
	CreateCheckpoint(Vector3(1043.635132, -875.206543, 100.000000), Vector3(45.000000, 179.241272, 0.000000), Vector3(17), resources::models["Checkpoint.obj"], 45.0f);
	CreateCheckpoint(Vector3(943.931152, -293.566711, 100.000000), Vector3(45.000000, 107.476852, 0.000000), Vector3(17), resources::models["Checkpoint.obj"], 45.0f);
	CreateCheckpoint(Vector3(586.608276, -1249.448486, 100.000000), Vector3(45.000000, 40.070156, 0.000000), Vector3(17), resources::models["Checkpoint.obj"], 90.0f);
	CreateCheckpoint(Vector3(1513.692383, -1462.996187, 50.000000), Vector3(90.000000, 90.901711, 0.000000), Vector3(14), resources::models["Finish_line.obj"], -1, true); // 10

	//Make the crowds manually
	CreateCrowd({ 1530, -1700, 10 }, resources::crowdAnims);
	CreateCrowd({ 1545, -1715, 11 }, resources::crowdAnims);
	CreateCrowd({ 1520, -1730, 12 }, resources::crowdAnims);

	PlayCountdown();
}

//Bind all input events here
void SetupInput()
{
	input::ConstructDigitalEvent("Pause");
	input::bindDigitalInput(GLFW_KEY_P, { "Pause" });
	// TODO: add controller pause key

	for (size_t i = 0; i < 4; i++)
	{
		input::ConstructAnalogEvent("Move" + std::to_string(i));

		input::ConstructDigitalEvent("Shoot" + std::to_string(i));
		input::ConstructDigitalEvent("Boost" + std::to_string(i));

		// Controller input
		input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_A, { "Shoot" + std::to_string(i) });
		input::bindAnalogControllerInput(i, { GLFW_GAMEPAD_AXIS_LEFT_X, GLFW_GAMEPAD_AXIS_RIGHT_Y }, { "Move" + std::to_string(i) });
	}
	
	// Keyboard input for player 0
	input::bindAnalogInput(GLFW_KEY_RIGHT, { "Move0" }, GLFW_GAMEPAD_AXIS_LEFT_X);
	input::bindAnalogInput(GLFW_KEY_LEFT, { "Move0" }, GLFW_GAMEPAD_AXIS_LEFT_X, -1);
	input::bindAnalogInput(GLFW_KEY_A, { "Move0" }, GLFW_GAMEPAD_AXIS_LEFT_Y);
	input::bindAnalogInput(GLFW_KEY_Z, { "Move0" }, GLFW_GAMEPAD_AXIS_LEFT_Y, -1);

	input::bindDigitalInput(GLFW_KEY_N, { "Shoot0" });
	input::bindDigitalInput(GLFW_KEY_M, { "Boost0" });

	/*
	input::bindDigitalInput(GLFW_KEY_LEFT, { "MoveLeft0" });
	input::bindDigitalInput(GLFW_KEY_RIGHT, { "MoveRight0" });
	input::bindDigitalInput(GLFW_KEY_UP, { "MoveUp0" });
	input::bindDigitalInput(GLFW_KEY_DOWN, { "MoveDown0" });
	input::bindDigitalInput(GLFW_KEY_ENTER, { "Select0" });
	*/
	
}

int main()
{
	GLFWwindow* window = engine::CreateGLWindow(1600, 900, "Window");

	EngineInit();

	//Make the camera
	engine::Camera cam = engine::Camera(1120, 630);
	cam.SetPosition(Vector3(0, 0, 1500));
	cam.SetRotation(Vector3(0, 0, 0));

	//Loads all globally used resources
	resources::LoadResources(&cam);

	input::initialize(window);

	//Get pointers and call init of every custom system
	std::shared_ptr<PauseSystem> pauseSystem = ecs::GetSystem<PauseSystem>();
	pauseSystem->Init(window);
	std::shared_ptr<PlayerController> playerController = ecs::GetSystem<PlayerController>();
	playerController->Init();

	//Bind all input actions
	SetupInput();

	//Load the first level
	LoadLevel1(&cam);

	//Game Loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		//Close window when Esc is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		input::update();

		UpdateCam(window, cam, resources::level1Map);
		engine::Update(&cam);

		// playerControl Update for frame if not paused
		if (!pauseSystem->isGamePause)
		{
			playerController->Update(window, deltaTime);
		}
		// if paused or Pause pressed update PauseSystem
		if (pauseSystem->isGamePause || input::GetNewPress("Pause"))
		{
			pauseSystem->Update();
		}

		glfwSwapBuffers(window);
	}

	input::uninitialize();
	ecs::DestroyAllEntities(true);
	glfwTerminate();
	return 0;
}