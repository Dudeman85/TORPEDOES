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

#include <engine/MapLayer.h>

MapLayer::MapLayer(tmx::Map& map, std::vector <std::shared_ptr<engine::Texture>>& textures)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
}

TiledMap MapLayer::fetchData(tmx::Map& map, const std::string tilemap)
{
	// Temporal class for TiledMap
	// used to store values to struct
	TiledMap tMap;

	// Temporal class for TiledGroup
	// used to store values to struct
	TiledGroup tGroup;

	// Temporal class for TiledObject
	// used to store values to struct
	TiledObject tObject;

	// Temporal class for TiledLayer
	// used to store values to struct
	TiledLayer tLayer;

	// Temporal class for TiledSed
	// used to store values to struct
	TiledSet tSet;

	// Temporal class for TiledTile
	// used to store values to struct
	TiledTile tTile;

	// Get all the tilesets from tilemap
	const auto& tilesets = map.getTilesets();

	// Iterate through all tilesets
	for (const auto& tileset : tilesets)
	{
		// Get all tiles from this tileset
		// as a list
		const auto& tiles = tileset.getTiles();

		// Iterate through the tiles
		for (const auto& tile : tiles)
		{
			// Get the tile properties
			const auto& tileProperties = tile.properties;

			// Iterate through tile properties
			for (const auto& tileProperty : tileProperties)
			{
				// Insert the property values into struct
				tTile.tile.insert(tileProperty.getName(), tileProperty.getStringValue());
			}

			// Insert the tile into tileset
			// struct
			tSet.tiles.push_back(tTile);
		}

		// Get the tileset properties
		const auto& tilesetProperties = tileset.getProperties();

		// Iterate the property values into struct
		for (const auto& tilesetProperty : tilesetProperties)
		{
			// Insert the property values into struct
			tSet.tileset.insert(tilesetProperty.getName(), tilesetProperty.getStringValue());
		}

		// Insert the tileset into tilemap
		// struct
		tMap.tilesets.push_back(tSet);
	}

	// Get all the different layers
	// from the tilemap
	const auto& alayers = map.getLayers();

	// Iterate through all layers
	for (const auto& layer : alayers)
	{
		// Get current layer's type
		const auto& cLayerType = layer->getType();

		// The different layer types handling
		// in switch loop
		switch (cLayerType)
		{
			// Group layers handling
		case tmx::Layer::Type::Group:
			// Get current group layers properties
			const auto& groupProperties = layer->getProperties();
			// Iterate the property values into struct
			for (const auto& groupProperty : groupProperties)
			{
				// Insert the property values into struct
				tGroup.group.insert(groupProperty.getName(), groupProperty.getStringValue());
			}

			// Insert the group layer into
			// tilemap struct
			tMap.groups.push_back(tGroup);

			break;

			// Object layers handling
		case tmx::Layer::Type::Object:
			// Get current object layers properties
			const auto& objectProperties = layer->getProperties();
			// Iterate the property values into struct
			for (const auto& objectProperty : objectProperties)
			{
				// Insert the property values into struct
				tObject.object.insert(objectProperty.getName(), objectProperty.getStringValue());
			}

			// Insert the object layer into
			// tilemap struct
			tMap.objects.push_back(tObject);

			break;

			// Tile layers handling
		case tmx::Layer::Type::Tile:
			// Get current tile layers properties
			const auto& tileProperties = layer->getProperties();
			// Iterate the property values into struct
			for (const auto& tileProperty : tileProperties)
			{
				// Insert the property values into struct
				tLayer.layer.insert(tileProperty.getName(), tileProperty.getStringValue());
			}

			// Insert the tile layer into
			// tilemap struct
			tMap.layers.push_back(tLayer);

			break;

		default:
			break;
		}
	}

	// Get the current tilemaps properties
	const auto& mapProperties = map.getProperties();

	// Iterate the property values into struct
	for (const auto& mapProperty : mapProperties)
	{
		// Inset the property values into struct
		tMap.tilemap.insert(mapProperty.getName(), mapProperty.getStringValue());
	}

	// return Tiledmap value
	return tMap;
}