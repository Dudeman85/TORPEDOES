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
#include <map>
#include <string>
#include <vector>
#include <memory>

// Tiled Tilemap struct
struct TMap
{
	// Tilemap's properties map list
	std::map<std::string, std::string> tilemap;
	// Tilemap's all Group layers vector
	std::vector<TGroup> groups;
	// Tilemap's all Object layers vector
	std::vector<TObject> objects;
	// Tilemap's all Tile layers vector
	std::vector<TLayer> layers;
	// Tilemap's all tilesets vector
	std::vector<TSet> tilesets;
};

// Tiled Group layer struct
struct TGroup
{
	// Group layers properties map list
	std::map<std::string, std::string> group;
};

// Tiled Object layer struct
struct TObject
{
	// Object layers properties map list
	std::map<std::string, std::string> object;
};

// Tiled Tile layer struct
struct TLayer
{
	// Tile layers properties map list
	std::map<std::string, std::string> layer;
};

// Tiled Tileset struct
struct TSet
{
	// Tileset's properties map list
	std::map<std::string, std::string> tileset;
	// Tileset's all tiles vector
	std::vector<TTile> tiles;
};

// Tiled Tile struct
struct TTile
{
	// Tile's properties map list
	std::map<std::string, std::string> tile;
};


class TMVariables
{
public:

	// fetchData() function to get data from
	// tmx and tsx files
	void fetchData(const std::string tilemap);

	// Tilemap Get() and Set() functions
	void tilemapGet()
	{

	}
	void tilemapSet()
	{

	}

	// Group layer Get() and Set() functions
	void groupGet()
	{

	}

	void groupSet()
	{

	}

	// Object layer Get() and Set() functions
	void objectGet()
	{

	}

	void objectSet()
	{

	}

	// Tile layer Get() and Set() functions
	void layerGet()
	{

	}

	void layerSet()
	{

	}



private:
	// Tiled Tilemaps in vector. From here we
	// get the properties and set new properties
	// if needed.
	std::unique_ptr<std::vector<TMap>> maps;
};

