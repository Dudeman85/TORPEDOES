#pragma once
#include <string>

//Asset paths
#ifdef _DEBUG
#ifdef PROJECT_NAME
const std::string assetPath = PROJECT_NAME "/assets/";
#else
const std::string assetPath = "assets/";
#endif
#else
const std::string assetPath = "assets/";
#endif