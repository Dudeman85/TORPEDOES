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

#include <engine/TMVariables.h>

void TMVariables::fetchData(const std::string tilemapPath)
{
	// map class for tmx::Map functions
	tmx::Map map;
	// Loading tilemap from filepath.
	map.load(tilemapPath);
	// Temp list of tilesets
	const auto& tilesets = map.getTilesets();
	// Iterate through tilesets
	for (const auto& tileset : tilesets)
	{
		// Get the list of tiles in this tileset
		const auto& tiles = tileset.getTiles();
		// Iterate through the tiles
		for (const auto& tile : tiles)
		{
			TTile Tile;
			const auto& properties = tile.properties;

			for (const auto& prop : properties)
			{

			}
		}
	}



}