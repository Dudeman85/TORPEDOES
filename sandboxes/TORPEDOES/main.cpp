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
bool isGamePaused = false;

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
}

std::vector<Vector3> cheeringSoundPos;

void SetupCheeringSounds(const std::vector<Vector3>& positions)
{
	for (const auto& pos : positions) {

		engine::ecs::Entity cheerEntity = engine::ecs::NewEntity(); 
		engine::ecs::AddComponent(cheerEntity, engine::Transform{ .position = pos });

		Audio* cheerSound = engine::AddAudio("Gameplay", "audio/cheering.wav", true, 0.3f, DistanceModel::LINEAR);

		cheerSound->setAbsoluteDirection(pos);

		// Randomize start and stop delays
		float startDelay = Random(0.0f, 500.0f); // Start delay between 0-50ms
		//float stopDelay = startDelay + Random(1000.0f, 5000.0f); // Stop after 1-5 seconds

		cheerSound->setStartTimeMilliseconds(startDelay);
		//cheerSound->setStopTimeMilliseconds(stopDelay);

		engine::ecs::AddComponent(cheerEntity, engine::SoundComponent{ .Sounds =
			{
				{"CrowdCheer", cheerSound}
			} , .maxDistance = 1500, .referenceDistance = 200, .rolloffFactor = 1 });
		cheerSound->play();
	}
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
	CreateCheckpoint(Vector3(1475.000000, -460.000000, 70.000000), Vector3(-25.000000, -90.000000, -90.000000), Vector3(24.f), resources::models["Prop_Goal_Ver2.obj"], 360.f, true);

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
		{ 1285, -665, 0 },
		{ 1205, -665, 0 },
		{ 1285, -365, 0 },
		{ 1205, -265, 0 },
		{ 3140, -410, 0 },
		{ 3325, -780, 0 }
	};
	SetupCheeringSounds(cheeringSoundPos);

	PlayCountdown(Vector3(1235.0f, -310.0f, 200.0f));
	PlayerController::lapCount = 3;
	//Create the players
	engine::ecs::GetSystem<PlayerController>()->CreatePlayers(playerShips, Vector2(1225.0f, -400.0f));
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
	CreateCheckpoint(Vector3(1245.000000, -1680.000000, 100.000000), Vector3(75.000000, -90.000000, 7.500000), Vector3(22), resources::models["Prop_Goal_Ver2.obj"], 0.0f, true);	// Finish line

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
	CreateCheckpoint(Vector3(2555.000000, -1600.000000, 100.000000), Vector3(-17.500000, -87.500000, -90.000000), Vector3(20.5f), resources::models["Prop_Goal_Ver2.obj"], 360.f, true);

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
	CreateCrowd({ 480.000000, -825.000000, 167}, resources::crowdAnims1);			// 2. crowd
	CreateSmallCrowd({ 445.000000, -910.000000, 166 }, resources::crowdAnims2);		// 3. crowd
	CreateSmallCrowd({ 445.000000, -935.000000, 167 }, resources::crowdAnims2);		// 4. crowd
	CreateSmallCrowd({ 445.000000, -1000.000000, 166 }, resources::crowdAnims2);	// 5. crowd
	CreateSmallCrowd({ 445.000000, -1070.000000, 166 }, resources::crowdAnims2);	// 6. crowd
	CreateSmallCrowd({ 445.000000, -1100.000000, 167 }, resources::crowdAnims2);	// 7. crowd
	CreateSmallCrowd({ 445.000000, -1130.000000, 168 }, resources::crowdAnims2);	// 8. crowd
	CreateCrowd({ 890.000000, -1220.000000, 166}, resources::crowdAnims1);			// 9. crowd
	CreateCrowd({ 870.5, -1235.000000, 167 }, resources::crowdAnims1);				// 10. crowd
	CreateCrowd({ 840, -1245.000000, 168 }, resources::crowdAnims1);				// 11. crowd
	CreateCrowd({ 890, -1265.000000, 169 }, resources::crowdAnims1);				// 12. crowd
	CreateCrowd({ 610.000000, -1220.000000, 166}, resources::crowdAnims1);
	CreateCrowd({ 560.5, -1235.000000, 167 }, resources::crowdAnims1);
	CreateCrowd({ 650, -1245.000000, 168 }, resources::crowdAnims1);
	CreateCrowd({ 600, -1265.000000, 169 }, resources::crowdAnims1);

	PlayCountdown(Vector3(2480.0f, -1460.0f, 200.0f));
	PlayerController::lapCount = 3;
	//Create the players
	engine::ecs::GetSystem<PlayerController>()->CreatePlayers(playerShips, Vector2(2480.0f, -1520.0f));
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
	CreateCheckpoint(Vector3(15760.000000, -925.000000, 100.000000), Vector3(-12.500000, -90.000000, -87.500000), Vector3(43.0f), resources::models["Prop_Goal_Ver2.obj"], 360.f, true);

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

	PlayCountdown(Vector3(1434.0f, -1470.0f, 200.0f));
	PlayerController::lapCount = 1;
	engine::ecs::GetSystem<PlayerController>()->CreatePlayers(playerShips, Vector2(1434.0f, -1520.0f));
}

//Bind all input events here
static void SetupInput()
{
	input::ConstructDigitalEvent("Pause");
	input::ConstructDigitalEvent("Menu");
	input::bindDigitalInput(GLFW_KEY_U, { "Menu" });
	input::ConstructDigitalEvent("StartGame");
	input::bindDigitalInput(GLFW_KEY_G, { "StartGame" });

	input::bindDigitalInput(GLFW_GAMEPAD_BUTTON_DPAD_LEFT, { "Turn" });

	// TODO: add controller pause key

	float AnalogPositiveMinDeadZone = 0;
	float AnalogPositiveMaxDeadZone = 0.2;

	float AnalogNegativeMinDeadZone = -0.2;
	float AnalogNegativeMaxDeadZone = 0;

	for (size_t i = 0; i < 4; i++)
	{
		input::ConstructAnalogEvent("Throttle" + std::to_string(i));
		input::ConstructAnalogEvent("Turn" + std::to_string(i));

		input::ConstructDigitalEvent("Shoot" + std::to_string(i));
		input::ConstructDigitalEvent("Boost" + std::to_string(i));

		// Controller input

		input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_A, { "Shoot" + std::to_string(i) });
		input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_B, { "Boost" + std::to_string(i) });
		input::bindDigitalControllerInput(i, GLFW_GAMEPAD_BUTTON_START, { "Pause" });

		input::bindAnalogControllerInput(i,
			{
				{ { input::digitalPositiveInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER ,  },
				{ { input::digitalNegativeInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER }
			},
			{ "Throttle" + std::to_string(i) });


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
	
	input::bindDigitalInput(GLFW_KEY_N, { "Shoot" + std::to_string(KeyboardPlayer) });
	input::bindDigitalInput(GLFW_KEY_B, { "Boost" + std::to_string(KeyboardPlayer) });

	int KeyboardPlayer2 = 1;
	input::bindAnalogInput(GLFW_KEY_D, { input::digitalPositiveInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, { "Turn" + std::to_string(KeyboardPlayer2) });
	input::bindAnalogInput(GLFW_KEY_A, { input::digitalNegativeInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, { "Turn" + std::to_string(KeyboardPlayer2) });

	input::bindAnalogInput(GLFW_KEY_W, { input::digitalPositiveInput, AnalogPositiveMinDeadZone, AnalogPositiveMaxDeadZone }, { "Throttle" + std::to_string(KeyboardPlayer2) });
	input::bindAnalogInput(GLFW_KEY_S, { input::digitalNegativeInput, AnalogNegativeMinDeadZone, AnalogNegativeMaxDeadZone }, { "Throttle" + std::to_string(KeyboardPlayer2) });

	input::bindDigitalInput(GLFW_KEY_H, { "Shoot" + std::to_string(KeyboardPlayer2) });
	input::bindDigitalInput(GLFW_KEY_J, { "Boost" + std::to_string(KeyboardPlayer2) });

	input::bindDigitalInput(GLFW_KEY_P, { "Pause" });
	input::bindDigitalInput(GLFW_KEY_G, { "StartGame" });
} 

static void PlayersMenu(std::shared_ptr<PlayerSelectSystem> ShipSelectionSystem)
{
	ShipSelectionSystem->isShipSelectionMenuOn = !ShipSelectionSystem->isShipSelectionMenuOn;
	isGamePaused = !isGamePaused;


	ShipSelectionSystem->ToggleMenuPlayerSelection();


	std::cout << "is Ship selection open:" << ShipSelectionSystem->isShipSelectionMenuOn;
}

engine::Camera* cam;

//Delete all entities and load menu
static void ReturnToMainMenu()
{
	ecs::DestroyAllEntities();

	spriteRenderSystem->SetTilemap(nullptr);
	isGamePaused = true;
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
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	GLFWwindow* window = engine::CreateGLWindow(1920, 1080, "Window", true);

	engine::EngineInit();

	//Make the camera
	cam = new engine::Camera(1920, 1080);
	cam->SetPosition(Vector3(0, 0, 1500));
	cam->SetRotation(Vector3(0, 0, 0));

	//Display the loading screen

	//Create the loading screen entity
	ecs::Entity loadingScreen = engine::ecs::NewEntity();
	Texture loadingTexture(assetPath + "menuUI/Loading.png");
	engine::ecs::AddComponent(loadingScreen, engine::SpriteRenderer{ .texture = &loadingTexture, .enabled = true, .uiElement = true });
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

	//Loads all globally used resources
	resources::LoadResources(cam);

	input::initialize(window);

	//Get pointers and call init of every custom system
	std::shared_ptr<PauseSystem> pauseSystem = engine::ecs::GetSystem<PauseSystem>();
	pauseSystem->Init(window);
	std::shared_ptr<PlayerController> playerController = engine::ecs::GetSystem<PlayerController>();
	playerController->Init();
	std::shared_ptr<HedgehogSystem> hedgehogSystem = engine::ecs::GetSystem<HedgehogSystem>();
	std::shared_ptr<SubmarineSystem> submarineSystem = ecs::GetSystem<SubmarineSystem>();
	std::shared_ptr<PickupSystem> pickupSystem = engine::ecs::GetSystem<PickupSystem>();
	TimerSystem::ScheduleFunction([pickupSystem]() {pickupSystem->Update(); }, 0.016, true);

	std::shared_ptr<LevelSelectionSystem> levelSelectionSystem = engine::ecs::GetSystem<LevelSelectionSystem>();
	std::shared_ptr<PlayerSelectSystem> playerSelectionSystem = engine::ecs::GetSystem<PlayerSelectSystem>();


	Vector3 newListenerPosition(2200.000000, -1075.000000, 0.00000);
	soundSystem->SetListeningPosition(newListenerPosition);


	GameState currentGameState = menuMainState;
	isGamePaused = true;

	MainMenuSystem::Load();

	//Bind all input actions
	SetupInput();

	/*
	//Object placement editor
	engine::ecs::Entity placementEditor = ecs::NewEntity();
	ecs::AddComponent(placementEditor, Transform{ .position = Vector3(500, -500, 166), .scale = 20 });
	ecs::AddComponent(placementEditor, ModelRenderer{ .model = resources::models["Prop_PowerUpBox2.obj"] });
	ecs::AddTag(placementEditor, "persistent");
	*/

	//Collision layer matrix setup
	//Currently 0 = default, 1 = surface players, 2 = underwater, 3 = bridges, 4 = projectiles
	collisionSystem->SetTileCollisionLayer(3, 3);
	collisionSystem->SetLayerInteraction(2, 3, CollisionSystem::LayerInteraction::none);
	collisionSystem->SetLayerInteraction(2, 1, CollisionSystem::LayerInteraction::none);
	collisionSystem->SetLayerInteraction(4, 3, CollisionSystem::LayerInteraction::none);

	//Delete loading screen
	ecs::DestroyEntity(loadingScreen);

	//Game Loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		//Close window when Esc is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		/*
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
		*/

		if (glfwGetKey(window, GLFW_KEY_0))
		{
			ReturnToMainMenu();
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
		case inGameOptionsState:
			pauseSystem->Update();
			break;
		case selectPlayersState:
			engine::modelRenderSystem->SetLight(Vector3(0, 0, -200), 255);
			playerSelectionSystem->Update();
			break;
		case gamePlayState:
			UpdateCam(cam, collisionSystem->tilemap, currentLevel == 4);
			//Camera position must be divided by 2 because of a known camera bug
			soundSystem->SetListeningPosition(Vector3(cam->position.x * 2, cam->position.y * 2, 30));
			newListenerPosition = (Vector3(cam->position.x * 2, cam->position.y * 2, 30));

			playerController->Update(window);
			submarineSystem->Update();
			hedgehogSystem->Update();
			soundSystem->Update();

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

	soundSystem->Uninitialize();

	engine::UninitializeTimers();
	input::uninitialize();

	ecs::DestroyAllEntities(true);
	glfwTerminate();
	return 0;
}