// Author: Viksteri Käppi. UI, sprite and animation testing

#define WIN32_LEAN_AND_MEAN
#include <engine/Tilemap.h>
#include "PlayerController.h"


int checkPointNumber = 0;
//
//void createChepoint(Vector3 position, Vector3 rotation, Vector3 scale, Model& checkPointModel, float hitboxrotation, bool finish_line = false)
//{
//	ecs::Entity checkpoint = ecs::NewEntity();
//
//	ecs::AddComponent(checkpoint, new Transform{ .position = position , .rotation = rotation , .scale = scale });
//	ecs::AddComponent(checkpoint, new ModelRenderer{ .model = &checkPointModel });
//	ecs::AddComponent(checkpoint, new  CheckPoint{ .checkPointID = checkPointNumber , .Finish_line = finish_line });
//	std::vector<Vector2> CheckpointcolliderVerts{ Vector2(4, 8), Vector2(4, -8), Vector2(-4, -8), Vector2(-4, 8) };
//	ecs::AddComponent(checkpoint, new PolygonCollider({ .vertices = CheckpointcolliderVerts, .trigger = true, .visualise = false, .rotationOverride = hitboxrotation }));
//
//	checkPointNumber++;
//};

int main()
{
	string username = "";
	string lap = "1";
	vector<string> playerNames(4);
	vector<int> playerCheckpoints(4);
	for (int i = 0; i < playerNames.size(); ++i)
	{
		cout << "Please enter Player's " + to_string(i + 1) + " name: ";
		cin >> username;
		playerNames[i] = username;
	}

	GLFWwindow* window = engine::CreateGLWindow(1600, 900, "Window");

	EngineInit();

	engine::Camera cam = engine::Camera(1120, 630);
	cam.SetPosition(Vector3(0, 0, 1500));
	//cam.perspective = true;
	cam.SetRotation(Vector3(0, 0, 0));
	float camScale = 1.0;
	float camScaleMin = 600.0f; //MAX 900
	float camScaleMax = 1650.0f; //MAX 1900
	float aspectRatio = 16.f / 9.f;
	float camPadding = 100;
	float camDeadzone = 10;
	//engine.physicsSystem->gravity = Vector2(0, -981);
	collisionSystem->cam = &cam;

	Model model("LaMuerte.obj");
	Model checkPointModel("Checkpoint.obj");
	Model model2("Finish_line.obj");
	//Texture GUItexture = Texture("GUI_backround.png");
	Texture torprldtexture = Texture("UI_Red_Torpedo_Icon.png");
	Texture torprdytexture = Texture("UI_Green_Torpedo_Icon.png");
	// Font http address:
	// https://www.dafont.com/stencil-ww-ii.font
	Font stencilFont("Stencil WW II.ttf", 0, 0, 48);


	Texture* winSprite = new Texture("winner.png");
	ecs::Entity playerWin = ecs::NewEntity();
	ecs::AddComponent(playerWin, TextRenderer{ .font = &stencilFont, .text = "", .offset = Vector3(-1.0f, 1.1f, 1.0f), .scale = Vector3(0.02f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
	ecs::AddComponent(playerWin, SpriteRenderer{ .texture = winSprite, .enabled = false, .uiElement = true });
	ecs::AddComponent(playerWin, Transform{ .position = Vector3(0, 0, 0), .scale = Vector3(0.5f) });
	std::shared_ptr<PlayerController> playerController = ecs::GetSystem<PlayerController>();
	playerController->Init();

	PlayerController::playerWin = playerWin;

	ecs::Entity pFont1 = ecs::NewEntity();
	ecs::Entity pFont2 = ecs::NewEntity();
	ecs::Entity pFont3 = ecs::NewEntity();
	ecs::Entity pFont4 = ecs::NewEntity();
	ecs::Entity pSFont1 = ecs::NewEntity();
	ecs::Entity pSFont2 = ecs::NewEntity();
	ecs::Entity pSFont3 = ecs::NewEntity();
	ecs::Entity pSFont4 = ecs::NewEntity();


	ecs::AddComponent(pSFont1, TextRenderer{ .font = &stencilFont, .text = lap + "/3", .offset = Vector3(1.2f, -0.3f, 0.0f), .scale = Vector3(0.02f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
	ecs::AddComponent(pSFont1, Transform{ .position = Vector3(-0.65, -0.9, -0.5), .scale = Vector3(0.05, 0.085, 1) });
	ecs::AddComponent(pSFont2, TextRenderer{ .font = &stencilFont, .text = lap + "/3", .offset = Vector3(1.2f, -0.3f, 0.0f), .scale = Vector3(0.02f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
	ecs::AddComponent(pSFont2, Transform{ .position = Vector3(-0.15, -0.9, -0.5), .scale = Vector3(0.05, 0.085, 1) });
	ecs::AddComponent(pSFont3, TextRenderer{ .font = &stencilFont, .text = lap + "/3", .offset = Vector3(1.2f, -0.3f, 0.0f), .scale = Vector3(0.02f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
	ecs::AddComponent(pSFont3, Transform{ .position = Vector3(0.25, -0.9, -0.5), .scale = Vector3(0.05, 0.085, 1) });
	ecs::AddComponent(pSFont4, TextRenderer{ .font = &stencilFont, .text = lap + "/3", .offset = Vector3(1.2f, -0.3f, 0.0f), .scale = Vector3(0.02f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
	ecs::AddComponent(pSFont4, Transform{ .position = Vector3(0.75, -0.9, -0.5), .scale = Vector3(0.05, 0.085, 1) });

	/*
	AudioEngine engineSpeaker;
	AudioEngine torpSpeaker;
	AudioEngine cheerSpeaker;

	Audio* torpedoSound = torpSpeaker.createAudio("torpedoshoot.wav", false, 1, Vector3(0, 0, 0));
	Audio* explosionSound = torpSpeaker.createAudio("explosion.wav", false, 1, Vector3(0, 0, 0));
	Audio* engineSound = engineSpeaker.createAudio("enginemono.wav", true, 1, Vector3(0, 0, 0));
	Audio* cheerSound = cheerSpeaker.createAudio("cheering.wav", true, 1, Vector3(0, 0, 0));
	*/
	//TODO: Set audiance cheer distance
	/*
	engineSpeaker.setLinearDistance(0.4f, 10.f, 400.f, 0.6f);
	engineSpeaker2.setLinearDistance(0.4f, 10.f, 400.f, 0.6f);
	engineSpeaker3.setLinearDistance(0.4f, 10.f, 400.f, 0.6f);
	engineSpeaker4.setLinearDistance(0.4f, 10.f, 400.f, 0.6f);
	torpSpeaker.setLinearDistance(1.0f, 10.f, 500.f, 0.5f);
	torpSpeaker2.setLinearDistance(1.0f, 10.f, 500.f, 0.5f);
	torpSpeaker3.setLinearDistance(1.0f, 10.f, 500.f, 0.5f);
	torpSpeaker4.setLinearDistance(1.0f, 10.f, 500.f, 0.5f);
	torpSpeaker5.setLinearDistance(1.0f, 10.f, 500.f, 0.5f);
	torpSpeaker6.setLinearDistance(1.0f, 10.f, 500.f, 0.5f);
	explosionSpeaker.setLinearDistance(1.0f, 10.f, 600.f, 1.f);
	cheerSpeaker.setLinearDistance(0.5f, 50.f, 350.f, 0.9f);
	*/
	ecs::Entity laMuerte = ecs::NewEntity();
	ecs::AddComponent(pFont1, TextRenderer{ .font = &stencilFont, .text = playerNames[0], .offset = Vector3(-30.0f, -1.0f, 0), .scale = Vector3(0.5f), .color = Vector3(0.5f, 0.8f, 0.2f) });
	ecs::AddComponent(pFont1, Transform{ .position = Vector3(1434.0f,-1449.0f, 100.0f) });
	ecs::AddComponent(laMuerte, Transform{ .position = Vector3(1474.321533, -1435.868286, 100.000000), .rotation = Vector3(45.000000, 0.0000, 0.000000), .scale = Vector3(7) });
	ecs::AddComponent(laMuerte, Player{ .acerationSpeed = 300.0f, .minAceleration = 120.0f, .playerID = 0, .playerFont = pFont1, .playername = playerNames[0], .playerLap = lap });
	ecs::AddComponent(laMuerte, ModelRenderer{ .model = &model });
	ecs::AddComponent(laMuerte, Rigidbody{ .drag = 0.025f });
	vector<Vector2> colliderVerts{ Vector2(2, 2), Vector2(2, -1), Vector2(-5, -1), Vector2(-5, 2) };
	ecs::AddComponent(laMuerte, PolygonCollider{ .vertices = colliderVerts, .callback = PlayerController::OnCollision , .visualise = false });
	//engineSpeaker2.Play(engineSound);
	//engineSpeaker2.SetLooping(1);

	Transform& test = ecs::GetComponent<Transform>(laMuerte);

	ecs::Entity laMuerte2 = ecs::NewEntity();

	ecs::AddComponent(pFont2, TextRenderer{ .font = &stencilFont, .text = playerNames[1], .offset = Vector3(1.0f, -1.0f, 0), .scale = Vector3(0.5f), .color = Vector3(0.5f, 0.8f, 0.2f) });
	ecs::AddComponent(pFont2, Transform{ .position = Vector3(1434.0f,-1349.0f, 100.0f) });
	ecs::AddComponent(laMuerte2, Transform{ .position = Vector3(1474.321533, -1369.868286, 100.000000), .rotation = Vector3(45.000000, 0.0000, 0.000000), .scale = Vector3(7) });
	ecs::AddComponent(laMuerte2, Player{ .acerationSpeed = 300.0f, .minAceleration = 120.0f, .playerID = 1, .playerFont = pFont2,.playername = playerNames[1], .playerLap = lap });
	ecs::AddComponent(laMuerte2, ModelRenderer{ .model = &model });
	ecs::AddComponent(laMuerte2, Rigidbody{ .drag = 0.025f });
	ecs::AddComponent(laMuerte2, PolygonCollider{ .vertices = colliderVerts, .callback = PlayerController::OnCollision , .visualise = false });
	//engineSpeaker2.Play(engineSound);
	//engineSpeaker2.SetLooping(1);


	ecs::Entity laMuerte3 = ecs::NewEntity();

	ecs::AddComponent(pFont3, TextRenderer{ .font = &stencilFont, .text = playerNames[2], .offset = Vector3(1.0f, -1.0f, 0), .scale = Vector3(0.5f), .color = Vector3(0.5f, 0.8f, 0.2f) });
	ecs::AddComponent(pFont3, Transform{ .position = Vector3(1434.0f,-1549.0f, 100.0f) });
	ecs::AddComponent(laMuerte3, Transform{ .position = Vector3(1474.321533, -1495.868286, 100.000000), .rotation = Vector3(45.000000, 0.0000, 0.000000), .scale = Vector3(7) });
	ecs::AddComponent(laMuerte3, Player{ .acerationSpeed = 300.0f, .minAceleration = 120.0f, .playerID = 2, .playerFont = pFont3,.playername = playerNames[2], .playerLap = lap });
	ecs::AddComponent(laMuerte3, ModelRenderer{ .model = &model });
	ecs::AddComponent(laMuerte3, Rigidbody{ .drag = 0.025f });
	ecs::AddComponent(laMuerte3, PolygonCollider{ .vertices = colliderVerts, .callback = PlayerController::OnCollision , .visualise = false });
	//engineSpeaker3.Play(engineSound);
	//engineSpeaker3.SetLooping(1);


	ecs::Entity laMuerte4 = ecs::NewEntity();

	ecs::AddComponent(pFont4, TextRenderer{ .font = &stencilFont, .text = playerNames[3], .offset = Vector3(1.0f, -1.0f, 0), .scale = Vector3(0.5f), .color = Vector3(0.5f, 0.8f, 0.2f) });
	ecs::AddComponent(pFont4, Transform{ .position = Vector3(1434.0f,-1549.0f, 100.0f) });
	ecs::AddComponent(laMuerte4, Transform{ .position = Vector3(1474.321533, -1569.868286, 100.000000), .rotation = Vector3(45.000000, 0.0000, 0.000000), .scale = Vector3(7) });
	ecs::AddComponent(laMuerte4, Player{ .acerationSpeed = 300.0f, .minAceleration = 120.0f, .playerID = 3, .playerFont = pFont4,.playername = playerNames[3], .playerLap = lap });
	ecs::AddComponent(laMuerte4, ModelRenderer{ .model = &model });
	ecs::AddComponent(laMuerte4, Rigidbody{ .drag = 0.025f });
	ecs::AddComponent(laMuerte4, PolygonCollider{ .vertices = colliderVerts, .callback = PlayerController::OnCollision , .visualise = false });
	//engineSpeaker4.Play(engineSound);
	//engineSpeaker4.SetLooping(1);

	//ecs::Entity GUIBackround = ecs::NewEntity();
	//ecs::AddComponent(GUIBackround, new SpriteRenderer{ .texture = &GUItexture, .uiElement = true });
	//ecs::AddComponent(GUIBackround, new Transform{ .position = Vector3(0, -0.95, -0.9), .scale = Vector3(1, 0.2, 1) });

	ecs::Entity torpIndicator1 = ecs::NewEntity();
	//ecs::AddComponent(torpIndicator1, TextRenderer{ .font = &stencilFont, .text = playerNames[0], .offset = Vector3(0.0f, 1.25f, 0.0f), .scale = Vector3(0.013f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
	ecs::AddComponent(torpIndicator1, SpriteRenderer{ .texture = &torprdytexture});
	ecs::AddComponent(torpIndicator1, Transform{ .position = Vector3(0, 0, 0), .scale = Vector3(15, 15, 8) });
	
	ecs::Entity torpIndicator2 = ecs::NewEntity();
	ecs::AddComponent(torpIndicator2, SpriteRenderer{ .texture = &torprdytexture});
	ecs::AddComponent(torpIndicator2, Transform{ .position = Vector3(0, 0, 0), .scale = Vector3(15, 15, 8) });

	ecs::Entity torpIndicator3 = ecs::NewEntity();
	ecs::AddComponent(torpIndicator3, TextRenderer{ .font = &stencilFont, .text = playerNames[1], .offset = Vector3(0.0f, 1.25f, 0.0f), .scale = Vector3(0.013f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
	ecs::AddComponent(torpIndicator3, SpriteRenderer{ .texture = &torprdytexture, .uiElement = true });
	ecs::AddComponent(torpIndicator3, Transform{ .position = Vector3(-0.25, -0.9, -0.5), .scale = Vector3(0.05, 0.085, 1) });
	ecs::Entity torpIndicator4 = ecs::NewEntity();
	ecs::AddComponent(torpIndicator4, SpriteRenderer{ .texture = &torprdytexture, .uiElement = true });
	ecs::AddComponent(torpIndicator4, Transform{ .position = Vector3(-0.15, -0.9, -0.55), .scale = Vector3(0.05, 0.085, 1) });

	ecs::Entity torpIndicator5 = ecs::NewEntity();
	ecs::AddComponent(torpIndicator5, TextRenderer{ .font = &stencilFont, .text = playerNames[2],.offset = Vector3(0.0f, 1.25f, 0.0f), .scale = Vector3(0.013f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
	ecs::AddComponent(torpIndicator5, SpriteRenderer{ .texture = &torprdytexture, .uiElement = true });
	ecs::AddComponent(torpIndicator5, Transform{ .position = Vector3(0.15, -0.9, -0.5), .scale = Vector3(0.05, 0.085, 1) });
	ecs::Entity torpIndicator6 = ecs::NewEntity();
	ecs::AddComponent(torpIndicator6, SpriteRenderer{ .texture = &torprdytexture, .uiElement = true });
	ecs::AddComponent(torpIndicator6, Transform{ .position = Vector3(0.25, -0.9, -0.55), .scale = Vector3(0.05, 0.085, 1) });

	ecs::Entity torpIndicator7 = ecs::NewEntity();
	ecs::AddComponent(torpIndicator7, TextRenderer{ .font = &stencilFont, .text = playerNames[3],.offset = Vector3(0.0f, 1.25f, 0.0f), .scale = Vector3(0.013f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
	ecs::AddComponent(torpIndicator7, SpriteRenderer{ .texture = &torprdytexture, .uiElement = true });
	ecs::AddComponent(torpIndicator7, Transform{ .position = Vector3(0.65, -0.9, -0.5), .scale = Vector3(0.05, 0.085, 1) });
	ecs::Entity torpIndicator8 = ecs::NewEntity();
	ecs::AddComponent(torpIndicator8, SpriteRenderer{ .texture = &torprdytexture, .uiElement = true });
	ecs::AddComponent(torpIndicator8, Transform{ .position = Vector3(0.75, -0.9, -0.55), .scale = Vector3(0.05, 0.085, 1) });


	// create explosion Animation PlayerController 
	Animation explosionAnim = AnimationsFromSpritesheet("explosion.png", 6, 1, vector<int>(6, 150))[0];
	playerController->ExplosionAnim = &explosionAnim;

	Animation crowdAnims = AnimationsFromSpritesheet("CrowdCheer14.png", 3, 1, vector<int>(3, 150))[0];
	ecs::Entity crowd = ecs::NewEntity();
	ecs::AddComponent(crowd, Transform{ .position = Vector3(1530, -1700, 10), .scale = Vector3(100, 30, 0) });
	ecs::AddComponent(crowd, SpriteRenderer{});
	ecs::AddComponent(crowd, Animator{});
	AnimationSystem::AddAnimation(crowd, crowdAnims, "CrowdCheer");
	AnimationSystem::PlayAnimation(crowd, "CrowdCheer", true);
	ecs::Entity crowd1 = ecs::NewEntity();
	ecs::AddComponent(crowd1, Transform{ .position = Vector3(1545, -1715, 11), .scale = Vector3(100, 30, 0) });
	ecs::AddComponent(crowd1, SpriteRenderer{});
	ecs::AddComponent(crowd1, Animator{});
	AnimationSystem::AddAnimation(crowd1, crowdAnims, "Cheer2");
	AnimationSystem::PlayAnimation(crowd1, "Cheer2", true);
	ecs::Entity crowd2 = ecs::NewEntity();
	ecs::AddComponent(crowd2, Transform{ .position = Vector3(1520, -1730, 12), .scale = Vector3(100, 30, 0) });
	ecs::AddComponent(crowd2, SpriteRenderer{});
	ecs::AddComponent(crowd2, Animator{});
	AnimationSystem::AddAnimation(crowd2, crowdAnims, "Cheer3");
	AnimationSystem::PlayAnimation(crowd2, "Cheer3", true);
	//cheerSpeaker.Play(cheerSound);
	//cheerSpeaker.SetLooping(1);


	Animation countdownAnim = AnimationsFromSpritesheet("UI_Countdown_Ver2.png", 5, 1, vector<int>(5, 1000))[0];
	ecs::Entity countdown = ecs::NewEntity();
	ecs::AddComponent(countdown, Transform{ .position = Vector3(1475, -1270, 10), .scale = Vector3(60, 100, 0) });
	ecs::AddComponent(countdown, SpriteRenderer{});
	ecs::AddComponent(countdown, Animator{ .onAnimationEnd = ecs::DestroyEntity });
	AnimationSystem::AddAnimation(countdown, countdownAnim, "CountDown");
	AnimationSystem::PlayAnimation(countdown, "CountDown", false);
	
	
	

	// Loand Map . Tilemap file 
	Tilemap map(&cam);
	map.loadMap("level1.tmx");
	spriteRenderSystem->SetTilemap(&map);
	collisionSystem->SetTilemap(&map);
	PhysicsSystem::SetTileProperty(1, TileProperty{ true });
	//call the function. createChepoint

	//createChepoint(Vector3(2100.226807, -963.837402, 100.000000), Vector3(30.000000, 159.245773, 0.000000), Vector3(17), checkPointModel, 45.0f); // 0
	//createChepoint(Vector3(2957.365723, -828.268005, 100.000000), Vector3(45.000000, 147.891968, 0.000000), Vector3(17), checkPointModel, 45.0f); // 1
	//createChepoint(Vector3(3387.268555, -355.873444, 100.000000), Vector3(45.000000, 99.936874, 0.000000), Vector3(17), checkPointModel, 45.0f); // 2
	//createChepoint(Vector3(3655.793701, -1339.042236, 100.000000), Vector3(45.000000, 147.891968, 0.000000), Vector3(17), checkPointModel, 45.0f); // 3
	//createChepoint(Vector3(2795.650391, -1489.039795, 100.000000), Vector3(45.000000, -368.616577, 0.000000), Vector3(17), checkPointModel, 45.0f); // 4
	//createChepoint(Vector3(2597.463135, -684.973389, 100.000000), Vector3(45.000000, 180.022018, 0.000000), Vector3(17), checkPointModel, 45.0f); // 5
	//createChepoint(Vector3(1668.260010, -990.794373, 100.000000), Vector3(45.000000, 147.891968, 0.000000), Vector3(17), checkPointModel, 45.0f); // 6 
	//createChepoint(Vector3(1043.635132, -875.206543, 100.000000), Vector3(45.000000, 179.241272, 0.000000), Vector3(17), checkPointModel, 45.0f); // 7
	//createChepoint(Vector3(943.931152, -293.566711, 100.000000), Vector3(45.000000, 107.476852, 0.000000), Vector3(17), checkPointModel, 45.0f); // 8
	//createChepoint(Vector3(586.608276, -1249.448486, 100.000000), Vector3(45.000000, 40.070156, 0.000000), Vector3(17), checkPointModel, 90.0f); // 9
	//createChepoint(Vector3(1513.692383, -1462.996187, 50.000000), Vector3(90.000000, 90.901711, 0.000000), Vector3(14), model2, -1, true); // 10

	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		// SetLitght position to Camara position & LitghtColor  
		modelRenderSystem->SetLight(Vector3(cam.position.x, cam.position.y, 1500), Vector3(255));

		// star Timer 
		float Timer = playerController->getTimer();
		std::string timerStr = std::to_string((int)round(Timer)); // round pyörista float arvo lahipakokonais 
		//if (Timer == 1) {
		//	Animator
		//}
		//winText.text = timerStr.c_str();
		//AnimationSystem::PlayAnimation(countdown, "CountDown", false);
		
		Transform& p1Transform = ecs::GetComponent<Transform>(laMuerte);
		Transform& torpIconLoc1 = ecs::GetComponent<Transform>(torpIndicator1);
		torpIconLoc1.position = Vector3(p1Transform.position.x + 5, p1Transform.position.y-18, 200);
		Transform& torpIconLoc2 = ecs::GetComponent<Transform>(torpIndicator2);
		torpIconLoc2.position = Vector3(p1Transform.position.x - 25, p1Transform.position.y - 18, 201);
		SpriteRenderer& torpIcon1 = ecs::GetComponent<SpriteRenderer>(torpIndicator1);
		SpriteRenderer& torpIcon2 = ecs::GetComponent<SpriteRenderer>(torpIndicator2);
		// UI System 				

		TextRenderer& winText = ecs::GetComponent<TextRenderer>(playerWin);
		TextRenderer& p1Win = ecs::GetComponent<TextRenderer>(pSFont1);
		TextRenderer& p2Win = ecs::GetComponent<TextRenderer>(pSFont2);
		TextRenderer& p3Win = ecs::GetComponent<TextRenderer>(pSFont3);
		TextRenderer& p4Win = ecs::GetComponent<TextRenderer>(pSFont4);
		Player& player = ecs::GetComponent<Player>(laMuerte);
		Player& player2 = ecs::GetComponent<Player>(laMuerte2);
		Player& player3 = ecs::GetComponent<Player>(laMuerte3);
		Player& player4 = ecs::GetComponent<Player>(laMuerte4);

		//player 1
		if (player.projectileTime1 > 0)
		{
			torpIcon1.texture = &torprldtexture;
		}
		else
		{
			torpIcon1.texture = &torprdytexture;
			//torpSpeaker.Play(torpedoSound);
			//soundDevice->SetSourceLocation(torpSpeaker, PlayerTransform.position.x, PlayerTransform.position.y, 0);
		}
		if (player.projectileTime2 > 0)
		{
			torpIcon2.texture = &torprldtexture;
		}
		else
		{
			torpIcon2.texture = &torprdytexture;
			//torpSpeaker2.Play(torpedoSound);
			//soundDevice->SetSourceLocation(torpSpeaker2, PlayerTransform.position.x, PlayerTransform.position.y, 0);
		}


		///// Player 2
		//if (player2.projectileTime1 > 0)
		//{
		//	torpicon3.texture = &torprldtexture;
		//}
		//else
		//{
		//	torpicon3.texture = &torprdytexture;
		//	//torpSpeaker3.Play(torpedoSound);
		//	//soundDevice->SetSourceLocation(torpSpeaker3, PlayerTransform2.position.x, PlayerTransform2.position.y, 0);
		//}
		//if (player2.projectileTime2 > 0)
		//{
		//	torpicon4.texture = &torprldtexture;
		//}
		//else
		//{
		//	torpicon4.texture = &torprdytexture;
		//	//torpSpeaker4.Play(torpedoSound);
		//	//soundDevice->SetSourceLocation(torpSpeaker4, PlayerTransform2.position.x, PlayerTransform2.position.y, 0);

		//}

		//// palyer 3
		//if (player3.projectileTime1 > 0)
		//{
		//	torpicon5.texture = &torprldtexture;
		//}
		//else
		//{
		//	torpicon5.texture = &torprdytexture;
		//	//torpSpeaker5.Play(torpedoSound);
		//	//soundDevice->SetSourceLocation(torpSpeaker5, PlayerTransform3.position.x, PlayerTransform3.position.y, 0);
		//}
		//if (player3.projectileTime2 > 0)
		//{
		//	torpicon6.texture = &torprldtexture;
		//}
		//else
		//{
		//	torpicon6.texture = &torprdytexture;
		//	//torpSpeaker6.Play(torpedoSound);
		//	//soundDevice->SetSourceLocation(torpSpeaker6, PlayerTransform3.position.x, PlayerTransform3.position.y, 0);
		//}

		//// palyer 4 

		//if (player4.projectileTime1 > 0)
		//{
		//	torpicon7.texture = &torprldtexture;
		//}
		//else
		//{
		//	torpicon7.texture = &torprdytexture;
		//	//torpSpeaker7.Play(torpedoSound);
		//	//soundDevice->SetSourceLocation(torpSpeaker7, PlayerTransform4.position.x, PlayerTransform4.position.y, 0);
		//}
		//if (player4.projectileTime2 > 0)
		//{
		//	torpicon8.texture = &torprldtexture;
		//}
		//else
		//{
		//	torpicon8.texture = &torprdytexture;
		//	//torpSpeaker8.Play(torpedoSound);
		//	//soundDevice->SetSourceLocation(torpSpeaker8, PlayerTransform4.position.x, PlayerTransform4.position.y, 0);
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

		if (player.playExlposionSound)
		{

			//explosionSpeaker.Play(explosionSound);
			//soundDevice->SetSourceLocation(explosionSpeaker, PlayerTransform.position.x, PlayerTransform.position.y, 0);
			player.playExlposionSound = false;
		}
		if (player2.playExlposionSound)
		{

			//explosionSpeaker.Play(explosionSound);
			//soundDevice->SetSourceLocation(explosionSpeaker, PlayerTransform2.position.x, PlayerTransform2.position.y, 0);
			player2.playExlposionSound = false;
		}
		if (player3.playExlposionSound)
		{

			//explosionSpeaker.Play(explosionSound);
			//soundDevice->SetSourceLocation(explosionSpeaker, PlayerTransform3.position.x, PlayerTransform3.position.y, 0);
			player3.playExlposionSound = false;
		}
		if (player4.playExlposionSound)
		{

			//explosionSpeaker.Play(explosionSound);
			//soundDevice->SetSourceLocation(explosionSpeaker, PlayerTransform4.position.x, PlayerTransform4.position.y, 0);
			player4.playExlposionSound = false;
		}


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

		float zoomOutThreshold = -camPadding * 2.5f;
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
			float zoomOutValue = zoomOutFactor - min(topDiff, min(bottomDiff, min(rightDiff, leftDiff))) / 10.0f;
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
		float desiredZoom = max(boundingBoxWidth / (cam.width * aspectRatio), boundingBoxHeight / cam.height);


		// Ajustar el zoom de la cmara solo si el zoom deseado supera los lmites establecidos
		if (desiredZoom > camScaleMin && desiredZoom < camScaleMax) {
			camScale = desiredZoom;
		}

		//Reset the average player position data
		playerController->avgPosition = Vector3();
		playerController->playerBounds = { -INFINITY, -INFINITY, INFINITY, INFINITY };

		//soundDevice->SetLocation(camPosX, camPosY, 1);
		//soundDevice->SetOrientation(0.f, 0.f, -1.f, 0.f, 0.f, 1.f);

		Update(&cam);

		// playerControl Update for frame 
		playerController->Update(window, deltaTime);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
//Zoom in
		//else if (topDiff - camPadding > camDeadzone && rightDiff - camPadding > camDeadzone && bottomDiff - camPadding > camDeadzone && leftDiff - camPadding > camDeadzone)
		//	camScale -=  min(topDiff, min(bottomDiff, min(rightDiff, leftDiff))) / 100;