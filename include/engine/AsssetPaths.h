#pragma once

#include "string"

/* Paths */
#if defined(_DEBUG)
inline std::string ASSET_PATH = "../../../assets/";		// Asset file path
#else
inline std::string ASSET_PATH = "assets/";				// Asset file path
#endif
inline std::string FONT_PATH = "fonts/";				// Font file path within ASSETS_PATH
inline std::string LEVEL_PATH = "levels/";				// Level file path within ASSETS_PATH
inline std::string MATERIAL_PATH = "materials/";		// Material file path within ASSETS_PATH
inline std::string SOUND_PATH = "sounds/";				// Sound file path within ASSETS_PATH
