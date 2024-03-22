/*********************************************************************
Matt Marchant 2016
http://trederia.blogspot.com

tmxlite - Zlib license.

This software is provided 'as-is', without any express or
implied warranty. In no event will the authors be held
liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute
it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented;
you must not claim that you wrote the original software.
If you use this software in a product, an acknowledgment
in the product documentation would be appreciated but
is not required.

2. Altered source versions must be plainly marked as such,
and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any
source distribution.
*********************************************************************/

#pragma once

#include <tmxlite/Map.hpp>
#include <engine/MapLayer.h>
#include <engine/Constants.h>
#include <engine/GL/Camera.h>
#include <engine/GL/Shader.h>

using namespace tmx;
using namespace engine;
using namespace std;

// OldTilemap still has Tilemap variable name and class
// so for now TMap to not cause errors. Need to fix name's
// later when new code is´finished and old code can be
// removed.
class Tilemap final
{
public:

	// Constructor
	Tilemap(const string mapPath, Camera* cam);

	// Destructor
	~Tilemap(){}

private:
	// Maplayer class
	MapLayer maplayer;
	// TiledMap variable that has all properties stored
	TiledMap tiledmap;
	// Shader and camera variables.
	Shader m_shader;
	Camera camera;
};