// Author: Viksteri Käppi. UI, sprite and animation testing
#define WIN32_LEAN_AND_MEAN

#include "engine/Input.h"  
#include "MenuSystem.h"	
#include "GameCamera.h"

int checkPointNumber = 0;
int activeMapLayer = 0;

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

void LoadLevel1(Camera* cam)
{
	collisionSystem->cam = cam;

	/*TimerSystem::ScheduleFunction(
		[]() {
			resources::level1Map->enabledLayers[0] = false;
			resources::level1Map->enabledLayers[1] = false;
			resources::level1Map->enabledLayers[2] = false;
			resources::level1Map->enabledLayers[3] = false;
			resources::level1Map->enabledLayers[activeMapLayer] = true;
			activeMapLayer++;
			if (activeMapLayer >1)
			{
				activeMapLayer = 0;
			}
		}, 0.3, true, ScheduledFunction::Type::seconds);
	resources::level1Map->enabledLayers[1] = true;*/

	//Set this level's tilemap
	spriteRenderSystem->SetTilemap(resources::level1Map);
	collisionSystem->SetTilemap(resources::level1Map);
	PhysicsSystem::SetTileProperty(1, TileProperty{ true });

	std::vector<ShipType> ships{ShipType::cannonBoat, ShipType::torpedoBoat, ShipType::torpedoBoat, ShipType::torpedoBoat};
	ecs::GetSystem<PlayerController>()->CreatePlayers(4, Vector2(1434.0f, -1370.0f), ships);

	//Make all the checkpoints manually
	CreateCheckpoint(Vector3(586.608276, -1249.448486, 100.000000), Vector3(45.000000, 40.070156, 0.000000), Vector3(17), resources::models["Checkpoint.obj"], 90.0f);
	CreateCheckpoint(Vector3(1513.692383, -1462.996187, 50.000000), Vector3(90.000000, 90.901711, 0.000000), Vector3(14), resources::models["Finish_line.obj"], -1, true); // 10

	//Make the crowds manually
	CreateCrowd({ 1530, -1700, 10 }, resources::crowdAnims);
	CreateCrowd({ 1545, -1715, 11 }, resources::crowdAnims);
	CreateCrowd({ 1520, -1730, 12 }, resources::crowdAnims);
	PlayCountdown();
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
	//SetupInput();

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
