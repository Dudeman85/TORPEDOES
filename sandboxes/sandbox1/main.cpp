#include <engine/Application.h>
#include <engine/Tilemap.h>
#include <vector>
#include <set>
#include <chrono>
#include <thread>
#include "PlayerController.h"
#include "PickupController.h"
#include "Physics.h"

using namespace std;
using namespace engine;

ECS ecs;


int main()
{
	NO_OPENAL = true;
	//Create the window and OpenGL context before creating EngineLib
	GLFWwindow* window = CreateGLWindow(1250, 1000, "Window");
	//Create the camera
	Camera cam = Camera(1250, 1000);

	//Initialize the default engine library
	EngineLib engine;


	//Register old physics
	shared_ptr<oldPhysics::PhysicsSystem> oldPhysicsSystem;
	ecs.registerComponent<oldPhysics::Rigidbody>();
	ecs.registerComponent<oldPhysics::BoxCollider>();
	oldPhysicsSystem = ecs.registerSystem<oldPhysics::PhysicsSystem>();
	Signature physicsSystemSignature;
	physicsSystemSignature.set(ecs.getComponentId<oldPhysics::Rigidbody>());
	physicsSystemSignature.set(ecs.getComponentId<Transform>());
	physicsSystemSignature.set(ecs.getComponentId<oldPhysics::BoxCollider>());
	ecs.setSystemSignature<oldPhysics::PhysicsSystem>(physicsSystemSignature);
	ecs.registerComponent<Player>();
	shared_ptr<PlayerController> playerController = ecs.registerSystem<PlayerController>();
	Signature playerControllerSignature;
	playerControllerSignature.set(ecs.getComponentId<Transform>());
	playerControllerSignature.set(ecs.getComponentId<Player>());
	playerControllerSignature.set(ecs.getComponentId<SpriteRenderer>());
	playerControllerSignature.set(ecs.getComponentId<oldPhysics::Rigidbody>());
	playerControllerSignature.set(ecs.getComponentId<oldPhysics::BoxCollider>());
	ecs.setSystemSignature<PlayerController>(playerControllerSignature);
	//Register Pickup Controller
	ecs.registerComponent<Pickup>();
	shared_ptr<PickupController> pickupController = ecs.registerSystem<PickupController>();
	Signature pickupControllerSignature;
	pickupControllerSignature.set(ecs.getComponentId<Transform>());
	pickupControllerSignature.set(ecs.getComponentId<Pickup>());
	pickupControllerSignature.set(ecs.getComponentId<SpriteRenderer>());
	pickupControllerSignature.set(ecs.getComponentId<oldPhysics::Rigidbody>());
	pickupControllerSignature.set(ecs.getComponentId<oldPhysics::BoxCollider>());
	ecs.setSystemSignature<PickupController>(pickupControllerSignature);

	engine.physicsSystem->gravity = Vector2(0, 0);
	engine.physicsSystem->step = 8;


	///////////////////Texture & audio loading////////////////////

	Texture texture = Texture("assets/Hull.png");
	Texture turretTexture = Texture("assets/Gun_01.png");
	Texture texture2 = Texture("assets/crosshairEdit.png");
	Texture texture3 = Texture("assets/bullet.png");
	Texture speedotexture = Texture("assets/speedometer.png");

	//Create a new entity
	Entity player = ecs.newEntity();
	Transform& playerTransform = ecs.addComponent(player, Transform{ .position = Vector3(168, -150, 1.5), .scale = Vector3(40, 40, 0) });
	ecs.addComponent(player, SpriteRenderer{ &texture });
	ecs.addComponent(player, Player{});
	oldPhysics::Rigidbody& playerRigidbody = ecs.addComponent(player, oldPhysics::Rigidbody{ .gravityScale = 1, .drag = 0, .friction = 0.0, .elasticity = 0 });
	oldPhysics::BoxCollider& playerCollider = ecs.addComponent(player, oldPhysics::BoxCollider{});


	SpriteRenderSystem::SetBackgroundColor(68, 154, 141);
	/*
	Tilemap map(&cam);
	map.loadMap("assets/torptest.tmx");
	engine.physicsSystem->SetTilemap(&map);
	engine.spriteRenderSystem->SetTilemap(&map);
	*/
	pickupController->CreatePickup(600, -75);
	pickupController->CreatePickup(325, -1400);
	pickupController->CreatePickup(1340, -1375);
	pickupController->CreatePickup(1450, -100);

	//Animation explosion = AnimationsFromSpritesheet("assets/explosion.png", 5, 1, vector<int>(5, 75))[0];

	Entity speedometer = ecs.newEntity();
	ecs.addComponent(speedometer, SpriteRenderer{ .texture = &speedotexture, .uiElement = true});
	ecs.addComponent(speedometer, Transform{ .position = Vector3(-0.75, -1, 0), .scale = Vector3(0.28f, 0.35f, 1) });
	

	
	Animation crowdAnims = AnimationsFromSpritesheet("assets/CrowdCheer14.png", 3, 1, vector<int>(3, 150))[0];
	Entity crowd = ecs.newEntity();
	ecs.addComponent(crowd, Transform{ .position = Vector3(140, -40, 10), .scale = Vector3(150, 50, 0) });
	ecs.addComponent(crowd, SpriteRenderer{});
	ecs.addComponent(crowd, Animator{});
	AnimationSystem::AddAnimation(crowd, crowdAnims, "CrowdCheer");
	AnimationSystem::PlayAnimation(crowd, "CrowdCheer", true);

	Entity crowd1 = ecs.newEntity();
	ecs.addComponent(crowd1, Transform{ .position = Vector3(195, -60, 11), .scale = Vector3(150, 50, 0) });
	ecs.addComponent(crowd1, SpriteRenderer{});
	ecs.addComponent(crowd1, Animator{});
	AnimationSystem::AddAnimation(crowd1, crowdAnims, "Cheer2");
	AnimationSystem::PlayAnimation(crowd1, "Cheer2", true);
	Entity crowd2 = ecs.newEntity();
	ecs.addComponent(crowd2, Transform{ .position = Vector3(250, -30, 8), .scale = Vector3(150, 50, 0) });
	ecs.addComponent(crowd2, SpriteRenderer{});
	ecs.addComponent(crowd2, Animator{});
	AnimationSystem::AddAnimation(crowd2, crowdAnims, "Cheer3");
	AnimationSystem::PlayAnimation(crowd2, "Cheer3", true);
	
	

	//Game Loop
	while (!glfwWindowShouldClose(window))
	{
		

		if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
		{
			cout << playerTransform.position.x << ", " << playerTransform.position.y << endl;
		}

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		

		//engine::sendReliable(client, channelId, std::vector<std::string>{std::to_string(input)});
		playerController->Update(window, engine.deltaTime, oldPhysicsSystem);
		pickupController->Update(player);
		
		//Update all engine systems, this usually should go last in the game loop
		//For greater control of system execution, you can update each one manually
		engine.Update(&cam);
		oldPhysicsSystem->Update(engine.deltaTime);

		cam.SetPosition(Vector3(playerTransform.position.x, playerTransform.position.y, 100));

		//OpenGL stuff, goes very last
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	return 0;
}
//#endif
