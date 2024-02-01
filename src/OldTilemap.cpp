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
#define _USE_MATH_DEFINES
#include <engine/Tilemap.h>
#include <engine/Constants.h>
#include <glm/gtc/matrix_transform.hpp>
#include <tmxlite/Map.hpp>
#include <engine/GL/Shader.h>
#include <engine/GL/Texture.h>
#include <engine/MapLayer.h>
#include <cassert>
#include <array>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <tmxlite/TileLayer.hpp>
#include <algorithm>

Tilemap::Tilemap(engine::Camera* cam)
{
	m_shader = new engine::Shader(
	R"(
	#version 330 core
	in vec3 a_position;
	in vec2 a_texCoord;

	uniform mat4 u_projectionMatrix;
	uniform mat4 u_viewMatrix;
	uniform mat4 u_modelMatrix;

	out vec2 v_texCoord;

	void main()
	{
	gl_Position =  u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(a_position, 1.0);

	v_texCoord = a_texCoord;
	}
	)", R"(
	#version 330 core
	#define FLIP_HORIZONTAL 8u
	#define FLIP_VERTICAL 4u
	#define FLIP_DIAGONAL 2u

	in vec2 v_texCoord;

	uniform usampler2D u_lookupMap;
	uniform sampler2D u_tileMap;

	uniform vec2 u_tileSize;
	uniform vec2 u_tilesetCount;
	uniform vec2 u_tilesetScale = vec2(1.0);

	uniform float u_opacity = 1.0;

	out vec4 colour;
	/*fixes rounding imprecision on AMD cards*/
	const float epsilon = 0.000005;

	void main()
	{
		uvec2 values = texture(u_lookupMap, v_texCoord).rg;
		float tileID = float(values.r);
		if (tileID > 0u)
		{
			float index = float(tileID) - 1.0;
			vec2 position = vec2(mod(index + epsilon, u_tilesetCount.x), floor((index / u_tilesetCount.x) + epsilon)) / u_tilesetCount;
			vec2 offsetCoord = (v_texCoord * (textureSize(u_lookupMap, 0) * u_tilesetScale)) / u_tileSize;

			vec2 texelSize = vec2(1.0) / textureSize(u_lookupMap, 0);
			vec2 offset = mod(v_texCoord, texelSize);
			vec2 ratio = offset / texelSize;
			offset = ratio * (1.0 / u_tileSize);
			offset *= u_tileSize / u_tilesetCount;

			if (values.g != 0u)
			{
				vec2 tileSize = vec2(1.0) / u_tilesetCount;
				if ((values.g & FLIP_DIAGONAL) != 0u)
				{
					float temp = offset.x;
					offset.x = offset.y;
					offset.y = temp;
					temp = tileSize.x / tileSize.y;
					offset.x *= temp;
					offset.y /= temp;
					offset.x = tileSize.x - offset.x;
					offset.y = tileSize.y - offset.y;
				}
				if ((values.g & FLIP_VERTICAL) != 0u)
				{
					offset.y = tileSize.y - offset.y;
				}
				if ((values.g & FLIP_HORIZONTAL) != 0u)
				{
					offset.x = tileSize.x - offset.x;
				}
			}
			colour = texture(u_tileMap, position + offset);
			colour.a = min(colour.a, u_opacity);
		}
		else
		{
			colour = vec4(0.0);
		}
	}
	)", false);

	camera = cam;
	collisionLayer = std::vector<std::vector<unsigned int>>();
	position = glm::vec3(0);
}

Tilemap::~Tilemap()
{
}

void Tilemap::draw(float layer)
{
	if (mapLayers.count(layer) == 0)
		return;

	m_shader->use();

	glm::mat4 model = glm::translate(glm::mat4(1.0f), position);

	unsigned int modelLoc = glGetUniformLocation(m_shader->ID, "u_modelMatrix");

	unsigned int viewLoc = glGetUniformLocation(m_shader->ID, "u_viewMatrix");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));

	//Give the shader the projection matrix
	unsigned int projLoc = glGetUniformLocation(m_shader->ID, "u_projectionMatrix");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

	unsigned int u_tilesetCount = glGetUniformLocation(m_shader->ID, "u_tilesetCount");

	unsigned int u_tileSize = glGetUniformLocation(m_shader->ID, "u_tileSize");

	for (int i = 0; i < mapLayers[layer].size(); i++)
	{
		mapLayers[layer][i]->draw(model, modelLoc, u_tilesetCount, u_tileSize);
	}
}

void Tilemap::loadMap(const std::string ownMap)
{
	tmx::Map map;
	map.load(assetPath + ownMap);

	//create shared resources, shader and tileset textures
	initGLStuff(map);

	bounds = map.getBounds();
	tileSize = map.getTileSize();
	zLayers.insert(0);

	//create a drawable object for each tile layer
	const auto& layers = map.getLayers();
	for (auto i = 0u; i < layers.size(); ++i)
	{
		if (layers[i]->getType() == tmx::Layer::Type::Tile)
		{
			//If the layer is a collision layer
			if (layers[i]->getName() == "collider")
			{
				//Resize the collision map
				collisionLayer.resize(map.getTileCount().x);
				for (int i = 0; i < collisionLayer.size(); i++)
				{
					collisionLayer[i].resize(map.getTileCount().y);
				}

				//Get the tile IDs
				auto& tiles = layers[i]->getLayerAs<tmx::TileLayer>().getTiles();

				//Transfer the tile IDs to the 2D collision vector
				for (int y = 0; y < collisionLayer[0].size(); y++)
				{
					for (int x = 0; x < collisionLayer.size(); x++)
					{
						collisionLayer[x][y] = tiles[(y * collisionLayer.size()) + x].ID;
					}
				}
			}
			else
			{
				std::shared_ptr<MapLayer> layer = std::make_unique<MapLayer>(map, i, allTextures);

				const auto& properties = layers[i]->getProperties();
				tmx::Tileset tileset = map.getTilesets()[0];
				layer->tileSize = glm::vec2(tileset.getTileSize().x, tileset.getTileSize().y);
				for (const auto& property : properties)
				{
					if (property.getName() == "Z")
					{
						zLayers.insert(property.getFloatValue());
						layer->zLayer = property.getFloatValue();
						break;
					}
				}

				mapLayers[layer->zLayer].push_back(layer);
			}
		}
	}
}

//Get the center position of a tile in world coordinates
engine::Vector2 Tilemap::GetTilePosition(unsigned int x, unsigned int y)
{
	//Move from top left to center position
	engine::Vector2 position;
	position.x += tileSize.x / 2;
	position.y -= tileSize.y / 2;

	//X increases in positive X
	position.x += x * tileSize.x;
	//Y increases in negative Y
	position.y -= y * tileSize.y;

	return position;
}

//Find any collision layer tiles in a box
std::vector<engine::Vector2> Tilemap::CheckCollisionBox(engine::Vector2 topLeft, engine::Vector2 bottomRight)
{
	//Log all tiles
	std::vector<engine::Vector2> collisions;

	if (collisionLayer.empty())
		return collisions;

	//Calculate the x and y index bounds
	int xMin = floor((topLeft.x + position.x) / tileSize.x);
	int xMax = ceil((bottomRight.x + position.x) / tileSize.x);
	int yMin = floor((-topLeft.y + position.y) / tileSize.y);
	int yMax = ceil((-bottomRight.y + position.y) / tileSize.y);

	//Check every position inside the box
	for (int x = xMin; x < xMax; x++)
	{
		for (int y = yMin; y < yMax; y++)
		{
			//Check bounds
			if (x < collisionLayer.size() && x >= 0 && y < collisionLayer[0].size() && y >= 0)
			{
				//If tile is in collision layer log it
				if (collisionLayer[x][y] != 0)
					collisions.push_back(engine::Vector2(x, y));
			}
		}
	}

	return collisions;
}

//Returns the collision layers tile ID at x and y
unsigned int Tilemap::checkCollision(float x, float y)
{
	if (collisionLayer.empty())
		return 0;

	int xIndex = floor((x + position.x) / tileSize.x);
	int yIndex = floor((-y + position.y) / tileSize.y);

	//Check out of bounds
	if (xIndex >= collisionLayer.size() || yIndex >= collisionLayer[0].size() || xIndex < 0 || yIndex < 0)
		return 0;

	return collisionLayer[xIndex][yIndex];
}

void Tilemap::initGLStuff(const tmx::Map& map)
{
	m_shader->use();

	//we'll make sure the current tile texture is active in 0, 
	//and lookup texture is active in 1 in MapLayer::draw()
	glUniform1i(glGetUniformLocation(m_shader->ID, "u_tileMap"), 0);
	glUniform1i(glGetUniformLocation(m_shader->ID, "u_lookupMap"), 1);


	const auto& tilesets = map.getTilesets();
	for (const auto& ts : tilesets)
	{
		std::string imagePath = ts.getImagePath();
		imagePath.erase(imagePath.begin(), imagePath.begin() + 7);
		auto texture = loadTexture(imagePath);
		allTextures.push_back(texture);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
}

std::shared_ptr<engine::Texture> Tilemap::loadTexture(const std::string& path)
{
	return std::make_shared<engine::Texture>(path.c_str(), GL_NEAREST, false);
}