#define WIN32_LEAN_AND_MEAN
//#include <engine/Tilemap.h>
//#include "PlayerController.h"
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

void LoadLevel1(Camera* cam)
{
	collisionSystem->cam = cam;

	//Set this level's tilemap
	spriteRenderSystem->SetTilemap(resources::level1Map);
	collisionSystem->SetTilemap(resources::level1Map);
	PhysicsSystem::SetTileProperty(1, TileProperty{ true });

	ecs::GetSystem<PlayerController>()->CreatePlayers(4, Vector2(1434.0f, -1370.0f));

	//Make all the checkpoints manually
	CreateCheckpoint(Vector3(2100.226807, -963.837402, 100.000000), Vector3(30.000000, 159.245773, 0.000000), Vector3(17), resources::checkPointModel, 45.0f); // 0
	CreateCheckpoint(Vector3(2957.365723, -828.268005, 100.000000), Vector3(45.000000, 147.891968, 0.000000), Vector3(17), resources::checkPointModel, 45.0f); // 1
	CreateCheckpoint(Vector3(3387.268555, -355.873444, 100.000000), Vector3(45.000000, 99.936874, 0.000000), Vector3(17), resources::checkPointModel, 45.0f); // 2
	CreateCheckpoint(Vector3(3655.793701, -1339.042236, 100.000000), Vector3(45.000000, 147.891968, 0.000000), Vector3(17), resources::checkPointModel, 45.0f); // 3
	CreateCheckpoint(Vector3(2795.650391, -1489.039795, 100.000000), Vector3(45.000000, -368.616577, 0.000000), Vector3(17), resources::checkPointModel, 45.0f); // 4
	CreateCheckpoint(Vector3(2597.463135, -684.973389, 100.000000), Vector3(45.000000, 180.022018, 0.000000), Vector3(17), resources::checkPointModel, 45.0f); // 5
	CreateCheckpoint(Vector3(1668.260010, -990.794373, 100.000000), Vector3(45.000000, 147.891968, 0.000000), Vector3(17), resources::checkPointModel, 45.0f); // 6 
	CreateCheckpoint(Vector3(1043.635132, -875.206543, 100.000000), Vector3(45.000000, 179.241272, 0.000000), Vector3(17), resources::checkPointModel, 45.0f); // 7
	CreateCheckpoint(Vector3(943.931152, -293.566711, 100.000000), Vector3(45.000000, 107.476852, 0.000000), Vector3(17), resources::checkPointModel, 45.0f); // 8
	CreateCheckpoint(Vector3(586.608276, -1249.448486, 100.000000), Vector3(45.000000, 40.070156, 0.000000), Vector3(17), resources::checkPointModel, 90.0f); // 9
	CreateCheckpoint(Vector3(1513.692383, -1462.996187, 50.000000), Vector3(90.000000, 90.901711, 0.000000), Vector3(14), resources::finishLineModel, -1, true); // 10

	//Make the crowds manually
	CreateCrowd({ 1530, -1700, 10 }, resources::crowdAnims);
	CreateCrowd({ 1545, -1715, 11 }, resources::crowdAnims);
	CreateCrowd({ 1520, -1730, 12 }, resources::crowdAnims);

	//Play the countdown
	ecs::Entity countdown = ecs::NewEntity();
	ecs::AddComponent(countdown, Transform{ .position = Vector3(1475, -1270, 10), .scale = Vector3(60, 100, 0) });
	ecs::AddComponent(countdown, SpriteRenderer{});
	ecs::AddComponent(countdown, Animator{ .onAnimationEnd = ecs::DestroyEntity });
	AnimationSystem::AddAnimation(countdown, resources::countdownAnim, "CountDown");
	AnimationSystem::PlayAnimation(countdown, "CountDown", false);
}

int main()
{

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

	//Loads all globally used resources
	resources::LoadResources(&cam);

	input::initialize(window);


	std::shared_ptr<PauseSystem> pauseSystem = ecs::GetSystem<PauseSystem>();
	pauseSystem->Init(window);
	std::shared_ptr<PlayerController> playerController = ecs::GetSystem<PlayerController>();
	playerController->Init();

	////////////////////////////     INPUTS  STARTS		/////////////////	 INPUTS  STARTS		//////////	
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
	////////////////////////////     INPUTS  Ends    //////////////////     INPUTS  Ends      //////////

	//Load the first level
	LoadLevel1(&cam);

	while (!glfwWindowShouldClose(window))
	{
		input::update();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		// SetLitght position to Camara position & LitghtColor  
		modelRenderSystem->SetLight(Vector3(cam.position.x, cam.position.y, 1500), Vector3(255));

		UpdateCam(window, cam, resources::level1Map);
		engine::Update(&cam);


		// playerControl Update for frame if not paused
		if (!pauseSystem->isGamePause)
		{

			playerController->Update(window, deltaTime);

		}
		// if paused  or Pause pressed update PauseSystem
		if (pauseSystem->isGamePause || input::GetNewPress("Pause"))
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