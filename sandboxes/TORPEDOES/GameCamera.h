#pragma once
#include <engine/Tilemap.h>
#include "PlayerController.h"

const float aspectRatio = 16.f / 9.f;
//Current height of the camera, width is calculated automatically
float camHeight = 1.f;
//Maximum units a player can be from the camera's edge before zooming in
const float zoomInThreshold = 350;
//Minimum units a player can be from the camera's edge before zooming out
const float zoomOutThreshold = 300;
//Minimum height the camera can have
const float minHeight = 300;

static void UpdateCam(Camera* cam, Tilemap* map)
{
	std::shared_ptr<PlayerController> playerController = ecs::GetSystem<PlayerController>();

	// Calculate the camera's bounds
	std::array<float, 4> camBounds{
		cam->position.y * 2 + cam->height / 2, //top
			cam->position.x * 2 + cam->width / 2, //right
			cam->position.y * 2 - cam->height / 2, //bottom
			cam->position.x * 2 - cam->width / 2 //left
	};

	//Calculate the difference between the player and camera bounds
	//Top, right, bottom, left
	std::array<float, 4> playerBounds = playerController->GetPlayerBounds();

	float topDiff = abs(camBounds[0] - playerBounds[0]);
	float rightDiff = abs(camBounds[1] - playerBounds[1]);
	float bottomDiff = abs(camBounds[2] - playerBounds[2]);
	float leftDiff = abs(camBounds[3] - playerBounds[3]);

	//Find which edge is the closest to the camera's edge, but still inside the zoom out threshold and not too close to the tilemap's edge
	float minDiffOut = 10000000000;
	if (topDiff < minDiffOut && topDiff < zoomOutThreshold && abs(playerBounds[0]) > zoomOutThreshold)
		minDiffOut = topDiff;
	if (rightDiff < minDiffOut && rightDiff < zoomOutThreshold && map->bounds.width - abs(playerBounds[1]) > zoomOutThreshold)
		minDiffOut = rightDiff;
	if (bottomDiff < minDiffOut && bottomDiff < zoomOutThreshold && map->bounds.height - abs(playerBounds[2]) > zoomOutThreshold)
		minDiffOut = bottomDiff;
	if (leftDiff < minDiffOut && leftDiff < zoomOutThreshold && abs(playerBounds[3]) > zoomOutThreshold)
		minDiffOut = leftDiff;
	
	//Find which edge is the closest to the camera's edge, but outside the zoom in threshold and not too close to the tilemap's edge
	float minDiffIn = 10000000000;
	if (topDiff < minDiffIn  && abs(playerBounds[0]) > zoomOutThreshold)
		minDiffIn = topDiff;
	if (rightDiff < minDiffIn  && map->bounds.width - abs(playerBounds[1]) > zoomOutThreshold)
		minDiffIn = rightDiff;
	if (bottomDiff < minDiffIn  && map->bounds.height - abs(playerBounds[2]) > zoomOutThreshold)
		minDiffIn = bottomDiff;
	if (leftDiff < minDiffIn  && abs(playerBounds[3]) > zoomOutThreshold)
		minDiffIn = leftDiff;

	//Zoom out when any player is close enough to the edge of the camera, but not too close to edge of tilemap
	if (minDiffOut < zoomOutThreshold)
	{
		//Zoom out just enough to keep everything in bounds
		camHeight += zoomOutThreshold - minDiffOut;
	}
	//Zoom in when all players are far enough from the camera's edge
	else if (minDiffIn > zoomInThreshold && minDiffIn < 1000000000)
	{
		//Zoom in just enough to keep everything in bounds
		camHeight -= minDiffIn - zoomInThreshold;
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
}