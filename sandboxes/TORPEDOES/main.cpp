#define ECS_ENABLE_CHECKS
#include "PlayerController.h"
#include "engine/Input.h"  
#include "MenuSystem.h"	
#include "GameCamera.h"
#include "engine/SoundComponent.h"
#include "Pickups.h"
#include "engine/Random.h"
#include <Windows.h>

using namespace engine;

int checkPointNumber = 0;
int currentLevel = 0;

static void CreateCheckpoint(Vector3 position, Vector3 rotation, Vector3 scale, engine::Model* checkPointModel, float hitboxrotation, bool finishLine = false)
{
	engine::ecs::Entity checkpoint = engine::ecs::NewEntity();

	engine::ecs::AddComponent(checkpoint, engine::Transform{ .position = position, .rotation = rotation, .scale = scale });
	engine::ecs::AddComponent(checkpoint, engine::ModelRenderer{ .model = checkPointModel });
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

	checkpointEntities.push_back(checkpoint);
};

static void CreateCrowd(Vector3 pos, engine::Animation& anim)
{
	engine::ecs::Entity crowd = engine::ecs::NewEntity();
	engine::ecs::AddComponent(crowd, engine::Transform{ .position = pos, .scale = Vector3(100, 30, 0) });
	engine::ecs::AddComponent(crowd, engine::SpriteRenderer{});
	// C++ random: ((float)rand() / (RAND_MAX)) + 1
	engine::ecs::AddComponent(crowd, engine::Animator{ .playbackSpeed = Random(0.6f, 2.0f) });
	engine::AnimationSystem::AddAnimation(crowd, anim, "CrowdCheer");
	engine::AnimationSystem::PlayAnimation(crowd, "CrowdCheer", true);
}

static void CreateSmallCrowd(Vector3 pos, engine::Animation& anim)
{
	engine::ecs::Entity crowd = engine::ecs::NewEntity();
	engine::ecs::AddComponent(crowd, engine::Transform{ .position = pos, .scale = Vector3(35, 30,0) });
	engine::ecs::AddComponent(crowd, engine::SpriteRenderer{});
	engine::ecs::AddComponent(crowd, engine::Animator{ .playbackSpeed = Random(0.6f, 2.0f) });
	engine::AnimationSystem::AddAnimation(crowd, anim, "CrowdCheer");
	engine::AnimationSystem::PlayAnimation(crowd, "CrowdCheer", true);
}

//Play the countdown timer and freeze players untill it is done
static void PlayCountdown(Vector3 pos)
{
	engine::ecs::Entity countdown = engine::ecs::NewEntity();
	engine::ecs::AddComponent(countdown, engine::Transform{ .position = pos, .scale = Vector3(60, 100, 100) });
	engine::ecs::AddComponent(countdown, engine::SpriteRenderer{});
	engine::ecs::AddComponent(countdown, engine::Animator{ .onAnimationEnd = engine::ecs::DestroyEntity });
	engine::AnimationSystem::AddAnimation(countdown, resources::countdownAnim, "CountDown");
	engine::AnimationSystem::PlayAnimation(countdown, "CountDown", false);
	engine::ecs::GetSystem<PlayerController>()->countdownTimer = 3;
	Audio* CountdownSound = engine::AddAudio("Gameplay", "audio/CountdownSound.wav", false, 0.2f, DistanceModel::LINEAR, 5000.0f, 1.0f, 0.0f);
	engine::ecs::AddComponent(countdown, engine::SoundComponent{ .Sounds =
	{
		{"Countdown", CountdownSound}
	} , .maxDistance = 2000, .referenceDistance = 1, .rolloffFactor = 0 });
	CountdownSound->play();
}

std::vector<Vector3> cheeringSoundPos;

void OnSoundComponentDestroyed(ecs::Entity e, engine::SoundComponent c)
{
	for (auto sound : c.Sounds)
	{
		sound.second->stop();
	}
}

void SetupCheeringSounds(const std::vector<Vector3>& positions)
{
	for (const auto& pos : positions) {

		engine::ecs::Entity cheerEntity = engine::ecs::NewEntity();
		engine::ecs::AddComponent(cheerEntity, engine::Transform{ .position = pos });

		Audio* cheerSound = engine::AddAudio("Gameplay", "audio/cheering.wav", true, 0.1f, DistanceModel::LINEAR);


		cheerSound->setAbsoluteDirection(pos);

		// Randomize start and stop delays
		float startDelay = Random(0.0f, 500.0f); // Start delay between 0-50ms
		//float stopDelay = startDelay + Random(1000.0f, 5000.0f); // Stop after 1-5 seconds

		cheerSound->setStartTimeMilliseconds(startDelay);
		//cheerSound->setStopTimeMilliseconds(stopDelay);

		engine::ecs::AddComponent(cheerEntity, engine::SoundComponent{ .Sounds =
			{
				{"CrowdCheer", cheerSound}
			} , .maxDistance = 700, .referenceDistance = 200, .rolloffFactor = 1 });
		cheerSound->play();
	}
}

void SetupGameMusic()
{
	engine::ecs::Entity musicEntity = engine::ecs::NewEntity();
	engine::ecs::AddComponent(musicEntity, engine::Transform{});
	Audio* gameMusic = engine::AddAudio("Music", "audio/TheStruggleLoop1.wav", true, 0.05f, DistanceModel::NONE);
	
	engine::ecs::AddComponent(musicEntity, engine::SoundComponent{ .Sounds =
			{
				{"Music", gameMusic}
			} , .maxDistance = 200000, .referenceDistance = 200000, .rolloffFactor = 0 });
	
	gameMusic->play();
}

// Create everything for level 1
static void LoadLevel1(engine::Camera* cam)
{
	currentLevel = 1;
	engine::collisionSystem->cam = cam;

	// Set this level's tilemap
	engine::spriteRenderSystem->SetTilemap(resources::level1Map);
	engine::collisionSystem->SetTilemap(resources::level1Map);
	engine::PhysicsSystem::SetTileProperty(1, engine::TileProperty{ true });

	////Make all the checkpoints manually
	checkPointNumber = 0;
	CreateCheckpoint(Vector3(1925.000000, -895.000000, 100.000000), Vector3(27.500000, 27.500000, 10.000000), Vector3(18.f), resources::models["Prop_Buoy.obj"], 110.f);
	CreateCheckpoint(Vector3(2590.000000, -1475.000000, 100.000000), Vector3(30.000000, 37.500000, 5.000000), Vector3(18.f), resources::models["Prop_Buoy.obj"], 110.f);
	CreateCheckpoint(Vector3(3590.000000, -1425.000000, 100.000000), Vector3(30.000000, 12.500000, -5.000000), Vector3(17.5f), resources::models["Prop_Buoy.obj"], 95.f);
	CreateCheckpoint(Vector3(3280.000000, -640.000000, 100.000000), Vector3(35.000000, 77.500000, 17.500000), Vector3(17.5f), resources::models["Prop_Buoy.obj"], 160.f);
	CreateCheckpoint(Vector3(1770.000000, -1585.000000, 100.000000), Vector3(50.000000, -62.500000, 0.000000), Vector3(18.5f), resources::models["Prop_Buoy.obj"], 20.f);
	CreateCheckpoint(Vector3(820.000000, -1610.000000, 100.000000), Vector3(47.500000, -77.500000, -5.000000), Vector3(20.5f), resources::models["Prop_Buoy.obj"], 10.f);
	CreateCheckpoint(Vector3(545.000000, -900.000000, 100.000000), Vector3(35.000000, -22.500000, 0.000000), Vector3(17.5f), resources::models["Prop_Buoy.obj"], 70.f);
	CreateCheckpoint(Vector3(1475.000000, -460.000000, 70.000000), Vector3(-25.000000, -90.000000, -90.000000), Vector3(24.f), resources::models["Prop_Goal.obj"], 360.f, true);

	//Pickups
	//crossroad
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2200.000000, -1075.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2200.000000, -1145.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2200.000000, -1230.000000, 0.300000));

	//center slalom
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3270.000000, -1125.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3270.000000, -1045.000000, 0.300000));

	//second last corner
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(700.000000, -1710.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(615.000000, -1645.000000, 0.300000));


	//Make the crowds manually
	// Start/Finish line crowd
	CreateCrowd({ 1090, -665, 167 }, resources::crowdAnims1);	// First row, 1. crowd
	CreateCrowd({ 1285, -665, 167 }, resources::crowdAnims1);	// First row, 2. crowd
	CreateCrowd({ 1418, -665, 167 }, resources::crowdAnims1);	// First row 3. crowd
	CreateCrowd({ 1128, -655, 166 }, resources::crowdAnims1);	// Second row, 1. crowd
	CreateCrowd({ 1328, -655, 166 }, resources::crowdAnims1);	// Second row, 2. crowd
	// ********************
	// Second crowd
	CreateCrowd({ 2940, -410, 167 }, resources::crowdAnims1);	// First row, 1. crowd
	CreateCrowd({ 3140, -410, 167 }, resources::crowdAnims1);	// First row, 2. crowd
	CreateCrowd({ 3200, -410, 167 }, resources::crowdAnims1);	// First row, 3. crowd
	CreateCrowd({ 2980, -400, 166 }, resources::crowdAnims1);	// Second row, 1. crowd
	CreateCrowd({ 3175, -400, 166 }, resources::crowdAnims1);	// Second row, 1. crowd
	// ********************
	// Small platform crowd
	CreateSmallCrowd({ 3265, -795,167 }, resources::crowdAnims2);	// 1. crowd
	CreateSmallCrowd({ 3325, -780,166 }, resources::crowdAnims2);	// 2. crowd
	CreateSmallCrowd({ 3290, -790,166.5 }, resources::crowdAnims2);	// 3. crowd
	// ********************

	//Adding cheering audio to crowd locations
	cheeringSoundPos = {
		{ 1100, -665, 0 },
		{ 1400, -665, 0 },
		{ 3140, -410, 0 },
		{ 3325, -780, 0 }
	};
	SetupCheeringSounds(cheeringSoundPos);

	PlayCountdown(Vector3(1235.0f, -310.0f, 200.0f));
	PlayerController::lapCount = 3;
	//Create the players
	engine::ecs::GetSystem<PlayerController>()->CreatePlayers(playerShips, Vector2(1225.0f, -400.0f));

	SetupGameMusic();
}

// Create everything for level 2
static void LoadLevel2(engine::Camera* cam)
{
	currentLevel = 2;
	engine::collisionSystem->cam = cam;

	// Set this level's tilemap
	engine::spriteRenderSystem->SetTilemap(resources::level2Map);
	engine::collisionSystem->SetTilemap(resources::level2Map);
	engine::PhysicsSystem::SetTileProperty(1, engine::TileProperty{ true });

	// Make all the checkpoint's manually
	checkPointNumber = 0;
	CreateCheckpoint(Vector3(1455.000000, -995.000000, 100.000000), Vector3(35.000000, -15.000000, -20.000000), Vector3(17), resources::models["Prop_Buoy.obj"], 65.0f);				// First checkpoint
	CreateCheckpoint(Vector3(2430.000000, -1630.000000, 100.000000), Vector3(7.500000, 92.500000, 90.000000), Vector3(17), resources::models["Prop_Buoy_Vertical.obj"], 0.0f);			// Second checkpoint
	CreateCheckpoint(Vector3(3595.000000, -1095.000000, 100.000000), Vector3(40.000000, 0.000000, 47.500000), Vector3(17), resources::models["Prop_Buoy_Vertical.obj"], 130.0f);		// Third checkpoint
	CreateCheckpoint(Vector3(2715.000000, -1130.000000, 100.000000), Vector3(45.000000, 0.000000, 0.000000), Vector3(17), resources::models["Prop_Buoy.obj"], 90.0f);					// Fourth checkpoint
	CreateCheckpoint(Vector3(2415.000000, -500.000000, 100.000000), Vector3(25.000000, -17.500000, -30.000000), Vector3(17), resources::models["Prop_Buoy.obj"], 55.0f);				// Fifth checkpoint
	CreateCheckpoint(Vector3(1505.000000, -335.000000, 100.000000), Vector3(52.500000, -32.500000, -5.000000), Vector3(17), resources::models["Prop_Buoy.obj"], 60.0f);					// Sixth checkpoint
	CreateCheckpoint(Vector3(600.000000, -665.000000, 100.000000), Vector3(40.000000, -25.000000, -17.500000), Vector3(17), resources::models["Prop_Buoy.obj"], 60.0f);					// Seventh checkpoint
	CreateCheckpoint(Vector3(530.000000, -1675.000000, 100.000000), Vector3(45.000000, 32.500000, 47.500000), Vector3(17), resources::models["Prop_Buoy_Vertical.obj"], 145.0f);		// Eight checkpoint
	CreateCheckpoint(Vector3(1245.000000, -1680.000000, 100.000000), Vector3(75.000000, -90.000000, 7.500000), Vector3(22), resources::models["Prop_Goal.obj"], 0.0f, true);	// Finish line

	//Pickups
	//first loop
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1710.000000, -995.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1660.000000, -995.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1610.000000, -995.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1560.000000, -995.000000, 0.300000));

	//second loop triple
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3365.000000, -1060.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3365.000000, -1025.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3365.000000, -985.000000, 0.300000));

	//second loop, between stones
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2950.000000, -1585.000000, 0.300000));

	//topright hairpin
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3355.000000, -440.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3355.000000, -400.000000, 0.300000));

	//before last loop
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2210.000000, -620.000000, 0.350000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2160.000000, -620.000000, 0.350000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2110.000000, -620.000000, 0.350000));

	//before chicane
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(425.000000, -855.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(480.000000, -855.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(540.000000, -855.000000, 0.300000));

	// Make the crowds manually
	// Start/Finish line crowd
	CreateCrowd({ 1030.000000, -1850.000000, 165 }, resources::crowdAnims1);	// First row, first crowd
	CreateCrowd({ 1230.000000, -1850.000000, 165 }, resources::crowdAnims1);	// First row, second crowd
	CreateCrowd({ 1430.000000, -1850.000000, 165 }, resources::crowdAnims1);	// First row, third crowd
	CreateCrowd({ 1010.000000, -1865.000000, 166 }, resources::crowdAnims1);	// Second row, first crowd
	CreateCrowd({ 1210.000000, -1865.000000, 166 }, resources::crowdAnims1);	// Second row, second crowd
	CreateCrowd({ 1410.000000, -1865.000000, 166 }, resources::crowdAnims1);	// Second row, third crowd	
	// ********************
	// Cave crowd
	CreateCrowd({ 3165.000000, -480.000000, 165 }, resources::crowdAnims1);	// First row, first crowd
	// ********************
	// Overhang crowd
	CreateCrowd({ 1210.000000, -850.000000, 165 }, resources::crowdAnims1); // First row, first crowd
	CreateCrowd({ 1410.000000, -850.000000, 165 }, resources::crowdAnims1); // First row, second crowd
	CreateCrowd({ 1610.000000, -850.000000, 165 }, resources::crowdAnims1); // First row, third crowd
	CreateCrowd({ 1810.000000, -850.000000, 165 }, resources::crowdAnims1); // First row, fourth crowd
	CreateCrowd({ 1250.000000, -860.000000, 166 }, resources::crowdAnims1); // Second row, first crowd
	CreateCrowd({ 1450.000000, -860.000000, 166 }, resources::crowdAnims1); // Second row, second crowd
	CreateCrowd({ 1650.000000, -860.000000, 166 }, resources::crowdAnims1); // Second row, third crowd
	CreateCrowd({ 1850.000000, -860.000000, 166 }, resources::crowdAnims1); // Second row, fourth crowd
	// ********************

	//Adding cheering audio to crowd locations
	cheeringSoundPos = {
		{ 1230, -1850, 0 },
		{ 3165, -480, 0 },
		{ 1210, -850, 0 },
		{ 1830, -860, 0 }
	};
	SetupCheeringSounds(cheeringSoundPos);

	PlayCountdown(Vector3(1150.0f, -1500.0f, 0.0f));
	PlayerController::lapCount = 3;

	engine::ecs::GetSystem<PlayerController>()->CreatePlayers(playerShips, Vector2(1160.0f, -1600.0f));
	SetupGameMusic();
}

//Create everything for level 3
static void LoadLevel3(engine::Camera* cam)
{
	currentLevel = 3;
	engine::collisionSystem->cam = cam;

	//Set this level's tilemap
	engine::spriteRenderSystem->SetTilemap(resources::level3Map);
	engine::collisionSystem->SetTilemap(resources::level3Map);
	engine::PhysicsSystem::SetTileProperty(1, engine::TileProperty{ true });

	//Make all the checkpoints manually
	checkPointNumber = 0;
	CreateCheckpoint(Vector3(3015.000000, -760.000000, 100.000000), Vector3(27.500000, 47.500000, 7.500000), Vector3(19), resources::models["Prop_Buoy.obj"], 37.5f + 90.f);
	CreateCheckpoint(Vector3(2645.000000, -975.000000, 100.000000), Vector3(27.500000, -40.000000, -7.500000), Vector3(19), resources::models["Prop_Buoy.obj"], -20.0f + 90.f);
	CreateCheckpoint(Vector3(2140.000000, -635.000000, 100.000000), Vector3(27.500000, 0.000000, 12.500000), Vector3(15.f), resources::models["Prop_Buoy.obj"], -2.5f + 90.f);
	CreateCheckpoint(Vector3(875.000000, -465.000000, 100.000000), Vector3(37.500000, -15.000000, 0.000000), Vector3(12.5f), resources::models["Prop_Buoy.obj"], 15.0f + 90.f);
	CreateCheckpoint(Vector3(1170.000000, -1250.000000, 100.000000), Vector3(37.500000, 0.000000, 0.000000), Vector3(13), resources::models["Prop_Buoy.obj"], 13.0f + 80.f);
	CreateCheckpoint(Vector3(2555.000000, -1600.000000, 100.000000), Vector3(-17.500000, -87.500000, -90.000000), Vector3(20.5f), resources::models["Prop_Goal.obj"], 360.f, true);

	//Pickups
	//first corner
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3215.000000, -1280.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3265.000000, -1280.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3315.000000, -1280.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3375.000000, -1280.000000, 0.300000));

	//first hairpin
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2860.000000, -425.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2810.000000, -425.000000, 0.300000));


	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1825.000000, -895.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1880.000000, -825.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(940.000000, -680.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(985.000000, -315.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1060.000000, -315.000000, 0.300000));


	//Make the crowds manually
	// Start/Finish line crowd
	CreateCrowd({ 2300, -1410, 166 }, resources::crowdAnims1);	// First row, first crowd
	CreateCrowd({ 2431, -1410, 166 }, resources::crowdAnims1);	// First row, second crowd
	CreateCrowd({ 2315, -1395, 165 }, resources::crowdAnims1);	// Second row, first crowd
	CreateCrowd({ 2380, -1395, 165 }, resources::crowdAnims1);	// Second row, second crowd
	// ********************
	// First checkpoint crowd
	CreateSmallCrowd({ 3119, -605, 165 }, resources::crowdAnims2);		// Right crowd
	CreateSmallCrowd({ 2811, -470, 166 }, resources::crowdAnims2);		// Left, First row, 1. crowd
	CreateSmallCrowd({ 2845, -500, 167 }, resources::crowdAnims2);		// Left, First row, 2. crowd
	CreateSmallCrowd({ 2810, -530, 168 }, resources::crowdAnims2);		// Left, First row, 3. crowd
	CreateSmallCrowd({ 2824.5, -560, 169 }, resources::crowdAnims2);	// Left, First row, 4. crowd
	CreateSmallCrowd({ 2820.5, -590, 170 }, resources::crowdAnims2);	// Left, First row, 5. crowd
	CreateSmallCrowd({ 2835, -620, 171 }, resources::crowdAnims2);		// Left, First row, 6. crowd
	CreateSmallCrowd({ 2835.5, -650, 172 }, resources::crowdAnims2);		// Left, First row, 7. crowd
	CreateSmallCrowd({ 2811.5, -680, 173 }, resources::crowdAnims2);	// Left, First row, 8. crowd
	CreateSmallCrowd({ 2816, -710, 174 }, resources::crowdAnims2);		// Left, First row, 9. crowd
	CreateSmallCrowd({ 2810.6, -740, 175 }, resources::crowdAnims2);	// Left, First row, 10. crowd
	CreateSmallCrowd({ 2838, -770, 176 }, resources::crowdAnims2);		// Left, First row, 11. crowd
	CreateSmallCrowd({ 2830, -800, 177 }, resources::crowdAnims2);		// Left, First row, 12. crowd
	CreateSmallCrowd({ 2816, -835, 178 }, resources::crowdAnims2);		// Left, First row, 13. crowd
	CreateSmallCrowd({ 2820.5, -510, 167.5 }, resources::crowdAnims2);	// Left, Second row, 1. crowd
	CreateSmallCrowd({ 2845.5, -540, 168.5 }, resources::crowdAnims2);	// Left, Second row, 2. crowd
	CreateSmallCrowd({ 2810.5, -570, 169.5 }, resources::crowdAnims2);	// Left, Second row, 3. crowd
	CreateSmallCrowd({ 2839, -600, 170.5 }, resources::crowdAnims2);	// Left, Second row, 4. crowd
	CreateSmallCrowd({ 2810.5, -630, 171.5 }, resources::crowdAnims2);	// Left, Second row, 5. crowd
	CreateSmallCrowd({ 2840.5, -660, 172.5 }, resources::crowdAnims2);	// Left, Second row, 6. crowd
	CreateSmallCrowd({ 2815, -690, 173.5 }, resources::crowdAnims2);	// Left, Second row, 7. crowd
	CreateSmallCrowd({ 2835, -720, 174.5 }, resources::crowdAnims2);	// Left, Second row, 8. crowd
	CreateSmallCrowd({ 2845, -750, 175.5 }, resources::crowdAnims2);	// Left, Second row, 9. crowd
	CreateSmallCrowd({ 2840, -780, 176.5 }, resources::crowdAnims2);	// Left, Second row, 10. crowd
	CreateSmallCrowd({ 2810, -810, 177.5 }, resources::crowdAnims2);	// Left, Second row, 11. crowd
	// ********************
	// Middle audience
	CreateSmallCrowd({ 2014, -720, 166 }, resources::crowdAnims2);
	CreateSmallCrowd({ 2014, -750, 167 }, resources::crowdAnims2);
	CreateSmallCrowd({ 2014, -780, 168 }, resources::crowdAnims2);
	CreateSmallCrowd({ 2014, -810, 169 }, resources::crowdAnims2);
	CreateSmallCrowd({ 2014, -840, 170 }, resources::crowdAnims2);
	CreateSmallCrowd({ 2014, -870, 171 }, resources::crowdAnims2);
	CreateSmallCrowd({ 2014, -900, 172 }, resources::crowdAnims2);
	CreateSmallCrowd({ 2014, -930, 173 }, resources::crowdAnims2);
	CreateSmallCrowd({ 2014, -960, 174 }, resources::crowdAnims2);
	CreateSmallCrowd({ 2014, -990, 175 }, resources::crowdAnims2);
	CreateSmallCrowd({ 2014, -1020, 176 }, resources::crowdAnims2);
	// ********************
	// Dock audience
	CreateCrowd({ 500.000000, -810.000000, 166 }, resources::crowdAnims1);			// 1. crowd from top.
	CreateCrowd({ 480.000000, -825.000000, 167 }, resources::crowdAnims1);			// 2. crowd
	CreateSmallCrowd({ 445.000000, -910.000000, 166 }, resources::crowdAnims2);		// 3. crowd
	CreateSmallCrowd({ 445.000000, -935.000000, 167 }, resources::crowdAnims2);		// 4. crowd
	CreateSmallCrowd({ 445.000000, -1000.000000, 166 }, resources::crowdAnims2);	// 5. crowd
	CreateSmallCrowd({ 445.000000, -1070.000000, 166 }, resources::crowdAnims2);	// 6. crowd
	CreateSmallCrowd({ 445.000000, -1100.000000, 167 }, resources::crowdAnims2);	// 7. crowd
	CreateSmallCrowd({ 445.000000, -1130.000000, 168 }, resources::crowdAnims2);	// 8. crowd
	CreateCrowd({ 890.000000, -1220.000000, 166 }, resources::crowdAnims1);			// 9. crowd
	CreateCrowd({ 870.5, -1235.000000, 167 }, resources::crowdAnims1);				// 10. crowd
	CreateCrowd({ 840, -1245.000000, 168 }, resources::crowdAnims1);				// 11. crowd
	CreateCrowd({ 890, -1265.000000, 169 }, resources::crowdAnims1);				// 12. crowd
	CreateCrowd({ 610.000000, -1220.000000, 166 }, resources::crowdAnims1);
	CreateCrowd({ 560.5, -1235.000000, 167 }, resources::crowdAnims1);
	CreateCrowd({ 650, -1245.000000, 168 }, resources::crowdAnims1);
	CreateCrowd({ 600, -1265.000000, 169 }, resources::crowdAnims1);
	// ********************

	cheeringSoundPos = {
		{ 2350, -1410, 0 },
		{ 3119, -605, 0 },
		{ 2811, -470, 0 },
		{ 2014, -870, 0 },
		{ 480, -825, 0 },
		{ 650, -1245, 0 }
	};
	SetupCheeringSounds(cheeringSoundPos);


	PlayCountdown(Vector3(2480.0f, -1460.0f, 200.0f));
	PlayerController::lapCount = 3;
	//Create the players
	engine::ecs::GetSystem<PlayerController>()->CreatePlayers(playerShips, Vector2(2480.0f, -1520.0f));
	SetupGameMusic();
}

// Create everything for level 4
static void LoadLevel4(engine::Camera* cam)
{
	currentLevel = 4;
	engine::collisionSystem->cam = cam;

	//set this level's tilemap
	engine::spriteRenderSystem->SetTilemap(resources::level4Map);
	engine::collisionSystem->SetTilemap(resources::level4Map);
	engine::PhysicsSystem::SetTileProperty(1, engine::TileProperty{ true });

	// Make all the checkpoints manually
	checkPointNumber = 0;

	CreateCheckpoint(Vector3(2225.000000, -1310.000000, -1660.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(28.f), resources::models["Prop_Buoy.obj"], 90.f);
	CreateCheckpoint(Vector3(580.000000, -560.000000, -1660.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(18.f), resources::models["Prop_Buoy.obj"], 45.f);
	CreateCheckpoint(Vector3(3125.000000, -1375.000000, -1660.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(18.f), resources::models["Prop_Buoy.obj"], 90.f);
	CreateCheckpoint(Vector3(4725.000000, -510.000000, -1660.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(18.f), resources::models["Prop_Buoy.obj"], 130.f);
	CreateCheckpoint(Vector3(4770.000000, -1735.000000, -1660.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(18.f), resources::models["Prop_Buoy.obj"], 0.f);
	CreateCheckpoint(Vector3(6285.000000, -1580.000000, -1660.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(18.f), resources::models["Prop_Buoy.obj"], 0.f);
	CreateCheckpoint(Vector3(6480.000000, -340.000000, -1660.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(18.f), resources::models["Prop_Buoy.obj"], 0.f);
	CreateCheckpoint(Vector3(7590.000000, -790.000000, -1660.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(38.f), resources::models["Prop_Buoy.obj"], 45.f);
	CreateCheckpoint(Vector3(7435.000000, -1675.000000, -1660.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(18.f), resources::models["Prop_Buoy.obj"], 0.f);
	CreateCheckpoint(Vector3(8775.000000, -575.000000, -1660.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(20.f), resources::models["Prop_Buoy.obj"], 0.f);
	CreateCheckpoint(Vector3(9130.000000, -1430.000000, -1660.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(17.f), resources::models["Prop_Buoy.obj"], 0.f);
	CreateCheckpoint(Vector3(10765.000000, -315.000000, -1660.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(17.f), resources::models["Prop_Buoy.obj"], 0.f);
	CreateCheckpoint(Vector3(11115.000000, -1695.000000, -1660.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(17.f), resources::models["Prop_Buoy.obj"], 0.f);
	CreateCheckpoint(Vector3(11850.000000, -915.000000, -1660.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(17.f), resources::models["Prop_Buoy.obj"], 0.f);
	CreateCheckpoint(Vector3(12250.000000, -370.000000, -1660.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(17.f), resources::models["Prop_Buoy.obj"], 0.f);
	CreateCheckpoint(Vector3(12760.000000, -1575.000000, -1660.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(17.f), resources::models["Prop_Buoy.obj"], 0.f);
	CreateCheckpoint(Vector3(13695.000000, -340.000000, -1660.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(19.f), resources::models["Prop_Buoy.obj"], 0.f);
	CreateCheckpoint(Vector3(14260.000000, -1365.000000, -1660.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(37.f), resources::models["Prop_Buoy.obj"], 45.f);

	CreateCheckpoint(Vector3(15760.000000, -925.000000, 100.000000), Vector3(-12.500000, -90.000000, -87.500000), Vector3(43.0f), resources::models["Prop_Goal.obj"], 360.f, true);

	//Collectibles
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1190.000000, -1325.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1265.000000, -1325.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1355.000000, -1325.000000, 0.300000));

	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2390.000000, -490.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2485.000000, -515.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2540.000000, -570.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2610.000000, -645.000000, 0.300000));

	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5055.000000, -730.000000, 0.300000));

	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(4605.000000, -1695.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(4685.000000, -1695.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(4765.000000, -1695.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(4825.000000, -1695.000000, 0.300000));

	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5715.000000, -1180.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5715.000000, -1125.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5715.000000, -1070.000000, 0.300000));

	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5740.000000, -555.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5740.000000, -500.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5740.000000, -435.000000, 0.300000));

	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(7455.000000, -825.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(7325.000000, -900.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(7155.000000, -965.000000, 0.300000));

	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(8560.000000, -670.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(8645.000000, -670.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(8720.000000, -670.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(8790.000000, -670.000000, 0.300000));

	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(10270.000000, -590.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(10240.000000, -465.000000, 0.300000));

	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(10095.000000, -570.000000, 0.300000));

	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(10590.000000, -1505.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(10665.000000, -1505.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(10745.000000, -1505.000000, 0.300000));

	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(13615.000000, -265.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(13685.000000, -265.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(13775.000000, -265.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(13870.000000, -265.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(14000.000000, -1305.000000, 0.300000));

	//Make the crowds manually
	// Starting point crowd
	CreateCrowd({ 1255.000000, -1445.000000, 167 }, resources::crowdAnims1);
	CreateCrowd({ 1277.000000, -1430.000000, 166 }, resources::crowdAnims1);
	CreateCrowd({ 1250.000000, -1410.000000, 165 }, resources::crowdAnims1);
	CreateCrowd({ 1260.000000, -1390.000000, 164 }, resources::crowdAnims1);

	// Small plateau
	CreateCrowd({ 3350.000000, -1350.000000, 167 }, resources::crowdAnims1);
	CreateCrowd({ 3340.000000, -1320.000000, 166 }, resources::crowdAnims1);
	CreateCrowd({ 3365.000000, -1300.000000, 165 }, resources::crowdAnims1);
	CreateCrowd({ 3345.000000, -1270.000000, 164.5 }, resources::crowdAnims1);
	CreateCrowd({ 3325.000000, -1250.000000, 164 }, resources::crowdAnims1);
	CreateCrowd({ 3375.000000, -1210.000000, 163 }, resources::crowdAnims1);
	CreateCrowd({ 3350.000000, -1185.000000, 162 }, resources::crowdAnims1);

	// U-turn
	CreateCrowd({ 4655.000000, -1625.000000, 167 }, resources::crowdAnims1);
	CreateCrowd({ 4765.000000, -1610.000000, 165 }, resources::crowdAnims1);
	CreateCrowd({ 4885.000000, -1620.000000, 166 }, resources::crowdAnims1);

	//Long pier
	CreateSmallCrowd({ 6870, -930, 160 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6845, -950, 161 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6805, -960, 162 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6788, -970, 163 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6862, -980, 164 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6855, -1000, 165 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6810, -1010, 166 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6799, -1025, 167 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6847, -1030, 168 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6803, -1040, 169 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6825, -1050, 170 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6811, -1060, 171 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6833, -1070, 172 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6787, -1085, 173 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6868, -1095, 174 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6842, -1105, 175 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6850, -1115, 176 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6792, -1125, 177 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6830, -1135, 178 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6860, -1145, 179 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6800, -1155, 180 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6789, -1165, 181 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6815, -1175, 182 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6855, -1185, 183 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6820, -1195, 184 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6849, -1205, 185 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6807, -1215, 186 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6785, -1225, 187 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6875, -1235, 188 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6833, -1245, 189 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6810, -1255, 190 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6842, -1265, 191 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6809, -1275, 192 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6858, -1285, 193 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6830, -1295, 194 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6797, -1305, 195 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6825, -1315, 196 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6786, -1325, 197 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6872, -1335, 198 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6865, -1345, 199 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6790, -1355, 200 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6812, -1365, 201 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6840, -1375, 202 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6789, -1385, 203 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6821, -1395, 204 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6862, -1405, 205 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6807, -1415, 206 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6846, -1425, 207 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6788, -1435, 208 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6873, -1445, 209 }, resources::crowdAnims2);
	CreateSmallCrowd({ 6810, -1455, 210 }, resources::crowdAnims2);

	//shortcut
	CreateCrowd({ 6185.000000, -565.000000, 167 }, resources::crowdAnims1);
	CreateCrowd({ 6200.000000, -540.000000, 165 }, resources::crowdAnims1);

	//Beach
	CreateCrowd({ 9200.000000, -725.000000, 167 }, resources::crowdAnims1);
	CreateCrowd({ 9180.000000, -710.000000, 166 }, resources::crowdAnims1);
	CreateCrowd({ 9205.000000, -698.000000, 164 }, resources::crowdAnims1);

	//Small pier
	CreateSmallCrowd({ 10880.000000, -440.000000, 206 }, resources::crowdAnims2);
	CreateSmallCrowd({ 10900.000000, -434.000000, 205 }, resources::crowdAnims2);
	CreateSmallCrowd({ 10910.000000, -428.000000, 204 }, resources::crowdAnims2);
	CreateSmallCrowd({ 10900.000000, -415.000000, 203 }, resources::crowdAnims2);
	CreateSmallCrowd({ 10890.000000, -400.000000, 202 }, resources::crowdAnims2);

	//Long narrow pier
	CreateCrowd({ 11971.000000, -420.000000, 166 }, resources::crowdAnims1);
	CreateCrowd({ 11955.000000, -415.000000, 165 }, resources::crowdAnims1);

	//Long pontoon
	CreateSmallCrowd({ 13155, -780, 160 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13135, -790, 161 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13165, -810, 162 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13145, -840, 163 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13155, -860, 164 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13150, -890, 165 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13135, -900, 166 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13175, -940, 167 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13145, -970, 168 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13170, -1000, 169 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13150, -1050, 170 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13130, -1080, 171 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13155, -1100, 172 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13165, -1140, 173 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13140, -1170, 174 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13155, -1200, 175 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13170, -1210, 176 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13155, -1235, 177 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13185, -1265, 178 }, resources::crowdAnims2);
	CreateSmallCrowd({ 13130, -1270, 179 }, resources::crowdAnims2);

	//Finish line
	CreateCrowd({ 15740, -1270, 166 }, resources::crowdAnims1);
	CreateCrowd({ 15760, -1255, 165 }, resources::crowdAnims1);
	CreateCrowd({ 15840, -1275, 167 }, resources::crowdAnims1);
	CreateCrowd({ 15800, -1250, 164 }, resources::crowdAnims1);
	CreateCrowd({ 15780, -1280, 168 }, resources::crowdAnims1);

	cheeringSoundPos = {
	{ 1260.000000, -1390.000000, 0 },
	{ 3345.000000, -1270.000000, 0 },
	{ 4765.000000, -1610.000000, 0 },
	{ 6805, -960, 0 },
	{ 6849, -1205, 0 },
	{ 6788, -1435, 0 },
	{ 6200.000000, -550.000000, 0 },
	{ 9180.000000, -710.000000, 0 },
	{ 10910.000000, -428.000000, 0 },
	{ 11965.000000, -420.000000, 0 },
	{ 13165, -810, 0 },
	{ 13170, -1000, 0 },
	{ 13155, -1235, 0 },
	{ 15800, -1260, 0 }
	};
	SetupCheeringSounds(cheeringSoundPos);

	PlayCountdown(Vector3(1434.0f, -1470.0f, 200.0f));
	PlayerController::lapCount = 1;
	engine::ecs::GetSystem<PlayerController>()->CreatePlayers(playerShips, Vector2(1434.0f, -1520.0f));
	SetupGameMusic();
}

// Create everything for level 5
static void LoadLevel5(engine::Camera* cam)
{
	currentLevel = 5;
	engine::collisionSystem->cam = cam;

	// Set this level's tilemap
	engine::spriteRenderSystem->SetTilemap(resources::level5Map);
	engine::collisionSystem->SetTilemap(resources::level5Map);
	engine::PhysicsSystem::SetTileProperty(1, engine::TileProperty{ true });

	////Make all the checkpoints manually
	checkPointNumber = 0;
	CreateCheckpoint(Vector3(4090.000000, -3175.000000, 166.000000), Vector3(25.000000, 20.000000, 15.000000), Vector3(18.f), resources::models["Prop_Buoy.obj"], 120.f);
	CreateCheckpoint(Vector3(2065.000000, -3590.000000, 166.000000), Vector3(22.500000, 82.500000, 27.500000), Vector3(20.f), resources::models["Prop_Buoy.obj"], 0.f);
	CreateCheckpoint(Vector3(1525.000000, -1880.000000, 166.000000), Vector3(37.500000, -10.000000, -5.000000), Vector3(18.f), resources::models["Prop_Buoy.obj"], 90.f);
	CreateCheckpoint(Vector3(7630.000000, -1255.000000, 166.000000), Vector3(25.000000, -87.500000, -25.000000), Vector3(20.5f), resources::models["Prop_Buoy.obj"], 0.f);
	CreateCheckpoint(Vector3(8320.000000, -2440.000000, 166.000000), Vector3(35.000000, -42.500000, -10.000000), Vector3(21.5f), resources::models["Prop_Buoy.obj"], 225.f);
	CreateCheckpoint(Vector3(8190.000000, -4185.000000, 166.000000), Vector3(35.000000, -57.500000, -22.500000), Vector3(22.f), resources::models["Prop_Buoy.obj"], 210.f);
	CreateCheckpoint(Vector3(5605.000000, -2740.000000, 166.000000), Vector3(22.500000, -47.500000, -20.000000), Vector3(22.f), resources::models["Prop_Buoy.obj"], 45.f);
	CreateCheckpoint(Vector3(1440.000000, -4970.000000, 166.000000), Vector3(22.500000, -132.500000, -20.000000), Vector3(18.f), resources::models["Prop_Buoy.obj"], 150.f);

	CreateCheckpoint(Vector3(3600.000000, -4140.000000, 70.000000), Vector3(-25.000000, -90.000000, -90.000000), Vector3(24.f), resources::models["Prop_Goal.obj"], 360.f, true);

	//Collectibles
	// First boxes
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3590.000000, -2965.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3630.000000, -2907.500000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3670.000000, -2850.000000, 0.300000));

	// First vertical straight
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1550.000000, -3360.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1600.000000, -3310.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1660.000000, -2745.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1745.000000, -2745.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1670.000000, -1415.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1640.000000, -1350.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1605.000000, -1290.000000, 0.300000));

	// Long straight
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3260.000000, -1865.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3260.000000, -1790.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3260.000000, -1715.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3815.000000, -1140.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3815.000000, -1065.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3815.000000, -1000.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(4680.000000, -1400.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(4635.000000, -1335.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(6850.000000, -1320.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(6850.000000, -1250.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(6850.000000, -1175.000000, 0.300000));

	// islands
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(8820.000000, -1135.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(8820.000000, -1195.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(8820.000000, -1255.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(9340.000000, -1740.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(9280.000000, -1740.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(9210.000000, -1740.000000, 0.300000));

	// next section
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(8300.000000, -2745.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(8225.000000, -2745.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(8375.000000, -2745.000000, 0.300000));

	// before small shortcuts
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(8480.000000, -4070.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(8480.000000, -4130.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(8480.000000, -4190.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(8480.000000, -4250.000000, 0.300000));

	// after small shortcuts
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5620.000000, -3060.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5537.500000, -3060.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5455.000000, -3060.000000, 0.300000));

	// wide section next to pontoon
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(6060.000000, -4465.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(6115.000000, -4465.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(6170.000000, -4465.000000, 0.300000));

	// bottom branching paths
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5455.000000, -5200.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5455.000000, -5250.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5455.000000, -5300.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5100.000000, -4860.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(4745.000000, -4925.000000, 0.300000));

	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3630.000000, -5140.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3630.000000, -5065.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3630.000000, -4990.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3630.000000, -5215.000000, 0.300000));

	// before finish line
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1465.000000, -4670.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1415.000000, -4670.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1365.000000, -4670.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1315.000000, -4670.000000, 0.300000));


	//Make the crowds manually
	// Starting point crowd
	CreateCrowd({ 3165.000000, -3935.000000, 168 }, resources::crowdAnims1);
	CreateCrowd({ 3445.000000, -3930.000000, 167 }, resources::crowdAnims1);
	CreateCrowd({ 3350.000000, -3925.000000, 166 }, resources::crowdAnims1);
	CreateCrowd({ 3245.000000, -3910.000000, 165 }, resources::crowdAnims1);
	CreateCrowd({ 3385.000000, -3900.000000, 164 }, resources::crowdAnims1);
	CreateCrowd({ 3180.000000, -3890.000000, 163 }, resources::crowdAnims1);
	CreateCrowd({ 3530.000000, -3885.000000, 162 }, resources::crowdAnims1);
	CreateCrowd({ 3260.000000, -3880.000000, 161 }, resources::crowdAnims1);

	// first island crowd
	CreateCrowd({ 1845.000000, -1755.000000, 168 }, resources::crowdAnims1);
	CreateCrowd({ 1825.000000, -1720.000000, 167 }, resources::crowdAnims1);
	CreateCrowd({ 1790.000000, -1680.000000, 166 }, resources::crowdAnims1);
	CreateCrowd({ 1850.000000, -1665.000000, 165 }, resources::crowdAnims1);

	// large platform crowd
	CreateCrowd({ 5625.000000, -1340.000000, 168 }, resources::crowdAnims1);
	CreateCrowd({ 5810.000000, -1325.000000, 167 }, resources::crowdAnims1);
	CreateCrowd({ 5710.000000, -1299.000000, 166 }, resources::crowdAnims1);
	CreateCrowd({ 5630.000000, -1275.000000, 165 }, resources::crowdAnims1);
	CreateCrowd({ 5800.000000, -1265.000000, 164 }, resources::crowdAnims1);
	CreateCrowd({ 5700.000000, -1255.000000, 163 }, resources::crowdAnims1);

	// islands platform crowd
	CreateCrowd({ 7995.000000, -1395.000000, 168 }, resources::crowdAnims1);
	CreateSmallCrowd({ 8015.000000, -1380.000000, 167 }, resources::crowdAnims2);
	CreateCrowd({ 7990.000000, -1365.000000, 166 }, resources::crowdAnims1);
	CreateSmallCrowd({ 7950.000000, -1355.000000, 165 }, resources::crowdAnims2);

	// first narrow path crowd
	CreateCrowd({ 7590.000000, -4050.000000, 168 }, resources::crowdAnims1);
	CreateCrowd({ 7600.000000, -4035.000000, 167 }, resources::crowdAnims1);
	CreateCrowd({ 7595.000000, -4020.000000, 166 }, resources::crowdAnims1);

	// largest platform crowd
	CreateCrowd({ 5500.000000, -4475.000000, 168 }, resources::crowdAnims1);
	CreateCrowd({ 5615.000000, -4460.000000, 167 }, resources::crowdAnims1);
	CreateCrowd({ 5695.000000, -4440.000000, 166 }, resources::crowdAnims1);
	CreateCrowd({ 5560.000000, -4420.000000, 165 }, resources::crowdAnims1);
	CreateCrowd({ 5515.000000, -4400.000000, 164 }, resources::crowdAnims1);
	CreateCrowd({ 5660.000000, -4385.000000, 163 }, resources::crowdAnims1);
	CreateCrowd({ 5500.000000, -4355.000000, 162 }, resources::crowdAnims1);
	CreateCrowd({ 5690.000000, -4315.000000, 161 }, resources::crowdAnims1);

	// small platform next to big one crowd
	CreateCrowd({ 5265.000000, -4690.000000, 168 }, resources::crowdAnims1);
	CreateCrowd({ 5185.000000, -4675.000000, 167 }, resources::crowdAnims1);
	CreateCrowd({ 5225.000000, -4655.000000, 166 }, resources::crowdAnims1);

	// audio for crowd
	cheeringSoundPos = {
		{ 3245.000000, -3910.000000, 0 },
		{ 1825.000000, -1700.000000, 0 },
		{ 5700.000000, -1300.000000, 0 },
		{ 8015.000000, -1380.000000, 0 },
		{ 7600.000000, -4035.000000, 0 },
		{ 5615.000000, -4400.000000, 0 },
		{ 5185.000000, -4675.000000, 0 }
	};
	SetupCheeringSounds(cheeringSoundPos);

	PlayCountdown(Vector3(3300.0f, -4000.0f, 200.0f));
	PlayerController::lapCount = 2;
	//Create the players
	engine::ecs::GetSystem<PlayerController>()->CreatePlayers(playerShips, Vector2(3390.0f, -4055.0f));
	SetupGameMusic();
}

static void LoadLevel6(engine::Camera* cam)
{
	currentLevel = 6;
	engine::collisionSystem->cam = cam;

	// Set this level's tilemap
	engine::spriteRenderSystem->SetTilemap(resources::level6Map);
	engine::collisionSystem->SetTilemap(resources::level6Map);
	engine::PhysicsSystem::SetTileProperty(1, engine::TileProperty{ true });

	////Make all the checkpoints manually
	checkPointNumber = 0;

	CreateCheckpoint(Vector3(13510.000000, -2225.000000, 166.000000), Vector3(25.000000, 0.000000, 0.000000), Vector3(18.f), resources::models["Prop_Buoy.obj"], 90.f);
	CreateCheckpoint(Vector3(1375.000000, -950.000000, 70.000000), Vector3(-25.000000, -90.000000, -90.000000), Vector3(28.f), resources::models["Prop_Goal.obj"], 360.f, true);

	//Collectibles
	// First boxes
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2900.000000, -1205.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2900.000000, -1262.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(2900.000000, -1320.000000, 0.300000));

	// second boxes
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1545.000000, -2390.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1490.000000, -2410.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1435.000000, -2435.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(1370.000000, -2460.000000, 0.300000));

	// boxes
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3305.000000, -2405.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3365.000000, -2418.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3425.000000, -2432.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(3490.000000, -2445.000000, 0.300000));

	// boxes
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(4945.000000, -1970.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(4990.000000, -2005.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5045.000000, -2050.000000, 0.300000));

	// boxes
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5705.000000, -2310.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5705.000000, -2365.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5705.000000, -2420.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(5705.000000, -2475.000000, 0.300000));

	// boxes
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(10040.000000, -1180.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(10040.000000, -1130.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(10040.000000, -1080.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(10040.000000, -1030.000000, 0.300000));

	// boxes
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(10330.000000, -2400.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(10245.000000, -2400.000000, 0.300000));

	// boxes
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(10785.000000, -1820.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(10785.000000, -1700.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(10785.000000, -1580.000000, 0.300000));

	// boxes
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(12785.000000, -2000.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(12725.000000, -1970.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(12885.000000, -2450.000000, 0.300000));
	engine::ecs::GetSystem<PickupSystem>()->SpawnPickup(Vector3(12810.000000, -2475.000000, 0.300000));



	PlayCountdown(Vector3(1235.0f, -780.0f, 200.0f));
	PlayerController::lapCount = 1;
	//Create the players
	engine::ecs::GetSystem<PlayerController>()->CreatePlayers(playerShips, Vector2(1225.0f, -900.0f));

	SetupGameMusic();
}


//Bind all input events here
static void SetupInput()
{
	float AnalogPositiveMinDeadZone = 0;
	float AnalogPositiveMaxDeadZone = 0.2;

	float AnalogNegativeMinDeadZone = -0.2;
	float AnalogNegativeMaxDeadZone = 0;

	input::ConstructDigitalEvent("Pause");
	input::ConstructAnalogEvent("MenuVertical");
	input::ConstructAnalogEvent("MenuHorizontal");
	input::ConstructDigitalEvent("MenuConfirm");
	input::ConstructDigitalEvent("MenuBack");
	input::ConstructDigitalEvent("MenuDpadUp");
	input::ConstructDigitalEvent("MenuDpadDown");
	input::ConstructDigitalEvent("MenuDpadLeft");
	input::ConstructDigitalEvent("MenuDpadRight");

	for (size_t i = 0; i < 4; i++)
	{
		input::ConstructAnalogEvent("Throttle" + std::to_string(i));
		input::ConstructAnalogEvent("Turn" + std::to_string(i));

		input::ConstructDigitalEvent("Shoot" + std::to_string(i));
		input::ConstructDigitalEvent("Boost" + std::to_string(i));

		//This is intentionally duplicate for map select
		input::ConstructDigitalEvent("MenuDpadLeft" + std::to_string(i));
		input::ConstructDigitalEvent("MenuDpadRight" + std::to_string(i));

		// Controller input

		input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_A, { "Shoot" + std::to_string(i) });
		input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_B, { "Boost" + std::to_string(i) });

		input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_START, { "Pause" });
		input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_A, { "MenuConfirm" });
		input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_B, { "MenuBack" });
		input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_DPAD_UP, { "MenuDpadUp" });
		input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_DPAD_DOWN, { "MenuDpadDown" });
		input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_DPAD_LEFT, { "MenuDpadLeft" });
		input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT, { "MenuDpadRight" });
		input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_DPAD_LEFT, { "MenuDpadLeft" + std::to_string(i) });
		input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT, { "MenuDpadRight" + std::to_string(i) });

		input::bindAnalogControllerInput(i,
			{
				{ { input::digitalPositiveInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER },
				{ { input::digitalNegativeInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER }
			},
			{ "Throttle" + std::to_string(i) });

		input::bindAnalogControllerInput(i,
			{
				{ { input::controllerMixedInput, AnalogNegativeMinDeadZone, AnalogPositiveMaxDeadZone }, GLFW_GAMEPAD_AXIS_LEFT_Y }
			},
			{ "MenuVertical" });

		input::bindAnalogControllerInput(i,
			{
				{ { input::controllerMixedInput, AnalogNegativeMinDeadZone, AnalogPositiveMaxDeadZone }, GLFW_GAMEPAD_AXIS_LEFT_X }
			},
			{ "MenuHorizontal" });

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

	input::bindAnalogInput(GLFW_KEY_X, { input::digitalPositiveInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, { "Throttle" + std::to_string(KeyboardPlayer) });
	input::bindAnalogInput(GLFW_KEY_Z, { input::digitalNegativeInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, { "Throttle" + std::to_string(KeyboardPlayer) });
	input::bindAnalogInput(GLFW_KEY_UP, { input::digitalPositiveInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, { "Throttle" + std::to_string(KeyboardPlayer) });
	input::bindAnalogInput(GLFW_KEY_DOWN, { input::digitalNegativeInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, { "Throttle" + std::to_string(KeyboardPlayer) });

	input::bindAnalogInput(GLFW_KEY_UP, { input::digitalNegativeInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, { "MenuVertical" });
	input::bindAnalogInput(GLFW_KEY_DOWN, { input::digitalPositiveInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, { "MenuVertical" });
	input::bindAnalogInput(GLFW_KEY_LEFT, { input::digitalNegativeInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, { "MenuHorizontal" });
	input::bindAnalogInput(GLFW_KEY_RIGHT, { input::digitalPositiveInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, { "MenuHorizontal" });
	input::bindDigitalInput(GLFW_KEY_N, { "MenuConfirm" });
	input::bindDigitalInput(GLFW_KEY_B, { "MenuBack" });

	input::bindDigitalInput(GLFW_KEY_N, { "Shoot" + std::to_string(KeyboardPlayer) });
	input::bindDigitalInput(GLFW_KEY_B, { "Boost" + std::to_string(KeyboardPlayer) });

	int KeyboardPlayer2 = 1;
	input::bindAnalogInput(GLFW_KEY_D, { input::digitalPositiveInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, { "Turn" + std::to_string(KeyboardPlayer2) });
	input::bindAnalogInput(GLFW_KEY_A, { input::digitalNegativeInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, { "Turn" + std::to_string(KeyboardPlayer2) });

	input::bindAnalogInput(GLFW_KEY_W, { input::digitalPositiveInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, { "Throttle" + std::to_string(KeyboardPlayer2) });
	input::bindAnalogInput(GLFW_KEY_S, { input::digitalNegativeInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, { "Throttle" + std::to_string(KeyboardPlayer2) });

	input::bindAnalogInput(GLFW_KEY_W, { input::digitalNegativeInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, { "MenuVertical" });
	input::bindAnalogInput(GLFW_KEY_S, { input::digitalPositiveInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, { "MenuVertical" });
	input::bindAnalogInput(GLFW_KEY_A, { input::digitalNegativeInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, { "MenuHorizontal" });
	input::bindAnalogInput(GLFW_KEY_D, { input::digitalPositiveInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, { "MenuHorizontal" });
	input::bindDigitalInput(GLFW_KEY_H, { "MenuConfirm" });
	input::bindDigitalInput(GLFW_KEY_J, { "MenuBack" });

	input::bindDigitalInput(GLFW_KEY_H, { "Shoot" + std::to_string(KeyboardPlayer2) });
	input::bindDigitalInput(GLFW_KEY_J, { "Boost" + std::to_string(KeyboardPlayer2) });

	input::bindDigitalInput(GLFW_KEY_G, { "Pause" });
}

engine::Camera* cam;

//Delete all entities and load menu
static void ReturnToMainMenu()
{
	engine::enablePhysics = true;
	engine::enableAnimation = true;

	ecs::DestroyAllEntities();

	spriteRenderSystem->SetTilemap(nullptr);
	canStartLoadingMap = false;
	ecs::GetSystem<PlayerSelectSystem>()->isShipSelectionMenuOn = false;
	cam->SetPosition(0);
	checkpointEntities.clear();

	MainMenuSystem::Load();
}

int main()
{
#ifdef _DEBUG
	ShowWindow(GetConsoleWindow(), SW_SHOW);
#else
	//Disable console in release mode
	ShowWindow(GetConsoleWindow(), SW_SHOW);
#endif

	GLFWwindow* window = engine::CreateGLWindow(1920, 1080, "Window", false);

	engine::EngineInit();

	//Make the camera
	cam = new engine::Camera(1920, 1080);
	cam->SetPosition(Vector3(0, 0, 1500));
	cam->SetRotation(Vector3(0, 0, 0));

	//Display the loading screen

	//Create the loading screen entity
	ecs::Entity loadingScreen = engine::ecs::NewEntity();
	Texture* loadingTexture = new Texture(assetPath + "misc/Loading.png");
	engine::ecs::AddComponent(loadingScreen, engine::SpriteRenderer{ .texture = loadingTexture, .enabled = true, .uiElement = true });
	engine::ecs::AddComponent(loadingScreen, engine::Transform{ .position = Vector3(0, 0, 0), .scale = Vector3(1) });
	//Display the loading screen
	engine::Update(cam);
	glfwSwapBuffers(window);


	//Init sound engine
	std::shared_ptr<engine::SoundSystem> soundSystem = engine::ecs::GetSystem<engine::SoundSystem>();
	soundSystem->AddSoundEngine("Gameplay");
	soundSystem->AddSoundEngine("Boat");
	soundSystem->AddSoundEngine("Background");
	soundSystem->AddSoundEngine("Music");
	soundSystem->SetGlobalVolume(1);
	ecs::SetComponentDestructor<engine::SoundComponent>(OnSoundComponentDestroyed);

	//Loads all globally used resources
	resources::LoadResources(cam);

	input::initialize(window);

	//Get pointers and call init of every custom system
	std::shared_ptr<PauseSystem> pauseSystem = engine::ecs::GetSystem<PauseSystem>();
	std::shared_ptr<PlayerController> playerController = engine::ecs::GetSystem<PlayerController>();
	playerController->Init();
	std::shared_ptr<HedgehogSystem> hedgehogSystem = engine::ecs::GetSystem<HedgehogSystem>();
	std::shared_ptr<SubmarineSystem> submarineSystem = ecs::GetSystem<SubmarineSystem>();
	std::shared_ptr<PickupSystem> pickupSystem = engine::ecs::GetSystem<PickupSystem>();
	std::shared_ptr<CruiseMissileSystem> cruiseMissileSystem = engine::ecs::GetSystem<CruiseMissileSystem>();

	std::shared_ptr<LevelSelectionSystem> levelSelectionSystem = engine::ecs::GetSystem<LevelSelectionSystem>();
	std::shared_ptr<PlayerSelectSystem> playerSelectionSystem = engine::ecs::GetSystem<PlayerSelectSystem>();


	Vector3 newListenerPosition(2200.000000, -1075.000000, 0.00000);
	soundSystem->SetListeningPosition(newListenerPosition);


	GameState currentGameState = menuMainState;

	MainMenuSystem::Load();

	//Bind all input actions
	SetupInput();


	//Object placement editor
	engine::ecs::Entity placementEditor = ecs::NewEntity();
	ecs::AddComponent(placementEditor, Transform{ .position = Vector3(1475, -460, 166), .scale = 20 });
	ecs::AddComponent(placementEditor, ModelRenderer{ .model = resources::models["Prop_PowerUpBox2.obj"] });
	ecs::AddTag(placementEditor, "persistent");


	//Collision layer matrix setup
	//Currently 0 = default, 1 = surface players, 2 = underwater, 3 = bridges, 4 = projectiles
	collisionSystem->SetTileCollisionLayer(3, 3);
	collisionSystem->SetLayerInteraction(2, 3, CollisionSystem::LayerInteraction::none);
	collisionSystem->SetLayerInteraction(2, 1, CollisionSystem::LayerInteraction::none);
	collisionSystem->SetLayerInteraction(4, 3, CollisionSystem::LayerInteraction::none);

	//Delete loading screen
	ecs::DestroyEntity(loadingScreen);
	delete loadingTexture;

	bool altDown = false;

	//Game Loop
	while (!glfwWindowShouldClose(window))
	{
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

		if (glfwGetKey(window, GLFW_KEY_0))
		{
			ReturnToMainMenu();
		}
		if (glfwGetKey(window, GLFW_KEY_ENTER) && glfwGetKey(window, GLFW_KEY_LEFT_ALT))
		{
			if (!altDown)
			{
				altDown = true;
				ToggleFullscreen();
				if (gameState == pauseMenuState)
					pauseSystem->UpdateFullscreenIcon();
			}
		}
		else
		{
			altDown = false;
		}


		input::update();

		//Handle updating of proper systems
		switch (gameState)
		{
		case menuMainState:
			MainMenuSystem::Update();
			break;
		case mapSelection:
			levelSelectionSystem->Update();
			break;
		case pauseMenuState:
			pauseSystem->Update();
			break;
		case selectPlayersState:
			engine::modelRenderSystem->SetLight(Vector3(0, 0, -200), 255);
			playerSelectionSystem->Update();
			break;
		case gamePlayState:
			engine::enablePhysics = true;
			engine::enableAnimation = true;
			//Check for pause has to be done here unfortunately
			if (input::GetNewPress("Pause") && playerController->countdownTimer <= 0)
			{
				pauseSystem->ToggleShowUIMenu();
				gameState = pauseMenuState;
				engine::enablePhysics = false;
				engine::enableAnimation = false;
			}

			UpdateCam(cam, collisionSystem->tilemap, currentLevel == 4);
			//Camera position must be divided by 2 because of a known camera bug
			soundSystem->SetListeningPosition(Vector3(cam->position.x * 2, cam->position.y * 2, 30));
			newListenerPosition = (Vector3(cam->position.x * 2, cam->position.y * 2, 30));

			playerController->Update(window);
			submarineSystem->Update();
			hedgehogSystem->Update();
			soundSystem->Update();
			pickupSystem->Update();
			cruiseMissileSystem->Update();

			break;
		default:
			std::cout << "\n ERROR NO STATE FOUND:" << gameState << std::endl;
			break;
		}

		//Update engine systems
		engine::Update(cam);

		ecs::Update();
		glfwSwapBuffers(window);
	}

	//Unitialize and free everything
	ecs::DestroyAllEntities(true);
	engine::UninitializeTimers();
	input::uninitialize();
	soundSystem->Uninitialize();
	glfwTerminate();

	return 0;
}