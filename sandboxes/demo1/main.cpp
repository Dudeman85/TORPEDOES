#include <engine/Application.h>
#include <engine/Tilemap.h>

#include "Physics.h"
#include "PlayerController.h"
#include "PickupController.h"
#include "EnemyController.h"
#include "TurretController.h"

#include <chrono>
#include <thread>


using namespace std;
using namespace engine;

ECS ecs;

int main()
{
	//Create the window and OpenGL context before creating EngineLib
	GLFWwindow* window = CreateGLWindow(1000, 800, "Window");

	//Create the camera
	Camera cam = Camera(1066, 600);

	//Initialize the default engine library
	EngineLib engine;

	//Register Custom Systems
	//Register Player Controller
	ecs.registerComponent<Player>();
	shared_ptr<PlayerController> playerController = ecs.registerSystem<PlayerController>();
	Signature playerControllerSignature;
	playerControllerSignature.set(ecs.getComponentId<Transform>());
	playerControllerSignature.set(ecs.getComponentId<Player>());
	playerControllerSignature.set(ecs.getComponentId<SpriteRenderer>());
	playerControllerSignature.set(ecs.getComponentId<oldPhysics::Rigidbody>());
	playerControllerSignature.set(ecs.getComponentId<oldPhysics::BoxCollider>());
	playerControllerSignature.set(ecs.getComponentId<Animator>());
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
	pickupControllerSignature.set(ecs.getComponentId<Animator>());
	ecs.setSystemSignature<PickupController>(pickupControllerSignature);
	//Register Enemy Controller
	ecs.registerComponent<Enemy>();
	shared_ptr<EnemyController> enemyController = ecs.registerSystem<EnemyController>();
	Signature enemyControllerSignature;
	enemyControllerSignature.set(ecs.getComponentId<Transform>());
	enemyControllerSignature.set(ecs.getComponentId<Enemy>());
	enemyControllerSignature.set(ecs.getComponentId<SpriteRenderer>());
	enemyControllerSignature.set(ecs.getComponentId<oldPhysics::Rigidbody>());
	enemyControllerSignature.set(ecs.getComponentId<oldPhysics::BoxCollider>());
	enemyControllerSignature.set(ecs.getComponentId<Animator>());
	ecs.setSystemSignature<EnemyController>(enemyControllerSignature);
	//Register Turret Controller
	ecs.registerComponent<Turret>();
	shared_ptr<TurretController> turretController = ecs.registerSystem<TurretController>();
	Signature turretControllerSignature;
	turretControllerSignature.set(ecs.getComponentId<Transform>());
	turretControllerSignature.set(ecs.getComponentId<Turret>());
	turretControllerSignature.set(ecs.getComponentId<SpriteRenderer>());
	turretControllerSignature.set(ecs.getComponentId<oldPhysics::Rigidbody>());
	turretControllerSignature.set(ecs.getComponentId<oldPhysics::BoxCollider>());
	turretControllerSignature.set(ecs.getComponentId<Animator>());
	ecs.setSystemSignature<TurretController>(turretControllerSignature);


	//Create the player entity
	Entity player = ecs.newEntity();
	Entity playerAttack = ecs.newEntity();
	Transform& playerTransform = ecs.addComponent(player, Transform{ .position = Vector3(110, 200, 1.5), .scale = Vector3(50, 50, 0) });
	ecs.addComponent(player, SpriteRenderer{});
	ecs.addComponent(player, Animator{});
	ecs.addComponent(player, oldPhysics::Rigidbody{});
	ecs.addComponent(player, oldPhysics::BoxCollider{ .scale = Vector2(0.2, 0.61), .offset = Vector2(0, -18) });
	ecs.addComponent(player, Player{ .attackHitbox = playerAttack });

	//Create the player's attack hitbox
	ecs.addComponent(playerAttack, Transform{ .scale = Vector3(10, 20, 0) });
	ecs.addComponent(playerAttack, oldPhysics::Rigidbody{ .kinematic = true });
	ecs.addComponent(playerAttack, oldPhysics::BoxCollider{ .isTrigger = true });

	//Add animation to player
	vector<Animation> anims = AnimationsFromSpritesheet("assets/warriorsheet.png", 8, 4, vector<int>(8 * 4, 100));
	AnimationSystem::AddAnimations(player, anims, vector<string>{"Idle", "Run", "Wallslide", "Jump"});
	vector<Animation> attackAnims = AnimationsFromSpritesheet("assets/warriorattack.png", 4, 4, vector<int>(4 * 4, 100));
	AnimationSystem::AddAnimations(player, attackAnims, vector<string>{"Jump Attack", "Attack 1", "Attack 2", "Attack 3"});


	//Make the waterfall sprites
	Animation waterfallAnims = AnimationsFromSpritesheet("assets/waterfall01.png", 4, 1, vector<int>(4 * 1, 100))[0];
	Entity waterfall = ecs.newEntity();
	ecs.addComponent(waterfall, Transform{ .position = Vector3(1296, -1410, -10), .scale = Vector3(110, 50, 0) });
	ecs.addComponent(waterfall, SpriteRenderer{});
	ecs.addComponent(waterfall, Animator{});
	AnimationSystem::AddAnimation(waterfall, waterfallAnims, "1");
	AnimationSystem::PlayAnimation(waterfall, "1", true);
	Entity waterfall2 = ecs.newEntity();
	ecs.addComponent(waterfall2, Transform{ .position = Vector3(1296, -1507, -15), .scale = Vector3(110, 50, 0) });
	ecs.addComponent(waterfall2, SpriteRenderer{});
	ecs.addComponent(waterfall2, Animator{});
	AnimationSystem::AddAnimation(waterfall2, waterfallAnims, "1");
	AnimationSystem::PlayAnimation(waterfall2, "1", true);
	Entity waterfall3 = ecs.newEntity();
	ecs.addComponent(waterfall3, Transform{ .position = Vector3(1296, -1604, -15), .scale = Vector3(110, 50, 0) });
	ecs.addComponent(waterfall3, SpriteRenderer{});
	ecs.addComponent(waterfall3, Animator{});
	AnimationSystem::AddAnimation(waterfall3, waterfallAnims, "1");
	AnimationSystem::PlayAnimation(waterfall3, "1", true);

	//Make the static torches
	Animation torchAnims = AnimationsFromSpritesheet("assets/torch.png", 9, 1, vector<int>(9 * 1, 40))[0];
	Entity torch = ecs.newEntity();
	ecs.addComponent(torch, Transform{ .position = Vector3(1480, -2036, -10), .scale = Vector3(15, 35, 0) });
	ecs.addComponent(torch, SpriteRenderer{});
	ecs.addComponent(torch, Animator{});
	AnimationSystem::AddAnimation(torch, torchAnims, "main");
	AnimationSystem::PlayAnimation(torch, "main", true);
	Entity torch1 = ecs.newEntity();
	ecs.addComponent(torch1, Transform{ .position = Vector3(850, -2336, -10), .scale = Vector3(15, 35, 0) });
	ecs.addComponent(torch1, SpriteRenderer{});
	ecs.addComponent(torch1, Animator{});
	AnimationSystem::AddAnimation(torch1, torchAnims, "main");
	AnimationSystem::PlayAnimation(torch1, "main", true);


	//Load the tilemap
	Tilemap map(&cam);
	map.loadMap("assets/level01.tmx");

	//Set the gravity and tilemap collider
	engine.physicsSystem->gravity = Vector2(0, -10000);
	engine.spriteRenderSystem->SetTilemap(&map);
	//Background color
	SpriteRenderSystem::SetBackgroundColor(133 / 2, 117 / 2, 87 / 2);


	//Sounds
	//Load and play the music
	MusicBuffer myMusic("assets/forest.wav");
	myMusic.SetVolume(0.5f);
	myMusic.Play();

	//Create the speakers and set their distance models
	SoundSource waterSpeaker;
	SoundSource torchSpeaker;
	SoundSource torchSpeaker2;
	SoundSource playerSpeaker;
	SoundSource pickSpeaker;
	SoundSource magicSpeaker;
	waterSpeaker.setLinearDistanceClamped(1.0f, 150.f, 700.f, 1.f);
	torchSpeaker.setLinearDistanceClamped(0.5f, 50.f, 500.f, 1.f);
	torchSpeaker2.setLinearDistanceClamped(0.5f, 50.f, 500.f, 1.f);
	playerSpeaker.setLinearDistanceClamped(1.f, 50.f, 500.f, 1.f);
	magicSpeaker.setLinearDistanceClamped(1.f, 50.f, 800.f, 1.f);

	//Make all the strawberries
	pickupController->CreatePickup(1880, -1100);
	pickupController->CreatePickup(1010, -680);
	pickupController->CreatePickup(720, -1510);
	pickupController->CreatePickup(1860, -1765);
	pickupController->CreatePickup(1890, -2620);

	//Make all the mage enemies
	turretController->player = player;
	turretController->CreateTurret(1110, -430);
	turretController->CreateTurret(600, -1520);
	turretController->CreateTurret(580, -1100);
	turretController->CreateTurret(730, -2285);
	turretController->CreateTurret(490, -2640);
	turretController->CreateTurret(1590, -2360);
	turretController->CreateTurret(1790, -2640);

	//Load all the sounds
	uint32_t waterSound = SoundBuffer::getFile()->addSoundEffect("assets/stream.wav");
	uint32_t torchSound = SoundBuffer::getFile()->addSoundEffect("assets/fire.wav");
	uint32_t pickSound = SoundBuffer::getFile()->addSoundEffect("assets/strawberry_touch.wav");
	uint32_t jumpSound = SoundBuffer::getFile()->addSoundEffect("assets/jump.wav");
	uint32_t attackSound = SoundBuffer::getFile()->addSoundEffect("assets/swing.wav");
	uint32_t magicSound = SoundBuffer::getFile()->addSoundEffect("assets/Magic.wav");

	//Set all static sounds to always play
	waterSpeaker.Play(waterSound);
	waterSpeaker.SetLooping(1);
	torchSpeaker.Play(torchSound);
	torchSpeaker.SetLooping(1);
	torchSpeaker2.Play(torchSound);
	torchSpeaker2.SetLooping(1);
	//Set all static sound locations
	engine.soundDevice->SetSourceLocation(waterSpeaker, 1296, -1556, -35);
	engine.soundDevice->SetSourceLocation(torchSpeaker, 1480, -2036, 1);
	engine.soundDevice->SetSourceLocation(torchSpeaker2, 850, -2336, 1);

	//Game Loop
	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		//Update custom systems
		playerController->Update(window, engine.deltaTime);
		pickupController->Update(player, engine.programTime);
		turretController->Update(engine.deltaTime);

		//Update all engine systems
		engine.Update(&cam);

		//Keep the camera in bounds of the tilemap
		float camPosX = clamp(playerTransform.position.x, map.position.x + cam.width / 2, map.position.x + map.bounds.width - cam.width / 2);
		float camPosY = clamp(playerTransform.position.y, map.position.y - map.bounds.height + cam.height / 2, map.position.y - cam.height / 2);
		cam.SetPosition(Vector3(camPosX, camPosY, 100));


		//Sounds unfortunately must be done in main for now
		{
			myMusic.updateBufferStream();
			engine.soundDevice->SetOrientation(0.f, 1.f, 0.f, 0.f, 0.f, 1.f);
			engine.soundDevice->SetLocation(playerTransform.position.x, playerTransform.position.y, 0);

			//Set player speaker location
			engine.soundDevice->SetSourceLocation(playerSpeaker, playerTransform.position.x, playerTransform.position.y, 1);

			//Player jump and sword
			for (Entity entity : playerController->entities)
			{
				Player& player = ecs.getComponent<Player>(entity);

				if (player.attackSound == 1)
				{

					playerSpeaker.Play(attackSound);
					player.attackSound = 0;
				}
				if (player.jumpSound == 1)
				{
					if (!playerSpeaker.isPlaying())
						playerSpeaker.Play(jumpSound);
					player.jumpSound = 0;
				}
			};

			//Pickup collect sound
			for (Entity entity : pickupController->entities)
			{
				Pickup& pickup = ecs.getComponent<Pickup>(entity);

				if (pickup.sound == 1)
				{
					if (!pickSpeaker.isPlaying())
						pickSpeaker.Play(pickSound);
					pickup.sound = 0;
				}
			};

			//Play the magic sound from nearest enemy
			if (turretController->playSound)
			{
				//Find the closest enemy
				Entity closestEnemy = 0;
				for (Entity entity : turretController->entities)
				{
					if (!ecs.entityExists(closestEnemy))
					{
						closestEnemy = entity;
						continue;
					}

					if (TransformSystem::Distance(player, entity) < TransformSystem::Distance(player, closestEnemy))
						closestEnemy = entity;
				};

				Transform closestEnemyTransform = ecs.getComponent<Transform>(closestEnemy);

				engine.soundDevice->SetSourceLocation(magicSpeaker, closestEnemyTransform.position.x, closestEnemyTransform.position.y, 0);

				turretController->playSound = false;
				if (!magicSpeaker.isPlaying())
					magicSpeaker.Play(magicSound);
			}
		}

		//OpenGL stuff, goes very last
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}