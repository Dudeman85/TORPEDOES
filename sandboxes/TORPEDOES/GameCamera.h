#pragma once
#include <engine/Tilemap.h>
#include "PlayerController.h"
#include "Globals.h"

static float purgeDelay = 1;

static void UpdateCam(engine::Camera* cam, Tilemap* map, bool isRiver = false)
{
	const float maxZoomSpeed = 30;
	const float maxMoveSpeed = 70;

	using namespace engine;

#ifdef _DEBUG
	if ((map->bounds.width / map->bounds.height) < (16.f / 9.f))
		std::cout << "map is wrong aspect\n";
#endif

	std::shared_ptr<PlayerController> playerController = ecs::GetSystem<PlayerController>();

	// Calculate the camera's bounds
	std::array<float, 4> camBounds{
		cam->position.y * 2 + cam->height / 2, //top
			cam->position.x * 2 + cam->width / 2, //right
			cam->position.y * 2 - cam->height / 2, //bottom
			cam->position.x * 2 - cam->width / 2 //left
	};

	//Get the bounds of all players
	//Top, right, bottom, left
	std::array<float, 4> playerBounds = playerController->GetPlayerBounds();
	//Bounds need to clamped to prevent infinite zoom
	playerBounds[0] = std::min(playerBounds[0], map->position.y - zoomInThreshold);
	playerBounds[1] = std::min(playerBounds[1], map->position.x + map->bounds.width - zoomInThreshold);
	playerBounds[2] = std::max(playerBounds[2], map->position.y - map->bounds.height + zoomInThreshold);
	playerBounds[3] = std::max(playerBounds[3], map->position.x + zoomInThreshold);

	//Calculate the difference between the player and camera bounds
	float topDiff = abs(camBounds[0] - playerBounds[0]);
	float rightDiff = abs(camBounds[1] - playerBounds[1]);
	float bottomDiff = abs(camBounds[2] - playerBounds[2]);
	float leftDiff = abs(camBounds[3] - playerBounds[3]);

	//Find which edge is the closest to the camera's edge
	float minDiff = std::min(topDiff, std::min(rightDiff, std::min(bottomDiff, leftDiff)));

	//Zoom out when any player is close enough to the edge of the camera
	if (minDiff < zoomOutThreshold)
	{
		//Zoom out just enough to keep everything in bounds
		camHeight += std::min(zoomOutThreshold - minDiff, maxZoomSpeed);
	}
	//Zoom in when all players are far enough from the camera's edge
	else if (minDiff >= zoomInThreshold)
	{
		//Special case for when one player is too close to the tilemap's edge
		if (minDiff == zoomInThreshold)
		{
			//Find a player which is not at the tilemap's edge
			if (topDiff > zoomInThreshold)
				minDiff = topDiff;
			if (rightDiff > zoomInThreshold && rightDiff < minDiff)
				minDiff = rightDiff;
			if (bottomDiff > zoomInThreshold && bottomDiff < minDiff)
				minDiff = bottomDiff;
			if (leftDiff > zoomInThreshold && leftDiff < minDiff)
				minDiff = leftDiff;
		}

		//Zoom in just enough to keep everything in bounds
		camHeight -= std::min(minDiff - zoomInThreshold, maxZoomSpeed);
	}

	//Restrict camera to size of tilemap
	camHeight = std::clamp(camHeight, minHeight, std::min(map->bounds.height, maxCamHeight));

	//Calculate the camera's position
	Vector2 playersCenter(playerBounds[3] + (playerBounds[1] - playerBounds[3]) / 2, playerBounds[2] + (playerBounds[0] - playerBounds[2]) / 2);
	const float positionXMax = map->position.x + map->bounds.width - (camHeight * aspectRatio) / 2;
	const float positionXMin = map->position.x + (camHeight * aspectRatio) / 2;
	const float positionYMax = map->position.y - camHeight / 2;
	const float positionYMin = map->position.y - map->bounds.height + camHeight / 2;

	//Special case for river map
	//If fully zoomed out
	if (camHeight == maxCamHeight && purgeDelay <= 0)
	{
		playerController->PurgeSlowPlayers();
		purgeDelay = 1;
	}
	purgeDelay -= engine::cappedDeltaTime;

	//Set the camera to the center of the player box
	Vector3 position;
	position.x = std::clamp(playersCenter.x, positionXMin, positionXMax);
	position.y = std::clamp(playersCenter.y, positionYMin, positionYMax);
	position.z = 1500;

	//Clamp the maximum translation of camera
	Vector3 diff = position - Vector3(cam->position) * 2;
	diff.x = std::clamp(diff.x, -maxMoveSpeed, maxMoveSpeed);
	diff.y = std::clamp(diff.y, -maxMoveSpeed, maxMoveSpeed);
	diff.z = 0;

	//Apply camera position and scale
	if (cam->position.x == 0)
		cam->SetPosition(position);
	else
		cam->Translate(diff);

	cam->SetDimensions(std::floor(camHeight * aspectRatio), camHeight);
	engine::modelRenderSystem->SetLight(Vector3(cam->position.x, cam->position.y, cam->position.z + 10000), Vector3(255));
}