#pragma once

//Current height of the camera, width is calculated automatically
inline float camHeight = 1.f;
inline const float aspectRatio = 16.f / 9.f;
//Maximum units a player can be from the camera's edge before zooming in
inline const float zoomInThreshold = 350;
//Minimum units a player can be from the camera's edge before zooming out
inline const float zoomOutThreshold = 300;
//Minimum height the camera can have
inline const float minHeight = 800;
