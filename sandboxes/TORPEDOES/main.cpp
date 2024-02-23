#define WIN32_LEAN_AND_MEAN
#include <engine/Tilemap.h>
#include "PlayerController.h"
#include "engine/Input.h"  
#include "MenuSystem.h"	

int checkPointNumber = 0;
Font* stencilFont = nullptr;

void createChepoint(Vector3 position, Vector3 rotation, Vector3 scale, Model& checkPointModel, float hitboxrotation, bool finish_line = false)
{
	ecs::Entity checkpoint = ecs::NewEntity();

	ecs::AddComponent(checkpoint, Transform{ .position = position , .rotation = rotation , .scale = scale });
	ecs::AddComponent(checkpoint, ModelRenderer{ .model = &checkPointModel });
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

int main()
{
	string username = "";
	string lap = "1";
	vector<string> playerNames(4);
	vector<int> playerCheckpoints(4);

	GLFWwindow* window = engine::CreateGLWindow(1600, 900, "Window");


	EngineInit();

	input::initialize(window);
	ecs::GetSystem<PauseSystem>()->Init(window);

	auto& isGamePause = ecs::GetSystem<PauseSystem>()->isGamePause;




	////////////////////////////     INPUTS  STARTS		/////////////////	 INPUTS  STARTS		//////////	
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
	//TODO: add controller input

	input::bindDigitalInput(GLFW_KEY_SPACE, { "Shoot0" });
	input::bindDigitalInput(GLFW_KEY_1, { "Shoot1" });
	input::bindDigitalInput(GLFW_KEY_2, { "Shoot2" });
	input::bindDigitalInput(GLFW_KEY_3, { "Shoot3" });

	input::bindDigitalInput(GLFW_KEY_LEFT, { "MoveLeft" });
	input::bindDigitalInput(GLFW_KEY_RIGHT, { "MoveRight" });
	input::bindDigitalInput(GLFW_KEY_UP, { "MoveUp" });
	input::bindDigitalInput(GLFW_KEY_DOWN, { "MoveDown" });
	input::bindDigitalInput(GLFW_KEY_ENTER, { "Select" });
	input::bindDigitalInput(GLFW_KEY_P, { "Pause" });
	////////////////////////////     INPUTS  Ends    //////////////////     INPUTS  Ends      //////////
		
	engine::Camera cam = engine::Camera(1120, 630);
	cam.SetPosition(Vector3(0, 0, 1500));
	//cam.perspective = true;
	cam.SetRotation(Vector3(0, 0, 0));
	float camScale = 1.0;
	float camScaleMin = 600.0f;
	float camScaleMax = 1650.0f;
	float aspectRatio = 16.f / 9.f;
	float camPadding = 100;
	float camDeadzone = 10;
	//engine.physicsSystem->gravity = Vector2(0, -981);
	collisionSystem->cam = &cam;

	Model checkPointModel("/3dmodels/Checkpoint.obj");
	Model model2("/3dmodels/Finish_line.obj");
	// Font http address:
	// https://www.dafont.com/stencil-ww-ii.font
	stencilFont = new Font("Stencil WW II.ttf", 0, 0, 48);


	Texture* winSprite = new Texture("/GUI/winner.png");
	ecs::Entity playerWin = ecs::NewEntity();
	ecs::AddComponent(playerWin, TextRenderer{ .font = stencilFont, .text = "", .offset = Vector3(-1.0f, 1.1f, 1.0f), .scale = Vector3(0.02f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
	ecs::AddComponent(playerWin, SpriteRenderer{ .texture = winSprite, .enabled = false, .uiElement = true });
	ecs::AddComponent(playerWin, Transform{ .position = Vector3(0, 0, 0), .scale = Vector3(0.5f) });
	std::shared_ptr<PlayerController> playerController = ecs::GetSystem<PlayerController>();
	playerController->Init();
	std::shared_ptr<PauseSystem> pauseSystem = ecs::GetSystem<PauseSystem>();

	//ShipSystems
	std::shared_ptr<U_99System> u_99System = ecs::GetSystem<U_99System>();
	std::shared_ptr<Pt_10System> pt_10System = ecs::GetSystem<Pt_10System>();
	std::shared_ptr<LaMuerteSystem> laMuerte = ecs::GetSystem<LaMuerteSystem>();

	PlayerController::playerWin = playerWin;

	ecs::Entity pSFont1 = ecs::NewEntity();
	ecs::Entity pSFont2 = ecs::NewEntity();
	ecs::Entity pSFont3 = ecs::NewEntity();
	ecs::Entity pSFont4 = ecs::NewEntity();

	ecs::AddComponent(pSFont1, TextRenderer{ .font = stencilFont, .text = lap + "/3", .offset = Vector3(1.2f, -0.3f, 0.0f), .scale = Vector3(0.02f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
	ecs::AddComponent(pSFont1, Transform{ .position = Vector3(-0.65, -0.9, -0.5), .scale = Vector3(0.05, 0.085, 1) });
	ecs::AddComponent(pSFont2, TextRenderer{ .font = stencilFont, .text = lap + "/3", .offset = Vector3(1.2f, -0.3f, 0.0f), .scale = Vector3(0.02f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
	ecs::AddComponent(pSFont2, Transform{ .position = Vector3(-0.15, -0.9, -0.5), .scale = Vector3(0.05, 0.085, 1) });
	ecs::AddComponent(pSFont3, TextRenderer{ .font = stencilFont, .text = lap + "/3", .offset = Vector3(1.2f, -0.3f, 0.0f), .scale = Vector3(0.02f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
	ecs::AddComponent(pSFont3, Transform{ .position = Vector3(0.25, -0.9, -0.5), .scale = Vector3(0.05, 0.085, 1) });
	ecs::AddComponent(pSFont4, TextRenderer{ .font = stencilFont, .text = lap + "/3", .offset = Vector3(1.2f, -0.3f, 0.0f), .scale = Vector3(0.02f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
	ecs::AddComponent(pSFont4, Transform{ .position = Vector3(0.75, -0.9, -0.5), .scale = Vector3(0.05, 0.085, 1) });


	playerController->CreatePlayers(4, Vector2(1434.0f, -1370.0f));


	// create explosion Animation PlayerController 
	Animation explosionAnim = AnimationsFromSpritesheet("/spritesheets/explosion.png", 6, 1, vector<int>(6, 150))[0];
	playerController->ExplosionAnim = &explosionAnim;

	Animation crowdAnims = AnimationsFromSpritesheet("/spritesheets/CrowdCheer14.png", 3, 1, vector<int>(3, 150))[0];
	CreateCrowd({ 1530, -1700, 10 }, crowdAnims);
	CreateCrowd({ 1545, -1715, 11 }, crowdAnims);
	CreateCrowd({ 1520, -1730, 12 }, crowdAnims);


	Animation countdownAnim = AnimationsFromSpritesheet("/spritesheets/UI_Countdown_Ver2.png", 5, 1, vector<int>(5, 1000))[0];
	ecs::Entity countdown = ecs::NewEntity();
	ecs::AddComponent(countdown, Transform{ .position = Vector3(1475, -1270, 10), .scale = Vector3(60, 100, 0) });
	ecs::AddComponent(countdown, SpriteRenderer{});
	ecs::AddComponent(countdown, Animator{ .onAnimationEnd = ecs::DestroyEntity });
	AnimationSystem::AddAnimation(countdown, countdownAnim, "CountDown");
	AnimationSystem::PlayAnimation(countdown, "CountDown", false);


	// Loand Map . Tilemap file 
	Tilemap map(&cam);
	map.loadMap("/levels/level1.tmx");
	spriteRenderSystem->SetTilemap(&map);
	collisionSystem->SetTilemap(&map);
	PhysicsSystem::SetTileProperty(1, TileProperty{ true });
	//call the function. createChepoint

	createChepoint(Vector3(2100.226807, -963.837402, 100.000000), Vector3(30.000000, 159.245773, 0.000000), Vector3(17), checkPointModel, 45.0f); // 0
	createChepoint(Vector3(2957.365723, -828.268005, 100.000000), Vector3(45.000000, 147.891968, 0.000000), Vector3(17), checkPointModel, 45.0f); // 1
	createChepoint(Vector3(3387.268555, -355.873444, 100.000000), Vector3(45.000000, 99.936874, 0.000000), Vector3(17), checkPointModel, 45.0f); // 2
	createChepoint(Vector3(3655.793701, -1339.042236, 100.000000), Vector3(45.000000, 147.891968, 0.000000), Vector3(17), checkPointModel, 45.0f); // 3
	createChepoint(Vector3(2795.650391, -1489.039795, 100.000000), Vector3(45.000000, -368.616577, 0.000000), Vector3(17), checkPointModel, 45.0f); // 4
	createChepoint(Vector3(2597.463135, -684.973389, 100.000000), Vector3(45.000000, 180.022018, 0.000000), Vector3(17), checkPointModel, 45.0f); // 5
	createChepoint(Vector3(1668.260010, -990.794373, 100.000000), Vector3(45.000000, 147.891968, 0.000000), Vector3(17), checkPointModel, 45.0f); // 6 
	createChepoint(Vector3(1043.635132, -875.206543, 100.000000), Vector3(45.000000, 179.241272, 0.000000), Vector3(17), checkPointModel, 45.0f); // 7
	createChepoint(Vector3(943.931152, -293.566711, 100.000000), Vector3(45.000000, 107.476852, 0.000000), Vector3(17), checkPointModel, 45.0f); // 8
	createChepoint(Vector3(586.608276, -1249.448486, 100.000000), Vector3(45.000000, 40.070156, 0.000000), Vector3(17), checkPointModel, 90.0f); // 9
	createChepoint(Vector3(1513.692383, -1462.996187, 50.000000), Vector3(90.000000, 90.901711, 0.000000), Vector3(14), model2, -1, true); // 10

	while (!glfwWindowShouldClose(window))
	{
		
		printf("deltatime: %f \n", deltaTime);
		input::update();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		// SetLitght position to Camara position & LitghtColor  
		modelRenderSystem->SetLight(Vector3(cam.position.x, cam.position.y, 1500), Vector3(255));

		// star Timer 
		float Timer = playerController->getTimer();
		std::string timerStr = std::to_string((int)round(Timer)); // round pyörista float arvo lahipakokonais 
		// UI System 				

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//Keep the camera in bounds of the tilemap and set it to the average position of the players
		Vector3 avgPos = playerController->avgPosition / playerController->entities.size();

		//// Center the camera on the average position of the players
		float camPosX = clamp(avgPos.x, map.position.x + cam.width / 2, map.position.x + map.bounds.width - cam.width / 2);
		float camPosY = clamp(avgPos.y, map.position.y - map.bounds.height + cam.height / 2, map.position.y - cam.height / 2);
		cam.SetPosition(Vector3(camPosX, camPosY, 1500));


		//Calculate the Bounding Box
		std::array<float, 4> camBounds{
			cam.position.y * 2 + cam.height / 2,  // yls pain 
				cam.position.x * 2 + cam.width / 2,   // leveys 
				cam.position.y * 2 - cam.height / 2,
				cam.position.x * 2 - cam.width / 2 };

		float zoomOutThreshold = -camPadding * 2.5f ;
		float zoomInThreshold = camPadding * 2.0f;

		//Calculate the difference between the player and camera bounds

		float topDiff = camBounds[0] - playerController->playerBounds[0];
		float rightDiff = camBounds[1] - playerController->playerBounds[1];
		float bottomDiff = playerController->playerBounds[2] - camBounds[2];
		float leftDiff = playerController->playerBounds[3] - camBounds[3];

		//Zoom out
		if (playerController->playerBounds[0] < camBounds[0] - zoomOutThreshold ||
			playerController->playerBounds[1] > camBounds[1] + zoomOutThreshold ||
			playerController->playerBounds[2] > camBounds[2] + zoomOutThreshold ||
			playerController->playerBounds[3] < camBounds[3] - zoomOutThreshold)
		{

			float zoomOutFactor = 10.0f;
			float zoomOutValue = zoomOutFactor - min(topDiff, min(bottomDiff, min(rightDiff, leftDiff))) / 10.0;
			camScale = max(camScale + zoomOutValue, camScaleMin);
			/*camScale += 10 - min(topDiff, min(bottomDiff, min(rightDiff, leftDiff))) / 10;*/
		}
		//Zoom int
		else if (topDiff > zoomInThreshold && rightDiff > zoomInThreshold && bottomDiff > zoomInThreshold && leftDiff > zoomInThreshold)
		{

			float zoomInValue = min(topDiff, min(bottomDiff, min(rightDiff, leftDiff))) / 100.0f;
			camScale = max(camScale - zoomInValue, camScaleMin);
		}

		//Clamp the camera zoom between min and max and set it's dimensions
		camScale = clamp(camScale, camScaleMin, camScaleMax);

		cam.height = camScale;
		cam.width = cam.height * aspectRatio;

		// New implentation 
		//boundigBoxin center point 
		float boundingBoxWidth = playerController->playerBounds[1] - playerController->playerBounds[3];
		float boundingBoxHeight = playerController->playerBounds[0] - playerController->playerBounds[2];
		Vector2 boundingBoxCenter = Vector2(playerController->playerBounds[3] + boundingBoxWidth * 0.5f, playerController->playerBounds[2] + boundingBoxHeight * 0.5f);

		//Ajustar la posicin de la cmara segn el centro de la bounding box
		camPosX = clamp(boundingBoxCenter.x, map.position.x + cam.width / 2, map.position.x + map.bounds.width - cam.width / 2);
		camPosY = clamp(boundingBoxCenter.y, map.position.y - map.bounds.height + cam.height / 2, map.position.y - cam.height / 2);
		cam.SetPosition(Vector3(camPosX, camPosY, 1500));

		// Calculate the desired zoom level and adjust the camera zoom.
		float aspectRatio = cam.width / cam.height;
		float desiredZoom = std::max(boundingBoxWidth / (cam.width * aspectRatio), boundingBoxHeight / cam.height);


		// Ajustar el zoom de la cmara solo si el zoom deseado supera los lmites establecidos
		if (desiredZoom > camScaleMin && desiredZoom < camScaleMax)
		{
			camScale = desiredZoom;
		}

		//Reset the average player position data
		playerController->avgPosition = Vector3();
		playerController->playerBounds = { -INFINITY, -INFINITY, INFINITY, INFINITY };

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		engine::Update(&cam);

		// playerControl Update for frame if not paused
		if (!isGamePause)
		{

			playerController->Update(window, deltaTime);

			u_99System ->Update();
			pt_10System->Update();
			laMuerte-> Update();
		}
		// if paused  or Pause pressed update PauseSystem
		if (isGamePause || input::GetNewPress("Pause"))
		{
			printf(" P  pauseSystem");
			pauseSystem->Update();
		}
		
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	input::uninitialize();
	ecs::DestroyAllEntities(true);
	glfwTerminate();
	return 0;
}