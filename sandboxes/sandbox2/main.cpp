#if 0
#include <engine/Application.h>
#include <engine/Tilemap.h>
#include "PlayerController.h"

using namespace engine;

ECS ecs;
int checkPointNumber = 0;

// Chepoiint funtion 
void createChepoint(Vector3 position, Vector3 rotation, Vector3 scale, Model& checkPointModel, float hitboxrotation, bool finish_line = false)
{


	Entity checkpoint = ecs.newEntity();

	ecs.addComponent(checkpoint, Transform{ .position = position , .rotation = rotation , .scale = scale });
	ecs.addComponent(checkpoint, ModelRenderer{ .model = &checkPointModel });
	ecs.addComponent(checkpoint, CheckPoint{ checkPointNumber , finish_line });
	std::vector<Vector2> CheckpointcolliderVerts{ Vector2(4, 8), Vector2(4, -8), Vector2(-4, -8), Vector2(-4, 8) };
	ecs.addComponent(checkpoint, PolygonCollider({ .vertices = CheckpointcolliderVerts, .trigger = true, .visualise = true, .rotationOverride = hitboxrotation }));

	checkPointNumber++;

};


int main()
{
	NO_OPENAL = true;

	string username, selectedFont = "";
	vector<string> playerNames(3);
	vector<Font> playerFonts;

	GLFWwindow* window = engine::CreateGLWindow(1600, 900, "Windos");

	engine::EngineLib engine;

	engine::Camera cam = engine::Camera(1120, 630);
	cam.SetPosition(Vector3(0, 0, 1500));
	//cam.perspective = true;
	cam.SetRotation(Vector3(0, 0, 0));
	//cam.fov = 10.0f;
	//engine.physicsSystem->gravity = Vector2(0, -981);
	engine.collisionSystem->cam = &cam;
	ecs.registerComponent<Player>();
	ecs.registerComponent<CheckPoint>(); // register component manual 
	shared_ptr<PlayerController>playerController = ecs.registerSystem< PlayerController>();
	Signature playerControllerSignature;
	playerControllerSignature.set(ecs.getComponentId< Transform>());
	playerControllerSignature.set(ecs.getComponentId< Player>());
	playerControllerSignature.set(ecs.getComponentId< Rigidbody>());
	playerControllerSignature.set(ecs.getComponentId< PolygonCollider>());
	playerControllerSignature.set(ecs.getComponentId< ModelRenderer>());
	ecs.setSystemSignature<PlayerController>(playerControllerSignature);
	Model model("assets/Objects/LaMuerte.obj");
	Model checkPointModel("assets/Objects/Checkpoint.obj");
	Model model2("assets/Objects/Finish_line.obj");
	Texture GUItexture = Texture("assets/GUI_backround.png");
	Texture speedotexture = Texture("assets/speedometer.png");
	Texture needletexture = Texture("assets/needle.png");
	Texture torprldtexture = Texture("assets/torpedoReloading.png");
	Texture torprdytexture = Texture("assets/torpedoReady.png");
	Font arialFont("assets/fonts/ARIAL.TTF", 0, 0, 48);
	Font wingdingsFont("assets/fonts/wingding.ttf", 0, 0, 48);
	Font comicFont("assets/fonts/COMIC.TTF", 0, 0, 48);

	for (int i = 0; i < playerNames.size(); ++i)
	{
		cout << "Please enter Player's " + to_string(i + 1) + " name: ";
		cin >> username;
		playerNames[i] = username;
		do
		{
			cout << "Please enter Player's " + to_string(i + 1) + " font from selection:\n 1. arial\n 2. wingdings\n 3. comic\n > ";
			cin >> selectedFont;
			if(selectedFont == "arial")
			{
				playerFonts.push_back(arialFont);
				break;
			}
			else if (selectedFont == "wingdings")
			{
				playerFonts.push_back(wingdingsFont);
				break;
			}
			else if (selectedFont == "comic")
			{
				playerFonts.push_back(comicFont);
				break;
			}
		} while (selectedFont != "arial" || selectedFont != "wingdings" || selectedFont != "comic");
	}

	Entity laMuerte = ecs.newEntity();

	Entity player1Font = ecs.newEntity();
	Entity pfLap1 = ecs.newEntity();
	Entity player2Font = ecs.newEntity();
	Entity pfLap2 = ecs.newEntity();
	Entity player3Font = ecs.newEntity();
	Entity pfLap3 = ecs.newEntity();

	// TODO Get player transform and place it into textRendering offset transform.position
	ecs.addComponent(player1Font, TextRenderer{ .font = &playerFonts[0], .text = playerNames[0], .offset = Vector3(1.0f, -1.0f, 0), .scale = Vector3(0.5f), .color = Vector3(0.5f, 0.8f, 0.2f)});
	Transform& FontTransform = ecs.addComponent(player1Font, Transform{ .position = Vector3(1434.0f,-1449.0f, 100.0f)});
	Transform& PlayerTransform = ecs.addComponent(laMuerte, Transform{ .position = Vector3(1474.0f,-1469.0f, 100.0f), .rotation = Vector3(45.000000, 0.0000, 0.000000), .scale = Vector3(7) });
	Player& player = ecs.addComponent(laMuerte, Player{ .acerationSpeed = 300.0f, .minAceleration = 120.0f, .playerID = 0 });
	ecs.addComponent(laMuerte, ModelRenderer{ .model = &model });
	Rigidbody& PlayerRigidbody = ecs.addComponent(laMuerte, Rigidbody{ .drag = 0.025f });
	vector<Vector2> colliderVerts{ Vector2(2, 2), Vector2(2, -2), Vector2(-5, -2), Vector2(-5, 2) };
	PolygonCollider& collider = ecs.addComponent(laMuerte, PolygonCollider{ .vertices = colliderVerts, .callback = PlayerController::OnCollision , .visualise = true });


	Entity laMuerte2 = ecs.newEntity();

	ecs.addComponent(player2Font, TextRenderer{ .font = &playerFonts[1], .text = playerNames[1], .offset = Vector3(1.0f, -1.0f, 0), .scale = Vector3(0.5f), .color = Vector3(0.5f, 0.8f, 0.2f) });
	Transform& Font2Transform = ecs.addComponent(player2Font, Transform{ .position = Vector3(1434.0f,-1349.0f, 100.0f) });
	Transform& PlayerTransform2 = ecs.addComponent(laMuerte2, Transform{ .position = Vector3(1474.321533, -1369.868286, 100.000000), .rotation = Vector3(45.000000, 0.0000, 0.000000), .scale = Vector3(7) });
	Player& player2 = ecs.addComponent(laMuerte2, Player{ .acerationSpeed = 300.0f, .minAceleration = 120.0f, .playerID = 1 });
	ecs.addComponent(laMuerte2, ModelRenderer{ .model = &model });
	Rigidbody& PlayerRigidbody2 = ecs.addComponent(laMuerte2, Rigidbody{ .drag = 0.025f });
	PolygonCollider& collider2 = ecs.addComponent(laMuerte2, PolygonCollider{ .vertices = colliderVerts, .callback = PlayerController::OnCollision , .visualise = true });


	Entity laMuerte3 = ecs.newEntity();

	ecs.addComponent(player3Font, TextRenderer{ .font = &playerFonts[2], .text = playerNames[2], .offset = Vector3(1.0f, -1.0f, 0), .scale = Vector3(0.5f), .color = Vector3(0.5f, 0.8f, 0.2f) });
	Transform& Font3Transform = ecs.addComponent(player3Font, Transform{ .position = Vector3(1434.0f,-1549.0f, 100.0f) });
	Transform& PlayerTransform3 = ecs.addComponent(laMuerte3, Transform{ .position = Vector3(1474.321533, -1569.868286, 100.000000), .rotation = Vector3(45.000000, 0.0000, 0.000000), .scale = Vector3(7) });
	Player& player3 = ecs.addComponent(laMuerte3, Player{ .acerationSpeed = 300.0f, .minAceleration = 120.0f, .playerID = 2 });
	ecs.addComponent(laMuerte3, ModelRenderer{ .model = &model });
	Rigidbody& PlayerRigidbody3 = ecs.addComponent(laMuerte3, Rigidbody{ .drag = 0.025f });
	PolygonCollider& collider3 = ecs.addComponent(laMuerte3, PolygonCollider{ .vertices = colliderVerts, .callback = PlayerController::OnCollision , .visualise = true });


	Entity GUIBackround = ecs.newEntity();
	ecs.addComponent(GUIBackround, SpriteRenderer{ .texture = &GUItexture, .uiElement = true });
	ecs.addComponent(GUIBackround, Transform{ .position = Vector3(0, -0.95, -0.9), .scale = Vector3(1, 0.2, 1) });


	Entity torpIndicator1 = ecs.newEntity();
	ecs.addComponent(torpIndicator1, TextRenderer{ .font = &playerFonts[0], .text = playerNames[0], .offset = Vector3(0.0f, 1.25f, 0.0f), .scale = Vector3(0.013f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
	SpriteRenderer& torpicon1 = ecs.addComponent(torpIndicator1, SpriteRenderer{ .texture = &torprdytexture, .uiElement = true });
	ecs.addComponent(torpIndicator1, Transform{ .position = Vector3(-0.85, -0.9, -0.5), .scale = Vector3(0.05, 0.085, 1) });
	Entity torpIndicator2 = ecs.newEntity();
	SpriteRenderer& torpicon2 = ecs.addComponent(torpIndicator2, SpriteRenderer{ .texture = &torprdytexture, .uiElement = true });
	ecs.addComponent(torpIndicator2, Transform{ .position = Vector3(-0.75, -0.9, -0.55), .scale = Vector3(0.05, 0.085, 1) });

	Entity torpIndicator3 = ecs.newEntity();
	ecs.addComponent(torpIndicator3, TextRenderer{ .font = &playerFonts[1], .text = playerNames[1], .offset = Vector3(0.0f, 1.25f, 0.0f), .scale = Vector3(0.013f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
	SpriteRenderer& torpicon3 = ecs.addComponent(torpIndicator3, SpriteRenderer{ .texture = &torprdytexture, .uiElement = true });
	ecs.addComponent(torpIndicator3, Transform{ .position = Vector3(-0.05, -0.9, -0.5), .scale = Vector3(0.05, 0.085, 1) });
	Entity torpIndicator4 = ecs.newEntity();
	SpriteRenderer& torpicon4 = ecs.addComponent(torpIndicator4, SpriteRenderer{ .texture = &torprdytexture, .uiElement = true });
	ecs.addComponent(torpIndicator4, Transform{ .position = Vector3(0.05, -0.9, -0.55), .scale = Vector3(0.05, 0.085, 1) });

	Entity torpIndicator5 = ecs.newEntity();
	ecs.addComponent(torpIndicator5, TextRenderer{ .font = &playerFonts[2], .text = playerNames[2],.offset = Vector3(0.0f, 1.25f, 0.0f), .scale = Vector3(0.013f), .color = Vector3(0.5f, 0.8f, 0.2f), .uiElement = true });
	SpriteRenderer& torpicon5 = ecs.addComponent(torpIndicator5, SpriteRenderer{ .texture = &torprdytexture, .uiElement = true });
	ecs.addComponent(torpIndicator5, Transform{ .position = Vector3(0.85, -0.9, -0.5), .scale = Vector3(0.05, 0.085, 1) });
	Entity torpIndicator6 = ecs.newEntity();
	SpriteRenderer& torpicon6 = ecs.addComponent(torpIndicator6, SpriteRenderer{ .texture = &torprdytexture, .uiElement = true });
	ecs.addComponent(torpIndicator6, Transform{ .position = Vector3(0.75, -0.9, -0.55), .scale = Vector3(0.05, 0.085, 1) });

	//Entity speedometer = ecs.newEntity();
	//ecs.addComponent(speedometer, SpriteRenderer{ .texture = &speedotexture, .uiElement = true });
	//ecs.addComponent(speedometer, Transform{ .position = Vector3(-0.85, -1, -0.5), .scale = Vector3(0.15, 0.2, 1) });

	//Entity speedoneedle = ecs.newEntity();
	//ecs.addComponent(speedoneedle, SpriteRenderer{ .texture = &needletexture, .uiElement = true });
	//Transform& needleTransform = ecs.addComponent(speedoneedle, Transform{ .position = Vector3(-0.86, -1, -0.25), .scale = Vector3(0.13, 0.003, 1) }); //.position = Vector3(-0.91, -1, -1), .scale = Vector3(0.075, 0.025, 1)



	Animation crowdAnims = AnimationsFromSpritesheet("assets/CrowdCheer14.png", 3, 1, vector<int>(3, 150))[0];
	Entity crowd = ecs.newEntity();
	ecs.addComponent(crowd, Transform{ .position = Vector3(1530, -1700, 10), .scale = Vector3(100, 30, 0) });
	ecs.addComponent(crowd, SpriteRenderer{});
	ecs.addComponent(crowd, Animator{});
	AnimationSystem::AddAnimation(crowd, crowdAnims, "CrowdCheer");
	AnimationSystem::PlayAnimation(crowd, "CrowdCheer", true);
	Entity crowd1 = ecs.newEntity();
	ecs.addComponent(crowd1, Transform{ .position = Vector3(1545, -1715, 11), .scale = Vector3(100, 30, 0) });
	ecs.addComponent(crowd1, SpriteRenderer{});
	ecs.addComponent(crowd1, Animator{});
	AnimationSystem::AddAnimation(crowd1, crowdAnims, "Cheer2");
	AnimationSystem::PlayAnimation(crowd1, "Cheer2", true);
	Entity crowd2 = ecs.newEntity();
	ecs.addComponent(crowd2, Transform{ .position = Vector3(1520, -1730, 12), .scale = Vector3(100, 30, 0) });
	ecs.addComponent(crowd2, SpriteRenderer{});
	ecs.addComponent(crowd2, Animator{});
	AnimationSystem::AddAnimation(crowd2, crowdAnims, "Cheer3");
	AnimationSystem::PlayAnimation(crowd2, "Cheer3", true);


	// Loand Map . Tilemap file 
	Tilemap map(&cam);
	map.loadMap("assets/Tiled/tilemaps/torptest.tmx");
	engine.spriteRenderSystem->SetTilemap(&map);
	engine.collisionSystem->SetTilemap(&map);

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

		// playerControl Update for frame 
		playerController->Update(window, engine.deltaTime);

		//Keep the camera in bounds of the tilemap
		float camPosX = clamp(PlayerTransform.position.x, map.position.x + cam.width / 2, map.position.x + map.bounds.width - cam.width / 2);
		float camPosY = clamp(PlayerTransform.position.y, map.position.y - map.bounds.height + cam.height / 2, map.position.y - cam.height / 2);
		cam.SetPosition(Vector3(camPosX, camPosY, 1500));

		// SetLitght position to Camara position & LitghtColor  
		engine.modelRenderSystem->SetLight(Vector3(cam.position.x, cam.position.y, 1500), Vector3(255));

		if (player.projectileTime1 > 0) {
			torpicon1.texture = &torprldtexture;
		}
		else {
			torpicon1.texture = &torprdytexture;
		}
		if (player.projectileTime2 > 0) {
			torpicon2.texture = &torprldtexture;
		}
		else {
			torpicon2.texture = &torprdytexture;
		}

		if (player2.projectileTime1 > 0) {
			torpicon3.texture = &torprldtexture;
		}
		else {
			torpicon3.texture = &torprdytexture;
		}
		if (player2.projectileTime2 > 0) {
			torpicon4.texture = &torprldtexture;
		}
		else {
			torpicon4.texture = &torprdytexture;
		}


		if (player3.projectileTime1 > 0) {
			torpicon5.texture = &torprldtexture;
		}
		else {
			torpicon5.texture = &torprdytexture;
		}
		if (player3.projectileTime2 > 0) {
			torpicon6.texture = &torprldtexture;
		}
		else {
			torpicon6.texture = &torprdytexture;
		}



		//needleTransform.rotation.z = PlayerRigidbody.velocity.Length() * -1;
		//CheckPoint print position rotation and scale 
		//std::cout << PlayerTransform.position.ToString() << PlayerTransform.rotation.ToString() << PlayerTransform.scale.ToString() << endl;


		engine.Update(&cam);


		glfwSwapBuffers(window);
		glfwPollEvents();

	}



	glfwTerminate();
	return 0;
}





//#include <engine/Application.h>
//#include <engine/Tilemap.h>
////#include <engine/Font.h>
//// enet wrappaus ja cpp:stä enetin kutsu tai CreateWindows funktion nimen muutos enginessä
//
//using namespace std;
//using namespace engine;
//
//float speed = 10.0f;
//static int minNumberPlayers = 2;
//static int maxNumberPlayers = 4;
//int vecPos = 0;
//Vector3 pos1, pos2;
//vector<Entity> players(maxNumberPlayers);
//
////***** Main scene!!! *****
//
//void playerSpawn(Font* font, string username, Vector3 color)
//{
//	if (vecPos <= maxNumberPlayers)
//	{
//		Entity player = ecs.newEntity();
//		ecs.addComponent(player, TextRenderer{ .font = font, .text = username, .offset = Vector3(-1.0f, 1.0f, 0.0f), .scale = Vector3(0.03f), .color = color });
//		ecs.addComponent(player, Transform{ .position = Vector3(0.0f, 0.0f, 0.0f), .scale = Vector3(30.0f, 30.0f, 0.0f) });
//		if (pos1 == NULL)
//		{
//			pos1 = Vector3(100.0f, 50.0f, 100.0f);
//			TransformSystem::Translate(player, pos1);
//		}
//		players[vecPos] = player;
//		vecPos++;
//	}
//}
//
//ECS ecs;
//
//
//int main()
//{
//	
//	// Player username
//	string username = "";
//
//	// Username check
//	do
//	{
//		cout << "Give your username: ";
//		cin >> username;
//	} while (username == "");	
//
//	// Create the window and OpenGL context before creating EngineLib
//	GLFWwindow* window = engine::CreateGLWindow(1600, 900, "Sandbox2");
//	
//	// Camera
//	Camera cam = Camera(1120, 630);
//	cam.SetPosition(Vector3{ 0, 0, 1500 });
//	cam.SetRotation(Vector3{ 0,0,0 });
//
//	// Initialize the default engine library
//	EngineLib engine;
//	
//	// Window background colour
//	SpriteRenderSystem::SetBackgroundColor(0, 120, 0);
//
//	// Texture
//	//Texture texture0 = Texture("assets/PeepoBlankie.png");
//	//Texture texture1 = Texture("assets/Enemy.png");
//
//	// Player 3D model
//	Model playerModel("assets/objects/LaMuerte.obj");
//
//	// Text
//	Font arialFont("assets/fonts/ARIAL.TTF", 0, 0, 48);
//	Font wingdingsFont("assets/fonts/wingding.ttf", 0, 0, 48);
//
//	// Entities
//	playerSpawn(&arialFont, username, Vector3(0.5f, 0.8f, 0.2f));
//	playerSpawn(&arialFont, "player", Vector3(0.5f, 0.8f, 0.2f));
//	playerSpawn(&arialFont, "player", Vector3(0.5f, 0.8f, 0.2f));
//	playerSpawn(&arialFont, "player", Vector3(0.5f, 0.8f, 0.2f));
//	cout << to_string(players.size()) << endl;
//	//Entity player1 = ecs.newEntity();
//	//ecs.addComponent(player1, SpriteRenderer{ &texture0,});
//	//ecs.addComponent(player1, TextRenderer{ .font = &arialFont, .text = username, .offset = Vector3(-1.0f, 1.0f, 0.0f), .scale = Vector3(0.03f), .color = Vector3(0.5f, 0.8f, 0.2f)});
//	//ecs.addComponent(player1, Transform{ .position = Vector3(100.0f, 50.0f, 100.0f), .scale= Vector3(30.0f, 30.0f, 0.0f)});
//	//Entity player2 = ecs.newEntity();
//	//ecs.addComponent(player2, SpriteRenderer{ &texture1 });
//	//ecs.addComponent(player2, TextRenderer{ .font = &wingdingsFont, .text = "player_2", .offset = Vector3(-1.0f, 1.0f, 0.0f), .scale = Vector3(0.03f), .color = Vector3(0.3f, 0.7f, 0.9f) });
//	//ecs.addComponent(player2, Transform{ .position = Vector3(-200.0f, 50.0f, 0.0f), .rotation = Vector3(10.0f, 20.0f, 0.0f), .scale = Vector3(30.0f, 30.0f, 0.0f)});
//
//	// Tilemap
//	Tilemap map(&cam);
//	map.loadMap("assets/Tiled/Tilemaps/torptest.tmx");
//	engine.spriteRenderSystem->SetTilemap(&map);
//	engine.collisionSystem->SetTilemap(&map);
//
//	// Game loop
//	// TODO: Players movement (move your own player and server moves others)
//	while (!glfwWindowShouldClose(window))
//	{
//		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		{
//			glfwSetWindowShouldClose(window, true);
//		}
//
//		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//		{
//			TransformSystem::Translate(players[0], Vector3(speed, 0, 0));
//		}
//
//		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//		{
//			TransformSystem::Translate(players[0], Vector3(0, speed, 0));
//		}
//
//		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//		{
//			TransformSystem::Translate(players[0], Vector3(speed, 0, 0));
//		}
//
//		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//		{
//			TransformSystem::Translate(players[0], Vector3(0, speed, 0));
//		}
//		
//		// Update all engine systems, this usually should go last in the game loop
//		// For greater control of system execution, you can update each one manually
//		engine.Update(&cam);
//
//		//arialFont.render(&cam, "This1", -390.0f, -290.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
//		//wingdingsFont.render(&cam, "This2", 150.0f, 275.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
//		
//		// OpenGL stuff, goes very last
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//
//		// Wait for message from server
//		//update(client, OnClientConnect, OnClientDisconnect, OnClientReceive, 10000);
//	}
//
//	glfwTerminate();
//	return 0;
//}
#endif