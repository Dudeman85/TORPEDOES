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

#include <engine/GL/Texture.h>
#include <tmxlite/Map.hpp>
#include <map>
#include <string>
#include <vector>
#include <memory>

// Tiled map struct declaration
struct TiledMap;
// Tiled group struct declaration
struct TiledGroup;
// Tiled object struct declaration
struct TiledObject;
// Tiled layer struct declaration
struct TiledLayer;
// Tiled set struct declaration
struct TiledSet;
// Tiled tile struct declaration
struct TiledTile;

// Tiled Tilemap struct
struct TiledMap
{
	// Tilemap's properties map list
	std::map<std::string, std::string> tilemap;
	// Tilemap's all Group layers vector
	std::vector<TiledGroup> groups;
	// Tilemap's all Object layers vector
	std::vector<TiledObject> objects;
	// Tilemap's all Tile layers vector
	std::vector<TiledLayer> layers;
	// Tilemap's all tilesets vector
	std::vector<TiledSet> tilesets;
};

// Tiled Group layer struct
struct TiledGroup
{
	// Group layers properties map list
	std::map<std::string, std::string> group;
};

// Tiled Object layer struct
struct TiledObject
{
	// Object layers properties map list
	std::map<std::string, std::string> object;
};

// Tiled Tile layer struct
struct TiledLayer
{
	// Tile layers properties map list
	std::map<std::string, std::string> layer;
};

// Tiled Tileset struct
struct TiledSet
{
	// Tileset's properties map list
	std::map<std::string, std::string> tileset;
	// Tileset's all tiles vector
	std::vector<TiledTile> tiles;
};

// Tiled Tile struct
struct TiledTile
{
	// Tile's properties map list
	std::map<std::string, std::string> tile;
};

class MapLayer final
{
public:
	// Constructor
	MapLayer(tmx::Map& map, std::vector <std::shared_ptr<engine::Texture>>& textures);
	// Destructor
	~MapLayer();

	// fetchData(tmx::Map& map, const std::string tilemap)
// function to get data from tsx and tmx files
	TiledMap fetchData(tmx::Map& map, const std::string tilemap);

private:
	// Vertex Array Object
	unsigned int VAO;

	//
	void createSubsets(tmx::Map& map, TiledMap tilemap);
};
