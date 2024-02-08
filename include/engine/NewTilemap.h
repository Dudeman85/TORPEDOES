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
#include <engine/Constants.h>
#include <engine/Vector.h>

struct TiledMap
{

};

struct TiledGroup
{
	
};

struct TiledObject
{

};

struct TiledLayer
{

};

struct TiledSet
{
	// Tileset Name
	string Name = "";
	// Tilset Class
	string Class = "";
	// Tileset Object Alignment
	Tileset::ObjectAlignment Align;
	// 
	 
	 
	// All the Tiles in this Tileset
	vector<TiledTiles> tiles;
};

// Struct for Tile properties inside Tileset
struct TiledTiles
{
	// Tile ID
	int* ID;
	// Tile Class
	string* Class;
	// Tile Width
	int* Width;
	// Tile Height
	int* Height;
	// Tile Probability
	float* Probability;
	// Tile X
	int* X;
	// Tile Y
	int* Y;
	// Tile Position
	Vector2u* Pos(int x, int y);
	// Tile Size
	Vector2u* Size(int h, int w);
	// Tile Image Rect
	Vector2u* rect(Vector2u p, Vector2u s);
	// Under here if Tile has Custom Properties
};


using namespace tmx;
using namespace std;

class Tilemap final
{
public:

	// Constructor
	Tilemap();

	// Destructor
	~Tilemap();

	// load() Function to load Tilemap and it's tileset
	void load(const string map);

	// draw() Function to go through all layers and
	// draws them on the screen
	void draw();

private:

	// TiledMap struct
	TiledMap* map;
	// TiledGroup struct
	TiledGroup* group;

};