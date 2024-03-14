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
	// Temporal class for TMap
	// used to store values to struct
	TMap tmap;

	// Temporal class for TGroup
	// used to store values to struct
	TGroup tgroup;

	// Temporal class for TObject
	// used to store values to struct
	TObject tobject;

	// Temporal class for TLayer
	// used to store values to struct
	TLayer tlayer;

	// Temporal class for TSet
	// used to store values to struct
	TSet tset;

	// Temporal class for TTile
	// used to store values to struct
	TTile ttile;

	// map class for tmx::Map functions
	tmx::Map map;
	// Loading tilemap from filepath.
	map.load(tilemapPath);
	// Get all the tilesets from
	// tilemap
	const auto& tilesets = map.getTilesets();
	// Iterate through tilesets
	for (const auto& tileset : tilesets)
	{
		// Get the list of tiles
		// in this tileset
		const auto& tiles = tileset.getTiles();
		// Iterate through the tiles
		for (const auto& tile : tiles)
		{
			// Get the tile properties
			const auto& properties = tile.properties;
			// Iterate through tile properties
			for (const auto& property : properties)
			{
				// Insert the property values in to struct
				ttile.tile.insert(property.getName(), property.getStringValue());
			}
			// Insert the tile into
			// tileset struct
			tset.tiles.push_back(ttile);
		}
		// Get the tileset properties
		const auto& properties = tileset.getProperties();
		// Iterate the property values in to struct
		for (const auto& property : properties)
		{
			// Insert the property values in to struct
			tset.tileset.insert(property.getName(), property.getStringValue());
		}
		// Insert the tileset
		// in to tilemap struct
		tmap.tilesets.push_back(tset);
	}

	// Get all the layers
	// from tilemap
	const auto& aLayers = map.getLayers();
	// Iterate through all layers
	for (const auto& layer : aLayers)
	{
		// get layer's type
		const auto& layerType = layer->getType();

		switch (layerType)
		{
		case tmx::Layer::Type::Group:

			break;
		case tmx::Layer::Type::Object:
			break;
		case tmx::Layer::Type::Tile:
			break;
		default:
			break;
		}
	}


}