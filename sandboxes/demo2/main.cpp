//#if 0
#include <engine/Application.h>
#include <engine/Tilemap.h>
#include <vector>
#include <set>
#include <chrono>
#include <thread>
#include "PlayerController.h"
#include "PickupController.h"
#include "TurretController.h"
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#endif
#include <enet/enet.h>

using namespace std;
using namespace engine;

ECS ecs;

int main()
{
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
	//Register Turret Controller
	ecs.registerComponent<Turret>();
	shared_ptr<TurretController> turretController = ecs.registerSystem<TurretController>();
	Signature turretControllerSignature;
	turretControllerSignature.set(ecs.getComponentId<Transform>());
	turretControllerSignature.set(ecs.getComponentId<Turret>());
	turretControllerSignature.set(ecs.getComponentId<SpriteRenderer>());
	turretControllerSignature.set(ecs.getComponentId<oldPhysics::Rigidbody>());
	turretControllerSignature.set(ecs.getComponentId<oldPhysics::BoxCollider>());
	ecs.setSystemSignature<TurretController>(turretControllerSignature);
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


	static SoundSource tankSpeaker;
	static SoundSource shootSpeaker;
	static SoundSource explosionSpeaker;
	static SoundSource enemySpeaker;

	///////////////////Texture & audio loading////////////////////

	Texture texture = Texture("assets/Hull.png");
	Texture turretTexture = Texture("assets/Gun_01.png");
	Texture texture2 = Texture("assets/crosshairEdit.png");
	Texture texture3 = Texture("assets/bullet.png");

	//load sound from file
	uint32_t sound1 = SoundBuffer::getFile()->addSoundEffect("assets/engineloop.wav");
	uint32_t sound2 = SoundBuffer::getFile()->addSoundEffect("assets/bang_05.wav");
	uint32_t sound3 = SoundBuffer::getFile()->addSoundEffect("assets/bang_09.wav");

	tankSpeaker.setLinearDistance(0.1f, 50.f, 600.f, 1.f);
	shootSpeaker.setLinearDistance(1.5f, 100.f, 700.f, 0.5f);
	explosionSpeaker.setLinearDistance(1.f, 10.f, 600.f, 1.f);
	enemySpeaker.setLinearDistance(1.f, 10.f, 600.f, 1.f);


	//Create a new entity
	Entity player = ecs.newEntity();
	Transform& playerTransform = ecs.addComponent(player, Transform{ .position = Vector3(168, -150, 1.5), .scale = Vector3(40, 40, 0) });
	ecs.addComponent(player, SpriteRenderer{ &texture });
	ecs.addComponent(player, Player{});
	oldPhysics::Rigidbody& playerRigidbody = ecs.addComponent(player, oldPhysics::Rigidbody{ .gravityScale = 1, .drag = 0, .friction = 0.0, .elasticity = 0 });
	oldPhysics::BoxCollider& playerCollider = ecs.addComponent(player, oldPhysics::BoxCollider{});

	Entity playerTurret = ecs.newEntity();
	Transform& playerTurretTransform = ecs.addComponent(playerTurret, Transform{ .scale = Vector3(50, 16.5, 0) });
	ecs.addComponent(playerTurret, SpriteRenderer{ .texture = &turretTexture });

	//play sound files
	
	tankSpeaker.Play(sound1);
	tankSpeaker.SetLooping(1);
	

	turretController->player = player;
	turretController->CreateTurret(1000, -1000);
	turretController->CreateTurret(360, -690);
	turretController->CreateTurret(1395, -1010);
	turretController->CreateTurret(1448, -376);
	turretController->CreateTurret(1066, -168);
	turretController->CreateTurret(350, -1280);

	// create entity crosshair for gamepad
	Entity crosshair = ecs.newEntity();
	// adds crosshair texture
	Transform crosshairTransform = ecs.addComponent(crosshair, Transform{ .position = Vector3(500, 500, 0), .scale = Vector3(20, 20, 0) });
	ecs.addComponent(crosshair, SpriteRenderer{ &texture2 });

	set<Entity>bullets;


	float fireCooldown = 0.04f;
	bool canFire = true;

	SpriteRenderSystem::SetBackgroundColor(68, 154, 141);
	Tilemap map(&cam);
	map.loadMap("assets/demo2.tmx");
	engine.spriteRenderSystem->SetTilemap(&map);

	pickupController->CreatePickup(600, -75);
	pickupController->CreatePickup(325, -1400);
	pickupController->CreatePickup(1340, -1375);
	pickupController->CreatePickup(1450, -100);

	Animation explosion = AnimationsFromSpritesheet("assets/explosion.png", 5, 1, vector<int>(5, 75))[0];

	//Game Loop
	while (!glfwWindowShouldClose(window))
	{


		if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
		{
			cout << playerTransform.position.x << ", " << playerTransform.position.y << endl;
		}

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		playerController->Update(window, engine.deltaTime, oldPhysicsSystem);
		pickupController->Update(player);
		engine.soundDevice->SetLocation(playerTransform.position.x, playerTransform.position.y, playerTransform.position.z);
		engine.soundDevice->SetOrientation(0.f, 1.f, 0.f, 0.f, 0.f, 1.f);
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////OHJAINSÄÄDÖT////////////////////////////////////////////////////
		int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
		if (present == GLFW_TRUE)
		{

			GLFWgamepadstate state;

			// set player speed
			float playerSpeed = 400.0f;
			//define deadzone
			float deadzoneSize = 0.5f;
			//calculate deadzone barrier
			float deadzoneTreshold = deadzoneSize / 2.0f;
			//Camera max distance
			float max_distance = 30.0f;


			int axisCount;
			int buttonCount;
			float radian = 5.0f;

			const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axisCount);
			const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);


			if (abs(axes[0]) > deadzoneTreshold || abs(axes[1]) > deadzoneTreshold)
			{
				// Sets movement vector based on movement speed
				Vector2 movement(axes[0] * playerSpeed, -axes[1] * playerSpeed);

				// move player based on movement vector
				oldPhysicsSystem->Move(player, movement * engine.deltaTime);
				//engine.physicsSystem->Move(hull, movement * engine.deltaTime);
				
				Vector2 leftThumbstick(axes[0], axes[1]);
				leftThumbstick = (leftThumbstick + Vector2(1.0f, 1.0f)) / 2.0f;
				leftThumbstick -= Vector2(0.5f, 0.5f);
				leftThumbstick *= 2.0f;

				playerTransform.rotation.z = atan2f(-leftThumbstick.y, leftThumbstick.x) * 180 / 3.14f;
				
			}

			if (abs(axes[2]) > deadzoneTreshold || abs(axes[3]) > deadzoneTreshold)
			{

				// Changes joystick cordinates into origo and scales it between -1, 1
				Vector2 rightThumbstick(axes[2], axes[3]);
				rightThumbstick = (rightThumbstick + Vector2(1.0f, 1.0f)) / 2.0f;
				rightThumbstick -= Vector2(0.5f, 0.5f);
				rightThumbstick *= 2.0f;

				// creates vector toward joystick direction
				float angle = atan2f(rightThumbstick.y, rightThumbstick.x);
				Vector2 aimdirection = Vector2(cosf(angle), sinf(angle));

				// normalizes vector and sets crosshair distance
				aimdirection.Normalize();
				aimdirection *= 150.0f;
				ecs.getComponent<SpriteRenderer>(crosshair).enabled = true;
				// crosshair position based on player position
				Vector3 playerPosition = playerTransform.position;
				Vector3 crosshairPosition = playerPosition + Vector3(aimdirection.x, -aimdirection.y, 10);
				TransformSystem::SetPosition(crosshair, crosshairPosition);

				playerTurretTransform.rotation.z = atan2f(-rightThumbstick.y, rightThumbstick.x) * 180 / 3.14f;

				if (fireCooldown <= 0)
				{
					if (axes[5] > 0.5f)
					{

						Entity bullet = ecs.newEntity();
						ecs.addComponent(bullet, Transform{ .position = Vector3(playerTransform.position.x + (aimdirection.x / 4), playerTransform.position.y - (aimdirection.y / 4), 5), .scale = Vector3(5, 5, 0) });
						ecs.addComponent(bullet, SpriteRenderer{ &texture3 });
						ecs.addComponent(bullet, oldPhysics::Rigidbody{ .velocity = Vector2(aimdirection.x * 50, -aimdirection.y * 50), .drag = 0, .elasticity = 0, .kinematic = true });
						ecs.addComponent(bullet, oldPhysics::BoxCollider{ .isTrigger = true });
						ecs.addComponent(bullet, Animator{});
						AnimationSystem::AddAnimation(bullet, explosion, "explosion");

						bullets.emplace(bullet);
						fireCooldown = 0.5f;

						//shooting sound
						shootSpeaker.Play(sound2);
					}
				}
			}
			
			else
			{
				ecs.getComponent<SpriteRenderer>(crosshair).enabled = false;
			}

			for (Entity bullet : bullets)
			{
				if (!ecs.entityExists(bullet))
					continue;

				oldPhysics::BoxCollider hit;
				if (ecs.hasComponent<oldPhysics::BoxCollider>(bullet))
					hit = ecs.getComponent<oldPhysics::BoxCollider>(bullet);
				auto& rb = ecs.getComponent<oldPhysics::Rigidbody>(bullet);
				auto& tf = ecs.getComponent<Transform>(bullet);
				auto& animator = ecs.getComponent<Animator>(bullet);

				if (!ecs.hasComponent<oldPhysics::BoxCollider>(bullet))
				{
					if (!animator.playingAnimation)
					{
						bullets.erase(bullets.find(bullet));
						ecs.destroyEntity(bullet);

					}
					break;
				}

				for (const oldPhysics::Collision& collision : hit.collisions)
				{
					if (collision.a == player || collision.b == player)
						continue;

					if (collision.type == oldPhysics::Collision::Type::entityTrigger && collision.b != bullet)
					{
						rb.velocity = Vector2(0, 0);
						tf.scale.x = 20;
						tf.scale.y = 20;
						ecs.destroyEntity(collision.b);
						ecs.removeComponent<oldPhysics::BoxCollider>(bullet);
						AnimationSystem::PlayAnimation(bullet, "explosion");


						engine.soundDevice->SetSourceLocation(explosionSpeaker, tf.position.x, tf.position.y, 1);
						explosionSpeaker.Play(sound3);


						break;
					}

					if (collision.type == oldPhysics::Collision::Type::tilemapTrigger)
					{
						rb.velocity = Vector2(0, 0);
						tf.scale.x = 20;
						tf.scale.y = 20;
						ecs.removeComponent<oldPhysics::BoxCollider>(bullet);
						AnimationSystem::PlayAnimation(bullet, "explosion");
						engine.soundDevice->SetSourceLocation(explosionSpeaker, tf.position.x, tf.position.y, 1);
						explosionSpeaker.Play(sound3);
						break;
					}
				}
			}
		}
		engine.soundDevice->SetSourceLocation(shootSpeaker, playerTransform.position.x, playerTransform.position.y, playerTransform.position.z);

		TransformSystem::SetPosition(playerTurret, Vector3(playerTransform.position.x, playerTransform.position.y, 1.6));

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////


		//engine sound
		engine.soundDevice->SetSourceLocation(tankSpeaker, playerTransform.position.x, playerTransform.position.y, 500);



		fireCooldown -= engine.deltaTime;

		turretController->Update(engine.deltaTime);
		//Update all engine systems, this usually should go last in the game loop
		//For greater control of system execution, you can update each one manually
		engine.Update(&cam);
		oldPhysicsSystem->Update(engine.deltaTime);

		cam.SetPosition(Vector3(playerTransform.position.x, playerTransform.position.y, 100));

		//OpenGL stuff, goes very last
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
//#endif


