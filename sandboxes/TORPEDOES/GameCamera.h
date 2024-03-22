#pragma once
#include <engine/Tilemap.h>
#include "PlayerController.h"

float camScale = 1.0;
float camScaleMin = 600.0f;
float camScaleMax = 1650.0f;
const float aspectRatio = 16.f / 9.f;
float camPadding = 100;
float camDeadzone = 10;
const float zoomThreshold = 5.0f;

//New stuff
const float aspectRatio = 16.f / 9.f;
//Maximum units a player can be from the camera's edge before zooming in
const float zoomInThreshold = 500;
//Minimum units a player can be from the camera's edge before zooming out
const float zoomOutThreshold = 200;
//How fast the camera can zoom in or out
const float zoomSpeed = 10;

static void UpdateCam(Camera* cam, Tilemap* map)
{
	//New camera implementation

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

	float topDiff = camBounds[0] - playerBounds[0];
	float rightDiff = camBounds[1] - playerBounds[1];
	float bottomDiff = playerBounds[2] - camBounds[2];
	float leftDiff = playerBounds[3] - camBounds[3];

	if (topDiff < zoomOutThreshold || rightDiff < zoomOutThreshold || bottomDiff < zoomOutThreshold || leftDiff < zoomOutThreshold)
	{
		camScale += zoomSpeed;
	}
	else if


	//OLD CAMERA IMPLEMENTATION

	camScaleMax = map->bounds.height - 1;


	// Calculate the Bounding Box
	std::array<float, 4> camBounds{
		cam.position.y * 2 + cam.height / 2,		//top
			cam.position.x * 2 + cam.width / 2,		//right
			cam.position.y * 2 - cam.height / 2,	//bottom
			cam.position.x * 2 - cam.width / 2 };	//left

	float zoomOutThreshold = -camPadding * 2.5f;
	float zoomInThreshold = camPadding * 2.0f;

	// Calculate the difference between the player and camera bounds
	// array values: Top, right, bottom, left
	std::array<float, 4> playerBounds = playerController->GetPlayerBounds();

	float topDiff = camBounds[0] - playerBounds[0];
	float rightDiff = camBounds[1] - playerBounds[1];
	float bottomDiff = playerBounds[2] - camBounds[2];
	float leftDiff = playerBounds[3] - camBounds[3];

	// Zoom out
	if (playerBounds[0] < camBounds[0] - zoomOutThreshold ||
		playerBounds[1] > camBounds[1] + zoomOutThreshold ||
		playerBounds[2] > camBounds[2] + zoomOutThreshold ||
		playerBounds[3] < camBounds[3] - zoomOutThreshold)
	{
		float zoomOutFactor = 10.0f;
		float zoomOutValue = zoomOutFactor - min(topDiff, min(bottomDiff, min(rightDiff, leftDiff))) / 10.0 + zoomThreshold;
		camScale = max(camScale + zoomOutValue, camScaleMin);
	}
	// Zoom in
	else if (topDiff > zoomInThreshold && 
			 rightDiff > zoomInThreshold && 
			 bottomDiff > zoomInThreshold && 
			 leftDiff > zoomInThreshold)
	{
		float zoomInValue = min(topDiff, min(bottomDiff, min(rightDiff, leftDiff))) / 100.0f;
		camScale = max(camScale - zoomInValue, camScaleMin);
	}

	// Clamp the camera zoom between min and max and set it's dimensions
	camScale = clamp(camScale, camScaleMin, camScaleMax);
	cam.height = camScale;
	cam.width = cam.height * aspectRatio;

	// Calculate the center point of the bounding box
	float boundingBoxWidth = playerBounds[1] - playerBounds[3];
	float boundingBoxHeight = playerBounds[0] - playerBounds[2];
	Vector2 boundingBoxCenter = Vector2(playerBounds[3] + boundingBoxWidth * 0.5f, playerBounds[2] + boundingBoxHeight * 0.5f);

	// Adjust the camera position based on the center of the bounding box
	Vector3 camPos;
	camPos.x = clamp(boundingBoxCenter.x, map->position.x + cam.width / 2, map->position.x + map->bounds.width - cam.width / 2);
	camPos.y = clamp(boundingBoxCenter.y, map->position.y - map->bounds.height + cam.height / 2, map->position.y - cam.height / 2);
	camPos.z = 1500;
	cam.SetPosition(camPos);

	// Calculate the desired zoom level and adjust the camera zoom.
	float desiredZoom = max(boundingBoxWidth / (cam.width * aspectRatio), boundingBoxHeight / cam.height);


	// Adjust the camera zoom only if the desired zoom exceeds the established limits
	if (desiredZoom > camScaleMin && desiredZoom < camScaleMax)
	{
		camScale = desiredZoom;
	}

	modelRenderSystem->SetLight(Vector3(cam.position.x, cam.position.y, 1500), Vector3(255));
}