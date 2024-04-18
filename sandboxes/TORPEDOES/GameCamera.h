#pragma once
#include <engine/Tilemap.h>
#include "PlayerController.h"

//Current height of the camera, width is calculated automatically
float camHeight = 1.f;
const float aspectRatio = 16.f / 9.f;
//Maximum units a player can be from the camera's edge before zooming in
const float zoomInThreshold = 350;
//Minimum units a player can be from the camera's edge before zooming out
const float zoomOutThreshold = 300;
//Minimum height the camera can have
const float minHeight = 600;

static void UpdateCam(engine::Camera* cam, Tilemap* map)
{
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
		camHeight += zoomOutThreshold - minDiff;
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
		camHeight -= minDiff - zoomInThreshold;
	}

	//Restrict camera to size of tilemap
	camHeight = std::clamp(camHeight, minHeight, map->bounds.height);

	//Calculate the camera's position
	Vector2 playersCenter(playerBounds[3] + (playerBounds[1] - playerBounds[3]) / 2, playerBounds[2] + (playerBounds[0] - playerBounds[2]) / 2);
	const float positionXMax = map->position.x + map->bounds.width - (camHeight * aspectRatio) / 2;
	const float positionXMin = map->position.x + (camHeight * aspectRatio) / 2;
	const float positionYMax = map->position.y - camHeight / 2;
	const float positionYMin = map->position.y - map->bounds.height + camHeight / 2;

	//Set the camera to the center of the player box
	Vector3 position;
	position.x = std::clamp(playersCenter.x, positionXMin, positionXMax);
	position.y = std::clamp(playersCenter.y, positionYMin, positionYMax);
	position.z = 1500;

	//Apply camera position and scale
	cam->SetPosition(position);
	cam->SetDimensions(std::floor(camHeight * aspectRatio), camHeight);
	engine::modelRenderSystem->SetLight(Vector3(cam->position.x, cam->position.y, cam->position.z + 10000), Vector3(255));
}