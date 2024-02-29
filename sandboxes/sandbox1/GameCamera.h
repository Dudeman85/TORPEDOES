#pragma once
#include <engine/Tilemap.h>
#include "PlayerController.h"

	const float camScale = 1.0;
	float camScaleMin = 600.0f;
	float camScaleMax = 1650.0f;
	const float aspectRatio = 16.f / 9.f;
	float camPadding = 100;
	float camDeadzone = 10;

void UpdateCam(GLFWwindow* window, Camera& cam, Tilemap& map) {
	std::shared_ptr<PlayerController> playerController = ecs::GetSystem<PlayerController>();
	

	//Calculate the Bounding Box
	std::array<float, 4> camBounds{
		cam.position.y * 2 + cam.height / 2,  // yls pain 
			cam.position.x * 2 + cam.width / 2,   // leveys 
			cam.position.y * 2 - cam.height / 2,
			cam.position.x * 2 - cam.width / 2 };

	float zoomOutThreshold = -camPadding * 2.5f;
	float zoomInThreshold = camPadding * 2.0f;

	//Calculate the difference between the player and camera bounds

	float topDiff = camBounds[0] - playerBounds[0];
	float rightDiff = camBounds[1] - playerController->playerBounds[1];
	float bottomDiff = playerController->playerBounds[2] - camBounds[2];
	float leftDiff = playerController->playerBounds[3] - camBounds[3];

	//Zoom out
	if (playerController->playerBounds[0] < camBounds[0] - zoomOutThreshold ||
		playerController->playerBounds[1] > camBounds[1] + zoomOutThreshold ||
		playerController->playerBounds[2] > camBounds[2] + zoomOutThreshold ||
		playerController->playerBounds[3] < camBounds[3] - zoomOutThreshold)
	{

		float zoomOutFactor = 10.0f;
		float zoomOutValue = zoomOutFactor - min(topDiff, min(bottomDiff, min(rightDiff, leftDiff))) / 10.0;
		camScale = max(camScale + zoomOutValue, camScaleMin);
		/*camScale += 10 - min(topDiff, min(bottomDiff, min(rightDiff, leftDiff))) / 10;*/
	}
	//Zoom int
	else if (topDiff > zoomInThreshold && rightDiff > zoomInThreshold && bottomDiff > zoomInThreshold && leftDiff > zoomInThreshold)
	{

		float zoomInValue = min(topDiff, min(bottomDiff, min(rightDiff, leftDiff))) / 100.0f;
		camScale = max(camScale - zoomInValue, camScaleMin);
	}

	//Clamp the camera zoom between min and max and set it's dimensions
	camScale = clamp(camScale, camScaleMin, camScaleMax);

	cam.height = camScale;
	cam.width = cam.height * aspectRatio;

	// New implentation 
	//boundigBoxin center point 
	float boundingBoxWidth = playerController->playerBounds[1] - playerController->playerBounds[3];
	float boundingBoxHeight = playerController->playerBounds[0] - playerController->playerBounds[2];
	Vector2 boundingBoxCenter = Vector2(playerController->playerBounds[3] + boundingBoxWidth * 0.5f, playerController->playerBounds[2] + boundingBoxHeight * 0.5f);

	//Ajustar la posicin de la cmara segn el centro de la bounding box
	camPosX = clamp(boundingBoxCenter.x, map.position.x + cam.width / 2, map.position.x + map.bounds.width - cam.width / 2);
	camPosY = clamp(boundingBoxCenter.y, map.position.y - map.bounds.height + cam.height / 2, map.position.y - cam.height / 2);
	cam.SetPosition(Vector3(camPosX, camPosY, 1500));

	// Calculate the desired zoom level and adjust the camera zoom.
	float aspectRatio = cam.width / cam.height;
	float desiredZoom = std::max(boundingBoxWidth / (cam.width * aspectRatio), boundingBoxHeight / cam.height);


	// Ajustar el zoom de la cmara solo si el zoom deseado supera los lmites establecidos
	if (desiredZoom > camScaleMin && desiredZoom < camScaleMax)
	{
		camScale = desiredZoom;
	}

	//Reset the average player position data
	playerController->avgPosition = Vector3();
	playerController->playerBounds = { -INFINITY, -INFINITY, INFINITY, INFINITY };
	// engine::Update(&cam);
}