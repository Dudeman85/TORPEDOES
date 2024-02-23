// Author: Viksteri Käppi. UI, sprite and animation testing
#define WIN32_LEAN_AND_MEAN
//#include <engine/Tilemap.h>
#include "GameCamera.h"

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


	// Load Map . Tilemap file 
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
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		// SetLitght position to Camara position & LitghtColor  
		modelRenderSystem->SetLight(Vector3(cam.position.x, cam.position.y, 1500), Vector3(255));

		// star Timer 
		float Timer = playerController->getTimer();
		std::string timerStr = std::to_string((int)round(Timer)); // round pyörista float arvo lahipakokonais 
		// UI System 				


		
		engine::Update(&cam);
		UpdateCam(window, cam, map);
		// playerControl Update for frame 
		playerController->Update(window, deltaTime);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ecs::DestroyAllEntities(true);
	glfwTerminate();
	return 0;
}//
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
