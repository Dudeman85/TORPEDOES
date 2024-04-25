#include "PlayerController.h"
#include "engine/Input.h"  
#include "MenuSystem.h"	
#include "GameCamera.h"
#include "engine/SoundComponent.h"

using namespace engine;

int checkPointNumber = 0;
bool isGamePaused = false;

static void CreateCheckpoint(Vector3 position, Vector3 rotation, Vector3 scale, engine::Model* checkPointModel, float hitboxrotation, bool finishLine = false)
{
	engine::ecs::Entity checkpoint = engine::ecs::NewEntity();

	engine::ecs::AddComponent(checkpoint, engine::Transform{.position = position, .rotation = rotation, .scale = scale });
	engine::ecs::AddComponent(checkpoint, engine::ModelRenderer{.model = checkPointModel });
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
	engine::ecs::AddComponent(crowd, engine::Transform{.position = pos, .scale = Vector3(100, 30, 0) });
	engine::ecs::AddComponent(crowd, engine::SpriteRenderer{});
	engine::ecs::AddComponent(crowd, engine::Animator{});
	engine::AnimationSystem::AddAnimation(crowd, anim, "CrowdCheer");
	engine::AnimationSystem::PlayAnimation(crowd, "CrowdCheer", true);
}

//Play the countdown timer and freeze players untill it is done
static void PlayCountdown()
{
	engine::ecs::Entity countdown = engine::ecs::NewEntity();
	engine::ecs::AddComponent(countdown, engine::Transform{.position = Vector3(2480, -1400, 10), .scale = Vector3(60, 100, 0) });
	engine::ecs::AddComponent(countdown, engine::SpriteRenderer{});
	engine::ecs::AddComponent(countdown, engine::Animator{.onAnimationEnd = engine::ecs::DestroyEntity });
	engine::AnimationSystem::AddAnimation(countdown, resources::countdownAnim, "CountDown");
	engine::AnimationSystem::PlayAnimation(countdown, "CountDown", false);
	engine::ecs::GetSystem<PlayerController>()->countdownTimer = 3;
}

//Create everything for level 1
static void LoadLevel1(engine::Camera* cam)
{
	engine::collisionSystem->cam = cam;

	//TEST
	//resources::level1Map->enabledLayers[1] = false;

	//Set this level's tilemap
	engine::spriteRenderSystem->SetTilemap(resources::level1Map);
	engine::collisionSystem->SetTilemap(resources::level1Map);
	engine::PhysicsSystem::SetTileProperty(1, engine::TileProperty{ true });

	engine::ecs::GetSystem<PlayerController>()->CreatePlayers(playerShips, Vector2(480.0f, -1520.0f));


	//Make all the checkpoints manually
	CreateCheckpoint(Vector3(15760.000000, -925.000000, 100.000000), Vector3(-12.500000, -90.000000, -87.500000), Vector3(43.0f), resources::models["Prop_Goal_Ver2.obj"], 360.f, true); // 10

	//Make the crowds manually
	CreateCrowd({ 1530, -1700, 10 }, resources::crowdAnims);
	CreateCrowd({ 1545, -1715, 11 }, resources::crowdAnims);
	CreateCrowd({ 1520, -1730, 12 }, resources::crowdAnims);

	PlayCountdown();
	PlayerController::lapCount = 1;
}

//Bind all input events here
static void SetupInput()
{
	input::ConstructDigitalEvent("Pause");
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

	//startLevel = LoadLevel1(&cam);
	std::shared_ptr<PlayerSelectSystem> ShipSelectionSystem = engine::ecs::GetSystem<PlayerSelectSystem>();
	ShipSelectionSystem->Init();

	//Bind all input actions
	SetupInput();

	//Load the first level
	//LoadLevel1(&cam);
	//ShipSelectionSystem->ToggleMenuPlayerSelection();

	//Object placement editor
	engine::ecs::Entity placementEditor = ecs::NewEntity();
	ecs::AddComponent(placementEditor, Transform{ .position = Vector3(500, -500, 0), .scale = 20 });
	ecs::AddComponent(placementEditor, ModelRenderer{ .model = resources::models["Prop_Goal_Ver2.obj"] });

	PlayersMenu(ShipSelectionSystem);
	bool mapLoaded = false;

	//Game Loop
	while (!glfwWindowShouldClose(window))
	{
		/*	if (PlayerSelectSystem::GetCanStartLoadingMap() && mapLoaded)
			{						  5
				mapLoaded = true;
				LoadLevel1(&cam);
			}*/

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
			UpdateCam(&cam, resources::level1Map);

		hedgehogSystem->Update();
		engine::Update(&cam);

		if (canStartLoadingMap)
		{
			isGamePaused = false;
			canStartLoadingMap = false;
			ShipSelectionSystem->isShipSelectionMenuOn = false;
			LoadLevel1(&cam);
		}

		// if paused or Pause pressed update PauseSystem
		if (input::GetNewPress("Pause"))
		{
			pauseSystem->isGamePause = true;// !(pauseSystem->isGamePause);
			//isGamePaused = !isGamePaused;
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


//ecs::Entity torpIndicator1 = ecs::NewEntity();
//ecs::AddComponent(torpIndicator1, SpriteRenderer{ .texture = &torprdytexture });
//ecs::AddComponent(torpIndicator1, Transform{ .position = Vector3(0, 0, 0), .scale = Vector3(14, 3.5, 8) });
//ecs::Entity torpIndicator2 = ecs::NewEntity();
//ecs::AddComponent(torpIndicator2, SpriteRenderer{ .texture = &torprdytexture });
//ecs::AddComponent(torpIndicator2, Transform{ .position = Vector3(0, 0, 0), .scale = Vector3(14, 3.5, 8) });
//
//ecs::Entity torpIndicator3 = ecs::NewEntity();
//ecs::AddComponent(torpIndicator3, SpriteRenderer{ .texture = &torprdytexture });
//ecs::AddComponent(torpIndicator3, Transform{ .position = Vector3(0, 0, 0), .scale = Vector3(14, 3.5, 8) });
//ecs::Entity torpIndicator4 = ecs::NewEntity();
//ecs::AddComponent(torpIndicator4, SpriteRenderer{ .texture = &torprdytexture });
//ecs::AddComponent(torpIndicator4, Transform{ .position = Vector3(0, 0, 0), .scale = Vector3(14, 3.5, 8) });
//
//ecs::Entity torpIndicator5 = ecs::NewEntity();
//ecs::AddComponent(torpIndicator5, SpriteRenderer{ .texture = &torprdytexture });
//ecs::AddComponent(torpIndicator5, Transform{ .position = Vector3(0, 0, 0), .scale = Vector3(14, 3.5, 8) });
//ecs::Entity torpIndicator6 = ecs::NewEntity();
//ecs::AddComponent(torpIndicator6, SpriteRenderer{ .texture = &torprdytexture });
//ecs::AddComponent(torpIndicator6, Transform{ .position = Vector3(0, 0, 0), .scale = Vector3(14, 3.5, 8) });
//
//ecs::Entity torpIndicator7 = ecs::NewEntity();
//ecs::AddComponent(torpIndicator7, SpriteRenderer{ .texture = &torprdytexture });
//ecs::AddComponent(torpIndicator7, Transform{ .position = Vector3(0, 0, 0), .scale = Vector3(14, 3.5, 8) });
//ecs::Entity torpIndicator8 = ecs::NewEntity();
//ecs::AddComponent(torpIndicator8, SpriteRenderer{ .texture = &torprdytexture });
//ecs::AddComponent(torpIndicator8, Transform{ .position = Vector3(0, 0, 0), .scale = Vector3(14, 3.5, 8) });
//
////Cannon texture
//ecs::Entity cannonIndicator = ecs::NewEntity();
//ecs::AddComponent(cannonIndicator, SpriteRenderer{ .texture = &cannonrdytexture });
//ecs::AddComponent(cannonIndicator, Transform{ .position = Vector3(0, 0, 0), .scale = Vector3(10, 10, 8) });
//
////hedgehog texture
//ecs::Entity hedgehogIndicator = ecs::NewEntity();
//ecs::AddComponent(hedgehogIndicator, SpriteRenderer{ .texture = &hedgehogrdytexture });
//ecs::AddComponent(hedgehogIndicator, Transform{ .position = Vector3(0, 0, 0), .scale = Vector3(10, 10, 8) });

		//// Player 1
		//Transform& p1Transform = ecs::GetComponent<Transform>(laMuerte);
		//Transform& HedgehogIconLoc = ecs::GetComponent<Transform>(hedgehogIndicator);
		//HedgehogIconLoc.position = Vector3(p1Transform.position.x - 5, p1Transform.position.y - 16, 200);
		////Transform& torpIconLoc6 = ecs::GetComponent<Transform>(torpIndicator6);
		////torpIconLoc6.position = Vector3(p3Transform.position.x - 5, p3Transform.position.y - 24, 201);
		//SpriteRenderer& hegdehogIcon = ecs::GetComponent<SpriteRenderer>(hedgehogIndicator);
		////SpriteRenderer& torpIcon6 = ecs::GetComponent<SpriteRenderer>(torpIndicator6);


		//if (player.projectileTime1 > 0)
		//{
		//	hegdehogIcon.texture = &hedgehogrldtexture;
		//}
		//else
		//{
		//	hegdehogIcon.texture = &hedgehogrdytexture;
		//	//torpSpeaker3.Play(torpedoSound);
		//	//soundDevice->SetSourceLocation(torpSpeaker3, PlayerTransform2.position.x, PlayerTransform2.position.y, 0);
		//}
		//// Player 2
		//Transform& p2Transform = ecs::GetComponent<Transform>(laMuerte2);
		//Transform& cannonIconLoc = ecs::GetComponent<Transform>(cannonIndicator);
		//cannonIconLoc.position = Vector3(p2Transform.position.x - 5, p2Transform.position.y - 16, 200);
		////Transform& torpIconLoc4 = ecs::GetComponent<Transform>(torpIndicator4);
		////torpIconLoc4.position = Vector3(p2Transform.position.x - 5, p2Transform.position.y - 24, 201);
		//SpriteRenderer& torpIcon3 = ecs::GetComponent<SpriteRenderer>(cannonIndicator);
		////SpriteRenderer& torpIcon4 = ecs::GetComponent<SpriteRenderer>(torpIndicator4);
		//

		//if (player2.projectileTime1 > 0)
		//{
		//	torpIcon3.texture = &cannonrldtexture;
		//}
		//else
		//{
		//	torpIcon3.texture = &cannonrdytexture;
		//	//torpSpeaker3.Play(torpedoSound);
		//	//soundDevice->SetSourceLocation(torpSpeaker3, PlayerTransform2.position.x, PlayerTransform2.position.y, 0);
		//}
		///*if (player2.projectileTime2 > 0)
		//{
		//	torpIcon4.texture = &torprldtexture;
		//}
		//else
		//{
		//	torpIcon4.texture = &torprdytexture;
		//	//torpSpeaker4.Play(torpedoSound);
		//	//soundDevice->SetSourceLocation(torpSpeaker4, PlayerTransform2.position.x, PlayerTransform2.position.y, 0);

		//}*/

		//// Player 3
		//Transform& p3Transform = ecs::GetComponent<Transform>(laMuerte3);
		//Transform& torpIconLoc5 = ecs::GetComponent<Transform>(torpIndicator5);
		//torpIconLoc5.position = Vector3(p3Transform.position.x - 5, p3Transform.position.y - 16, 200);
		//Transform& torpIconLoc6 = ecs::GetComponent<Transform>(torpIndicator6);
		//torpIconLoc6.position = Vector3(p3Transform.position.x - 5, p3Transform.position.y - 24, 201);
		//SpriteRenderer& torpIcon5 = ecs::GetComponent<SpriteRenderer>(torpIndicator5);
		//SpriteRenderer& torpIcon6 = ecs::GetComponent<SpriteRenderer>(torpIndicator6);


		//if (player3.projectileTime1 > 0)
		//{
		//	torpIcon5.texture = &torprldtexture;
		//}
		//else
		//{
		//	torpIcon5.texture = &torprdytexture;
		//	//torpSpeaker3.Play(torpedoSound);
		//	//soundDevice->SetSourceLocation(torpSpeaker3, PlayerTransform2.position.x, PlayerTransform2.position.y, 0);
		//}
		//if (player3.projectileTime2 > 0)
		//{
		//	torpIcon6.texture = &torprldtexture;
		//}
		//else
		//{
		//	torpIcon6.texture = &torprdytexture;
		//	//torpSpeaker4.Play(torpedoSound);
		//	//soundDevice->SetSourceLocation(torpSpeaker4, PlayerTransform2.position.x, PlayerTransform2.position.y, 0);

		//}

		//// Player 4
		//Transform& p4Transform = ecs::GetComponent<Transform>(laMuerte4);
		//Transform& torpIconLoc7 = ecs::GetComponent<Transform>(torpIndicator7);
		//torpIconLoc7.position = Vector3(p4Transform.position.x - 5, p4Transform.position.y - 16, 200);
		//Transform& torpIconLoc8 = ecs::GetComponent<Transform>(torpIndicator8);
		//torpIconLoc8.position = Vector3(p4Transform.position.x - 5, p4Transform.position.y - 24, 201);
		//SpriteRenderer& torpIcon7 = ecs::GetComponent<SpriteRenderer>(torpIndicator7);
		//SpriteRenderer& torpIcon8 = ecs::GetComponent<SpriteRenderer>(torpIndicator8);


		//if (player4.projectileTime1 > 0)
		//{
		//	torpIcon7.texture = &torprldtexture;
		//}
		//else
		//{
		//	torpIcon7.texture = &torprdytexture;
		//	//torpSpeaker3.Play(torpedoSound);
		//	//soundDevice->SetSourceLocation(torpSpeaker3, PlayerTransform2.position.x, PlayerTransform2.position.y, 0);
		//}
		//if (player4.projectileTime2 > 0)
		//{
		//	torpIcon8.texture = &torprldtexture;
		//}
		//else
		//{
		//	torpIcon8.texture = &torprdytexture;
		//	//torpSpeaker4.Play(torpedoSound);
		//	//soundDevice->SetSourceLocation(torpSpeaker4, PlayerTransform2.position.x, PlayerTransform2.position.y, 0);

		//}


		//engine Sounds
		//soundDevice->SetSourceLocation(engineSpeaker, PlayerTransform.position.x, PlayerTransform.position.y, 0);
		//soundDevice->SetSourceLocation(engineSpeaker2, PlayerTransform2.position.x, PlayerTransform2.position.y, 0);
		//soundDevice->SetSourceLocation(engineSpeaker3, PlayerTransform3.position.x, PlayerTransform3.position.y, 0);
		//soundDevice->SetSourceLocation(engineSpeaker4, PlayerTransform4.position.x, PlayerTransform4.position.y, 0);
		//adding pitch based on speed
		//float normalizedVelocity = PlayerRigidbody.velocity.Length() / 166.0f;
		//float accLevel = std::lerp(0.0f, 1.5f, normalizedVelocity);
		////engineSpeaker.setPitch(0.5f + accLevel);

		//float normalizedVelocity2 = PlayerRigidbody2.velocity.Length() / 166.0f;
		//float accLevel2 = std::lerp(0.0f, 1.5f, normalizedVelocity2);
		////engineSpeaker2.setPitch(0.5f + accLevel2);

		//float normalizedVelocity3 = PlayerRigidbody3.velocity.Length() / 166.0f;
		//float accLevel3 = std::lerp(0.0f, 1.5f, normalizedVelocity3);
		////engineSpeaker3.setPitch(0.5f + accLevel3);

		//float normalizedVelocity4 = PlayerRigidbody4.velocity.Length() / 166.0f;
		//float accLevel4 = std::lerp(0.0f, 1.5f, normalizedVelocity4);
		//engineSpeaker4.setPitch(0.5f + accLevel4);


		//soundDevice->SetSourceLocation(cheerSpeaker, 1530, -1700, 1);
